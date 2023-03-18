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
/*std::set<std::string_view> MakeUniqueNonEmptyStrings(const std::vector<std::string_view>& strings) {
    std::set<std::string_view> non_empty_strings;
    for (const auto& str : strings) {
        if (!str.size()==0) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}*/
