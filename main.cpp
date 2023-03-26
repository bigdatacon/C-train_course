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
using namespace std;
using namespace std::string_literals;


template <typename Key, typename Value>
class ConcurrentMap {
public:
	static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);

	explicit ConcurrentMap(size_t bucket_count)
	
		:locks_(bucket_count) // определаю размер вектора mutex_ов 
	{
		for (unsigned int i = 0; i < bucket_count; ++i) {
			sub_maps_.push_back(map<Key, Value>());  // добавляю в вектор пустые map
		}
	}

	struct Access {
		Value& ref_to_value;
		lock_guard<mutex> guard;
	};

	Access operator[](const Key& key) {
		unsigned int i_l = key % sub_maps_.size();
		unsigned int sub_map_index = key % sub_maps_.size();
		lock_guard guard(locks_[i_l]);
		//lock_guard(locks_[i_l]
		auto it = sub_maps_[sub_map_index].find(key);
		if (it != sub_maps_[sub_map_index].end()) {
			//return { it->second, lock_guard guard(locks_[i_l]) };
			return { it->second, lock_guard(locks_[i_l]) };
		}
		else {
			it = sub_maps_[sub_map_index].emplace(key, Value()).first;
			//return { it->second, lock_guard guard(locks_[i_l])  };
			return { it->second, lock_guard(locks_[i_l]) };
		}
	}

	std::map<Key, Value> BuildOrdinaryMap() {
		std::map<Key, Value> result;
		for (size_t i = 0; i < sub_maps_.size(); ++i) {
			lock_guard guard(locks_[i]);  // выставляю мютекс по индексу подсловаря 
			for (const auto& [key, value] : sub_maps_[i]) {
				result[key] = value;
			}
		}
		return result;
	}

private:
	std::vector<std::map<Key, Value>> sub_maps_;
	std::vector<std::mutex> locks_;
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
