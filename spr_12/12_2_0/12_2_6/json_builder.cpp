#include <vector>
#include "json_builder.h"

namespace json {


    Node Builder::Build() const {
        if (on_top() && have_something_) {
            return root_;
        }
        else {
            throw std::logic_error("Returning an incomplete document");
        }
    }



    BaseContex Builder::Key(const std::string& key) {
        if (in_dict()) {
            current_key_ = key;
            have_key_ = true;
        }
        else {
            throw std::logic_error("Adding a key while not in dictionary");
        }
        return BaseContex(*this);
    }

    BaseContex Builder::StartDict() {
        have_something_ = true;
        if (in_array()) {
            nodes_stack_.back()->AsArray().push_back(Dict());
            nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
        }
        else if (in_dict()) {
            if (have_key_) {
                nodes_stack_.back()->AsDict()[current_key_] = Dict();
                nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
                have_key_ = false;
            }
            else {
                throw std::logic_error("Adding dict to dict with no key");
            }
        }
        else if (on_top()) {
            root_ = Dict();
            nodes_stack_.push_back(&root_);
        }
        else {
            throw std::logic_error("Starting a dictionary while neither on top nor in array/dict");
        }
        return BaseContex(*this);
    }

    Builder& Builder::EndDict() {
        if (in_dict()) {
            if (have_key_) {
                throw std::logic_error("Ending a dictionary with no value for last key");
            }
            nodes_stack_.pop_back();
        }
        else {
            throw std::logic_error("Ending a dictionary while not in dictionary");
        }
        return *this;
    }

    BaseContex Builder::StartArray() {
        have_something_ = true;
        if (in_array()) {
            nodes_stack_.back()->AsArray().push_back(Array());
            nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
        }
        else if (in_dict()) {
            if (have_key_) {
                nodes_stack_.back()->AsDict()[current_key_] = Array();
                nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
                have_key_ = false;
            }
            else {
                throw std::logic_error("Adding array to dict with no key");
            }
        }
        else if (on_top()) {
            root_ = Array();
            nodes_stack_.push_back(&root_);
        }
        else {
            throw std::logic_error("Starting an array while neither on top nor in array/dict");
        }
        return BaseContex(*this);
    }

    Builder& Builder::EndArray() {
        if (in_array()) {
            nodes_stack_.pop_back();
        }
        else {
            throw std::logic_error("Ending an array while not in dictionary");
        }
        return *this;
    }


    bool Builder::on_top() const {
        return nodes_stack_.empty();
    }

    bool Builder::in_array() const {
        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsArray()) {
                return true;
            }
        }
        return false;
    }

    bool Builder::in_dict() const {
        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsDict()) {
                return true;
            }
        }
        return false;
    }


    // СЂРµР°Р»РёР·Р°С†РёСЏ С„СѓРЅРєС†РёР№ BaseContex
    BaseContex::BaseContex(Builder& builder) : builder_(builder) {}

    Node BaseContex::Build() const {
        return builder_.Build();
    }

    BaseContex BaseContex::Key(const std::string& key) {
        return  builder_.Key(key);
    }

    BaseContex BaseContex::StartDict() {
        return  builder_.StartDict();
    }
    Builder& BaseContex::EndDict() {
        return  builder_.EndDict();
    }

    BaseContex BaseContex::StartArray() {
        return  builder_.StartArray();
    }
    Builder& BaseContex::EndArray() {
        return  builder_.EndArray();
    }


    // 1 Р РµР°Р»РёР·Р°С†РёСЏ РґР»СЏ РєР»Р°СЃСЃР° DictKeyContext
    //DictItemContextKey::DictItemContextKey(Builder& builder) : builder_(builder) {}
    template<typename ValueType>
    Builder DictKeyContext::Value(const ValueType& value) {
        return BaseContex::Value(value);
    }
    BaseContex DictKeyContext::StartDict() { return  BaseContex::StartDict(); }
    BaseContex DictKeyContext::StartArray() { return  BaseContex::StartArray(); }

    // 2 Р РµР°Р»РёР·Р°С†РёСЏ РґР»СЏ РєР»Р°СЃСЃР° DictValueContext
    BaseContex DictValueContext::Key(const std::string& key) { return BaseContex::Key(key); }


    Builder& DictValueContext::EndDict() { return  BaseContex::EndDict(); }

    // 3 Р РµР°Р»РёР·Р°С†РёСЏ РґР»СЏ РєР»Р°СЃСЃР°  DictStartArrayContex

    template<typename ValueType>
    Builder  DictStartArrayContex::Value(const ValueType& value) { return BaseContex::Value(value); }

    BaseContex  DictStartArrayContex::StartDict() { return  BaseContex::StartDict(); }
    BaseContex  DictStartArrayContex::StartArray() { return  BaseContex::StartArray(); }
    Builder& DictStartArrayContex::EndArray() { return  BaseContex::EndArray(); }




}
