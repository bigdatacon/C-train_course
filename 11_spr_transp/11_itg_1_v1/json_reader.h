#pragma once
#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>

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

inline void ReadInputJsonRequest() {
    std::string input_str;
    std::getline(std::cin, input_str);

    json::Document d = LoadJSON(input_str);
    cout << d.GetRoot().AsString() << endl;

    std::istringstream input_stream(input_str);
    //json::Document res = json::Load(input_stream);

    //Node dict_node(input_str);
    //cout << res.GetRoot().AsString() << endl;
    //cout << dict_node.AsMap().AsString() << endl;


}
