#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы xml.h и xml.cpp
#include "xml.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(spendings.begin(), spendings.end(), 0,
                      [](int current, const Spending& spending) {
                          return current + spending.amount;
                      });
}

string FindMostExpensiveCategory(const vector<Spending>& spendings) {
    assert(!spendings.empty());
    auto compare_by_amount = [](const Spending& lhs, const Spending& rhs) {
        return lhs.amount < rhs.amount;
    };
    return max_element(begin(spendings), end(spendings), compare_by_amount)->category;
}
/*
Прочитать XML можно функцией Load. Она возвратит объект типа Document, содержащий все необходимые данные.
XML состоит из элементов или тегов, главный из которых — корневой. Получить его можно методом документа GetRoot().
Вам нужны вложенные теги корневого элемента. Доступ к ним можно получить методом Children().
Нужные значения получи́те, применив шаблонный метод AttributeValue тега, который принимает один аргумент — название атрибута.
Используйте AttributeValue<int> для получения числовых атрибутов и AttributeValue<string> для строковых.
*/
vector<Spending> LoadFromXml(istream& input) {
    // место для вашей реализации
    // пример корректного XML-документа в условии
    vector<Spending> res;
	std::vector<Node> as_array_;
	Document first = Load(input);
	as_array_ = first.GetRoot().Children();
	for (auto el : as_array_) {
		//auto maps = el.AsMap(); 
		Spending sp;
		//sp.category = maps["category"].AsString();
		//sp.amount = maps["amount"].AsInt();

		sp.category = el.at("category").AsString();
		sp.amount = maps.at("amount").AsInt();
		res.push_back(sp);
	}
}

int main() {
    const vector<Spending> spendings = LoadFromXml(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}