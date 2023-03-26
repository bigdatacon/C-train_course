#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "concurrent_map.h" // заголовочный файл для реализации ConcurrentMap

ConcurrentMap<std::string, int> word_count; // создание ConcurrentMap для подсчета количества слов

void add_word(const std::string& word) {
    word_count[std::move(word)]++; // добавление слова в ConcurrentMap
}

void process_data() {
    // создание вектора, содержащего все слова в ConcurrentMap
    std::vector<std::pair<std::string, int>> words(word_count.begin(), word_count.end());

    // сортировка вектора по количеству вхождений слов
    std::sort(words.begin(), words.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });

    // вывод первых 10 слов с наибольшим количеством вхождений
    for (int i = 0; i < 10 && i < words.size(); ++i) {
        std::cout << words[i].first << ": " << words[i].second << std::endl;
    }
}

int main() {
    // добавление слов в ConcurrentMap
    add_word("apple");
    add_word("orange");
    add_word("banana");
    add_word("orange");
    add_word("orange");
    add_word("apple");

    // обработка данных
    process_data();

    return 0;
}
