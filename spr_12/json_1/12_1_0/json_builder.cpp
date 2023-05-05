#include <vector>
#include "json_builder.h"
#include <stdexcept>

namespace json {


        Node Builder::Build() const {
            return root_;
        }


        Builder& Builder::Key(const std::string& key) {
            if (in_dict()) {
                current_key_ = key;
            }
            else {
                throw std::logic_error("Get not a dict in Key func.");
            }
            return *this;
        }

        Builder& Builder::StartDict() {
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
            return *this;
        }

        Builder& Builder::EndDict() {
            if (in_dict()) {
                nodes_stack_.pop_back();
            }
            else {
                throw std::logic_error("Get not a EndDict in Key func.");
            }
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
            return *this;
        }

        Builder& Builder::EndArray() {
            if (in_array()) {
                nodes_stack_.pop_back();
            }
            else {
                throw std::logic_error("Get not a EndArray in Key func.");
            }
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
