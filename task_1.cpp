#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

// Задание 
/*
Перепишите компаратор из предыдущего урока, применив кортежи.
Код должен вывести следующее:
104 0 0.3 5
102 0 1.2 4
100 0 0.5 4
101 1 0.5 4
103 2 1.2 4 
*/


/*Документы нужно упорядочить сначала по возрастанию статусов, а при их равенстве — по убыванию рейтинга, затем по убыванию релевантности. Но при сравнении кортежей все компоненты сравниваются одинаково. Поэтому при создании кортежа удобно умножить рейтинг и релевантность на −1.*/
/*

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
};


// стало
/*void SortDocuments(vector<Document>& matched_documents) {
    vector<tuple> matched_tuple;
    for (auto el : matched_documents){matched_tuple.push_back(tuple(el.id, el.status, el.relevance*-1, el.rating*-1))};
    
    sort(matched_tuple.begin(), matched_tuple.end(),
         [](const tuple& lhs, const tuple& rhs) {

             return lhs< rhs;
         });
} */


int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    /*SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }
    */
    // проверяю что работает заполнение тюпла 
    vector<tuple<Document>> matched_tuple;
    for (auto el : documents){matched_tuple.push_back(tuple(el.id, el.status, el.relevance*-1, el.rating*-1));};
    
    for (auto el : matched_tuple){cout << el.id << endl;}
    
    
    return 0;
}
