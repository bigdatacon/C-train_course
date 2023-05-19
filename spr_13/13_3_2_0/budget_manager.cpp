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



BudgetManager::BudgetManager() : days_(Date::ComputeDistance(START_DATE, END_DATE)) {
}

double  BudgetManager::ComputeIncome(const Date date_from, const Date date_to) {
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    //int quant_days = Date::ComputeDistance(date_from, date_to);
    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);
    double sum = std::accumulate(days_.begin() + initial_index, days_.begin() +  end_index+1, 0.0, [](double acc, const DayInfo& day_info) {
        return acc + day_info.value;
        });
    
    return sum;

}

void  BudgetManager::Earn(const DayInfo& initial_info, int start_index) {
    auto before_val = days_[start_index].value;
    if (days_[start_index].value) {
        days_[start_index].value += initial_info.value;

    }
    else {
        days_[start_index] =  initial_info;
    }

    assert(before_val + initial_info.value == days_[start_index].value);


}

void  BudgetManager::PayTax(const Date date_from, const Date date_to, double tax_rate) {
    double tax_rate_curr = (1 - tax_rate / 100.0);
    //cout << tax_rate_curr << endl;
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);
    double sum = ComputeIncome(date_from, date_to);
    //std::cout << "sum before tax :" << sum << std::endl;
    double diff = 0;
    for (int i = initial_index; i < end_index +1; ++i) {
        //cout << "this days_[i].value before tax : " << days_[i].value <<  "  diff :" << diff  << endl;
        //double val_before = days_[i].value;
        //days_[i].value = days_[i].value - (abs(days_[i].value) * tax_rate / 100.0);
        //diff += (val_before - days_[i].value );
        
        
        if (days_[i].value && days_[i].value > 0) {
            //cout << "this days_[i].value before tax : " << days_[i].value << "  diff :" << diff << endl;
            days_[i].value *= tax_rate_curr;
            diff += (days_[i].value / tax_rate_curr - days_[i].value);
            ///cout << "this days_[i].value after tax : " << days_[i].value << "  diff :" << diff << endl;
        }
        //cout << "this days_[i].value after tax : " << days_[i].value << endl;
        
    }
    double sum_after = ComputeIncome(date_from, date_to);
    double diff_diff =   sum - diff  -  sum_after;
    assert(diff_diff < 0.1);
    //cout << "DIFF TAX :: " << diff <<  " SUM BEFORE :  " << sum << " SUM AFTER : " << sum_after  << " diff_diff : " << diff_diff << endl;
    //std::cout << "Pay  tax of sum :" << sum - sum_after << std::endl;
    //assert(abs(sum* (1 - tax_rate / 100) - sum_after) <= 1);

}

void  BudgetManager::Spend(const Date date_from, const Date date_to, double amount) {
    int initial_index = Date::ComputeDistance(BudgetManager::START_DATE, date_from);
    int end_index = Date::ComputeDistance(BudgetManager::START_DATE, date_to);
    double sum = amount/(end_index - initial_index+1);
    double sum_before = ComputeIncome(date_from, date_to);
    //std::cout << "sum before spend amount : " <<amount << "SUM : " << sum_before << std::endl;

    for (int i = initial_index; i <= end_index ; ++i) {
        days_[i].value -= sum;

    }
    double sum_after = ComputeIncome(date_from, date_to);

    double diff =  sum_before - sum_after ;
    double diff_diff = amount - diff ;
    assert(abs(amount - diff) <= 0.1);

    //std::cout << "sum after spend amount : " << amount << "SUM : " << sum_after << std::endl;

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

