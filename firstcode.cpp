#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <iterator>

using namespace std;

int main() {
    std::vector<int> first = { 7, 5, 16, 8 };
    std::vector<int> sec = { 8, 6, 17, 9,10,11,12 };
    std::vector<int> for_coopy{1};

    // Проверяю копирование
    std::copy(first.begin(), first.end() , std::back_inserter(for_coopy)  /*for_coopy.begin()*/ );
    for (auto el : for_coopy) { cout << " " << el << ' '; }

    auto pos = sec.begin() + 5;
    std::copy_backward(first.begin(), first.end(), pos);

    std::cout << "destination contains: ";
    for (auto i : sec)
        std::cout << i << ' ';
    std::cout << '\n';


    sec.resize(sec.size() + 1);
    std::cout << "destination 2  contains: ";
    for (auto i : sec)
        std::cout << i << ' ';
    std::cout << '\n';


    // Пробую скопировать полный фрагмент 
    auto post = sec.begin() + 5;
    std::copy_backward(post-2, post+3, sec.end());

    std::cout << "destination 3  contains: ";
    for (auto i : sec)
        std::cout << i << ' ';
    std::cout << '\n';



}
