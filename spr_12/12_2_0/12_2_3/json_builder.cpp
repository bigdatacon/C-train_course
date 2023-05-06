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



    DictItemContextKey Builder::Key(const std::string& key) {
        if (in_dict()) {
            current_key_ = key;
            have_key_ = true;
        }
        else {
            throw std::logic_error("Adding a key while not in dictionary");
        }
        return DictItemContextKey(*this);
    }

    DictItemContext Builder::StartDict() {
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
        return DictItemContext(*this);
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

    Builder& Builder::StartArray() {
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

    DictItemContextKey DictItemContext::Key(const std::string& key) {
        return builder_.Key(key);
    }


}
