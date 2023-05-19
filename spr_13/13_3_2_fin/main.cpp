#define _CRT_SECURE_NO_WARNINGS
#include "budget_manager.h"

#include <iostream>
#include <string_view>
#include <sstream>

using namespace std;



void ParseAndProcessQuery(BudgetManager& manager, string_view line) {
    // разработайте функцию чтения и обработки запроса
    //istringstream iss(line.data(), line.size());
    istringstream iss(line.data(), static_cast<std::ios_base::openmode>(line.size()));

    //istringstream iss(string(line));
    string command;

    iss >> command;
    if (command == "Earn") {
        string date_from_str, date_to_str;
        double amount;
        iss >> date_from_str >> date_to_str >> amount;

        Date date_from = Date::FromString(date_from_str);
        Date date_to = Date::FromString(date_to_str);
        int quant_days = Date::ComputeDistance(date_from, date_to)+1;
        int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);

        for (int i = 0; i < quant_days; ++i) {
            double money_in_day = amount / quant_days;
            DayInfo day_info(money_in_day);

            manager.Earn(day_info, initial_index);
            ++initial_index;
        }
    }
    if (command == "PayTax") {
        string date_from_str, date_to_str;
        double tax_rate;
        iss >> date_from_str >> date_to_str >> tax_rate;
        Date date_from = Date::FromString(date_from_str);
        Date date_to = Date::FromString(date_to_str);
        //cout << manager.PayTax(date_from, date_to) << endl;
        manager.PayTax(date_from, date_to, tax_rate);


    }
    if (command == "ComputeIncome") {
        string date_from_str, date_to_str;
        iss >> date_from_str >> date_to_str;
        Date date_from = Date::FromString(date_from_str);
        Date date_to = Date::FromString(date_to_str);
        //cout << "Compute income : " << manager.ComputeIncome(date_from, date_to) << endl;
        cout << manager.ComputeIncome(date_from, date_to) << endl;
    }
    
    if (command == "Spend") {
        string date_from_str, date_to_str;
        double amount;
        iss >> date_from_str >> date_to_str >> amount;
        Date date_from = Date::FromString(date_from_str);
        Date date_to = Date::FromString(date_to_str);
        //cout << "Compute income : " << manager.ComputeIncome(date_from, date_to) << endl;
        manager.Spend(date_from, date_to, amount);
    }

}

int ReadNumberOnLine(istream& input) {
    std::string line;
    std::getline(input, line);
    return std::stoi(line);
}

int main() {
#ifdef _DEBUG
    if (freopen("input.txt.txt", "r", stdin) == nullptr) {
        puts("can't open input.txt.txt");
        return 1;
    }
#endif
    BudgetManager manager;
    const int query_count = ReadNumberOnLine(cin);

    for (int i = 0; i < query_count; ++i) {
        std::string line;
        std::getline(cin, line);
        ParseAndProcessQuery(manager, line);
    }
}
