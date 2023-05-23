#pragma once

#include "entities.h"
#include "summing_segment_tree.h"

#include <cstdint>
#include <cmath>
#include <iostream>
#include <string>


struct IncomeExpense {
    double income = 0.0;
    double expense = 0.0; 

    IncomeExpense() = default;

    IncomeExpense(double income, double expense)
        : income(income), expense(expense) {}

    IncomeExpense(double value) {
        if (value >= 0) {
            income = value;
            expense = 0;
        }
        else {
            income = 0;
            expense = -value;
        }
    }

    IncomeExpense operator+(const IncomeExpense& other) const {
        return IncomeExpense(income + other.income, expense + other.expense);
    }

    IncomeExpense operator+(double other) const {
        return IncomeExpense(income + other, expense );
    }

    IncomeExpense operator*(double taxRate) const {
        return IncomeExpense(income * taxRate, expense);
    }


    explicit operator double() const {
        return income - expense;
    }
    double ComputeProfit() const {
        return income - expense;
    }
};



struct BulkTaxApplier {
    double ComputeFactor() const {
        return factor;
    }

    BulkTaxApplier() = default;

    BulkTaxApplier( double factor) : factor(factor) {}

    BulkTaxApplier& operator()( double newFactor) {
        factor = newFactor;
        return *this;
    }

    double factor = 1.0;
};


class BulkLinearUpdater {
public:
    BulkLinearUpdater() = default;

    BulkLinearUpdater(const IncomeExpense& add)
        : add_(add) {
    }

    BulkLinearUpdater(const BulkTaxApplier& tax)
        : tax_(tax) {
    }

    /*

    void CombineWith(const BulkLinearUpdater& other) {
        add_.expense = add_.expense + other.add_.expense;
        add_.income = add_.income * (other.tax_.ComputeFactor() * tax_.ComputeFactor());
        add_.income = add_.income + other.add_.income * (other.tax_.ComputeFactor() * tax_.ComputeFactor());
    }


    IncomeExpense Collapse(IncomeExpense origin, IndexSegment segment) const {
        double inc = origin.income * tax_.ComputeFactor() + add_.income * static_cast<double>(segment.length());
        double exp = add_.expense * static_cast<double>(segment.length()) + origin.expense ;
        return IncomeExpense(inc, exp);
    }
    */

    void CombineWith(const BulkLinearUpdater& other) {

        tax_.factor *= other.tax_.factor;

        add_.income = add_.income * other.tax_.ComputeFactor() + other.add_.income;
        add_.expense += other.add_.expense;
    }


    IncomeExpense Collapse(IncomeExpense origin, IndexSegment segment) const {
        return { origin.income * tax_.ComputeFactor() + add_.income * static_cast<double>(segment.length()),
                origin.expense + add_.expense * static_cast<double>(segment.length()) };
    }

    std::string ToString() const {
        std::string str = "Add: Income=" + std::to_string(add_.income) + ", Expense=" + std::to_string(add_.expense) + "\n";
        str += "Tax: Factor=" + std::to_string(tax_.ComputeFactor());
        return str;
    }


private:
    // apply tax first, then add
    BulkTaxApplier tax_;
    IncomeExpense add_;
};
