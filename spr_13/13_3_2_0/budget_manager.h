#pragma once
#include "date.h"



#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>




struct DayInfo {
    double value;

    DayInfo() : value(0) {}
    DayInfo(double value) : value(value) {}
};


class BudgetManager {
public:
    inline static const Date START_DATE{ 2000, 1, 1 };
    inline static const Date END_DATE{ 2100, 1, 1 };

    BudgetManager();

    double ComputeIncome(const Date date_from, const Date date_to);
    

    void Earn(const DayInfo& initial_info, int start_index);

    void PayTax(const Date date_from, const Date date_to, double tax_rate);
    void Spend(const Date date_from, const Date date_to, double tax_rate);

    double GetValue(int day) const;

    void ModifyValue(int day, double value);

private:
    //static constexpr double DEFAULT_TAX_RATE = 0.13;
    std::vector<DayInfo> days_;
};


/*
class Request {
public:
    virtual ~Request() = default;
    virtual void Execute(BudgetManager& manager) = 0;
};

class ReadRequest : public Request {
public:
    ReadRequest(int day) : day_(day) {}

    void Execute(BudgetManager& manager) override {
        double value = manager.GetValue(day_);
        std::cout << "Value for day " << day_ << ": " << value << std::endl;
    }

private:
    int day_;
};

class ModifyRequest : public Request {
public:
    ModifyRequest(int day, double value) : day_(day), value_(value) {}

    void Execute(BudgetManager& manager) override {
        manager.ModifyValue(day_, value_);
    }

private:
    int day_;
    double value_;
};
*/