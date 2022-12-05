#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <clocale>

#include <cstdlib>
#include <ctime>


using namespace std;

/*
Прочитайте слова в вектор, а потом отсортируйте их алгоритмом sort. По умолчанию sort сравнивает элементы операцией <, которая для строк учитывает регистр символов. Чтобы сравнивать слова без учёта регистра символов, передайте алгоритму sort компаратор.
В этом компараторе сравнивайте строки алгоритмом lexicographical_compare. Чтобы lexicographical_compare игнорировал регистр символов, передайте этому алгоритму компаратор, который перед сравнением приводит символы к нижнему регистру функцией tolower.
*/
string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}
vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += tolower(c);
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
 
    return words;
}

/*bool HasDocumentGreaterRelevance(const Document& lhs, const Document& rhs) {
    return lhs.relevance > rhs.relevance;
}*/

bool lexicographical_compare(string first1, 
                             string first2)
    {
        if (first1 < first2) return true;
        else  return false;
    }




int main() {
    // считайте входные данные и сформируйте вывод программы
    const string string_query = ReadLine();
    //transform(string_query.begin(), string_query.end(), string_query.begin(), tolower)   // Не работает 
    vector<string> words = SplitIntoWords(string_query );
    sort (words.begin() , words.end());
    for (auto el : words){cout << el << endl;}
    
    
    sort (words.begin() , words.end(), lexicographical_compare); // не работает 
    
    for (auto el : words){cout << el << endl;}

    

}
