#pragma once
#include <vector>
#include <optional>
#include "json.h"

namespace json {

    class Builder {
    public:
        class BaseContext;
        class DictValueContext;
        class DictItemContext;
        class ArrayItemContext;

    public:
        class BaseContext {
        public:
            explicit BaseContext(Builder* builder) { builder_ = builder; }

            Node Build();

            BaseContext Value(const json::Node& value);

            DictValueContext Key(const std::string& key);

            DictItemContext StartDict();
            BaseContext EndDict();
            ArrayItemContext StartArray();
            BaseContext EndArray();

        protected:
            Builder* builder_;
        };


        class DictValueContext : public BaseContext {
        public:
            explicit DictValueContext(Builder* builder) : BaseContext(builder) {}

            Node Build() = delete;

            DictItemContext Value(const json::Node& value);

            DictValueContext Key(const std::string& key) = delete;
            BaseContext EndArray() = delete;
            BaseContext EndDict() = delete;
        };


        class DictItemContext : public BaseContext {
        public:
            explicit DictItemContext(Builder* builder) : BaseContext(builder) {}

            Node Build() = delete;

            BaseContext Value(const json::Node& value) = delete;
            DictItemContext& StartDict() = delete;
            ArrayItemContext& StartArray() = delete;
            BaseContext EndArray() = delete;
        };


        class ArrayItemContext : public BaseContext {
        public:
            explicit ArrayItemContext(Builder* builder) : BaseContext(builder) {}

            Node Build() = delete;

            ArrayItemContext Value(const json::Node& value);

            DictValueContext Key(const std::string& key) = delete;
            BaseContext EndDict() = delete;
        };
    public:
        Builder();

        Node Build() const;

        BaseContext Value(const json::Node& value);
        DictValueContext Key(const std::string& key);

        template<typename Container>
        DictItemContext StartDictOrArrayInner(Container value) {
            if (in_array()) {
                nodes_stack_.back()->AsArray().push_back(value);
                nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
            }
            else if (in_dict()) {
                if (current_key_) {
                    nodes_stack_.back()->AsDict()[current_key_.value()] = value;
                    nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_.value()]);
                    current_key_.reset();
                }
                else {
                    throw std::logic_error("Inserting a value with no key");
                }
            }
            else if (on_top()) {
                root_ = value;
                nodes_stack_.push_back(&root_);
            }
            else {
                throw std::logic_error("Starting a dictionary while neither on top nor in array/dict");
            }
            return DictItemContext(this);
        }

        DictItemContext StartDict();



        BaseContext EndDict();
        ArrayItemContext StartArray();
        BaseContext EndArray();

    private:
        bool on_top() const;
        bool in_array() const;
        bool in_dict() const;

        Node root_;
        std::vector<Node*> nodes_stack_;
        std::optional<std::string> current_key_;
    };
}
