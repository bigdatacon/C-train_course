#pragma once
#include <vector>
#include "json.h"

namespace json {
    class Builder {
    public:
        Builder() {
        }

        Node Build() const;

        template<typename ValueType>
        Builder& Value(const ValueType& value) {
            if (in_array()) {
                nodes_stack_.back()->AsArray().push_back(value);
            }
            else if (in_dict()) {
                nodes_stack_.back()->AsDict()[current_key_] = value;
                current_key_.clear();
            }
            else if (on_top()) {
                root_ = value;
            }
            else {
                throw std::logic_error("Trying to put value while neither on top nor in array/dict");
            }
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
    };
}
