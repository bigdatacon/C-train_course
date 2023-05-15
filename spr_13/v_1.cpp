#include <iostream>
#include <list>
#include <set>
#include <string_view>
#include <vector>
 
using namespace std;
 
 
template <typename Container>
void Merge(const Container& src1, const Container& src2, std::ostream& out) {
    typename Container::const_iterator it1 = src1.begin();
    typename Container::const_iterator it2 = src2.begin();
 
    while (it1 != src1.end() && it2 != src2.end()) {
        if (std::less<>()(*it1, *it2)) {
            out << *it1++ << std::endl;
        }
        else {
            out << *it2++ << std::endl;
        }
    }
 
    while (it1 != src1.end()) {
        out << *it1++ << std::endl;
    }
 
    while (it2 != src2.end()) {
        out << *it2++ << std::endl;
    }
}
 
 
template <typename Container1, typename Container2>
void MergeSomething(const Container1& src1, const Container2& src2, std::ostream& out) {
    Merge(src1, src2, out);
}
 
 
// Реализуйте эту функцию:
template <typename T>
void MergeHalves(const std::vector<T>& src, std::ostream& out) {
    size_t mid = (src.size() + 1) / 2;
    std::vector<T> left(src.begin(), src.begin() + mid);
    std::vector<T> right(src.begin() + mid, src.end());
    Merge(left, right, out);
}
 
 
int main() {
    vector<int> v1{ 60, 70, 80, 90 };
    vector<int> v2{ 65, 75, 85, 95 };
    vector<int> combined{ 60, 70, 80, 90, 65, 75, 85, 95 };
    list<double> my_list{ 0.1, 72.5, 82.11, 1e+30 };
    string_view my_string = "ACNZ"sv;
    set<unsigned> my_set{ 20u, 77u, 81u };
 
    // пока функция MergeSomething реализована пока только для векторов
    cout << "Merging vectors:"sv << endl;
    MergeSomething(v1, v2, cout);
 
    cout << "Merging vector and list:"sv << endl;
    MergeSomething(v1, my_list, cout);
 
    cout << "Merging string and list:"sv << endl;
    MergeSomething(my_string, my_list, cout);
 
    cout << "Merging set and vector:"sv << endl;
    MergeSomething(my_set, v2, cout);
 
    cout << "Merging vector halves:"sv << endl;
    MergeHalves(combined, cout);
}
