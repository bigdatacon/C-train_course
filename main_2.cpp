#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

/*Функция move поможет вам избежать копирования объектов и при создании pool, и при переносе выживших обратно в контейнер.*/
/*
Задание
Дан диапазон объектов некоторого типа. Напишите функцию, которая переставляет его элементы в соответствии с порядком, определённым считалкой Иосифа Флавия с заданным размером шага. Вы можете почитать о задаче здесь (https://habr.com/ru/company/goto/blog/351092/) или разобраться с примером в заготовке кода.
template <typename RandomIt>
void MakeJosephusPermutation(RandomIt range_begin, RandomIt range_end, uint32_t step_size); 
Предлагаемое решение задачи о считалке копирует элементы и не укладывается в ограничения по времени, но в остальном оно корректное. Исправьте его или напишите своё. Если найдёте, что в решении можно улучшить кроме замены копирования на перемещение, не стесняйтесь. Главное, чтобы оно продолжало корректно работать. Сохраните себе решение этого задания, оно вам ещё пригодится.
Формат входных данных
Гарантируется, что итераторы range_begin и range_end являются итераторами произвольного доступа, то есть допускают вычитание одного из другого и сложение с числом. Кроме того, вы можете полагаться на то, что step_size > 0. Тип переупорядочиваемых объектов можно получить с помощью выражения typename RandomIt::value_type. 
Максимальный размер диапазона — 10^5, максимальный размер шага — 10^2. Время выполнения одного вызова функции ограничено одной секундой.
Ограничения
Объекты такого типа копировать запрещено. При их копировании вы получите ошибку компиляции.
Тесты
Достаточно нескольких небольших ручных тестов и одного большого с максимальными ограничениями.
*/
template <typename RandomIt>
void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
    vector<typename RandomIt::value_type> pool(first, last);
    size_t cur_pos = 0;
    while (!pool.empty()) {
        *(first++) = pool[cur_pos];
        pool.erase(pool.begin() + cur_pos);
        if (pool.empty()) {
            break;
        }
        cur_pos = (cur_pos + step_size - 1) % pool.size();
    }
}

vector<int> MakeTestVector() {
    vector<int> numbers(10);
    iota(begin(numbers), end(numbers), 0);
    return numbers;
}

void TestIntVector() {
    const vector<int> numbers = MakeTestVector();
    {
        vector<int> numbers_copy = numbers;
        MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 1);
        assert(numbers_copy == numbers);
    }
    {
        vector<int> numbers_copy = numbers;
        MakeJosephusPermutation(begin(numbers_copy), end(numbers_copy), 3);
        assert(numbers_copy == vector<int>({0, 3, 6, 9, 4, 8, 5, 2, 7, 1}));
    }
}

// Это специальный тип, который поможет вам убедиться, что ваша реализация
// функции MakeJosephusPermutation не выполняет копирование объектов.
// Сейчас вы, возможно, не понимаете как он устроен, однако мы расскажем
// об этом далее в нашем курсе

struct NoncopyableInt {
    int value;

    NoncopyableInt(const NoncopyableInt&) = delete;
    NoncopyableInt& operator=(const NoncopyableInt&) = delete;

    NoncopyableInt(NoncopyableInt&&) = default;
    NoncopyableInt& operator=(NoncopyableInt&&) = default;
};

bool operator==(const NoncopyableInt& lhs, const NoncopyableInt& rhs) {
    return lhs.value == rhs.value;
}

ostream& operator<<(ostream& os, const NoncopyableInt& v) {
    return os << v.value;
}

void TestAvoidsCopying() {
    vector<NoncopyableInt> numbers;
    numbers.push_back({1});
    numbers.push_back({2});
    numbers.push_back({3});
    numbers.push_back({4});
    numbers.push_back({5});

    MakeJosephusPermutation(begin(numbers), end(numbers), 2);

    vector<NoncopyableInt> expected;
    expected.push_back({1});
    expected.push_back({3});
    expected.push_back({5});
    expected.push_back({4});
    expected.push_back({2});

    assert(numbers == expected);
}

int main() {
    TestIntVector();
    TestAvoidsCopying();
}
