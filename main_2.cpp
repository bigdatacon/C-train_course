#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using namespace std;

/*
ПОдсказка
Среди стандартных алгоритмов есть удобная функция adjacent_find. Она позволяет искать последовательность из двух элементов, удовлетворяющих условию, которое задано пользователем. Вы можете применить эту функцию, чтобы определить конец предложения.
Найдите конец предложения, потом пройдите по лексемам от начала до конца и переместите значения лексем в предложение. Само предложение после этого можно переместить в вектор результатов.

Задание
Дан вектор лексем. Напишите функцию, разбивающую лексемы на предложения:
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens); 
Token — шаблонный тип. О нём известно лишь, что он имеет константный метод IsEndSentencePunctuation. Этот метод возвращает true, если лексема — знак пунктуации, заканчивающий предложение. В противном случае он возвращает false. Объекты такого типа могут быть копируемыми, а могут и не быть. Реализуйте функцию так, чтобы при передаче некопируемых объектов не возникало ошибок компиляции.
Sentence — синоним для типа vector, объявленный следующим образом:
// Объявляем Sentence<Token> для произвольного типа Token
// синонимом vector<Token>.
// Благодаря этому в качестве возвращаемого значения
// функции можно указать не малопонятный вектор векторов,
// а вектор предложений — vector<Sentence<Token>>.
template <typename Token>
using Sentence = vector<Token>; 
Предложением считается последовательность лексем, которая заканчивается одной или несколькими идущими подряд лексемами — знаками пунктуации в конце предложения. Иными словами, любое предложение должно состоять из двух частей:
лексемы, для которых IsEndSentencePunctuation возвращает false. Такие лексемы должны быть в каждом предложении кроме, возможно, первого;
лексемы, для которых IsEndSentencePunctuation возвращает true. Такие лексемы должны быть в каждом предложении кроме, возможно, последнего.
Сохраните себе решение этой задачи, оно вам ещё понадобится.
Ограничения
Максимальное количество лексем — 10^6. Время выполнения одного вызова функции ограничено одной секундой.
Пример использования
Допустим, вектор лексем задан так:
{Красиво}{ты}{вошла}{в}{мою}{грешную}{жизнь}{!}{Красиво}{ты}{ушла}{из}{неё}{.} 
При этом ! и . — символы конца предложения, и функция IsEndSentencePunctuation возвращает true. Значит, результирующий вектор предложений будет состоять из двух элементов:
{{Красиво}{ты}{вошла}{в}{мою}{грешную}{жизнь}{!}}
{{Красиво}{ты}{ушла}{из}{неё}{.}} 
В прекоде уже есть тест, где используется копируемый тип TestToken. Напишите тест для класса, объекты которого нельзя копировать, и проверьте, что ваша функция работает корректно.
*/



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
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    // Напишите реализацию функции, не копируя объекты типа Token
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
