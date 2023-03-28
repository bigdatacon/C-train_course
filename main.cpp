#include <array>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

/*
Используйте в классе HashableContainer одно поле типа vector<vector<T>>.
operator== для класса VehiclePlate должен проверять равенство всех компонентов номера.
При добавлении номера примените алгоритм find, чтобы выяснить, дублируется ли он.
*/

class VehiclePlate {
public:
    VehiclePlate(char l0, char l1, int digits, char l2, int region)
        : letters_{ l0, l1, l2 }
        , digits_(digits)
        , region_(region) {
    }

    string ToString() const {
        ostringstream out;
        out << letters_[0] << letters_[1];
        // чтобы дополнить цифровую часть номера слева нулями
        // до трёх цифр, используем подобные манипуляторы:
        // setfill задаёт символ для заполнения,
        // right задаёт выравнивание по правому краю,
        // setw задаёт минимальное желаемое количество знаков
        out << setfill('0') << right << setw(3) << digits_;
        out << letters_[2] << setw(2) << region_;

        return out.str();
    }

    int Hash() const {
        return digits_;
    }

    /*bool operator==(const VehiclePlate& rhs) {
        return letters_ == rhs.letters_ &&
            digits_ == rhs.digits_ &&
            region_ == rhs.region_;
    }*/

    bool operator==(const VehiclePlate& other) const {
        return ToString()[0] == other.ToString()[0]
            && ToString()[1] == other.ToString()[1]
            && ToString()[2] == other.ToString()[2]
            && ToString()[3] == other.ToString()[3]
            && ToString()[4] == other.ToString()[4]
            && ToString()[5] == other.ToString()[5];
    }


private:
    array<char, 3> letters_;
    int digits_;
    int region_;
};

ostream& operator<<(ostream& out, VehiclePlate plate) {
    out << plate.ToString();
    return out;
}



bool operator==(const VehiclePlate& l, const VehiclePlate& r) {
    return  l.ToString()[0] == r.ToString()[0] 
        && l.ToString()[1] == r.ToString()[1]
    && l.ToString()[2] == r.ToString()[2]
    && l.ToString()[3] == r.ToString()[3]
    && l.ToString()[4] == r.ToString()[4]
    && l.ToString()[5] == r.ToString()[5];
    //&& {l.[6] == r[6]}
    //&& {l.[0] == r[0]}

}

template <typename T>
class HashableContainer {
public:
    void Insert(T elem) {
        int index = elem.Hash();

        // если вектор недостаточно велик для этого индекса,
        // то увеличим его, выделив место с запасом
        if (index >= int(elements_.size())) {
            elements_.resize(index * 2 + 1);
        }

        /*auto it = std::find(elements_[index].begin(), elements_[index].end(), elem);
        if (it == = elements_[index].end()) {
            elements_[index] = move(elem);
        }*/
        auto& vec = elements_[index];
        if (find(vec.begin(), vec.end(), elem) == vec.end()) {
            vec.push_back(elem);
        }


        void PrintAll(ostream & out) const {
            for (auto& e : elements_) {
                if (!e.has_value()) {
                    continue;
                }
                out << e.value() << endl;
            }
        }

        const auto& GetVector() const {
            return elements_;
        }

private:
    //vector<optional<T>> elements_;
    vector<vector<T>> elements_;
    };

    int main() {
        HashableContainer<VehiclePlate> plate_base;
        plate_base.Insert({ 'B', 'H', 840, 'E', 99 });
        plate_base.Insert({ 'O', 'K', 942, 'K', 78 });
        plate_base.Insert({ 'O', 'K', 942, 'K', 78 });
        plate_base.Insert({ 'O', 'K', 942, 'K', 78 });
        plate_base.Insert({ 'O', 'K', 942, 'K', 78 });
        plate_base.Insert({ 'H', 'E', 968, 'C', 79 });
        plate_base.Insert({ 'T', 'A', 326, 'X', 83 });
        plate_base.Insert({ 'H', 'H', 831, 'P', 116 });
        plate_base.Insert({ 'P', 'M', 884, 'K', 23 });
        plate_base.Insert({ 'O', 'C', 34, 'P', 24 });
        plate_base.Insert({ 'M', 'Y', 831, 'M', 43 });
        plate_base.Insert({ 'K', 'T', 478, 'P', 49 });
        plate_base.Insert({ 'X', 'P', 850, 'A', 50 });

        plate_base.PrintAll(cout);
    }
