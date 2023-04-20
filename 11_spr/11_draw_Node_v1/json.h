#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    // ��������� ���������� Dict � Array ��� ���������
    using Dict = std::map<std::string, Node>;
    using Map = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // ��� ������ ������ ������������� ��� ������� �������� JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    bool operator==(const Node& lhs, const Node& rhs) ;
    bool operator!=(const Node& lhs, const Node& rhs) ;



    //using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    class Node {
    public:
        /* ���������� Node, ��������� std::variant */
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() : value_(nullptr) {}
        explicit Node(Array array);
        explicit Node(Dict map);
        explicit Node(int value);
        explicit Node(std::string value);

        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;

        const Value& GetValue() const { return value_; }

        bool IsInt() const;
        bool IsDouble() const; //���������� true, ���� � Node �������� int ���� double.
            bool IsPureDouble() const; //���������� true, ���� � Node �������� double.
            bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;


        bool AsBool() const;
        double AsDouble() const; //.���������� �������� ���� double, ���� ������ �������� double ���� int.� ��������� ������ ������������ ���������� � double ��������.


    private:
        Array as_array_;
        Dict as_map_;
        int as_int_ = 0;
        std::string as_string_;
        Value value_;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json
