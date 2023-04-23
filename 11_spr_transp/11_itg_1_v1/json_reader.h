#pragma once
#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>
#include <iostream>
#include <fstream>
#include <string>

#include "json.h"

using namespace json;
using namespace std::literals;
using namespace std;



inline json::Document LoadJSON(const std::string& s) {
    std::istringstream strm(s);
    return json::Load(strm);
}

inline std::string Print(const Node& node) {
    std::ostringstream out;
    Print(Document{ node }, out);
    return out.str();
}

/*
inline void ReadInputJsonRequest() {
    std::string input_str;
    std::getline(std::cin, input_str);

    json::Document d = LoadJSON(input_str);
    cout << d.GetRoot().AsString() << endl;

    std::istringstream input_stream(input_str);

}*/

inline void ReadInputJsonRequest() {
    std::ifstream inFile("input.json.txt"); // открытие файла для чтения
    if (!inFile) { // проверка успешности открытия файла
        std::cerr << "Ошибка при открытии файла " << std::endl;
        return;
    }

    std::string full_line;
    std::string line;
    while (std::getline(inFile, line)) {
        full_line += line;
    }

    inFile.close(); // закрытие файла
    std::cout << "eto fulL_line : " << full_line << std::endl;

    json::Document d = LoadJSON(full_line);
    cout << d.GetRoot().AsString() << endl;

    std::istringstream input_stream(full_line);

}
