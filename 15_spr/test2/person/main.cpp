#include <person.pb.h>

#include <iostream>
using namespace std;

int main() {
    Address addr;
    addr.set_building(15);
    cout << addr.building() << endl;
}