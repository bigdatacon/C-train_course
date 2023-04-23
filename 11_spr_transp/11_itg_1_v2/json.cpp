#include "json.h"

/*
 * ����� ��� ����� JSON-����������
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
                input >> std::ws;  // очистка потока от пробелов
                array.push_back(LoadNode(input));
                input >> std::ws;  // очистка потока от пробелов
                if (input.peek() == ',') {
                    input >> c;  // пропускаем запятую
                }
                else if (input.peek() != ']') {
                    throw ParsingError("Error: Expected ',' or ']'");
                }
            }
            input >> c;  // пропускаем ']'

            return Node(move(array));
        }

        using Number = std::variant<int, double>;

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
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
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
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
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return Node(std::move(std::stoi(parsed_num)));
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return Node(std::move(std::stod(parsed_num)));
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
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
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
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
                input >> ws;  // очистка потока от пробелов
                input >> c;
                string key = LoadString(input).AsString();
                input >> c;
                input >> std::ws;  // очистка потока от пробелов
                dict[key] = LoadNode(input);
                input >> std::ws;  // очистка потока от пробелов
                if (input.peek() == ',') {
                    input >> c;  // пропускаем запятую
                    input >> std::ws;
                }

            }
            if (input.peek() != '}') {
                throw ParsingError("Error: Expected ',' or '}'");
            }
            input >> c;  // пропускаем '}'

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
            /*else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
                //return LoadString(input);
            }*/
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
            if (c == '\r') {        // экранируем '\r'
                result += "\\r";
            }
            else if (c == '\n') { // экранируем '\n'
                result += "\\n";
            }
            else if (c == '\t') {
                result += "\t";     // экранировать символ '\t' не нужно
            }
            else if (c == '\\') {
                result += "\\\\";   // При '\' добавляем доп символ '\'
            }
            else if (c == '\"') {
                result += "\\\"";   // При '\"' добавляем доп символ '\'
            }
            else {
                result += c;
            }
        }
        out << "\"" << result << "\""; // В начало и конец добавляем кавычки
    }

    void PrintValue(const std::string& value, std::ostream& out) {
        ParsingOutString(value, out);
    }

    void PrintValue(std::nullptr_t, std::ostream& out) {
        out << "null";
    }
    // Другие перегрузки функции PrintValue пишутся аналогично
    void PrintValue(const Array& value, std::ostream& out) {
        out << "["; // добавляем открывающую скобку массива
        bool is_first = true;
        for (const auto& val : value) {
            if (!is_first) {
                out << ",";
            }
            PrintNode(val, out);
            is_first = false;
        }
        out << "]"; // добавляем закрывающую скобку массива
    }

    void PrintValue(const Dict& value, std::ostream& out) {
        out << "{"; // добавляем открывающую скобку словаря
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

        // � еализуйте функцию самостоятельно
    }


}  // namespace json