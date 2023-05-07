#include "json_builder.h"
#include <iostream>


using namespace std;

int main() {
    //json::Builder{}.StartDict().Build();
    //json::Builder{}.StartDict().StartArray();
    //json::Builder{}.StartDict().Value();

    json::Builder b;
    b.StartDict().Key("key").Value(1).Value(2).EndDict();
    b.StartDict().Key("key").Value(1).EndArray().EndDict();
    b.StartDict().Key("key").Value(1).Build();
    b.StartDict().Key("key").Value(1).StartArray().EndDict();
}
