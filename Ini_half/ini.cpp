#include "ini.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
using namespace std;


namespace ini {
    //using Section = std::unordered_map<std::string, std::string>;

    // методы Document 
    //class Document {
        //public:
    Section& Document::AddSection(std::string name) {
        return sections_[std::move(name)];
    }

    const Section& Document::GetSection(const std::string& name) const {
        static const Section empty_section;
        auto it = sections_.find(name);
        if (it == sections_.end()) {
            return empty_section;
        }
        return it->second;
    }

    std::size_t Document::GetSectionCount() const {
        return sections_.size();
    }


    Section Document::GetSectionSimple(const std::string& name) const {
        Section empty_section;
        auto it = sections_.find(name);
        if (it == sections_.end()) {
            return empty_section;
        }
        return it->second;
    }

    void Document::AddData(std::string& name, std::string& key,  std::string& value)  {
        sections_.at(name)[key] = value;
    }



    //private:
    //std::unordered_map<std::string, Section> sections_;
//};
/*    std::istringstream input{
        "[vegetables]\n"
        "potatoes=10\n"
        "onions=1 \n"
        "\n"
        "cucumbers=12\n"
        "\n"
        "[guests] \n"
        "guest1_name = Ivan Durak\n"
        "guest2_name =  Vasilisa Premudraya\n"
        "[guest black list]" };*/

    std::string RemSpace(std::string line) {
        size_t lastChar = line.find_last_not_of(" \t\n\v\f\r");
        if (lastChar != std::string::npos) {
            line.erase(lastChar + 1);
        }
        return line;
    }


    std::string EraseFirstSpace(std::string line) {
        while (!line.empty() && line[0] == ' ') {
            line.erase(0, 1);
        }

        return line;
    }

    Document Load(std::istream& input) {
        Document doc;
        std::string line;
        std::string current_section;

        while (std::getline(input, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }


            line = RemSpace(line);


            if (line[0] == '[' && line[line.size() - 1] == ']') {
                current_section = line.substr(1, line.size() - 2);
                doc.AddSection(current_section);
            }
            else {
                auto delimiter_pos = line.find('=');
                auto name = line.substr(0, delimiter_pos);
                auto value = line.substr(delimiter_pos + 1);
                name = RemSpace(name);
                value = EraseFirstSpace(value);
                //doc.GetSectionSimple(current_section).emplace(name, value
                doc.AddData(current_section, name, value);
            }
        }

        return doc;
    }
}
