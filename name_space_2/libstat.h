#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

//using namespace std; //6.уберите using namespace std; из h-файлов,

using namespace std::string_view_literals;
namespace statistics {

namespace aggregations {
//using namespace std::literals;
using namespace std::string_view_literals;
    
//using namespace std::literals::string_view_literals;

class /*SumAggregation*/ Sum {
public:
    void PutValue(double value);
    std::optional<double> Get() const;

    static std::string_view GetValueName() {
        return "sum"sv;
    }

private:
    double sum_ = 0;
};

class /*AggregateMaximum*/  Max{
public:
    void PutValue(double value);
    std::optional<double> Get() const;

    static std::string_view GetValueName() {
        return "max"sv;
    }

private:
    std::optional<double> cur_max_;
};

class /*AggregatorAverage*/ Mean{
public:
    void PutValue(double value);
    std::optional<double> Get() const;

    static std::string_view GetValueName() {
        return "mean"sv;
    }

private:
    /*::*/Sum/*Aggregation*/ sum_;
    size_t count_ = 0;
};

class /*AggregStd*/ StandardDeviation {
public:
    void PutValue(double value);
    std::optional<double> Get() const;

    static std::string_view GetValueName() {
        return "standard deviation"sv;
    }

private:
    /*::*/Sum/*Aggregation*/ sum_;
    /*::*/Sum/*Aggregation*/ sum_sq_;
    size_t count_ = 0;
};

class Mode {
public:
    void PutValue(double value);
    std::optional<double> Get() const;

    static std::string_view GetValueName() {
        return "mode"sv;
    }

private:
    std::unordered_map<double, size_t> counts_;
    std::optional<double> cur_max_;
    size_t cur_count_ = 0;
};

    
} // закрыт блок aggregation 

namespace tests {
    //В пространстве имён statistics::tests используйте using namespace aggregations, чтобы не прибегать каждый раз к квалификации. При этом могут возникнуть конфликты
using namespace aggregations;    
    

//переименуйте тесты, убрав общий префикс TestStat, но оставив Aggreg,    
void /*TestStat*/ AggregSum();
void /*TestStat*/ AggregMax();
void /*TestStat*/ AggregMean();
void /*TestStat*/ AggregStandardDeviation();
void /*TestStat*/ AggregMode();
void /*TestStat*/ AggregPrinter();   

} // закрыт блок tests

template <typename Aggreg>
class AggregPrinter {
public:
    void PutValue(double value) {
        inner_.PutValue(value);
    }

    void Print(std::ostream& out) const {
        auto val = inner_.Get();
        out << inner_.GetValueName() << " is "sv;
        if (val) {
            out << *val;
        } else {
            out << "undefined"sv;
        }
        out << std::endl;
    }

private:
    Aggreg inner_;
};

} // закрыт statistics
