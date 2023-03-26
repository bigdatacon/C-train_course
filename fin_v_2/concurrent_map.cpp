#ifndef CONCURRENT_MAP_H
#define CONCURRENT_MAP_H

#include <map>
#include <mutex>

template<typename K, typename V>
class ConcurrentMap {
public:
    /*using Iterator = Type*;
    using ConstIterator = const Type*;*/

 

    void Insert(K key, V value) {
        std::lock_guard<std::mutex> guard(mutex_);
        data_[std::move(key)] = std::move(value);
    }

    V& operator[](const K& key) {
        std::lock_guard<std::mutex> guard(mutex_);
        return data_[key];
    }

    std::map<K, V> BuildOrdinaryMap() {
        std::lock_guard<std::mutex> guard(mutex_);
        return data_;
    }

    auto begin() { return data_.begin(); }
    auto end() { return data_.end(); }

private:
    std::map<K, V> data_;
    std::mutex mutex_;



};

#endif // CONCURRENT_MAP_H
