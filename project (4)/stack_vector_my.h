#pragma once

#include <array>
#include <stdexcept>

using namespace std; 

template <typename T, size_t N>
class StackVector {
public:
    //Конструктор принимает размер вектора — по аналогии со стандартным вектором. Если аргумент конструктора больше вместимости вектора, конструктор должен выбрасывать исключение invalid_argument
    explicit StackVector(size_t a_size = 0) {
      if (a_size > Capacity()){
          throw invalid_argument("invalid_argument");}
      else {
           //array<T, a_size>  array_;
          T array_[a_size] = {};   // вот так нашел в интернете 
       }
       
        
    } ;

    T& operator[](size_t index){
        return array_[index];
    };
    const T& operator[](size_t index) const{
        return array_[index];} 

    /*std::array<T, size>::iterator*/ auto  begin(){return array_.begin();}
    /*std::array<T, size>::iterator*/ auto end(){return array_.end();}
    /*std::array<T, size>::iterator*/ auto begin() const {return array_.cbegin();}
    /*std::array<T, size>::iterator*/ auto end() const {return array_.cend();}

    size_t Size() const {
        return array_.size();
    };
    size_t Capacity() const {
        return array_.capacity();
    };

    void PushBack(const T& value){
        
        
    };
    T PopBack();
        auto el_to_del = *end();
        delete array_[el_to_del]
    
private:
    size_t array_size_;
    array<T, size_t>  array_; // vec.capacity()
    
};
