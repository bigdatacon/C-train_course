#include <vector>
#include "json_builder.h"

namespace json {


        Node Builder::Build() const {
            if (on_top()) {
                return root_;
            }
            else {
                throw std::logic_error("Returning an incomplete document");
            }
        }



        Builder& Builder::Key(const std::string& key) {
            if (in_dict()) {
                current_key_ = key;
            }
            else {
                throw std::logic_error("Adding a key while not in dictionary");
            }
            return *this;
        }

        Builder& Builder::StartDict() {
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
            return *this;
        }

        Builder& Builder::EndDict() {
            if (in_dict()) {
                nodes_stack_.pop_back();
            }
            else {
                throw std::logic_error("Ending a dictionary while not in dictionary");
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
            }
            else if (on_top()) {
                root_ = Array();
                nodes_stack_.push_back(&root_);
            }
            else {
                throw std::logic_error("Starting an array while neither on top nor in array/dict");
            }
            return *this;
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


    
}
