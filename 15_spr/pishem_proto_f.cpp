-----------------------------------------------------------------------
library.proto
-----------------------------------------------------------------------
syntax = "proto3";
 
message Author {
    string first_name = 1;
    string last_name = 2;
    uint32 birth_year = 3;
}
 
message Publishing {
    string name = 1;
    string address = 2;
} 
 
message Date {
    uint32 year = 1;
    uint32 month = 2;
    uint32 day = 3;
}
 
message User {
    uint32 id = 1;
    string name = 2;
    Date registration_date = 3;
}
 
message Book {
    Publishing publishing = 1;
    repeated Author author = 2;
    uint32 year = 3;
    string name = 4;
    Date borrow_date = 5;
    Date return_date = 6;
    repeated uint32 holder = 7;
}
 
message Library {
    repeated Book book = 1;
    repeated User reader = 2;
}
