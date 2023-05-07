#pragma once
#include <vector>
#include "json.h"

namespace json {
    class DictItemContextValueAftKey;
    class DictItemContextAftStartArray;
    class DictItemContextKey;
    class  BaseContex;



    class Builder {
    public:
        Builder() {
            have_key_ = false;
            have_something_ = false;
        }

        Node Build() const;

        template<typename ValueType>
        Builder& Value(const ValueType& value) {
            if (in_array()) {
                nodes_stack_.back()->AsArray().push_back(value);
            }
            else if (in_dict()) {
                if (have_key_) {
                    nodes_stack_.back()->AsDict()[current_key_] = value;
                    current_key_.clear();
                    have_key_ = false;
                }
                else {
                    throw std::logic_error("Adding a value to dict with no key");
                }
            }
            else if (on_top() && !have_something_) {
                root_ = value;
            }
            else {
                throw std::logic_error("Trying to put value while neither on top nor in array/dict");
            }
            have_something_ = true;
            return *this;
        }

        Builder& Key(const std::string& key);

        Builder& StartDict();

        Builder& EndDict();

        Builder& StartArray();

        Builder& EndArray();

    private:
        bool on_top() const;
        bool in_array() const;

        bool in_dict() const;

        Node root_;
        std::vector<Node*> nodes_stack_;
        std::string current_key_;
        bool have_key_;
        bool have_something_;
    };

    

    // 0 Базовый класс 
    class  BaseContex : public Builder {
    public:

        BaseContex(Builder& builder);

        Node Build() const;

        template<typename ValueType>
        BaseContex Value(const ValueType& value) {
            return builder_.Value(value);
        }

        DictItemContextKey Key(const std::string& key);

        BaseContex StartDict();

        Builder& EndDict();

        BaseContex StartArray();

        Builder& EndArray();


    private:
        Builder& builder_;
    };


    //1 Непосредственно после Key вызван не Value, не StartDict и не StartArray.
    class DictItemContextKey : public BaseContex {
    public:
        DictItemContextKey(BaseContex& builder);


        Document Build() = delete;
        Builder& EndDict() = delete;
        Builder& EndArray() = delete;


        DictItemContextKey Key(const std::string& key) ;

        template<typename ValueType>
        Builder Value(const ValueType& value);

        BaseContex StartDict();

        BaseContex StartArray();


    private:
        BaseContex& builder_;

    };

    /*

    //2 После вызова Value, последовавшего за вызовом Key, вызван не Key и не EndDict.
    //3 За вызовом StartDict следует не Key и не EndDict.
    class DictItemContextValueAftKey : public BaseContex {
        Document Build() = delete;
        BaseContex StartDict() = delete;
        BaseContex StartArray() = delete;
        Builder& EndArray() = delete;
        template<typename ValueType>
        BaseContex Value(const ValueType& value) = delete;


        DictItemContextKey Key(const std::string& key);

        Builder& EndDict();


    };

    // 4 За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.
    //5 После вызова StartArray и серии Value следует не Value, не StartDict, не StartArray и не EndArray
    class DictItemContextAftStartArray : public BaseContex {

        Document Build() = delete;
        Builder& EndDict() = delete;
        DictItemContextKey Key(const std::string& key) = delete;
        


        template<typename ValueType>
        Builder Value(const ValueType& value);

        BaseContex StartDict() ;
        BaseContex StartArray() ;
        Builder& EndArray() ;

        };

        */


}