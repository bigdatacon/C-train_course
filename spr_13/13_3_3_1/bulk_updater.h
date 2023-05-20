#pragma once

#include "entities.h"
#include "summing_segment_tree.h"

#include <cstdint>
#include <cmath>
#include <iostream>

struct BulkMoneyAdder {
    double delta = {};
};

struct BulkTaxApplier {
    double ComputeFactor() const {
        double new_rate = 1 - factor / 100;
        //std::cout << "new rate : " << new_rate << std::endl;
        return std::pow(new_rate, count);
    }

    BulkTaxApplier() = default;

    BulkTaxApplier(int count, double factor) : count(count), factor(factor) {}

    BulkTaxApplier& operator()(int newCount, double newFactor) {
        count = newCount;
        factor = newFactor;
        return *this;
    }

    int count = 0;
    double factor = 0.82;
};

/*
struct BulkTaxApplier {
    double ComputeFactor() const {
        static const double factor = 0.87;
        return std::pow(factor, count);
    }

    int count = 0;
};
*/

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
        tax_.count += other.tax_.count;
        add_.delta = add_.delta * other.tax_.ComputeFactor() + other.add_.delta;
    }

    double Collapse(double origin, IndexSegment segment) const {
        return origin * tax_.ComputeFactor() + add_.delta * static_cast<double>(segment.length());
    }

private:
    // apply tax first, then add
    BulkTaxApplier tax_;
    BulkMoneyAdder add_;
};
