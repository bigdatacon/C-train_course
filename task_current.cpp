#include <algorithm>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

/*
Перепишите компаратор из предыдущего урока, применив кортежи.
Код должен вывести следующее:
104 0 0.3 5
102 0 1.2 4
100 0 0.5 4
101 1 0.5 4
103 2 1.2 4

Документы нужно упорядочить сначала по возрастанию статусов, а при их равенстве — по убыванию рейтинга, затем по убыванию релевантности. Но при сравнении кортежей все компоненты сравниваются одинаково. Поэтому при создании кортежа удобно умножить рейтинг и релевантность на −1.
*/

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
};

// было 

/*void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
               // Declaring tuple
               //tuple < Document> geek;   -- так не работает 
               tuple <int, Status, double, int> geek;
               geek = make_tuple(lhs.id, lhs.status, lhs.relevance, lhs.rating);
               cout << "tuple "s << std::get<2>(geek) << endl; // так ошибка, geek.id - тоже ошибка
                 
             
             if (lhs.status==rhs.status){
             
             return pair(lhs.rating, lhs.relevance) > pair(rhs.rating, rhs.relevance);}
             
             if (lhs.status<rhs.status){return true;}
             return false;
         });

}*/

void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
               // Declaring tuple
               //tuple < Document> geek;   -- так не работает 
               tuple <int, Status, double, int> geek;
                 tuple <int, Status, double, int> week;
               geek = make_tuple(lhs.id, lhs.status, lhs.relevance, lhs.rating);
               week = make_tuple(rhs.id, rhs.status, rhs.relevance, rhs.rating);
                 
               //cout << "tuple "s << std::get<2>(geek) << endl; // так работает 
               cout << "tuple "s << std::get<2>(geek) == std::get<2>(week) << endl; // так ошибка 
             
             if (lhs.status==rhs.status){
             
             return pair(lhs.rating, lhs.relevance) > pair(rhs.rating, rhs.relevance);}
             
             if (lhs.status<rhs.status){return true;}
             return false;
         });

}




// стало
/*void SortDocuments(vector<Document>& matched_documents) {
  
    //1 сортирую по возрастению статусов 
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
                //return std::get<1>(a) > std::get<1>(b);
             return std::get<1>(tuple(lhs.id, lhs.status, lhs.relevance*-1, lhs.rating*-1)) <std::get<1>(tuple(rhs.id, rhs.status, rhs.relevance*-1, rhs.rating*-1));
         });
    
    //2 сортирую по убыванию рейтинга
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
                //return std::get<1>(a) > std::get<1>(b);
             return std::get<3>(tuple(lhs.id, lhs.status, lhs.relevance*-1, lhs.rating*-1)) > std::get<3>(tuple(rhs.id, rhs.status, rhs.relevance*-1, rhs.rating*-1));
         });
    
        //3 сортирую по убыванию релевантности
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
                //return std::get<1>(a) > std::get<1>(b);
             return std::get<2>(tuple(lhs.id, lhs.status, lhs.relevance*-1, lhs.rating*-1)) > std::get<2>(tuple(rhs.id, rhs.status, rhs.relevance*-1, rhs.rating*-1));
         });
    
} */


int main() {
    vector<Document> documents = {
        {100, Status::ACTUAL, 0.5, 4}, {101, Status::EXPIRED, 0.5, 4},
        {102, Status::ACTUAL, 1.2, 4}, {103, Status::DELETED, 1.2, 4},
        {104, Status::ACTUAL, 0.3, 5},
    };
    SortDocuments(documents);
    for (const Document& document : documents) {
        cout << document.id << ' ' << static_cast<int>(document.status) << ' ' << document.relevance
             << ' ' << document.rating << endl;
    }

    return 0;
}
