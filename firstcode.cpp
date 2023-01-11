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
void EraseAndPrint(Container& container, Iterator iter_beyond) {
	if (iter_beyond>= container.begin() && iter_beyond <= container.end()){
    container.erase(iter_beyond);
	PrintRange(container.begin(), iter_beyond);
    PrintRange(iter_beyond, container.end());
}
}

int main() {
    vector<string> langs = {"Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s};
    EraseAndPrint(langs, langs.begin());
    

    
    return 0;
}
