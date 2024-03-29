#include "ini.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

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

    Section& Document::GetSectionSimple(const std::string& name) {
        static Section empty_section;
        empty_section.clear();
        auto it = sections_.find(name);
        if (it == sections_.end()) {
            return empty_section;
        }
        return it->second;
    }



    /* void Document::AddData(const std::string& name, const std::string& key,  const std::string& value)  {
         sections_.at(name)[key] = value;
     }*/



    std::string RemSpace(std::string str) {

        str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
            return !std::isspace(ch);
            }).base(), str.end());
        return str;
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
        std::string raw_val;
        std::string name;
        std::string value;

        std::string current_section;
        /*
        "   [first]\n"
        " a=b\n"
        "    b=a"
        */
        while (std::getline(input, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t pos_f_nospase = line.find_first_not_of(" ");
            size_t pos_l_nospase = line.find_last_not_of(" ");

            //line = RemSpace(line);
            //line = EraseFirstSpace(line);
            if (line[0] == '[' && line[line.size() - 1] == ']') {
                current_section = line.substr(1, line.size() - 2);
                doc.AddSection(current_section);
            }
            else {
                std::string excludedChars = " \t\r\n\f\v!@#$%^&*()-_=+[{]}\\|;:'   \",<.>/?";
                std::string inludedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
                size_t pos = line.find_first_not_of(inludedChars);
                if (pos != string::npos) {
                    auto name = line.substr(0, pos); // обрезаю строку для ключа 
                    auto raw_val = line.substr(pos); // обрезаю строку со значением до конца
                    size_t pos_2 = raw_val.find_first_not_of(excludedChars);
                    size_t pos_3 = raw_val.find_last_not_of(" \t\r\n\f\v");  //size_t pos_3 = raw_val.find_last_not_of(excludedChars);
                    if (pos_2 != string::npos && pos_3 != string::npos) {


                        auto value = raw_val.substr(pos_2, pos_3);
                        doc.GetSectionSimple(current_section).emplace(name, value);
                    }
                }




            }
        }

        return doc;
    }
}
