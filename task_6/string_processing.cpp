#include "string_processing.h"
#include <vector>
#include <string>
#include <string_view>

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
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

// SplitIntoWords для Stringview
std::vector<std::string_view> SplitIntoWords(const std::string_view& text) {
    std::vector<std::string_view> words;
    std::string_view word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                //word.clear();
                word= {};
            }
        } else {
            /*букву к string_view, нужно создать новый string_view, состоящий из старого + новая буква. Сделать это можно так: 
```
string_view old_str_view = "Это старый string_view";
char new_char = '!';
string new_str(old_str_view.data(), old_str_view.size() + 1); // создаём новый string более большого размера
new_str.back() = new_char; // добавляем новую букву в конец строки
string_view new_str_view(new_str); // создаём новый string_view на основе нового string
old_str_view = new_str_view; // присваиваем старому string_view значение нового*/
            std::string new_str(word.data(), word.size() + 1);
            new_str.back() = c;
            std::string_view new_str_view(new_str);
            word = new_str_view;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}


//MakeUniqueNonEmptyStrings для string_view
std::set<std::string_view> MakeUniqueNonEmptyStrings(const std::vector<std::string_view>& strings) {
    std::set<std::string_view> non_empty_strings;
    for (const auto& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}