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
            //word += c;
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

string ToLowerString(string s) {
    string new_string; 
    for (const char c : s) {new_string+= tolower(c);}
    return new_string;
}



bool lexicographical_compare(string l, 
                             string r)
        {return lexicographical_compare( l.begin(), l.end(), r.begin(), r.end() );}

int main() {
    // считайте входные данные и сформируйте вывод программы
    const string string_query = ReadLine();
    //transform(string_query.begin(), string_query.end(), string_query.begin(), tolower)   // Не работает 
    vector<string> words = SplitIntoWords(string_query );
    sort (words.begin() , words.end());
    for (auto el : words){cout << el << endl;}
    
    sort (words.begin() , words.end(), ToLowerString); // не работает 
    //sort (words.begin() , words.end(), lexicographical_compare); // не работает 
    
    /*sort(words.begin() , words.end(), [](const string& l, const string& r){
        return lexicographical_compare(l.begin(), l.end(), r.begin(), r.end(), [](char ch){return tolower(ch);}
                                      );
    }
        );
    */
    sort(words.begin() , words.end(), lexicographical_compare);

    
    for (auto el : words){cout << el << endl;}

    

}
