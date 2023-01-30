#include "search_server.h"
#include "remove_duplicates.h"
#include "document.h"
#include "string_processing.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <deque>
#include <numeric>


void RemoveDuplicates(SearchServer& search_server) {
    for (auto i = search_server.begin(); i != search_server.end(); i++) {
        for (auto j = i + 1; j != search_server.end(); j++) {
            if (search_server.GetId_words()[*i] == search_server.GetId_words()[*j]) {
                std::cout << "Found duplicate document id " << *j << std::endl;
                search_server.RemoveDocument(*j);
                j--;
            }
        }
    }
}

