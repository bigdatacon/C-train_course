//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "string_processing.h"
#include <vector>
#include <string>

std::vector<string> SplitIntoWords(const std::string& text) {
    std::vector<string> words;
    std::string word;
    for (const std::char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

