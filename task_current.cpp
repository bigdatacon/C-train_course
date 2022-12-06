#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;


int CountAndAddNewDogs(const vector<string>& new_dogs, const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    return count_if(new_dogs.begin(), new_dogs.end(), [&shelter, max_amount](string breed_new_dog){
        if (shelter[ breed_new_dog] < max_amount[breed_new_dog]){return true;}
        else {return false;}
                });                                                                       
}




int main() {
    map<string, int> shelter = {{"landseer"s, 1}, {"otterhound"s, 2}, {"pekingese"s, 2}, {"pointer"s, 3}};
    map<string, int> max_amount = {{"landseer"s, 2}, {"otterhound"s, 3}, {"pekingese"s, 4}, {"pointer"s, 7}};
    vector<string> new_dogs = {"landseer"s, "otterhound"s, "otterhound"s, "otterhound"s, "pointer"s};
    
    /*for (auto [breed, q] : shelter){cout << "breed " << breed << " q " << q << endl;}
    int dog_arrival= 0;
    for (auto breed : new_dogs){
        [&shelter, max_amount](string breed){
            if (shelter[breed]!=0 && shelter[breed]<max_amount[breed] ){++shelter[breed]; ++dog_arrival;}
        }
        
    }*/
    

    сout << "NEW " <<  CountAndAddNewDogs(new_dogs, max_amount, shelter ) << endl;
    return 0;
}
