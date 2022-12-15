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
    auto MakeKey() {
        return tuple(status, -rating, -relevance);
    }
};



void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs)  {
               // Declaring tuple
               //tuple < Document> geek;   -- так не работает 
               //tuple <int, Status, double, int> geek;
                 //tuple <int, Status, double, int> week;
             
                /* tuple < Status,  int ,double, int> geek;
                 tuple < Status,  int ,double, int> week;
             
               geek = make_tuple( lhs.status, lhs.rating*-1, lhs.relevance*-1, lhs.id);
               week = make_tuple(rhs.status, rhs.rating*-1, rhs.relevance*-1, rhs.id);*/
                 
            return  lhs.MakeKey() < rhs.MakeKey();
             
            /* if ((std::get<1>(geek) == std::get<1>(week))){
             
            return tuple(get<3>(geek), get<2>(geek) ) < tuple(get<3>(week), get<2>(week));
             }
             
             if ((std::get<1>(geek) < std::get<1>(week))){return true;}
             return false;*/
         });

}

t1  2753.74
t2  1949.71
t3   3480.47
t123 8183.92

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
