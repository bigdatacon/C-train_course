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

	struct Access {
		std::lock_guard<std::mutex> guard;
		Value& ref_to_value;
	};

	explicit ConcurrentMap(size_t bucket_count)
		:
		mtx_(bucket_count),
		map_(bucket_count),
		bucket_count_(bucket_count)
	{
	}

	Access operator[](const Key& key) {
		auto index = static_cast<uint64_t>(key) % bucket_count_;
		std::map<Key, Value>& bucket = map_[index];
		return { std::lock_guard(mtx_[index]), bucket[key] };
	}

	std::map<Key, Value> BuildOrdinaryMap() {
		std::map<Key, Value> result;
		for (int i = 0; i < bucket_count_; ++i) {
			std::lock_guard guard_(mtx_[i]);
			result.insert(map_[i].begin(), map_[i].end());
		}
		return result;
	}

private:
	std::vector<std::mutex> mtx_;
	std::vector<std::map<Key, Value>> map_;
	//std::mutex mtx_;
	int bucket_count_;
};

/*
template <typename Key, typename Value>
class ConcurrentMap {
public:
	static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);

	explicit ConcurrentMap(size_t bucket_count)
	
		:locks_(bucket_count)
		, sub_maps_(bucket_count)// определаю размер вектора mutex_ов 
		, count_(bucket_count)
	{

	}

	struct Access {
		Value& ref_to_value;
		lock_guard<mutex> guard;
	};


	Access operator[](const Key& key) {
		unsigned int i_l = key % sub_maps_.size();
		std::map<Key, Value>& result = sub_maps_[i_l];
		return { result[key], lock_guard(locks_[i_l]) };

	}

	std::map<Key, Value> BuildOrdinaryMap() {
		std::map<Key, Value> result;
		for (int i = 0; i < count_; ++i) {
			lock_guard guard(locks_[i]);  // выставляю мютекс по индексу подсловаря 
			result.insert(sub_maps_[i].begin(), sub_maps_[i].end());
		}
		return result;
	}


private:
	std::vector<std::map<Key, Value>> sub_maps_;
	std::vector<std::mutex> locks_;
	mutex m_;
	int count_;
};
*/

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
