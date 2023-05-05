#include <vector>
#include "json_builder.h"
#include <stdexcept>

namespace json {


    Node Builder::Build() const {
        /*if (nodes_stack_.size() != 1) {
            throw std::logic_error("Build called with incomplete object");
        }*/
        //is_built_ = true;
        //is_not_first_operation_ = true;
        return root_;
    }


    Builder& Builder::Key(const std::string& key) {
        if (!is_dict_) {
            throw std::logic_error("Call Key method behinde dict.");
        }
        if (in_dict()) {
            current_key_ = key;
        }
        else {
            throw std::logic_error("Get not a dict in Key func.");
        }
        is_not_first_operation_ = true;
        return *this;
    }

    Builder& Builder::StartDict() {
        if ((!is_array_ && !is_dict_) && is_not_first_operation_) {
            throw std::logic_error("Wrong context.");
        }
        if (in_array()) {
            nodes_stack_.back()->AsArray().push_back(json::Dict());
            nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
        }
        else if (in_dict()) {
            nodes_stack_.back()->AsDict()[current_key_] = Dict();
            nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
            current_key_.clear();
        }
        else {
            root_ = Dict();
            nodes_stack_.push_back(&root_);
        }
        is_dict_ = true;
        //is_array_ = true;
        is_not_first_operation_ = true;
        return *this;
    }

    Builder& Builder::EndDict() {
        if ((!is_array_ && !is_dict_) && is_not_first_operation_) {
            throw std::logic_error("Wrong context.");
        }
        if (in_dict()) {
            nodes_stack_.pop_back();
        }
        else {
            throw std::logic_error("Get not a EndDict in Key func.");
        }
        is_dict_ = false;
        //is_array_ = false;
        is_not_first_operation_ = true;
        return *this;
    }

    Builder& Builder::StartArray() {
        if (in_array()) {
            nodes_stack_.back()->AsArray().push_back(Array());
            nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
        }
        else if (in_dict()) {
            nodes_stack_.back()->AsDict()[current_key_] = Array();
            nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
            current_key_.clear();
        }
        else {
            root_ = Array();
            nodes_stack_.push_back(&root_);
        }
        is_array_ = true;
        is_not_first_operation_ = true;
        return *this;
    }

    Builder& Builder::EndArray() {
        if (in_array()) {
            nodes_stack_.pop_back();
        }
        else {
            throw std::logic_error("Get not a EndArray in Key func.");
        }
        is_array_ = false;
        is_not_first_operation_ = true;
        return *this;
    }


    bool Builder::in_array() {
        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsArray()) {
                return true;
            }
        }

        return false;
    }

    bool Builder::in_dict() {
        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsDict()) {
                return true;
            }
        }
        return false;
    }

}
