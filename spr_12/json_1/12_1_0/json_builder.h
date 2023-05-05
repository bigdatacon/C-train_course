#pragma once
#include <vector>
#include "json.h"

namespace json {
    class Builder {
    public:
        Builder() {
        }

        Node Build() const {
            return root_;
        }

        template<typename ValueType>
        Builder& Value(const ValueType& value) {
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
            return *this;
        }

        Builder& Key(const std::string& key) {
            if (in_dict()) {
                current_key_ = key;
            }
            return *this;
        }

        Builder& StartDict() {
            if (in_array()) {
                nodes_stack_.back()->AsArray().push_back(Dict());
                nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
            }
            else if (in_dict()) {
                nodes_stack_.back()->AsDict()[current_key_] = Dict();
                nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
            }
            else {
                root_ = Dict();
                nodes_stack_.push_back(&root_);
            }
            return *this;
        }

        Builder& EndDict() {
            if (in_dict()) {
                nodes_stack_.pop_back();
            }
            return *this;
        }

        Builder& StartArray() {
            if (in_array()) {
                nodes_stack_.back()->AsArray().push_back(Array());
                nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
            }
            else if (in_dict()) {
                nodes_stack_.back()->AsDict()[current_key_] = Array();
                nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
            }
            else {
                root_ = Array();
                nodes_stack_.push_back(&root_);
            }
            return *this;
        }

        Builder& EndArray() {
            if (in_array()) {
                nodes_stack_.pop_back();
            }
            return *this;
        }

    private:
        bool in_array() {
            if (!nodes_stack_.empty()) {
                if (nodes_stack_.back()->IsArray()) {
                    return true;
                }
            }
            return false;
        }

        bool in_dict() {
            if (!nodes_stack_.empty()) {
                if (nodes_stack_.back()->IsDict()) {
                    return true;
                }
            }
            return false;
        }

        Node root_;
        std::vector<Node*> nodes_stack_;
        std::string current_key_;
    };
};
