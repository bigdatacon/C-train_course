
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

/*Задание 2
Введём статусы документов: ACTUAL (актуальные), EXPIRED (просроченные) и DELETED (удалённые). Исправьте компаратор так, чтобы после сортировки порядок документов был следующим:
актуальные,
просроченные,
удалённые.
Внутри каждого статуса документы должны быть упорядочены по убыванию рейтинга, а затем по убыванию релевантности.
Код из тренажёра должен вывести следующее:
104 0 0.3 5
102 0 1.2 4
100 0 0.5 4
101 1 0.5 4
103 2 1.2 4 */

/*Подставить все три поля в пару, к сожалению, не выйдет. Но можно написать логическое выражение ещё большей вложенности. А можно скомбинировать два подхода: самостоятельно сравнить статусы и, если они равны, — сравнить пары двух оставшихся характеристик.*/

enum class Status { ACTUAL, EXPIRED, DELETED };

struct Document {
    int id;
    Status status;
    double relevance;
    int rating;
};

// было 
/*
void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {
             if (lhs.status==rhs.status){
             
             return pair(lhs.rating, lhs.relevance) > pair(rhs.rating, rhs.relevance);}
             
             if (lhs.status<rhs.status){return true;}
             return false;
         });
} 
}*/

// стало
void SortDocuments(vector<Document>& matched_documents) {
    sort(matched_documents.begin(), matched_documents.end(),
         [](const Document& lhs, const Document& rhs) {

             return tuple(lhs.id, lhs.status, lhs.relevance*-1, lhs.rating*-1)
                 < tuple(rhs.id, rhs.status, rhs.relevance*-1, rhs.rating*-1);
         });
} 


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
