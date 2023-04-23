#pragma once
/*
 * ����� ��� ����� JSON-����������
 */

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

    //bool operator==(const Node& lhs, const Node& rhs);
    //bool operator!=(const Node& lhs, const Node& rhs);



    class Node {
    public:
        /* ���������� Node, ��������� std::variant */
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
        Node() = default;
        Node(std::nullptr_t);
        Node(int val);
        Node(double val);
        Node(std::string val);
        Node(bool val);
        Node(const Dict& val);
        Node(Dict&& val);
        Node(const Array& val);
        Node(Array&& val);

        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;

        const Value& GetValue() const { return value_; }

        //bool operator==(const Node& rhs); 

        //bool operator!=(const Node& rhs); 


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
        Value value_;
    };

    inline bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }
    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
        //return lhs.GetValue() != rhs.GetValue();
    }

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        //bool operator==(const Document& rhs); 

        //bool operator!=(const Document& rhs); 


    private:
        Node root_;
    };

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }
    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }


    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);
    void PrintNode(const Node& node, std::ostream& out);

}  // namespace json
