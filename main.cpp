#include <algorithm>
#include <execution>
#include <iostream>
#include <future>
#include <functional>

using namespace std;



int compare_string(const string &s1, const string &s2)
{
    return s1.compare(s2);
}




template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::sequenced_policy&, RandomAccessIterator range_begin, RandomAccessIterator range_end, const Value& target) {
    auto left = range_begin;
    auto right = range_end;
    while (left +1 < right) {
        const auto mid1 = left + (right - left) / 3;
        const auto mid2 = right - (right - left) / 3;     
        future<int> cmp1 = async([&target, mid2]{return compare_string(ref(target), ref(*mid2));});
       if (*mid1 == target) {
            return mid1;
        }
        else if (*mid2 == target) {
            return mid2;
        }
        else if (target < *mid1) {
            right = mid1;
        }
        else if (cmp1.get()) {
            left = mid2;
        }
        else {
            left = mid1 +1;
            right = mid2 ;
        }

    }
    if (left == range_begin && !(*left < target)) {
        return left;
    }
    else {
        return right;
    }
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(RandomAccessIterator range_begin, RandomAccessIterator range_end,
    const Value& value) {
    return LowerBound(execution::seq, range_begin, range_end, value);
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::parallel_policy&, RandomAccessIterator range_begin,
    RandomAccessIterator range_end, const Value& value) {
    return LowerBound(execution::seq, range_begin, range_end, value);
}


int main() {
    const vector<string> strings = { "cat", "dog", "dog", "horse" };

    const vector<string> requests = { "bear", "cat", "deer", "dog", "dogs", "horses" };

    // последовательные версии
    cout << "Request [" << requests[0] << "] → position "
         << LowerBound(strings.begin(), strings.end(), requests[0]) - strings.begin() << endl;
    cout << "Request [" << requests[1] << "] → position "
         << LowerBound(execution::seq, strings.begin(), strings.end(), requests[1])
            - strings.begin()
         << endl;
    cout << "Request [" << requests[2] << "] → position "
         << LowerBound(execution::seq, strings.begin(), strings.end(), requests[2])
            - strings.begin()
         << endl;

    // параллельные
    cout << "Request [" << requests[3] << "] → position "
         << LowerBound(execution::par, strings.begin(), strings.end(), requests[3])
            - strings.begin()
         << endl;
    cout << "Request [" << requests[4] << "] → position "
         << LowerBound(execution::par, strings.begin(), strings.end(), requests[4])
            - strings.begin()
         << endl;
    cout << "Request [" << requests[5] << "] → position "
         << LowerBound(execution::par, strings.begin(), strings.end(), requests[5])
            - strings.begin()
         << endl;
}
