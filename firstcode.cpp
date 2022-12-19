#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;
/*
Задание
Реализуйте шаблонную функцию ComputeTfIdfs, которая вычисляет TF-IDF заданного слова для каждого документа из набора.
Первый параметр documents — контейнер документов. циклом for (const auto& document : documents) можно перебрать все документы в нём, а в каждом документе — все слова. Тип слова, документа и набора документов может быть произвольным — ваша функция должна быть готова к любым, если есть возможность итерирования. Гарантируется, что и набор документов, и каждый отдельный документ имеют методы begin, end и size. Например, документом может быть строка, а словом — символ.
Второй параметр term — слово, у которого нужно вычислить TF-IDF. Его тип совпадает с типом слов, которые получаются при итерировании по документу.
Функция должна вернуть вектор вещественных чисел, который совпадает по длине с количеством документов. В первом элементе должен лежать TF-IDF слова term для первого документа, в последнем элементе — TF-IDF слова term для последнего документа.
Напоминаем, что TF-IDF — это произведение TF и IDF. TF слова term в документе — доля слов документа, совпадающих с term. IDF вычисляется для слова независимо от конкретного документа и равен логарифму (функция log из <cmath>) от documents.size() / document_freq, где знаменатель — это количество документов, содержащих term.

Пример из тренажёра должен вывести 0.081093 0.101366 0.

Ваша функция должна иметь два шаблонных параметра: Documents для набора документов и Term для слова. Указывайте их через запятую. Тип документа при этом никак не называйте: достаточно написать auto.
Используйте алгоритм count для подсчёта количества вхождений слова в документ.
Не забывайте перед делением приводить целые числа к double оператором static_cast.
*/
/*
template <typename Term>
map<Term, int> ComputeTermFreqs(const vector<Term>& terms) {
*/

template <typename Term, typename Documents >
vector<double> ComputeTfIdfs (const Documents &documents, const Term &term){
//template<typename Term>
//vector<double> ComputeTfIdfs(const vector<vector<Term>> &documents, Term term) {
	vector<double> TFIDF_VEC;
	double TFIDF;
	int document_freq = 0;
	// высчитываю IDF
	for (auto document : documents) {
		for (auto word : document) {
			if (word == term) {
				++document_freq;
			}
		}
	}
	double IDF = log((documents.size() * 1.0) / document_freq);

	// считаю TF для каждого документа
	for (auto document : documents) {
		double TF = 0;
		double frakciya = 1.0 / document.size(); // высчитываю долю одного слова
		for (auto word : document) {
			if (word == term) {
				TF += frakciya;
			}
		}
		TFIDF = TF*IDF;
		TFIDF_VEC.push_back(TFIDF);
	}
	return TFIDF_VEC;

}

int main() {
    const vector<vector<string>> documents = {
        {"белый"s, "кот"s, "и"s, "модный"s, "ошейник"s},
        {"пушистый"s, "кот"s, "пушистый"s, "хвост"s},
        {"ухоженный"s, "пёс"s, "выразительные"s, "глаза"s},
    };
    const auto& tf_idfs = ComputeTfIdfs(documents, "кот"s);
    for (const double tf_idf : tf_idfs) {
        cout << tf_idf << " "s;
    }
    cout << endl;
    return 0;
}
