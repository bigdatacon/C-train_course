#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using namespace std;

// Объявляем Sentence<Token> для произвольного типа Token
// синонимом vector<Token>.
// Благодаря этому в качестве возвращаемого значения
// функции можно указать не малопонятный вектор векторов,
// а вектор предложений — vector<Sentence<Token>>.
template <typename Token>
using Sentence = vector<Token>;

template <typename TokenForwardIt>
TokenForwardIt FindSentenceEnd(TokenForwardIt tokens_begin, TokenForwardIt tokens_end) {
    const TokenForwardIt before_sentence_end
        = adjacent_find(tokens_begin, tokens_end, [](const auto& left_token, const auto& right_token) {
              return left_token.IsEndSentencePunctuation() && !right_token.IsEndSentencePunctuation();
          });
    return before_sentence_end == tokens_end ? tokens_end : next(before_sentence_end);
}

// Класс Token имеет метод bool IsEndSentencePunctuation() const
// Через объект по значению вообще не компилирует https://disk.yandex.ru/i/nSnng2aANSzW4A
/*vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> res;
    Sentence<Token> sent_res;
    for (Token tok: tokens){
        if (!tok.IsEndSentencePunctuation()){
            sent_res.push_back(std::move(tok));
        }
        else {
            sent_res.push_back(std::move(tok));
            res.push_back(sent_res);
            sent_res.clear(); 
        }
    }
    return res;
}*/





// Через rvalues ссылку : так вообще не компилруется https://disk.yandex.ru/i/_KlMbAtizDUIaQ
/*vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> res;
    Sentence<Token> sent_res;
    for (const Token&& tok: tokens){
        if (!tok.IsEndSentencePunctuation()){
            sent_res.push_back(std::move(tok));
        }
        else {
            sent_res.push_back(std::move(tok));
            res.push_back(sent_res);
            sent_res.clear(); 
        }
    }
    return res;
}*/





// Через константную ссылку на тестах такая ошибка: https://disk.yandex.ru/i/ZR5RSjAPkqevRw
template <typename Token>
/*vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> res;
    Sentence<Token> sent_res;
    for (const Token& tok: tokens){
        if (!tok.IsEndSentencePunctuation()){
            sent_res.push_back(tok);
        }
        else {
            sent_res.push_back(tok);
            res.push_back(sent_res);
            sent_res.clear(); 
        }
    }
    return res;
}*/


template <typename Token> // Просто через ссылку
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> res;
    Sentence<Token> sent_res;
    for (Token &tok: tokens){
        bool end = false;
        if (tok.IsEndSentencePunctuation()){
            end = true;
        }
        sent_res.push_back(std::move(tok));
        if (end) {
            res.push_back(std::move(sent_res));
        }
    }
    return res;
}

struct TestToken {
    string data;
    bool is_end_sentence_punctuation = false;

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }
    bool operator==(const TestToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }
};

ostream& operator<<(ostream& stream, const TestToken& token) {
    return stream << token.data;
}

// Тест содержит копирования объектов класса TestToken.
// Для проверки отсутствия копирований в функции SplitIntoSentences
// необходимо написать отдельный тест.
void TestSplitting() {
    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}}));

    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}}));

    assert(SplitIntoSentences(vector<TestToken>(
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}, {"Without"s}, {"copies"s}, {"."s, true}}))
           == vector<Sentence<TestToken>>({
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}},
               {{"Without"s}, {"copies"s}, {"."s, true}},
           }));
}

int main() {
    TestSplitting();
}
