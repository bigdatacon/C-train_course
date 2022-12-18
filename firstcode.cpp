#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

/*
Задание
Убедимся, что полученная шаблонная функция может иметь и более сложный параметр-тип. Примените шаблонную функцию ComputeTermFreqs и определите, какое животное встречается наибольшее число раз. Животное в этом задании задаётся парой pair<string, int>, содержащей имя и возраст.
Если максимальное число раз встречаются несколько животных, выведите наименьшего из них. Гарантируется, что вектор содержит хотя бы одно животное.
Попробуйте сделать то же самое со структурой Animal вместо пары. Ничего не получится, если не научите компилятор сравнивать эти структуры. Это нормально.
*/

/*
Вызовите ComputeTermFreqs для вектора animals, проитерируйтесь по результату и найдите ключ с наибольшим значением. Если очередной ключ имеет частоту, равную уже найденной максимальной, не переписывайте им ключ-ответ.
*/


template <typename Term>
map<Term, int> ComputeTermFreqs(const vector<Term>& terms) {
    map<Term, int> term_freqs;
    for (const Term& term : terms) {
        ++term_freqs[term];
    }
    return term_freqs;
}


pair<string, int> FindMaxFreqAnimal(const vector<pair<string, int>>& animals) {
    // верните животного с максимальной частотой
    map<pair<string, int>, int> animal_freqs = ComputeTermFreqs(animals);

    int max_q = 0;
    pair<string, int> min_par;

    for (auto [pair , q] : animal_freqs){/*cout << " pair.first : " <<  pair.first << " pair.second :"s << pair.second << " quantity : " <<  q << endl;*/
    if (q>max_q){max_q = q; min_par = pair;}
    else if ( (q==max_q) && (min_par<pair) ) {continue; }
    else if ((q==max_q) && (min_par>pair) ) {min_par = pair;}
    else continue;
    }
    pair<string, int> max_key = {"jd" , 0};
    //sort(animal_freqs.begin(), animal_freqs.end()); - не работает
    /*sort(animal_freqs.begin(), animal_freqs.end(), [](map<pair<string, int>& a, map<pair<string, int>& b)
    {
        return a.second > b.second;
    }
    );*/ //- Так тоже не работает

    max_key = min_par;
    return max_key;
}




int main() {
    const vector<pair<string, int>> animals = {
        {"Murka"s, 5},  // 5-летняя Мурка
        {"Belka"s, 6},  // 6-летняя Белка
        {"Murka"s, 7},  // 7-летняя Мурка не та же, что 5-летняя!
        {"Murka"s, 5},  // Снова 5-летняя Мурка
        {"Belka"s, 6},  // Снова 6-летняя Белка
    };
    const pair<string, int> max_freq_animal = FindMaxFreqAnimal(animals);
    cout << max_freq_animal.first << " "s << max_freq_animal.second << endl;
}
