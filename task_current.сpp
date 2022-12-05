#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

/*Примените count_if и захват по ссылке в лямбда-выражении.*/
/*int CountShortQueryWords(const vector<string>& query,
    const set<string>& minus_words, int max_length)
{
    return count_if(query.begin(), query.end(),
        [max_length, &minus_words](const string& w) {
            return w.size() <= max_length && minus_words.count(w) == 0;
        });
}


int CountAndAddNewDogs(const vector<string>& new_dogs, const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    //...
}*/

int main() {
    map<string, int> shelter = {{"landseer"s, 1}, {"otterhound"s, 2}, {"pekingese"s, 2}, {"pointer"s, 3}};
    map<string, int> max_amount = {{"landseer"s, 2}, {"otterhound"s, 3}, {"pekingese"s, 4}, {"pointer"s, 7}};
    vector<string> new_dogs = {"landseer"s, "otterhound"s, "otterhound"s, "otterhound"s, "pointer"s};
    
    for (auto [breed, q] : shelter){cout << "breed " << breed << " q " << q << endl;}
    int dog_arrival= 0;
    for (auto breed : new_dogs){
        [&shelter, max_amount](string breed){
            if (shelter[breed]!=0 && shelter[breed]<max_amount[breed] ){++shelter[breed]; ++dog_arrival;}
        }
        
        
        
    
    }
    
    cout << "breedafter " << breed << " q " << q << endl;
    return 0;
}
