#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

int EffectiveCount(const vector<int>& v, int n, int i) {
    // место для вашего решения
}

int EffectiveCount(const vector<int>& v, int n, int i) {
    // место для вашего решения
    //1. нахожу номер позиции числа большего i
    auto pos_if = find_if( v.begin(), v.end(),  [i](int el) {return i < el;}) - v.begin();
    //auto pos = find( v.begin(), v.end(),  i);
    //cout << pos<< end;
    
    //2 определяю какой алгоритм выбрать  - для худшего или хорошего случая 
    auto good  = (pos_if <= log2(v.size())) || (pos_if <= static_cast<int64_t>(v.size())*(i + 1)/(n + 1)) ;
    if (good) {
        
    cout << "Using find_if"; return pos_if;}
    else {
        cout << "Using upper_bound"; return upper_bound( v.begin(), v.end(),  i) - v.begin() << endl;} 
    ///return *pos;     
}



int main() {
    static const int NUMBERS = 1'000'000;
    static const int MAX = 1'000'000'000;

    mt19937 r;
    uniform_int_distribution<int> uniform_dist(0, MAX);

    vector<int> nums;
    for (int i = 0; i < NUMBERS; ++i) {
        int random_number = uniform_dist(r);
        nums.push_back(random_number);
    }
    sort(nums.begin(), nums.end());

    int i;
    cin >> i;
    int result = EffectiveCount(nums, MAX, i);
    cout << "Total numbers before "s << i << ": "s << result << endl;
}
