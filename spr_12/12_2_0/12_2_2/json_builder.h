#pragma once
#include <vector>
#include "json.h"

namespace json {

    class DictItemContextKey;
    class DictItemContext ;
    

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

        DictItemContextKey Key(const std::string& key);

        DictItemContext StartDict();

        Builder& EndDict();

        Builder& StartArray();

        Builder& EndArray();

        bool have_something_;

    private:
        bool on_top() const;
        bool in_array() const;

        bool in_dict() const;

        Node root_;
        std::vector<Node*> nodes_stack_;
        std::string current_key_;
        bool have_key_;

    };

    //3 За вызовом StartDict следует не Key и не EndDict.
    class DictItemContext : public Builder {
    public:
        DictItemContext(Builder& builder) : builder_(builder) {}

        DictItemContextKey Key(const std::string& key) {
            return builder_.Key(key);
        }

        Builder& EndDict() {
            return builder_.EndDict();
        }

    private:
        Builder& builder_;
    };

    //1 Непосредственно после Key вызван не Value, не StartDict и не StartArray.
    class DictItemContextKey : public Builder {
    public:
        DictItemContextKey(Builder& builder) : builder_(builder) {}

        template<typename ValueType>
        Builder& Value(const ValueType& value) {
            return builder_.Value(value);
        }

        DictItemContext StartDict() {
            return builder_.StartDict();
        }


        Builder& StartArray() {
            return builder_.StartArray();
        }

   
    private:
        Builder& builder_;

    };


}

