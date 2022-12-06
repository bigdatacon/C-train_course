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
    return 0;
}
