// разработайте менеджер личного бюджета самостоятельно
#include "budget_manager.h"

#include <iostream>

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

BudgetManager::BudgetManager() : days_(Date::ComputeDistance(START_DATE, END_DATE)) {
}

double  BudgetManager::ComputeIncome(const Date date_from, const Date date_to) {
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    int quant_days = Date::ComputeDistance(date_from, date_to);
    double sum = std::accumulate(days_.begin() + initial_index, days_.begin() + initial_index + quant_days + 1, 0.0, [](double acc, const DayInfo& day_info) {
        return acc + day_info.value;
        });
    return sum;

}

void  BudgetManager::Earn(const DayInfo& initial_info, int start_index) {
    days_.insert(days_.begin() + start_index, initial_info);
}

void  BudgetManager::PayTax(const Date date_from, const Date date_to) {
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    int quant_days = Date::ComputeDistance(date_from, date_to);
    double sum = ComputeIncome(date_from, date_to);
    //std::cout << "sum before tax :" << sum << std::endl;

    for (int i = initial_index; i < initial_index + quant_days+1; ++i) {
        days_[i].value *=(1- DEFAULT_TAX_RATE);

    }
    double sum_after = ComputeIncome(date_from, date_to);
    std::cout << "Pay  tax of sum :" << sum - sum_after << std::endl;

}

double  BudgetManager::GetValue(int day) const {
    if (day >= days_.size()) {
        return 0.0;
    }
    return days_[day].value;
}

void  BudgetManager::ModifyValue(int day, double value) {
    if (day >= days_.size()) {
        days_.resize(day + 1);
    }
    days_[day].value = value;
}

