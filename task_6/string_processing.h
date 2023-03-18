#pragma once
#include <vector>
#include <string>
#include <set>

std::vector<std::string> SplitIntoWords(const std::string& text) ;
std::vector<std::string_view> SplitIntoWords(const std::string_view& text);

/*std::set<std::string_view> MakeUniqueNonEmptyStrings(const std::vector<std::string_view>& strings);*/

template <typename StringContainer>
std::set<std::string, std::less> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string, std::less> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}
