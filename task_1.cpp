
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
    // находит нужный документ
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        assert(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        assert(doc0.id == doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        assert(server.FindTopDocuments("in"s).empty());
    }
}

//1. Добавление документов. Добавленный документ должен находиться по поисковому запросу, который содержит слова из документа.
void TestAddDocument() {
    SearchServer server;
    server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    vector<Document> document = server.FindTopDocuments("белый кот и модный ошейник"s);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].id == 0);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].relevance == 0);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].rating == 2);
}

//2. Поддержка стоп-слов. Стоп-слова исключаются из текста документов
void TestExcludeStopWord() {
    SearchServer server;
    server.SetStopWords("и в на"s);
    server.AddDocument(0, "белый кот и в на модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    vector<Document> document = server.FindTopDocuments("и в на"s);
    assert(document.size() == 0);
}

//3. Поддержка минус-слов. Документы, содержащие минус-слова поискового запроса, не должны включаться в результаты поиска.
void TestMinusWord() {
    SearchServer server;
    //server.SetStopWords("и в на"s);
    server.AddDocument(0, "белый кот модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "черный пес бульдог"s,        DocumentStatus::ACTUAL, {8, -7});
    tuple<vector<string>, DocumentStatus> match_data =  server.MatchDocument("белый кот модный -ошейник"s, 0);
    tuple<vector<string>, DocumentStatus> match_data_2 =  server.MatchDocument("черный пес -бульдог"s, 1);
    int size_vec_1 = get<0>(match_data).size();
    int size_vec_2 = get<0>(match_data_2).size();

    assert(size_vec_1 == 0);  // проверяю что с минус словом ничего не попало
    assert(size_vec_2 == 0);  // проверяю что с минус словом ничего не попало
}

// 4. Матчинг документов. При матчинге документа по поисковому запросу должны быть возвращены все слова из поискового запроса,
//присутствующие в документе. Если есть соответствие хотя бы по одному минус-слову, должен возвращаться пустой список слов.
void TestMatchDoc() {
    SearchServer server;
    //server.SetStopWords("и в на"s);
    server.AddDocument(0, "белый кот модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "черный пес бульдог"s,        DocumentStatus::ACTUAL, {8, -7});
    tuple<vector<string>, DocumentStatus> match_data =  server.MatchDocument("белый кот модный ошейник"s, 0);

    tuple<vector<string>, DocumentStatus> match_data_2 =  server.MatchDocument("черный пес -бульдог"s, 1);
    int size = tuple_size<decltype(match_data)>::value;
    int size_vec = get<0>(match_data).size();
    string result_string = ""s;
    for (auto el : get<0>(match_data)){cout << el<< " "s ;  result_string+=el; result_string+=" "s;}
    int size_vec_2 = get<0>(match_data_2).size();

    assert(size_vec_2 == 0);  // проверяю что с минус словом ничего не попало


}

// 5. Сортировка найденных документов по релевантности. Возвращаемые при поиске документов результаты должны быть отсортированы в порядке убывания релевантности.

void TestSortDocument() {
    SearchServer server;
    server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, -7});
    vector<Document> document = server.FindTopDocuments("белый кот и модный ошейник"s);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].relevance > server.FindTopDocuments("белый кот и модный ошейник"s)[1].relevance);

}

//6. Вычисление рейтинга документов. Рейтинг добавленного документа равен среднему арифметическому оценок документа.
void TestRatingDocument() {
    SearchServer server;
    server.AddDocument(1, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, 3});
    vector<Document> document = server.FindTopDocuments("белый пес модный"s);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].rating == 5);
}

//7. Фильтрация результатов поиска с использованием предиката, задаваемого пользователем.
void TestFiltrocument() {
    SearchServer server;
    server.AddDocument(0, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, 3});
    server.AddDocument(1, "белый пес старомодный"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(2, "полосатый пес в крапинку со шнурками на ботинках"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(3, "черный пес с пятном белым"s,        DocumentStatus::IRRELEVANT, {8, 4});
    server.AddDocument(4, "желтый кот с серым ошейником"s,        DocumentStatus::BANNED, {8, 8});
    vector<Document> document = server.FindTopDocuments("пушистый ухоженный пес"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; });

    assert(document.size() == 2);
}

//8 Поиск документов, имеющих заданный статус.
void TestFiltrSTATDocument() {
    SearchServer server;
    server.AddDocument(0, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, 3});
    server.AddDocument(1, "белый пес старомодный"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(2, "полосатый пес в крапинку со шнурками на ботинках"s,        DocumentStatus::ACTUAL, {8, 14});
    server.AddDocument(3, "черный пес с пятном белым"s,        DocumentStatus::IRRELEVANT, {8, 4});
    server.AddDocument(4, "желтый кот с серым ошейником"s,        DocumentStatus::BANNED, {8, 8});
    vector<Document> document = server.FindTopDocuments("пушистый ухоженный пес"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::IRRELEVANT; });
    vector<Document> document_2 = server.FindTopDocuments("пушистый ухоженный пес"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; });   // ничего не находит
    assert(document.size() == 1 && document_2.size()==3);
}


//9 Корректное вычисление релевантности найденных документов.
// Внимание релевантность как то не так считатет????
void TestRelevanceDocument() {
    SearchServer server;
    server.AddDocument(0, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, 3});
    server.AddDocument(1, "белый пес старомодный"s,        DocumentStatus::ACTUAL, {8, 2});

    vector<Document> document = server.FindTopDocuments("белый пес модный"s);
    assert( (server.FindTopDocuments("белый пес модный"s)[0].relevance - 0.231049 ) < 0.00001 );
}




/*
Разместите код остальных тестов здесь
*/

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocument();
    TestExcludeStopWord();
    TestMinusWord();
    TestMatchDoc();
    TestSortDocument();
    TestRatingDocument();
    TestFiltrSTATDocument();

    TestFiltrocument();
    TestRelevanceDocument();

    // Не забудьте вызывать остальные тесты здесь
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}
