#include <algorithm>
#include <cstdlib>
#include <future>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <mutex>


#include "log_duration.h"
#include "test_framework.h"

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap {
public:
	static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);
	
	explicit ConcurrentMap(size_t bucket_count) {
		for (unsigned int i = 0; i < bucket_count; ++i) {
			sub_maps_.push_back(map<Key, Value>());
		}
	}

	/*struct Access {
		Value& ref_to_value;
		lock_guard<mutex> guard;
	};*/

	struct Access {
		Access() = default;

		Access(Value& value_, mutex& value_mutex_)
			: ref_to_value(value_)
			, struct_value_mutex(value_mutex_)
		{
			struct_value_mutex.lock();
		}

		~Access() {
			//std::unique_lock<std::mutex> lock(struct_value_mutex);
			struct_value_mutex.unlock();
		}

		Value& ref_to_value;
		mutex& struct_value_mutex;
	};



	/*Добавление в словарь — непростая операция, которая может изменить всю его структуру. Поэтому не получится увеличить количество мьютексов*/
	Access operator[](const Key& key) {
		auto it = itog_map.find(key);
		if (it != itog_map.end()) {
			//return { it->second, lock_guard(m_) };
			return Access(it->second, m_);
		}
		else {
			unsigned int sub_map_index = key % sub_maps_.size();
			std::lock_guard<std::mutex> lock(m_);
			sub_maps_[sub_map_index][key] = Value();
			//return { Value(), lock_guard(m_) };
			return Access(Value(), m_);
		}
	}

	std::map<Key, Value> BuildOrdinaryMap() {
		std::lock_guard<std::mutex> lock(m_);
		for (auto el : sub_maps_) {
			itog_map.insert(el.begin(), el.end());
		}
	}

private:
	Value value_;
	vector<map<Key, Value>> sub_maps_;
	map < Key, Value> itog_map;
	mutex m_;
};

using namespace std;

void RunConcurrentUpdates(ConcurrentMap<int, int>& cm, size_t thread_count, int key_count) {
	auto kernel = [&cm, key_count](int seed) {
		vector<int> updates(key_count);
		iota(begin(updates), end(updates), -key_count / 2);
		shuffle(begin(updates), end(updates), mt19937(seed));

		for (int i = 0; i < 2; ++i) {
			for (auto key : updates) {
				++cm[key].ref_to_value;
			}
		}
	};

	vector<future<void>> futures;
	for (size_t i = 0; i < thread_count; ++i) {
		futures.push_back(async(kernel, i));
	}
}

void TestConcurrentUpdate() {
	constexpr size_t THREAD_COUNT = 3;
	constexpr size_t KEY_COUNT = 50000;

	ConcurrentMap<int, int> cm(THREAD_COUNT);
	RunConcurrentUpdates(cm, THREAD_COUNT, KEY_COUNT);

	const auto result = cm.BuildOrdinaryMap();
	ASSERT_EQUAL(result.size(), KEY_COUNT);
	for (auto& [k, v] : result) {
		AssertEqual(v, 6, "Key = " + to_string(k));
	}
}

void TestReadAndWrite() {
	ConcurrentMap<size_t, string> cm(5);

	auto updater = [&cm] {
		for (size_t i = 0; i < 50000; ++i) {
			cm[i].ref_to_value.push_back('a');
		}
	};
	auto reader = [&cm] {
		vector<string> result(50000);
		for (size_t i = 0; i < result.size(); ++i) {
			result[i] = cm[i].ref_to_value;
		}
		return result;
	};

	auto u1 = async(updater);
	auto r1 = async(reader);
	auto u2 = async(updater);
	auto r2 = async(reader);

	u1.get();
	u2.get();

	for (auto f : { &r1, &r2 }) {
		auto result = f->get();
		ASSERT(all_of(result.begin(), result.end(), [](const string& s) {
			return s.empty() || s == "a" || s == "aa";
			}));
	}
}

void TestSpeedup() {
	{
		ConcurrentMap<int, int> single_lock(1);

		LOG_DURATION("Single lock");
		RunConcurrentUpdates(single_lock, 4, 50000);
	}
	{
		ConcurrentMap<int, int> many_locks(100);

		LOG_DURATION("100 locks");
		RunConcurrentUpdates(many_locks, 4, 50000);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestConcurrentUpdate);
	RUN_TEST(tr, TestReadAndWrite);
	RUN_TEST(tr, TestSpeedup);
}
