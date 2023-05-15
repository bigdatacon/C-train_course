#include <iostream>
#include <list>
#include <set>
#include <string_view>
#include <vector>
 
using namespace std;
 
template <typename InputIt1 , typename InputIt2>
void Merge(InputIt1 src1_1, InputIt2 src1_2, InputIt1 src2_1, InputIt2 src2_2,  std::ostream& out) {
 
    while (src1_1 != src1_2 && src2_1 != src2_2) {
        if (std::less<>()(*src1_1, *src2_1)) {
            out << *src1_1++ << std::endl;
        }
        else {
            out << *src2_1++ << std::endl;
        }
    }
 
    while (src1_1 != src1_2) {
        out << *src1_1++ << std::endl;
    }
 
    while (src2_1 != src2_2) {
        out << *src2_1++ << std::endl;
    }
}
 
template <typename Container1, typename Container2>
void MergeSomething(const Container1& src1, const Container2& src2, std::ostream& out) {
    Merge(src1.begin(), src1.end(),  src2.begin(), src2.end(), out);
 
}
 
// Реализуйте эту функцию:
template <typename T>
void MergeHalves(const std::vector<T>& src, std::ostream& out) {
    size_t mid = (src.size() + 1) / 2;
    std::vector<T> left(src.begin(), src.begin() + mid);
    std::vector<T> right(src.begin() + mid, src.end());
    MergeSomething(left, right, out);
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
