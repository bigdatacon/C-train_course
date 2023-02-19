// Вставьте сюда своё решение задачи о разделении вектора лексем на предложения.
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using namespace std;

/*
Задание 2
В прошлом уроке вы разделили вектор лексем на предложения. Теперь перепишите своё решение, используя make_move_iterator. В тестах будет проверяться наличие соответствующей функции в коде.
*/
/*Используйте метод push_back для итогового вектора с итераторами начала и конца диапазона в качестве аргументов.*/


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


template <typename Token> // Просто через ссылку
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> res;
    Sentence<Token> sent_res;
    bool was_end = false;
    for (Token &tok: tokens){
    	string s = tok.data;
        bool end = tok.IsEndSentencePunctuation();
        if (was_end && !end) {
            res.push_back(std::move(sent_res));
        }
        sent_res.push_back(std::move(tok));
        was_end = end;
    }
    if (!sent_res.empty()) {
        res.push_back(std::move(sent_res));
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
