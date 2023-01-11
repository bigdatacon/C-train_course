// разработайте сигнатуру фунции MakeVector по аналогии с функцией MakeSet из урока
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename It>
auto MakeSet(It range_begin, It range_end) {
    return set(range_begin, range_end);
}

template <typename It>
auto MakeVector(It range_begin, It range_end) {
    return vector(range_begin, range_end);
}



template <typename Container, typename Iterator>
void EraseAndPrint(Container& container, Iterator iter_beyond, Iterator p1,  Iterator p2) {

    auto res = container.erase(container.begin() +iter_beyond);
	PrintRange(container.begin(), res);
    PrintRange(res, container.end());
    
    auto res_2 = container.erase(container.begin() +p1, container.begin() +p2);
    PrintRange(container.begin(), res_2);
    PrintRange(res_2, container.end());
}



int main() {
    vector<string> langs = {"Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s};
    EraseAndPrint(langs, 0, 0, 2);
    return 0;
}
