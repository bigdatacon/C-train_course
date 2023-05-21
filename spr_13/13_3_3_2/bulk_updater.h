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

    IncomeExpense operator*(double taxRate) const {
        return IncomeExpense(income * taxRate, expense);
    }

    double ComputeProfit() const {
        return income - expense;
    }
};


struct BulkMoneyAdder {
    BulkMoneyAdder() = default;
    BulkMoneyAdder(double income, double expense) : field(income, expense) {}
    //double delta = {};
    IncomeExpense field;
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

    BulkLinearUpdater(const BulkMoneyAdder& add)
        : add_(add) {
    }

    BulkLinearUpdater(const BulkTaxApplier& tax)
        : tax_(tax) {
    }

    void CombineWith(const BulkLinearUpdater& other) {
        //tax_.count += other.tax_.count;
        //.delta = add_.delta * other.tax_.ComputeFactor() + other.add_.delta;
        add_.field.income = add_.field.income * other.tax_.ComputeFactor() + other.add_.field.income;
    }


    double Collapse(double origin, IndexSegment segment) const {
        return origin * tax_.ComputeFactor() + add_.field.income * static_cast<double>(segment.length());
    }

    /*
   BulkMoneyAdder Collapse(double origin, IndexSegment segment) const {
        //return origin * tax_.ComputeFactor() + add_.delta * static_cast<double>(segment.length());
        //return origin * tax_.ComputeFactor() + add_.field.income * static_cast<double>(segment.length());
        //add_.field.income = add_.field.income * tax_.factor;
        //add_.field.income = add_.field.income * static_cast<double>(segment.length());
        return BulkMoneyAdder(5.0, 5.0);


    }
    */

private:
    // apply tax first, then add
    BulkTaxApplier tax_;
    BulkMoneyAdder add_;
};
