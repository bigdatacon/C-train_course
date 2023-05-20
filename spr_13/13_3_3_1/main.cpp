#define _CRT_SECURE_NO_WARNINGS
#include "parser.h"

#include <iostream>

using namespace std;

void ParseAndProcessQuery(BudgetManager& manager, string_view line) {
    auto query = ParseQuery(line);

    if (!query) {
        return;
    }

    query->ProcessAndPrint(manager, cout);
}

int ReadNumberOnLine(istream& input) {
    std::string line;
    std::getline(input, line);
    return std::stoi(line);
}

int main() {
    #ifdef _DEBUG
        if (freopen("input.txt.txt", "r", stdin) == nullptr) {
            puts("can't open input.txt.txt");
            return 1;
        }
    #endif
    BudgetManager manager;

    const int query_count = ReadNumberOnLine(cin);

    for (int i = 0; i < query_count; ++i) {
        std::string line;
        std::getline(cin, line);
        ParseAndProcessQuery(manager, line);
    }
}
