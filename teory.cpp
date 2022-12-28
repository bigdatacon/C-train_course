#include <iostream>
#include <string>
#include <cassert>

using namespace std;



template <typename T>
void RunTestImpl(const T & func, const string & name) {
    //cerr << "Starting tests for " << name << endl;
    func();
    cerr << "Ending tests for " << name << endl;
}

#define RUN_TEST(func) RunTestImpl(func, #func)

void Test1() {
    //cerr << "A message from Test1()" << endl;
    assert(2==2);
}

int main() {
    RUN_TEST(Test1);
}

