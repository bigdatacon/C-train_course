#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

// Это разбор решения от наставников  - до 20 строки
int CountAndAddNewDogs(const vector<string>& new_dogs,
                       const map<string, int>& max_amount,
                       map<string, int>& shelter) {
		return count_if(собаки из new_dogs, [...](порода) {
						eсли лимит породы > текущее количество породы в приюте то {
								добавить в приют
	              return true; // увеличиваем счётчик
						} или {
								return false; // не увеличиваем счётчик
						}		
				};
}


int CountAndAddNewDogs(const vector<string>& new_dogs, const map<string, int>& max_amount,
                       map<string, int>& shelter) {
    return count_if(new_dogs.begin(), new_dogs.end(), [&shelter, &max_amount](string breed_new_dog){
        if (shelter.at(breed_new_dog) < max_amount.at(breed_new_dog)){return true;}
        else {return false;}
                });                                                                       
}




int main() {
    map<string, int> shelter = {{"landseer"s, 1}, {"otterhound"s, 2}, {"pekingese"s, 2}, {"pointer"s, 3}};
    map<string, int> max_amount = {{"landseer"s, 2}, {"otterhound"s, 3}, {"pekingese"s, 4}, {"pointer"s, 7}};
    vector<string> new_dogs = {"landseer"s, "otterhound"s, "otterhound"s, "otterhound"s, "pointer"s};
    
    for (auto [breed, q] : shelter){cout << "breed " << breed << " q " << q << endl;}
    int dog_arrival= 0;
    for (auto breed : new_dogs){
        [&shelter, max_amount, &dog_arrival](string breed){
            if (shelter.at(breed)!=0 && shelter.at(breed)<max_amount.at(breed) ){++shelter.at(breed); ++dog_arrival;}
        };    
    }
    
    for (auto [breed, q] : shelter){cout << "breednew " << breed << " q " << q << endl;}
     cout << "NEW " << dog_arrival << endl;
    //cout << "NEW " <<  CountAndAddNewDogs(new_dogs, max_amount, shelter ) << endl;
    return 0;
}
