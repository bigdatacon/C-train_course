#pragma once
// напишите решение с нуля
// код сохраните в свой git-репозиторий
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

#include "geo.h"

using namespace std;

enum class QueryType {
    Stop,
    Bus
};


struct Stop {
    string stop;
    Coordinates coordinates;
};

struct Bus {
    string bus;
    vector<string> stops;
};

struct UpdateQuery {
    QueryType type;
    Bus bus;
    Stop stop;

    string toString() const {
        if (type == QueryType::Bus) {
            return "Bus "s + bus.bus;
        }
        if (type == QueryType::Stop) {
            return "Stop "s + stop.stop;
        }
        return ""s;
    }
};

vector<string> SplitStringBySign(std::string str);
pair<double, double> SplitStringByComma(string str);
istream& operator>>(istream& is, UpdateQuery& q);


class InputReader {
public:
    InputReader(istream& is);
    int getNumUpdateQueries();
    UpdateQuery getUpdateQuery();

private:
    istream& is_;
    int num_update_q_;
};
