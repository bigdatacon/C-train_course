#pragma once
#include <vector>
#include "json.h"

namespace json {
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
        Builder& Value(const ValueType& value) {
            return builder_.Value(value);
        }

        Builder& Key(const std::string& key);

        Builder& StartDict();

        Builder& EndDict();

        Builder& StartArray();

        Builder& EndArray();


    private:
        Builder& builder_;
    };


    //1 Непосредственно после Key вызван не Value, не StartDict и не StartArray.
    class DictItemContextKey : public BaseContex {
    public:

        
        //DictItemContextKey(Builder& builder);

        Document Build() = delete;
        Builder& EndDict() = delete;
        Builder& EndArray()=delete;
        Builder& Key(const std::string& key) = delete;


        template<typename ValueType>
        Builder Value(const ValueType& value) {
            return builder_.Value(value);
        }

        Builder StartDict();

        Builder StartArray();


    private:
        Builder& builder_;

    };






}
