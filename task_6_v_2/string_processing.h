#pragma once
#include <vector>
#include <string>
#include <set>

std::vector<std::string_view> SplitIntoWords(const std::string_view& text);

template <typename StringContainer>
std::set<std::string, std::less<>> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string, std::less<>> non_empty_strings;
    for (const  auto& str : strings) {
        if ( !str.size()==0) {
            //std::string my_str = std::string{str.data(), str.size()};
            std::string my_str(str);
            non_empty_strings.emplace(my_str);
        }
    }
    return non_empty_strings;
}
