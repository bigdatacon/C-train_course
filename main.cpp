#include <iostream>
#include <iterator>
#include <vector>
#include <set>


using namespace std;

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, char prefix) {
    auto pairs = make_pair(lower_bound(range_begin, range_end, string(1, prefix)),
        lower_bound(range_begin, range_end, string(1, char(prefix + 1))));

    //cout << "first : " << pairs.first;

    return pairs;
}

int main() {
    const vector<string> sorted_strings = { "moscow", "murmansk", "vologda" };
    /*auto m_result_2 = (sorted_strings.begin(), sorted_strings.end());
    for (auto el : m_result_2) { cout << el<< endl; };*/

    const auto m_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'm');
    for (auto it = m_result.first; it != m_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;
    const auto p_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'p');
    cout << (p_result.first - begin(sorted_strings)) << " " << (p_result.second - begin(sorted_strings)) << endl;
    const auto z_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'z');
    cout << (z_result.first - begin(sorted_strings)) << " " << (z_result.second - begin(sorted_strings)) << endl;
    return 0;
}
