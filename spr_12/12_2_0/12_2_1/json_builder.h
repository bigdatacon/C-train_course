#pragma once
#include <vector>
#include "json.h"

namespace json {

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

        Builder& Key(const std::string& key);

        DictItemContext StartDict();

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

    class DictItemContext : public Builder {
    public:
        DictItemContext(Builder& builder) : builder_(builder) {}

        Builder& Key(const std::string& key) {
            return builder_.Key(key);
        }

        Builder& EndDict() {
            return builder_.EndDict();
        }

    private:
        Builder& builder_;
    };

}

