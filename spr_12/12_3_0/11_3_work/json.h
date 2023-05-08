#pragma once


#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <sstream>





namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

        bool IsInt() const {
            return std::holds_alternative<int>(*this);
        }
        int AsInt() const {
            using namespace std::literals;
            if (!IsInt()) {
                throw std::logic_error("Not an int"s);
            }
            return std::get<int>(*this);
        }

        bool IsPureDouble() const {
            return std::holds_alternative<double>(*this);
        }
        bool IsDouble() const {
            return IsInt() || IsPureDouble();
        }
        double AsDouble() const {
            using namespace std::literals;
            if (!IsDouble()) {
                throw std::logic_error("Not a double"s);
            }
            return IsPureDouble() ? std::get<double>(*this) : AsInt();
        }

        bool IsBool() const {
            return std::holds_alternative<bool>(*this);
        }
        bool AsBool() const {
            using namespace std::literals;
            if (!IsBool()) {
                throw std::logic_error("Not a bool"s);
            }

            return std::get<bool>(*this);
        }

        bool IsNull() const {
            return std::holds_alternative<std::nullptr_t>(*this);
        }

        bool IsArray() const {
            return std::holds_alternative<Array>(*this);
        }
        const Array& AsArray() const {
            using namespace std::literals;
            if (!IsArray()) {
                throw std::logic_error("Not an array"s);
            }

            return std::get<Array>(*this);
        }

        bool IsString() const {
            return std::holds_alternative<std::string>(*this);
        }
        const std::string& AsString() const {
            using namespace std::literals;
            if (!IsString()) {
                throw std::logic_error("Not a string"s);
            }

            return std::get<std::string>(*this);
        }

        bool IsDict() const {
            return std::holds_alternative<Dict>(*this);
        }
        const Dict& AsMap() const {
            using namespace std::literals;
            if (!IsDict()) {
                throw std::logic_error("Not a dict"s);
            }

            return std::get<Dict>(*this);
        }

        bool operator==(const Node& rhs) const {
            return GetValue() == rhs.GetValue();
        }

        const Value& GetValue() const {
            return *this;
        }
    };

    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

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

    inline json::Document LoadJSON(const std::string& s) {
        std::istringstream strm(s);
        return json::Load(strm);
    }

}