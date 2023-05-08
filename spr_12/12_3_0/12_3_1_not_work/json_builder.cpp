#include "json_builder.h"

json::Builder::Builder() {
}

json::Node json::Builder::Build() const {
    if (on_top()) {
        return root_;
    }
    else {
        throw std::logic_error("Returning an incomplete document");
    }
}

json::DictValueContext json::Builder::Key(const std::string& key) {
    if (in_dict()) {
        current_key_ = key;
    }
    else {
        throw std::logic_error("Adding a key while not in dictionary");
    }
    return DictValueContext(this);
}

json::DictItemContext json::Builder::StartDict() {
    if (in_array()) {
        nodes_stack_.back()->AsArray().push_back(Dict());
        nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
    }
    else if (in_dict()) {
        nodes_stack_.back()->AsDict()[current_key_] = Dict();
        nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
    }
    else if (on_top()) {
        root_ = Dict();
        nodes_stack_.push_back(&root_);
    }
    else {
        throw std::logic_error("Starting a dictionary while neither on top nor in array/dict");
    }
    return DictItemContext(this);
}

json::BaseContext json::Builder::EndDict() {
    if (in_dict()) {
        nodes_stack_.pop_back();
    }
    else {
        throw std::logic_error("Ending a dictionary while not in dictionary");
    }
    return BaseContext(this);
}

json::ArrayItemContext json::Builder::StartArray() {
    if (in_array()) {
        nodes_stack_.back()->AsArray().push_back(Array());
        nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
    }
    else if (in_dict()) {
        nodes_stack_.back()->AsDict()[current_key_] = Array();
        nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
    }
    else if (on_top()) {
        root_ = Array();
        nodes_stack_.push_back(&root_);
    }
    else {
        throw std::logic_error("Starting an array while neither on top nor in array/dict");
    }
    return json::ArrayItemContext(this);
}

json::BaseContext json::Builder::EndArray() {
    if (in_array()) {
        nodes_stack_.pop_back();
    }
    else {
        throw std::logic_error("Ending an array while not in dictionary");
    }
    return json::BaseContext(this);
}

bool json::Builder::on_top() const {
    return nodes_stack_.empty();
}

bool json::Builder::in_array() const {
    if (!nodes_stack_.empty()) {
        if (nodes_stack_.back()->IsArray()) {
            return true;
        }
    }
    return false;
}

bool json::Builder::in_dict() const {
    if (!nodes_stack_.empty()) {
        if (nodes_stack_.back()->IsDict()) {
            return true;
        }
    }
    return false;
}

json::Node json::BaseContext::Build() {
    return builder_->Build();
}

json::DictValueContext json::BaseContext::Key(const std::string& key)
{
    return builder_->Key(key);
}

json::DictItemContext json::BaseContext::StartDict()
{
    return builder_->StartDict();
}

json::BaseContext json::BaseContext::EndDict()
{
    return builder_->EndDict();
}

json::ArrayItemContext json::BaseContext::StartArray()
{
    return builder_->StartArray();
}

json::BaseContext json::BaseContext::EndArray()
{
    return builder_->EndArray();
}
