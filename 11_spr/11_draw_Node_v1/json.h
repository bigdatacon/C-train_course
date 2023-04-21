#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Map = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    //bool operator==(const Node& lhs, const Node& rhs);
    //bool operator!=(const Node& lhs, const Node& rhs);



    class Node {
    public:
        /* Реализуйте Node, используя std::variant */
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
        //Node() = default;
        Node() : value_(nullptr) {};
        Node(std::nullptr_t);
        Node(bool val);
        Node(Array array);
        Node(Dict map);
        Node(int value);
        Node(double value);
        Node(std::string value);

        //bool operator==(const Node& rhs) const;
        //bool operator!=(const Node& rhs) const;
   
        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;

        const Value& GetValue() const { return value_; }

        bool IsInt() const;
        bool IsDouble() const; //Возвращает true, если в Node хранится int либо double.
            bool IsPureDouble() const; //Возвращает true, если в Node хранится double.
            bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;


        bool AsBool() const;
        double AsDouble() const; //.Возвращает значение типа double, если внутри хранится double либо int.В последнем случае возвращается приведённое в double значение.


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

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json
