#include "common.h"
 
#include <algorithm>
#include <cctype>
#include <sstream>
 
const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;
 
const Position Position::NONE = {-1, -1};
 
bool Position::operator==(const Position rhs) const {return (row == rhs.row) && (col == rhs.col);}
bool Position::operator<(const Position rhs) const {return std::make_tuple(row, col) < std::make_tuple(rhs.row, rhs.col);}
bool Position::IsValid() const {return (row >= 0 && col >= 0) && (row < MAX_ROWS && col < MAX_COLS);}
 
std::string Position::ToString() const {
    
    if (IsValid()) {
        
        std::string converted_string;
        
        int columns = col;
        while (columns >= 0) {
            converted_string.insert(converted_string.begin(), 'A' + columns % LETTERS);
            columns = columns / LETTERS - 1;
        }
 
        converted_string += std::to_string(row + 1);
        return converted_string;
        
    } else {
        return "";
    }  
}
 
Position Position::FromString(std::string_view str) {
    
    auto it = std::find_if(str.begin(), str.end(), [](const char c) {
        return !(std::isalpha(c) && std::isupper(c));
    });
    
    auto letters = str.substr(0, it - str.begin());
    auto digits = str.substr(it - str.begin());

 
    if ((letters.empty() || digits.empty()) 
        || (letters.size() > MAX_POS_LETTER_COUNT) 
        || (std::isdigit(digits[0]) == 0)) {
        
        return Position::NONE;
        
    }
    
    int row = 0;
    std::istringstream row_in{std::string{digits}};
        
    if (!(row_in >> row) || !row_in.eof()) {
        return Position::NONE;
    }
 
    int col = 0;
    for (char ch : letters) {
        col *= LETTERS;
        col += ch - 'A' + 1;
    }
 
    return {row - 1, col - 1};
}