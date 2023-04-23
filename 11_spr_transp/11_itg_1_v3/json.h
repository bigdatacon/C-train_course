#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>

namespace json {

    class Node;
    // —охраните объ€влени€ Dict и Array без изменени€
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Ёта ошибка должна выбрасыватьс€ при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        /* –еализуйте Node, использу€ std::variant */

        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() = default;
        Node(const Array& array);
        Node(Array&& array);
        Node(const Dict& map);
        Node(Dict&& map);
        Node(int value);
        Node(std::string value);
        Node(bool bul);
        Node(double doub);
        Node(std::nullptr_t);

        const Value& GetValue() const { return value_; }

        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;
        double AsDouble() const;
        bool AsBool() const;

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        bool operator==(const Node& node) const {
            return value_ == node.value_;
        }

        bool operator!=(const Node& node) const {
            return value_ != node.value_;
        }

    private:
        Value value_;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& lhs) const {
            return root_ == lhs.root_;
        }

        bool operator!=(const Document& lhs) const {
            return !(root_ == lhs.root_);
        }

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    inline void PrintNode(const Node& node, std::ostream& out);

    inline void PrintValue(int value, std::ostream& out) {
        out << value;
    }

    inline void PrintValue(double value, std::ostream& out) {
        out << value;
    }

    inline void PrintValue(std::nullptr_t, std::ostream& out) {
        out << "null";
    }

    /*inline void PrintValue(const std::string& line, std::ostream& out) {
        out << "\"" << line << "\"";
    }*/

    inline void PrintValue(const std::string& value, std::ostream& out) {
        //  \n, \r, \", \t, \\.
        using namespace std::literals;
        out << '"';
        for (const char c : value) {
            switch (c) {
            case '"':
                out << "\\\""sv;
                break;
            case '\n':
                out << "\\n"sv;
                break;
            case '\t':
                out << "\t"sv;
                break;
            case '\r':
                out << "\\r"sv;
                break;
            case '\\':
                out << "\\\\"sv;
                break;
            default:
                out << c;
                break;
            }
        }
        out << '"';
    }

    inline void PrintValue(bool bul, std::ostream& out) {
        if (bul) out << "true";
        else out << "false";
    }

    inline void PrintValue(const Array& array, std::ostream& out) {
        out << "[" << std::endl;
        for (auto node = array.begin(); node != array.end(); node++) {
            if (node + 1 == array.end()) {
                PrintNode((*node), out);
                out << std::endl;
            }
            else {
                PrintNode((*node), out);
                out << ", " << std::endl;
            }
        }
        out << "]";
    }

    inline void PrintValue(const Dict& map, std::ostream& out) {
        out << "{" << std::endl;
        for (auto i = map.begin(); i != map.end(); i++) {
            auto temp = i;
            if (++temp == map.end()) {
                PrintValue(i->first, out);
                out << ": ";
                PrintNode(i->second, out);
                out << std::endl;
            }
            else {
                PrintValue(i->first, out);
                out << ": ";
                PrintNode(i->second, out);
                out << ", " << std::endl;
            }
        }
        out << "}";
    }

    inline void PrintNode(const Node& node, std::ostream& out) {
        std::visit(
            [&out](const auto& value) { PrintValue(value, out); },
            node.GetValue());
    }

    void Print(const Document& doc, std::ostream& output);

}  // namespace json