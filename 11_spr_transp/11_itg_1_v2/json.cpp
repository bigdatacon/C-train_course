#include "json.h"

/*
 * Место для вашей JSON-библиотеки
 */

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include "json.h"

using namespace std;

namespace json {

    namespace {
        string Trim(string_view str) {
            while (!str.empty() && (str.front() == ' ' || str.front() == '\t'
                || str.front() == '\n' || str.front() == '\r')) {
                str.remove_prefix(1);
            }
            while (!str.empty() && (str.back() == ' ' || str.back() == '\t'
                || str.back() == '\n' || str.back() == '\r')) {
                str.remove_suffix(1);
            }
            return string(str);
        }

        Node LoadNode(istream& input);

        Node LoadNull(istream& input) {
            string null;
            getline(input, null, ',');
            null = Trim(null);
            if (null == "null") {
                input.putback(',');
                return Node(nullptr);
            }
            else {
                throw ParsingError("Invalid null argument");
            }
        }

        Node LoadBool(istream& input) {
            string str;
            //input >> str;
            getline(input, str, ',');
            str = Trim(str);
            if (str == "true") {
                input.putback(',');
                return Node(true);
                //return Node("true");
            }
            else if (str == "false") {
                input.putback(',');
                return Node(false);
                //return Node("false");
            }
            else {
                throw ParsingError("Invalid boolean value: " + str);
            }
        }

        Node LoadArray(istream& input) {
            Array array;
            char c;
            input >> c;
            if (c != '[') {
                throw ParsingError("Error: Expected '['");
            }
            while (input.peek() != ']') {
                input >> std::ws;  // РѕС‡РёСЃС‚РєР° РїРѕС‚РѕРєР° РѕС‚ РїСЂРѕР±РµР»РѕРІ
                array.push_back(LoadNode(input));
                input >> std::ws;  // РѕС‡РёСЃС‚РєР° РїРѕС‚РѕРєР° РѕС‚ РїСЂРѕР±РµР»РѕРІ
                if (input.peek() == ',') {
                    input >> c;  // РїСЂРѕРїСѓСЃРєР°РµРј Р·Р°РїСЏС‚СѓСЋ
                }
                else if (input.peek() != ']') {
                    throw ParsingError("Error: Expected ',' or ']'");
                }
            }
            input >> c;  // РїСЂРѕРїСѓСЃРєР°РµРј ']'

            return Node(move(array));
        }

        using Number = std::variant<int, double>;

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // РЎС‡РёС‚С‹РІР°РµС‚ РІ parsed_num РѕС‡РµСЂРµРґРЅРѕР№ СЃРёРјРІРѕР» РёР· input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // РЎС‡РёС‚С‹РІР°РµС‚ РѕРґРЅСѓ РёР»Рё Р±РѕР»РµРµ С†РёС„СЂ РІ parsed_num РёР· input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // РџР°СЂСЃРёРј С†РµР»СѓСЋ С‡Р°СЃС‚СЊ С‡РёСЃР»Р°
            if (input.peek() == '0') {
                read_char();
                // РџРѕСЃР»Рµ 0 РІ JSON РЅРµ РјРѕРіСѓС‚ РёРґС‚Рё РґСЂСѓРіРёРµ С†РёС„СЂС‹
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // РџР°СЂСЃРёРј РґСЂРѕР±РЅСѓСЋ С‡Р°СЃС‚СЊ С‡РёСЃР»Р°
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // РџР°СЂСЃРёРј СЌРєСЃРїРѕРЅРµРЅС†РёР°Р»СЊРЅСѓСЋ С‡Р°СЃС‚СЊ С‡РёСЃР»Р°
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // РЎРЅР°С‡Р°Р»Р° РїСЂРѕР±СѓРµРј РїСЂРµРѕР±СЂР°Р·РѕРІР°С‚СЊ СЃС‚СЂРѕРєСѓ РІ int
                    try {
                        return Node(std::move(std::stoi(parsed_num)));
                    }
                    catch (...) {
                        // Р’ СЃР»СѓС‡Р°Рµ РЅРµСѓРґР°С‡Рё, РЅР°РїСЂРёРјРµСЂ, РїСЂРё РїРµСЂРµРїРѕР»РЅРµРЅРёРё,
                        // РєРѕРґ РЅРёР¶Рµ РїРѕРїСЂРѕР±СѓРµС‚ РїСЂРµРѕР±СЂР°Р·РѕРІР°С‚СЊ СЃС‚СЂРѕРєСѓ РІ double
                    }
                }
                return Node(std::move(std::stod(parsed_num)));
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // РЎС‡РёС‚С‹РІР°РµС‚ СЃРѕРґРµСЂР¶РёРјРѕРµ СЃС‚СЂРѕРєРѕРІРѕРіРѕ Р»РёС‚РµСЂР°Р»Р° JSON-РґРѕРєСѓРјРµРЅС‚Р°
        // Р¤СѓРЅРєС†РёСЋ СЃР»РµРґСѓРµС‚ РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїРѕСЃР»Рµ СЃС‡РёС‚С‹РІР°РЅРёСЏ РѕС‚РєСЂС‹РІР°СЋС‰РµРіРѕ СЃРёРјРІРѕР»Р° ":
        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // РџРѕС‚РѕРє Р·Р°РєРѕРЅС‡РёР»СЃСЏ РґРѕ С‚РѕРіРѕ, РєР°Рє РІСЃС‚СЂРµС‚РёР»Рё Р·Р°РєСЂС‹РІР°СЋС‰СѓСЋ РєР°РІС‹С‡РєСѓ?
                    throw ParsingError("String parsing error");
                }
                if (*it == '}') {
                    //++it;
                    break;
                }

                const char ch = *it;
                if (ch == '"') {
                    // Р’СЃС‚СЂРµС‚РёР»Рё Р·Р°РєСЂС‹РІР°СЋС‰СѓСЋ РєР°РІС‹С‡РєСѓ
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Р’СЃС‚СЂРµС‚РёР»Рё РЅР°С‡Р°Р»Рѕ escape-РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚Рё
                    ++it;
                    if (it == end) {
                        // РџРѕС‚РѕРє Р·Р°РІРµСЂС€РёР»СЃСЏ СЃСЂР°Р·Сѓ РїРѕСЃР»Рµ СЃРёРјРІРѕР»Р° РѕР±СЂР°С‚РЅРѕР№ РєРѕСЃРѕР№ С‡РµСЂС‚С‹
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // РћР±СЂР°Р±Р°С‚С‹РІР°РµРј РѕРґРЅСѓ РёР· РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚РµР№: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Р’СЃС‚СЂРµС‚РёР»Рё РЅРµРёР·РІРµСЃС‚РЅСѓСЋ escape-РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚СЊ
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // РЎС‚СЂРѕРєРѕРІС‹Р№ Р»РёС‚РµСЂР°Р» РІРЅСѓС‚СЂРё- JSON РЅРµ РјРѕР¶РµС‚ РїСЂРµСЂС‹РІР°С‚СЊСЃСЏ СЃРёРјРІРѕР»Р°РјРё \r РёР»Рё \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // РџСЂРѕСЃС‚Рѕ СЃС‡РёС‚С‹РІР°РµРј РѕС‡РµСЂРµРґРЅРѕР№ СЃРёРјРІРѕР» Рё РїРѕРјРµС‰Р°РµРј РµРіРѕ РІ СЂРµР·СѓР»СЊС‚РёСЂСѓСЋС‰СѓСЋ СЃС‚СЂРѕРєСѓ
                    s.push_back(ch);
                }
                ++it;
            }
            return Node(std::move(s));
        }

        Node LoadDict(istream& input) {
            Dict dict;

            char c;
            input >> c;
            if (c != '{') {
                throw ParsingError("Error: Expected '{'");
            }
            while (input.peek() != '}') {
                input >> ws;  // РѕС‡РёСЃС‚РєР° РїРѕС‚РѕРєР° РѕС‚ РїСЂРѕР±РµР»РѕРІ
                input >> c;
                string key = LoadString(input).AsString();
                input >> c;
                input >> std::ws;  // РѕС‡РёСЃС‚РєР° РїРѕС‚РѕРєР° РѕС‚ РїСЂРѕР±РµР»РѕРІ
                dict[key] = LoadNode(input);
                input >> std::ws;  // РѕС‡РёСЃС‚РєР° РїРѕС‚РѕРєР° РѕС‚ РїСЂРѕР±РµР»РѕРІ
                if (input.peek() == ',') {
                    input >> c;  // РїСЂРѕРїСѓСЃРєР°РµРј Р·Р°РїСЏС‚СѓСЋ
                    input >> std::ws;
                }

            }
            if (input.peek() != '}') {
                throw ParsingError("Error: Expected ',' or '}'");
            }
            input >> c;  // РїСЂРѕРїСѓСЃРєР°РµРј '}'

            return Node(move(dict));
        }

        Node LoadNode(istream& input) {
            input >> ws;
            char c;
            input >> c;
            if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                //return LoadBool(input);
                //if (c == 't') { return Node("true"s); }
                //else { return Node("false"s); }
                return LoadString(input);
            }
            else if (c == '\"') {
                return LoadString(input);
            }
            else if (c == '[') {
                input.putback(c);
                return LoadArray(input);
            }
            else if (c == '{') {
                input.putback(c);
                return LoadDict(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace

    Node::Node(std::nullptr_t) : value_(nullptr) {}
    Node::Node(int val) : value_(val) {}
    Node::Node(double val) : value_(val) {}
    Node::Node(std::string val) : value_(std::move(val)) {}
    Node::Node(bool val) : value_(val) {}
    Node::Node(const Dict& val) : value_(val) {}
    Node::Node(Dict&& val) : value_(std::move(val)) {}
    Node::Node(const Array& val) : value_(val) {}
    Node::Node(Array&& val) : value_(std::move(val)) {}

    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }
    bool Node::IsDouble() const {
        bool result = false;
        if (std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_)) {
            result = true;
        }
        return result;
    }
    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }
    bool Node::IsNull() const {
        bool a = std::holds_alternative<std::nullptr_t>(value_);
        return a;
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }
    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }

    int Node::AsInt() const {
        if (auto val = std::get_if<int>(&value_)) {
            return *val;
        }
        else {
            throw std::logic_error("Node value is not an int");
        }
    }
    bool Node::AsBool() const {
        if (auto val = std::get_if<bool>(&value_)) {
            return *val;
        }
        else {
            throw std::logic_error("Node value is not a bool");
        }
    }
    double Node::AsDouble() const {
        if (auto val = std::get_if<double>(&value_)) {
            return *val;
        }
        else if (auto val = std::get_if<int>(&value_)) {
            return static_cast<double>(*val);
        }
        else {
            throw std::logic_error("Node value is not a double");
        }
    }
    const std::string& Node::AsString() const {
        if (auto val = std::get_if<std::string>(&value_)) {
            return *val;
        }
        else {
            throw std::logic_error("Node value is not a string");
        }
    }
    const Array& Node::AsArray() const {
        if (auto val = std::get_if<Array>(&value_)) {
            return *val;
        }
        else {
            throw std::logic_error("Node value is not an array");
        }
    }
    const Dict& Node::AsMap() const {
        if (auto val = std::get_if<Dict>(&value_)) {
            return *val;
        }
        else {
            throw std::logic_error("Node value is not a map");
        }
    }

    /*bool operator==(const Node& lhs, const Node& rhs) {
        if (lhs.GetValue().index() != rhs.GetValue().index()) {
            return false;
        }
        if (std::holds_alternative<std::nullptr_t>(lhs.GetValue())) {
            return true;
        }
        return lhs.GetValue() == rhs.GetValue();
    }

    bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }*/

    /*bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }
    bool operator!=(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() != rhs.GetRoot();
    }*/

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }
    template <typename V>
    void PrintNode(const V& node, std::ostream& out);

    void PrintValue(const int value, std::ostream& out) {
        out << value;
    }
    void PrintValue(const double value, std::ostream& out) {
        out << value;
    }
    void PrintValue(bool value, std::ostream& out) {
        out << boolalpha << value;
    }
    void ParsingOutString(const std::string& value, std::ostream& out) {
        std::string result;
        for (char c : value) {
            if (c == '\r') {        // СЌРєСЂР°РЅРёСЂСѓРµРј '\r'
                result += "\\r";
            }
            else if (c == '\n') { // СЌРєСЂР°РЅРёСЂСѓРµРј '\n'
                result += "\\n";
            }
            else if (c == '\t') {
                result += "\t";     // СЌРєСЂР°РЅРёСЂРѕРІР°С‚СЊ СЃРёРјРІРѕР» '\t' РЅРµ РЅСѓР¶РЅРѕ
            }
            else if (c == '\\') {
                result += "\\\\";   // РџСЂРё '\' РґРѕР±Р°РІР»СЏРµРј РґРѕРї СЃРёРјРІРѕР» '\'
            }
            else if (c == '\"') {
                result += "\\\"";   // РџСЂРё '\"' РґРѕР±Р°РІР»СЏРµРј РґРѕРї СЃРёРјРІРѕР» '\'
            }
            else {
                result += c;
            }
        }
        out << "\"" << result << "\""; // Р’ РЅР°С‡Р°Р»Рѕ Рё РєРѕРЅРµС† РґРѕР±Р°РІР»СЏРµРј РєР°РІС‹С‡РєРё
    }

    void PrintValue(const std::string& value, std::ostream& out) {
        ParsingOutString(value, out);
    }

    void PrintValue(std::nullptr_t, std::ostream& out) {
        out << "null";
    }
    // Р”СЂСѓРіРёРµ РїРµСЂРµРіСЂСѓР·РєРё С„СѓРЅРєС†РёРё PrintValue РїРёС€СѓС‚СЃСЏ Р°РЅР°Р»РѕРіРёС‡РЅРѕ
    void PrintValue(const Array& value, std::ostream& out) {
        out << "["; // РґРѕР±Р°РІР»СЏРµРј РѕС‚РєСЂС‹РІР°СЋС‰СѓСЋ СЃРєРѕР±РєСѓ РјР°СЃСЃРёРІР°
        bool is_first = true;
        for (const auto& val : value) {
            if (!is_first) {
                out << ",";
            }
            PrintNode(val, out);
            is_first = false;
        }
        out << "]"; // РґРѕР±Р°РІР»СЏРµРј Р·Р°РєСЂС‹РІР°СЋС‰СѓСЋ СЃРєРѕР±РєСѓ РјР°СЃСЃРёРІР°
    }

    void PrintValue(const Dict& value, std::ostream& out) {
        out << "{"; // РґРѕР±Р°РІР»СЏРµРј РѕС‚РєСЂС‹РІР°СЋС‰СѓСЋ СЃРєРѕР±РєСѓ СЃР»РѕРІР°СЂСЏ
        bool is_first = true;
        for (const auto& [key, val] : value) {
            if (!is_first) {
                out << ", ";
            }
            out << "\"" << key << "\": ";
            PrintNode(val, out);

            is_first = false;
        }
        out << '}';
    }

    void PrintNode(const Node& node, std::ostream& out) {
        std::visit(
            [&out](const auto& value) { PrintValue(value, out); },
            node.GetValue());
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), output);

        // Р РµР°Р»РёР·СѓР№С‚Рµ С„СѓРЅРєС†РёСЋ СЃР°РјРѕСЃС‚РѕСЏС‚РµР»СЊРЅРѕ
    }


}  // namespace json
