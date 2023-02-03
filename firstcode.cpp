#include "octopus.h"

#include <cassert>
#include <iostream>

using namespace std;

/*
Используйте классы ScopedPtr и PtrVector. С ними класс Octopus станет заметно проще.
Когда щупальца хранятся внутри PtrVector, создавать собственный конструктор копирования и деструктор для класса Octopus не придётся — конструктор копирования PtrVector корректно скопирует щупальца, а деструктор удалит их.
*/

template <typename T>
class ScopedPtr {
public:
    ScopedPtr() = default;
    explicit ScopedPtr(T* raw_ptr) noexcept {
        ptr_ = raw_ptr; 
    }
    ScopedPtr(const ScopedPtr&) = delete;
    ~ScopedPtr() {
        delete ptr_;
    }
    T* GetRawPtr() const noexcept {
        return ptr_;
    }
    T* Release() noexcept {
        T * p = ptr_;
        ptr_ = nullptr;
        return p;
    }
    explicit operator bool() const noexcept {
        if (!ptr_) {return false;}
        else {return true;}
    }
    T& operator*() const {
        if (!ptr_){throw logic_error( " logic_error vector empty " );}
        return *ptr_;
    }
    T* operator->() const {
        if (!ptr_ ){throw logic_error( " logic_error vector empty " );}
        return ptr_;
    }
private:
    T* ptr_ = nullptr;
};

//----------PtrVector
template <typename T>
class PtrVector {
public:
    PtrVector() = default;
    PtrVector(const PtrVector& other) {
        items_.reserve(other.size());
        for (const auto item : other.items_) {
            if(item) {
                items_.push_back(*item );
            } else {
                items_.push_back(nullptr);
            }
        }
    }
    ~PtrVector() {
        for (const auto item : items_) {
        delete item;
    }
    }
    vector<T*>& GetItems() noexcept {
       return items_;
    }
    vector<T*> const& GetItems() const noexcept {
        return items_;
    }
private:
    vector<T*> items_;
    int size() const {
            return items_.size();
        }
        };


int main() {
    // Проверка конструирования осьминогов
    {
        // По умолчанию осьминог имеет 8 щупалец
        Octopus default_octopus;
        assert(default_octopus.GetTentacleCount() == 8);

        // Осьминог может иметь отличное от 8 количество щупалец
        Octopus quadropus(4);
        assert(quadropus.GetTentacleCount() == 4);

        // И даже вообще не иметь щупалец
        Octopus coloboque(0);
        assert(coloboque.GetTentacleCount() == 0);
    }

    // Осьминогу можно добавлять щупальца
    {
        Octopus octopus(1);
        Tentacle* t0 = &octopus.GetTentacle(0);
        Tentacle* t1 = &octopus.AddTentacle();
        assert(octopus.GetTentacleCount() == 2);
        Tentacle* t2 = &octopus.AddTentacle();
        assert(octopus.GetTentacleCount() == 3);

        // После добавления щупалец ранее созданные щупальца не меняют своих адресов
        assert(&octopus.GetTentacle(0) == t0);
        assert(&octopus.GetTentacle(1) == t1);
        assert(&octopus.GetTentacle(2) == t2);

        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            assert(octopus.GetTentacle(i).GetId() == i + 1);
        }
    }

    // Осьминоги могут прицепляться к щупальцам друг друга
    {
        Octopus male(2);
        Octopus female(2);

        assert(male.GetTentacle(0).GetLinkedTentacle() == nullptr);

        male.GetTentacle(0).LinkTo(female.GetTentacle(1));
        assert(male.GetTentacle(0).GetLinkedTentacle() == &female.GetTentacle(1));

        male.GetTentacle(0).Unlink();
        assert(male.GetTentacle(0).GetLinkedTentacle() == nullptr);
    }

    // Копия осьминога имеет свою собственную копию щупалец, которые
    // копируют состояние щупалец оригинального осьминога
    {
        // Перебираем осьминогов с разным количеством щупалец
        for (int num_tentacles = 0; num_tentacles < 10; ++num_tentacles) {
            Octopus male(num_tentacles);
            Octopus female(num_tentacles);
            // Пусть они хватают друг друга за щупальца
            for (int i = 0; i < num_tentacles; ++i) {
                male.GetTentacle(i).LinkTo(female.GetTentacle(num_tentacles - 1 - i));
            }

            Octopus male_copy(male);
            // Проверяем состояние щупалец копии
            assert(male_copy.GetTentacleCount() == male.GetTentacleCount());
            for (int i = 0; i < male_copy.GetTentacleCount(); ++i) {
                // Каждое щупальце копии размещается по адресу, отличному от адреса оригинального щупальца
                assert(&male_copy.GetTentacle(i) != &male.GetTentacle(i));
                // Каждое щупальце копии прицепляется к тому же щупальцу, что и оригинальное
                assert(male_copy.GetTentacle(i).GetLinkedTentacle() == male.GetTentacle(i).GetLinkedTentacle());
            }
        }
        // Если вы видите эту надпись, то разрушение осьминогов, скорее всего,
        // прошло без неопределённого поведения
        cout << "Everything is OK"s << endl;
    }
}
