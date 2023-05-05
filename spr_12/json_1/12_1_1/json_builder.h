#pragma once
#include <vector>
#include "json.h"
#include <stdexcept>

namespace json {
    class Builder {
    public:
        Builder() {
        }

        Node Build() const;

        template<typename ValueType>
        Builder& Value(const ValueType& value) {
            if ((!is_array_ && !is_dict_) && is_not_first_operation_) {
                throw std::logic_error("Wrong context.");
            }
            if (in_array()) {
                nodes_stack_.back()->AsArray().push_back(value);
            }
            else if (in_dict()) {
                nodes_stack_.back()->AsDict()[current_key_] = value;
                current_key_.clear();
            }
            else {
                root_ = value;
            }
            is_not_first_operation_ = true;
            return *this;
        }

        Builder& Key(const std::string& key);

        Builder& StartDict();

        Builder& EndDict();

        Builder& StartArray();

        Builder& EndArray();

    private:
        bool in_array();

        bool in_dict();

        Node root_;
        std::vector<Node*> nodes_stack_;
        std::string current_key_;
        bool is_array_= false;
        bool is_dict_ = false;
        bool is_built_ = false;
        bool is_not_first_operation_ = false;
    };
}
