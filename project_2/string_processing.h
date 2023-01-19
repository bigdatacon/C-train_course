//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#pragma once
#include <vector>
#include <string>
#include <set>

vector<string> SplitIntoWords(const std::string& text) {
}

template <typename StringContainer>
std::set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<string> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}
