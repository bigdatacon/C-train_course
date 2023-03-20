#include <algorithm>
#include <execution>
#include <iostream>

using namespace std;

/*
Если ваша программа зацикливается, поймайте значения границ, на которых это происходит — наверняка понадобится отдельно обработать крайний случай. 
Между созданием future и вызовом get для future должны выполняться некоторые сложные вычисления.
*/

/*int ternary_search(vector<string> arr, string target) {
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right) {
        int mid1 = left + (right - left) / 3;
        int mid2 = right - (right - left) / 3;

        if (arr[mid1] == target) {
            return mid1;
        } else if (arr[mid2] == target) {
            return mid2;
        } else if (target < arr[mid1]) {
            right = mid1 - 1;
        } else if (target > arr[mid2]) {
            left = mid2 + 1;
        } else {
            left = mid1 + 1;
            right = mid2 - 1;
        }
    }

    return -1; // Искомый элемент не найден
}*/

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator ternary_search(const execution::sequenced_policy&, RandomAccessIterator range_begin, RandomAccessIterator range_end, const Value& target) {
    auto left = range_begin;
    auto right = range_end ;

    while (left <= right) {
        const auto mid1 = left + (right - left) / 3;
        const auto mid2 = right - (right - left) / 3;

        if (*mid1 == target) {
            return mid1;
        } else if (*mid2 == target) {
            return mid2;
        } else if (target < *mid1) {
            right = mid1 - 1;
        } else if (target > *mid2) {
            left = mid2 + 1;
        } else {
            left = mid1 + 1;
            right = mid2 - 1;
        }
    }

    return range_begin /*-1*/; // Искомый элемент не найден
}



template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::sequenced_policy&,
                                RandomAccessIterator range_begin, RandomAccessIterator range_end,
                                const Value& value) {
    auto left_bound = range_begin;
    auto right_bound = range_end;
    while (left_bound + 1 < right_bound) {
        const auto middle = left_bound + (right_bound - left_bound) / 2;
        if (*middle < value) {
            left_bound = middle;
        } else {
            right_bound = middle;
        }
    }
    if (left_bound == range_begin && !(*left_bound < value)) {
        return left_bound;
    } else {
        return right_bound;
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
    const vector<string> strings = {"cat", "dog", "dog", "horse"};

    const vector<string> requests = {"bear", "cat", "deer", "dog", "dogs", "horses"};
    
    cout << "базовая тернарная функция : " << endl;
        cout << "Request [" << requests[1] << "] → position ternar "
         << ternary_search(execution::seq, strings.begin(), strings.end(), requests[1])  << endl;
    
    
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
