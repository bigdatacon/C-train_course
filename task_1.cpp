#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

/*
Вызовите метод size множества stop_words_ в методе GetStopWordsSize класса SearchServer.
Вызовите метод GetStopWordsSize сервера в функции GetStopWordsSize.
*/


class SearchServer {
    // Содержимое раздела public: доступно для вызова из кода вне класса
public:
    int GetStopWordsSize() const {
        // Верните количество стоп-слов
        return stop_words_.size();
    }

    // Содержимое раздела private: доступно только внутри методов самого класса
private:
    struct DocumentContent {
        int id = 0;
        vector<string> words;
    };

    DocumentContent documents_;
    set<string> stop_words_;
};

int GetStopWordsSize(const SearchServer& server) {
    // Верните количество стоп-слов у server
    return GetStopWordsSize();
}

int main() {
    // 1. Создайте переменную server типа SearchServer
    SearchServer server;
    // 2. Вызовите функцию GetStopWordsSize, передав ей объект server
    GetStopWordsSize(server);
    // 3. Выведите результат, возвращёный функцией GetStopWordsSize в cout
    cout << GetStopWordsSize(server) << endl;
}
