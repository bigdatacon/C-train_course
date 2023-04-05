#pragma once
// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <deque>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <math.h>
#include <regex>


using namespace std;


ostream& operator<<(ostream& os, const AllBusInfoBusResponse& r); 

class Stat_reader {
public:

    istream& operator>>(istream& is, UpdateQuery& q);  
    void Output();
    
private:
    deque<pair <string, string>> deq_; // тут перечень запросов на вывод 
    };
