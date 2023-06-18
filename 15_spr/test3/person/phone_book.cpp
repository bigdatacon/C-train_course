#include "phone_book.h"
 
using namespace std;
 
 
PhoneBook::PhoneBook(std::vector<Contact> contacts) : contacts_(contacts) {};
 
PhoneBook::ContactRange PhoneBook::FindByNamePrefix(std::string_view name_prefix) const {
    std::sort(contacts_.begin(), 
              contacts_.end(), [](const Contact& lhs, const Contact& rhs) {
                                  return lhs.name < rhs.name;
                                });
    
    if (name_prefix.size() == 0) {
        return {contacts_.begin(), contacts_.end()};
        
    } else {
        
        Iterator begin_cont = std::lower_bound(contacts_.begin(), 
                                         contacts_.end(), 
                                          name_prefix, [](const Contact& contact, std::string_view prefix) {
                                                          return prefix > contact.name;
                                                       });
        
        Iterator end_cont = std::upper_bound(begin_cont, 
                                        contacts_.end(), 
                                        name_prefix, [](std::string_view prefix, const Contact& contact) {
                                                        int helper = contact.name.compare(0, prefix.size(), prefix);
                                                        if (helper == 0) {
                                                            return false;
                                                            
                                                        } else {
                                                            return prefix < contact.name;
                                                        }
                                                     });
        return {begin_cont, end_cont};
    }   
};
 
void PhoneBook::SaveTo(std::ostream& output) const {
    phone_book_serialize::ContactList contact_list;
    for (const Contact contact : contacts_) {
        phone_book_serialize::Contact contact_from_phone_book;
        contact_from_phone_book.set_name(contact.name);
        if (contact.birthday.has_value()) {         
            contact_from_phone_book.mutable_birthday()->set_day(contact.birthday->day);
            contact_from_phone_book.mutable_birthday()->set_month(contact.birthday->month);
            contact_from_phone_book.mutable_birthday()->set_year(contact.birthday->year);
            
        }
/*		else {     
            for (const std::string& phone_number : contact.phones) {
                contact_from_phone_book.add_phone_number(phone_number);
            }
            contact_list.mutable_contact()->Add(std::move(contact_from_phone_book));
        }*/      

		for (const std::string& phone_number : contact.phones) {
                contact_from_phone_book.add_phone_number(phone_number);
            }
            contact_list.mutable_contact()->Add(std::move(contact_from_phone_book));
    }
    
    contact_list.SerializeToOstream(&output);
}
 
PhoneBook DeserializePhoneBook(std::istream& input) {
    
    phone_book_serialize::ContactList contact_list;
    
    if (contact_list.ParseFromIstream(&input)) {
        
        std::vector<Contact> contact_list_vect(contact_list.contact_size());
 
        for (size_t i = 0; i < contact_list.contact_size(); ++i) {
            contact_list_vect[i].name = contact_list.contact(i).name();
            
            if (contact_list.contact(i).has_birthday()) {
                              
                Date birth_date = {contact_list.contact(i).birthday().year(), 
                                   contact_list.contact(i).birthday().month(), 
                                   contact_list.contact(i).birthday().day()};
                
                contact_list_vect[i].birthday.emplace(birth_date);    
            } else {
                
                for (size_t j = 0; j < contact_list.contact(i).phone_number_size(); ++j) {
                    contact_list_vect[i].phones.push_back(contact_list.contact(i).phone_number(j));
                }
            }  
        }
        return PhoneBook(contact_list_vect);
    }
    
    return PhoneBook({});
}
