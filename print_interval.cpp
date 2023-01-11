#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <set>
using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end){
    for (auto it = range_begin; it != range_end; it++) {
            cout << *it << " "s;
            
        }
        cout << endl;
    }

/*template<typename Container, typename Type>
void FindAndPrint(Container container, Type range) {
 
	auto it = find_if(container.begin(), container.end(), [&range](const Type& type) {return range == type; });
	PrintRange(container.begin(), it);
 
	if (it != container.end()) {
		PrintRange(it, container.end());
	}
 
}*/

template<typename Container, typename Type>
void FindAndPrint(Container container, Type range) {
 
	auto it = find_if(container.begin(), container.end(), [&range](const Type& type) {return range == type; });
	PrintRange(container.begin(), it);
    PrintRange(it, container.end());
 
}


int main() {
    set<int> test = {1, 1, 1, 2, 3, 4, 5, 5};
    cout << "Test1"s << endl;
    FindAndPrint(test, 3);
    cout << "Test2"s << endl;
    FindAndPrint(test, 0); // элемента 0 нет в контейнере
    cout << "End of tests"s << endl;
}

