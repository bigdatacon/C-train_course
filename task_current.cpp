#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int CountAndAddNewDogs(const vector<string>& new_dogs, const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    return count_if(new_dogs.begin(), new_dogs.end(), [&shelter, &max_amount](string breed_new_dog){
        if (shelter.at(breed_new_dog) < max_amount.at(breed_new_dog)){++shelter[breed_new_dog]; return true;}
        else {return false;}
                });                                                                       
}




int main() {
        //map<string, int> shelter = {{"landseer"s, 1}, {"otterhound"s, 2}, {"pekingese"s, 2}, {"pointer"s, 3}};
    //map<string, int> shelter = {{"landseer"s, 1}, {"otterhound"s, 2}, {"pekingese"s, 2}, {"pointer"s, 3}};
    map<string, int> max_amount = {{"landseer"s, 2}, {"otterhound"s, 3}, {"pekingese"s, 4}, {"pointer"s, 7}};
    //vector<string> new_dogs = {"landseer"s, "otterhound"s, "otterhound"s, "otterhound"s, "pointer"s};
    vector<string> new_dogs = { "pointer"s, "pointer"s, "pointer"s, "pointer"s, "otterhound"s, "landseer"s, "pekingese"s, "pekingese"s}; // проверка что все собаки помещяются 
        
        map<string, int> shelter = {{"landseer"s, 2}, {"otterhound"s, 2}, {"pekingese"s, 2}, {"pointer"s, 3}};
        //vector<string> new_dogs = { "pointer"s,  "landseer"s, "otterhound"s}; // проверка что 1 собака помещяется
        //vector<string> new_dogs = { "pointer"s,  "landseer"s}; // проверка что 0 собак помещяется
    
    for (auto [breed, q] : shelter){cout << "breed " << breed << " q " << q << endl;}
    int dog_arrival= 0;
    for (auto breed : new_dogs){
        [&shelter, max_amount, &dog_arrival](string breed){
            if ( shelter.at(breed)<max_amount.at(breed) ){++shelter[breed]; ++dog_arrival;}
        };    
    }
    
    for (auto [breed, q] : shelter){cout << "breednew " << breed << " q " << q << endl;}
     cout << "NEW " << dog_arrival << endl;
    cout << "NEW " <<  CountAndAddNewDogs(new_dogs, max_amount, shelter ) << endl;
    return 0;

}
