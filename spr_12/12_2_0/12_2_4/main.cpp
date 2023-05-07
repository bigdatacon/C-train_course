#include "json_builder.h"
#include <iostream>


using namespace std;

int main() {
 json::Builder{}.StartDict().Build();
    json::Builder{}.StartDict().StartArray();
    json::Builder{}.StartDict().Value();
        

}
