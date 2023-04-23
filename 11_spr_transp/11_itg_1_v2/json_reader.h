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


std::vector<std::string> GetStringRequests(std::string full_line) {
    std::vector<std::string> itg_vec;
    //1 ќбреза€ от строки сначала блок "base_requests" - чтобы убрать "base_requests" и оставить только что в нем
    std::size_t base_requests_pos = full_line.find("\"base_requests\":");
    std::size_t brace_pos = full_line.find('{', base_requests_pos);
    std::string stripped_line = full_line.substr(brace_pos);

    // 3 обрезаю до "stat_requests"
    std::string stripped_line_no_statr = stripped_line.substr(0, stripped_line.find("\"stat_requests\""));


    // 4 удал€ю пробелы с конца и ],
    size_t last_non_space = stripped_line_no_statr.find_last_not_of(" ");
    if (last_non_space != std::string::npos) {
        stripped_line_no_statr = stripped_line_no_statr.substr(0, last_non_space + 1);
    }

    // ”даление знаков '], с конца'
    size_t last_bracket = stripped_line_no_statr.find_last_of("]");
    if (last_bracket != std::string::npos) {
        stripped_line_no_statr = stripped_line_no_statr.substr(0, last_bracket);
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    //II отдельно делаю строку в формате map дл€ stat_requests

    itg_vec.push_back(stripped_line_no_statr);

    //5 оставл€ю только то что в stat_requests
    std::size_t pos = stripped_line.find("\"stat_requests\":");
    if (pos != std::string::npos) {
        stripped_line = stripped_line.substr(pos + 16);
    }

    //6 ”даление пробелов и скобок
    size_t last_non_space2 = stripped_line.find_last_not_of(" ");
    if (last_non_space2 != std::string::npos) {
        stripped_line = stripped_line.substr(0, last_non_space2 - 1);
    }

    size_t start = stripped_line.find_first_not_of("[ ");
    size_t end = stripped_line.find_last_not_of(" ]");
    std::string stat_reader = stripped_line.substr(start, end);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    //std::cout << "ITG : " << stat_reader << '\n';
    itg_vec.push_back(stat_reader);
    return itg_vec;

}



inline void ReadInputJsonRequest() {
    std::ifstream inFile("input.json.txt"); // открытие файла дл€ чтени€
    if (!inFile) { // проверка успешности открыти€ файла
        std::cerr << "ќшибка при открытии файла " << std::endl;
        return;
    }

    std::string full_line;
    std::string line;
    while (std::getline(inFile, line)) {
        full_line += line;
    }
    // –азабираю запрос на 2 строки с подзапросами дл€ base и stat
    string stat_r = GetStringRequests(full_line)[1];
    string base_r = GetStringRequests(full_line)[0];

    cout << "stat_r       " << stat_r << endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    cout << "base_r        " << base_r << endl;

    json::Document d = LoadJSON(stat_r);
    cout << d.GetRoot().AsString() << endl;

    std::istringstream input_stream(stat_r);

}


