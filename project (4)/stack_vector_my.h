#pragma once

#include <array>
#include <stdexcept>

using namespace std; 

template <typename T, size_t N>
class StackVector {
public:
    //Конструктор принимает размер вектора — по аналогии со стандартным вектором. Если аргумент конструктора больше вместимости вектора, конструктор должен выбрасывать исключение invalid_argument
    explicit StackVector(size_t a_size = 0) {
        if (a_size > N) {
            throw invalid_argument("invalid_argument");
        }
        array_size_ = a_size;
    }

    T& operator[](size_t index)
    {
        return array_[index];
    }
    const T& operator[](size_t index) const
    {
        return array_[index];
    }
    
    typename array<T, N>::iterator begin(){return array_.begin();}
    typename array<T, N>::iterator end(){return array_.end();}
    typename array<T, N>::const_iterator begin() const {return array_.cbegin();}
    typename array<T, N>::const_iterator end() const {return array_.cend();}


    size_t Size() const {
        return array_size_;
    }
    size_t Capacity() const {
        return N;
    }

    void PushBack(const T& value){
        if (array_size_ >= N) {
            throw overflow_error("push to a full stack");
        }
        array_[array_size_] = value;
        array_size_++;
    }

    T PopBack() {
        if (!array_size_) {
            throw underflow_error("pop from an empty stack");
        }
        T value = array_[array_size_ - 1];
        array_size_--;
        return value;
    }

private:
    size_t array_size_;
    std::array<T, N>  array_; // vec.capacity()
};
