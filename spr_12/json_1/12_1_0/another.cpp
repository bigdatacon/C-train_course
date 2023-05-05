#pragma once
#include <vector>
#include "json.h"

#include <stack>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

    class Builder {
    public:
        Builder() {
            nodes_stack_.push(&root_);
        }

        Builder& StartDict() {
            Dict dict;


            nodes_stack_.top()->emplace(std::move(current_key_), std::move(dict));
            current_key_.clear();
            nodes_stack_.push(&std::get<Dict>(*nodes_stack_.top())[current_key_]);
            return *this;
        }

        Builder& EndDict() {
            if (!nodes_stack_.top()->IsDict()) {
                throw std::logic_error("Can't end dictionary here");
            }
            nodes_stack_.pop();
            return *this;
        }

        Builder& StartArray() {
            Array array;
            nodes_stack_.top()->emplace(std::move(current_key_), std::move(array));
            current_key_.clear();
            nodes_stack_.push(&std::get<Array>(*nodes_stack_.top())[std::get<Array>(*nodes_stack_.top()).size() - 1]);
            return *this;
        }

        Builder& EndArray() {
            if (!nodes_stack_.top()->IsArray()) {
                throw std::logic_error("Can't end array here");
            }
            nodes_stack_.pop();
            return *this;
        }

        Builder& Key(std::string key) {
            current_key_ = std::move(key);
            return *this;
        }

        Builder& Value(const char* value) {
            return Value(std::string(value));
        }

        Builder& Value(std::string value) {
            nodes_stack_.top()->emplace(std::move(current_key_), std::move(value));
            current_key_.clear();
            return *this;
        }

        Builder& Value(std::nullptr_t) {
            nodes_stack_.top()->emplace(std::move(current_key_), nullptr);
            current_key_.clear();
            return *this;
        }

        Builder& Value(bool value) {
            nodes_stack_.top()->emplace(std::move(current_key_), value);
            current_key_.clear();
            return *this;
        }

        Builder& Value(int value) {
            nodes_stack_.top()->emplace(std::move(current_key_), value);
            current_key_.clear();
            return *this;
        }

        Builder& Value(double value) {
            nodes_stack_.top()->emplace(std::move(current_key_), value);
            current_key_.clear();
            return *this;
        }

        Node Build() {
            if (nodes_stack_.size() != 1) {
                throw std::logic_error("Builder is not finished");
            }
            return std::move(root_);
        }

    private:
        Node root_;
        std::stack<Node*> nodes_stack_;
        std::string current_key_;
    };

}  // namespace json
