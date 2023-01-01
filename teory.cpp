#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

using namespace std;

string getKeyAtIndex(int index, map<string, int> myMap) {
    map<string, int>::const_iterator end = myMap.end();

    int counter = 0;
    for (map<string, int>::const_iterator it = myMap.begin(); it != end; ++it) {
        counter++;

        if (counter == index)
            return it->first;
    }
}


int main() {
    std::map<string, int> myMap;
    myMap["banana"] = 1;
    myMap["apple"] = 1;
    myMap["orange"] = 1;


    string k = getKeyAtIndex(1, myMap);
    cout << "HERE K : "s << k << endl;


}


