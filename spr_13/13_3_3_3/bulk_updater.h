#pragma once

#include "entities.h"
#include "summing_segment_tree.h"

#include <cstdint>
#include <cmath>
#include <iostream>


struct IncomeExpense {
    double income = 0.0;
    double expense = 0.0; 

    IncomeExpense() = default;

    IncomeExpense(double income, double expense)
        : income(income), expense(expense) {}

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

    double factor = 0.5;
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

    void CombineWith(const BulkLinearUpdater& other) {
        //tax_.count += other.tax_.count;
        //.delta = add_.delta * other.tax_.ComputeFactor() + other.add_.delta;
        add_.income = add_.income * other.tax_.ComputeFactor() + other.add_.income;
    }


    double Collapse(IncomeExpense origin, IndexSegment segment) const {
        //return origin * tax_.ComputeFactor() + add_.delta * static_cast<double>(segment.length());
        
        return origin.expense * tax_.ComputeFactor() + add_.income * static_cast<double>(segment.length())+ add_.expense * static_cast<double>(segment.length());
    }



private:
    // apply tax first, then add
    BulkTaxApplier tax_;
    IncomeExpense add_;
};
