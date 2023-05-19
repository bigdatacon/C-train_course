// разработайте менеджер личного бюджета самостоятельно
#include "budget_manager.h"

#include <iostream>

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cassert>

using namespace std;


/*



ВЫВОД 
0
0
21283.2
139760
-2.2208e+06 // или -2 220 800
-4.2749e+06  // или -4 274 900

потрачено 
Spend 2001-02-13 2001-02-19 975815  // 7 дней или 139 402,142 в день 
Spend 2001-02-02 2001-02-13 531910  // 12 дней или 44 325,83

ComputeIncome 2001-02-11 2001-02-19   // -2 220 800
// у меня до последних 2 Spend -2,906,600.

МОЙ ВЫВОД 
Compute income : 0
Compute income : 0
Compute income : 21283.2
Compute income : 139760
Compute income : -2.2208e+06  // -2 220 800
Compute income : -4.18627e+06  // -4 186 270   // разница -88 630

ComputeIncome 2001-02-04 2001-02-18     //

// 1 с 02 по 12  - 11 дней потрачено по 44 325,83 в день  = 487 584,13
//2 13 числа потрачено 44 325,83 + 139 402,142 = 183 727,972
// с 14 по 18 - 5 дней потрачено по 139 402,142 = 697 010,71
Итого : 1 368 322,812
У меня 1,279,670. // разница -88 630

443 258,34

*/



BudgetManager::BudgetManager() : days_earned_(Date::ComputeDistance(START_DATE, END_DATE)), days_spended_(Date::ComputeDistance(START_DATE, END_DATE)){
}

double  BudgetManager::ComputeIncome(const Date date_from, const Date date_to) {  
    //return ComputeEarned(date_from, date_to) - ComputeSpended(date_from, date_to);
    double prev_treat = ComputeEarned(date_from, date_to) - ComputeSpended(date_from, date_to);

    double sum = 0;
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);

    for (int i = initial_index; i <= end_index; i++) {

        sum += days_earned_[i].value - days_spended_[i].value;
    }

    assert(abs(prev_treat) - abs(sum) < 0.1);
    //cout << "diff between  prev and current treatment : " << abs(prev_treat - sum);

    return sum;
}

double  BudgetManager::ComputeEarned(const Date date_from, const Date date_to) {
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);

    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);
    double sum = std::accumulate(days_earned_.begin() + initial_index, days_earned_.begin() + end_index + 1, 0.0, [](double acc, const DayInfo& day_info) {
        return acc + day_info.value;
        });

    return sum;

}

double  BudgetManager::ComputeSpended(const Date date_from, const Date date_to) {
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);

    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);
    double sum = std::accumulate(days_spended_.begin() + initial_index, days_spended_.begin() + end_index + 1, 0.0, [](double acc, const DayInfo& day_info) {
        return acc + day_info.value;
        });

    return sum;

}

void  BudgetManager::Earn(const DayInfo& initial_info, int start_index) {
    auto before_val = days_earned_[start_index].value;
    if (days_earned_[start_index].value) {
        days_earned_[start_index].value += initial_info.value;

    }
    else {
        days_earned_[start_index] =  initial_info;
    }

    assert(before_val + initial_info.value == days_earned_[start_index].value);


}

void  BudgetManager::PayTax(const Date date_from, const Date date_to, double tax_rate) {
    //double tax_rate_curr = (1 - tax_rate / 100.0);

    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);
    //double sum = ComputeIncome(date_from, date_to);

    //double diff = 0;
    for (int i = initial_index; i < end_index +1; ++i) {
        //days_earned_[i].value = days_earned_[i].value - (abs(days_earned_[i].value) * tax_rate / 100.0);
        days_earned_[i].value *=(1.0 - tax_rate / 100.0);
    }

}

void  BudgetManager::Spend(const Date date_from, const Date date_to, double amount) {
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);
    double sum = amount/(end_index - initial_index+1);
    //double sum_before = ComputeIncome(date_from, date_to);


    for (int i = initial_index; i <= end_index ; ++i) {
        days_spended_[i].value += sum;

    }
    //double sum_after = ComputeIncome(date_from, date_to);


}

double  BudgetManager::GetValue(int day) const {
    if (day >= days_earned_.size()) {
        return 0.0;
    }
    return days_earned_[day].value;
}

void  BudgetManager::ModifyValue(int day, double value) {
    if (day >= days_earned_.size()) {
        days_earned_.resize(day + 1);
    }
    days_earned_[day].value = value;
}

