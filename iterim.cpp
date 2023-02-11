#include <cassert>
#include <iostream>
#include <string_view>
#include <vector>
#include <algorithm>


using namespace std;


//Вернитесь к описанию алгоритма или откройте документацию для методов find и substr. Пока эти страницы есть только на английском.
// новая реализация 
vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    //1.Удалите начало из str до первого непробельного символа, воспользовавшись методом remove_prefix. Он уберёт из string_view указанное количество символов.
    str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
    //std::cout << "View  : '" << v << "'\n";
    
    const int64_t pos_end = str.npos;
    //Цикл следует продолжать, пока строка str непуста. Можно найти инвариант: в начале итерации цикла str никогда не начинается с пробела.
    while (!str.empty()) {
        //2.В цикле используйте метод find с одним параметром, чтобы найти номер позиции первого пробела.
        int64_t space = str.find(' '); // поскольку возможный пробел в начале уже обрезан, просто нахожу позицию первого пробела 
        //3.Добавьте в результирующий вектор элемент string_view, полученный вызовом метода substr, где начальная позиция будет 0, а конечная — найденная позиция пробела или npos.
        result.push_back(space == pos_end ? str.substr(0, pos_end) : str.substr(0, space ));
        
        if (space == pos_end) {result.push_back(str.substr(0, pos_end));}
        else {result.push_back(str.substr(0, space));
                     //4.Сдвиньте начало str так, чтобы оно указывало на позицию за пробелом. Это можно сделать методом remove_prefix, передвигая начало str на указанное в аргументе количество позиций.
        str.remove_prefix(std::min(str.find_first_not_of(" "), str.size())); // поскольку найдено слово до пробела , просто удаляю пробел . если пробелов несколько
              // до удалятся все пробелы до первого не пробельного символа или до конца строки 
             }
    }
    return result;
}


// прежняя реализация 
/*
vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    int64_t pos = str.find_first_not_of(" ");
    const int64_t pos_end = str.npos;
    
    while (pos != pos_end) {
        int64_t space = str.find(' ', pos);
        result.push_back(space == pos_end ? str.substr(pos) : str.substr(pos, space - pos));
        pos = str.find_first_not_of(" ", space);
    }
    return result;
}
*/

int main() {
    assert((SplitIntoWordsView("") == vector<string_view>{}));
    assert((SplitIntoWordsView("     ") == vector<string_view>{}));
    assert((SplitIntoWordsView("aaaaaaa") == vector{"aaaaaaa"sv}));
    assert((SplitIntoWordsView("a") == vector{"a"sv}));
    assert((SplitIntoWordsView("a b c") == vector{"a"sv, "b"sv, "c"sv}));
    assert((SplitIntoWordsView("a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    cout << "All OK" << endl;
}
