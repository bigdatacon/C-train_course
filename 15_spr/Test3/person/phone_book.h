#pragma once
 
#include "iterator_range.h"
 
#include <optional>
#include <sstream>
#pragma once
 
#include "iterator_range.h"
 
#include <string>
#include <string_view>
#include <vector>
 
#include <contact.pb.h>
 
struct Date {int year, month, day;};
 
struct Contact {
    std::string name;
    std::optional<Date> birthday;
    std::vector<std::string> phones;
};
 
class PhoneBook {
public:
    explicit PhoneBook(std::vector<Contact> contacts);
 
    using Iterator = std::vector<Contact>::iterator;;
    using ContactRange = IteratorRange<Iterator>;
 
    ContactRange FindByNamePrefix(std::string_view name_prefix) const;
    void SaveTo(std::ostream& output) const;
    
private:
    mutable std::vector<Contact> contacts_;
};
 
PhoneBook DeserializePhoneBook(std::istream& input);