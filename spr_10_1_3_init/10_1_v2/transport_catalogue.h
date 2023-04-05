#pragma once
// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <deque>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <math.h>
#include <regex>

using namespace std;

struct AllBusInfoBusResponse {
    // Наполните полями эту структуру
    string bus;
    int stops;
    int uniq_stops;
    int r_length;
};

class TransportCatalogue {
public:
    void AddBus(const UpdateQuery q);

    void AddStop(const UpdateQuery q);

    vector<string> FindBus(const string bus);
    pair<double, double> FindStop(const string stop);

    AllBusInfoBusResponse GetAllBusInfo(const string bus);
private:
    unordered_map<string, vector<string>> buses_;
    unordered_map<string, pair<double, double>> stops_;

    int countUnique(std::vector<string> vec) {
        std::sort(vec.begin(), vec.end());
        auto last = std::unique(vec.begin(), vec.end());
        return std::distance(vec.begin(), last);
    }
};


