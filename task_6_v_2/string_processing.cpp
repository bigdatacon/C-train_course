#include "string_processing.h"
#include <vector>
#include <string>
#include <string_view>


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

