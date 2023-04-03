#include "libstat.h"

#include <iostream>
using namespace std;

int main() {
    /*TestStat*/statistics::tests::AggregSum();
    /*TestStat*/statistics::tests::AggregMax();
    /*TestStat*/statistics::tests::AggregMean();
    /*TestStat*/statistics::tests::AggregStandardDeviation();
    /*TestStat*/statistics::tests::AggregMode();
    /*TestStat*/statistics::tests::AggregPrinter();

    cout << "Test passed!"sv << endl;
}
