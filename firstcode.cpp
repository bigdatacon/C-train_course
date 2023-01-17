/*
Задание 2
Напишите функцию FindStartsWith, которая
принимает отсортированный набор строк в виде итераторов range_begin, range_end и один символ prefix;
возвращает полуинтервал [range_begin, range_end) строк в виде пары итераторов. Эти строки начинаются с символа prefix.
Если итоговый диапазон пуст, его границы должны указывать на место в контейнере, куда можно без нарушения порядка сортировки вставить любую строку, начинающуюся с символа prefix — подобно алгоритму equal_range. Гарантируется, что строки состоят лишь из строчных латинских букв, а символ prefix — строчная латинская буква.

ПОДСКАЗКА 
Символы, которые относятся к типу данных char, можно складывать с числами. Вычитать из них числа тоже можно. Так вы получите следующие или предыдущие буквы в алфавитном порядке. Например, для строки s выражение -s[0] заменит её первую букву на предыдущую.
Выражение 'a' + 1 имеет тип int. Поэтому может понадобиться привести его к типу char, применив static_cast.

Пример вывода

moscow murmansk
2 2
3 3 
*/

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, char prefix) {
    // напишите реализацию
}


int main() {
    const vector<string> sorted_strings = {"moscow", "murmansk", "vologda"};
    const auto m_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'm');
    for (auto it = m_result.first; it != m_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;
    const auto p_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'p');
    cout << (p_result.first - begin(sorted_strings)) << " " << (p_result.second - begin(sorted_strings)) << endl;
    const auto z_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), 'z');
    cout << (z_result.first - begin(sorted_strings)) << " " << (z_result.second - begin(sorted_strings)) << endl;
    return 0;
}
