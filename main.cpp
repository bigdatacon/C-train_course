#include <iostream>
#include <vector>

using namespace std;

template <class T>
double mean(const std::vector<T>& v) {
    T acc;
    const T* v_ptr = v.data();

    for (size_t i = 0; i <= v.size(); ++i) {
        acc += v_ptr[i];
    }

    return static_cast<double>(acc) / v.size();
}

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    cout << "Mean: "s << mean(v) << endl;
}
