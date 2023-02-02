#include <cassert>
#include <stdexcept>


/*
В операциях * и -> сначала проверьте сырой указатель на равенство nullptr и выбросьте исключение logic_error в случае, когда эти операции вызваны у пустого ScopedPtr. Если всё хорошо, верните ссылку или указатель на объект в зависимости от реализуемой операции.
В операции приведения к bool верните false, если ScopedPtr ссылается на nullptr, и true в ином случае.
*/


// Умный указатель, удаляющий связанный объект при своём разрушении. 
// Параметр шаблона T задаёт тип объекта, на который ссылается указатель
template <typename T>
class ScopedPtr {
public:
    // Конструктор по умолчанию создаёт нулевой указатель,
    // так как поле ptr_ имеет значение по умолчанию nullptr
    ScopedPtr() = default;

    // Создаёт указатель, ссылающийся на переданный raw_ptr.
    // raw_ptr ссылается либо на объект, созданный в куче при помощи new,
    // либо является нулевым указателем
    // Спецификатор noexcept обозначает, что метод не бросает исключений
    explicit ScopedPtr(T* raw_ptr) noexcept {
        // Реализуйте самостоятельно
        //1. В конвертирующем конструкторе сохраните переданный «‎сырой»‎ указатель в поле ptr_.
        ptr_ = raw_ptr; 
    }
    
    // Удаляем у класса конструктор копирования
    ScopedPtr(const ScopedPtr&) = delete;

    // Деструктор. Удаляет объект, на который ссылается умный указатель.
    ~ScopedPtr() {
        // Реализуйте тело деструктора самостоятельно
        // 3.Удалите объект, на который ссылается поле ptr_ в деструкторе.
        delete ptr_;
    }

    // Возвращает указатель, хранящийся внутри ScopedPtr
    T* GetRawPtr() const noexcept {
        // Напишите код метода самостоятельно
        //2. В методе GetRawPtr верните текущее значение ptr_.
        return ptr_;
    }

    // Прекращает владение объектом, на который ссылается умный указатель.
    // Возвращает прежнее значение "сырого" указателя и устанавливает поле ptr_ в null
    T* Release() noexcept {
        T * p = ptr_;
        // Реализуйте самостоятельно
        //4.В методе Release обнулите поле ptr_, не удаляя сам объект, и верните прежнее значение поля. 
        ptr_ = nullptr;
        return p;
    }
    
    //--------------------------------------------------------------------------------НОВОЕ
        // Оператор приведения к типу bool позволяет узнать, ссылается ли умный указатель
    // на какой-либо объект
    explicit operator bool() const noexcept {
        // Реализуйте самостоятельно
        //1.В операции приведения к bool верните false, если ScopedPtr ссылается на nullptr, и true в ином случае.
        if (!ptr_) {return false;}
        else {return true;}
    }
 
    // Оператор разыменования возвращает ссылку на объект
    // Выбрасывает исключение std::logic_error, если указатель нулевой
    T& operator*() const {
        // Реализуйте самостоятельно
        //2.В операциях * и -> сначала проверьте сырой указатель на равенство nullptr и выбросьте исключение logic_error в случае, когда эти операции вызваны у пустого ScopedPtr. Если всё хорошо, верните ссылку или указ  throw std::logic_error( " logic_error vector empty " );
        // форма try также не работает 
        /*try {
            ptr_ != nullptr;
            }
        } catch (const logic_error&) {
            throw;
        }*/
        if (ptr_ = nullptr){throw logic_error( " logic_error vector empty " );}
        return &ptr_;
    }

    // Оператор -> должен возвращать указатель на объект
    // Выбрасывает исключение std::logic_error, если указатель нулевой
    T* operator->() const {
        // Реализуйте самостоятельно
        if (ptr_ = nullptr){throw logic_error( " logic_error vector empty " );}
        return ptr_;
    }
    

private:
    T* ptr_ = nullptr;
};

// Этот main тестирует класс ScopedPtr
int main() {
    // Проверка работы оператора приведения к типу bool
    {
        // Для нулевого указателя приведение к типу bool возвращает false
        const ScopedPtr<int> empty_ptr;
        assert(!empty_ptr);

        // Для ненулевого указателя приведение к типу bool возвращает true
        const ScopedPtr<int> ptr_to_existing_object(new int(0));
        assert(ptr_to_existing_object);

        static_assert(noexcept(static_cast<bool>(ptr_to_existing_object)));
    }

    // Проверка работы оператора разыменования *
    {
        string* raw_ptr = new string("hello");
        ScopedPtr<string> smart_ptr(raw_ptr);
        // Ссылка, возвращаемая оператором разыменования, должна ссылаться на объект,
        // на который указывает умный указатель
        assert(&*smart_ptr == raw_ptr);

        try {
            ScopedPtr<int> empty_ptr;
            // При попытке разыменовать пустой указатель должно быть выброшено
            // исключение logic_error
            *empty_ptr;
            // Сюда попасть мы не должны
            assert(false);
        } catch (const logic_error&) {
            // мы там, где нужно
        } catch (...) {
            // Других исключений выбрасываться не должно
            assert(false);
        }
    }

    // Проверка работы оператора ->
    {
        string* raw_ptr = new string("hello");
        ScopedPtr<string> smart_ptr(raw_ptr);
        // Доступ к членам класса через умный указатель должен быть аналогичен
        // доступу через "сырой" указатель
        assert(smart_ptr->data() == raw_ptr->data());

        try {
            ScopedPtr<string> empty_ptr;
            // При попытке разыменовать пустой указатель должно быть выброшено
            // исключение logic_error
            empty_ptr->clear();
            // Сюда попасть мы не должны
            assert(false);
        } catch (const logic_error&) {
            // мы там, где нужно
        } catch (...) {
            // Других исключений выбрасываться не должно
            assert(false);
        }
    }

    // Пример использования
    {
        // На этой структуре будет проверяться работа умного указателя
        struct Object {
            Object() {
                cout << "Object is default constructed"s << endl;
            }
            void DoSomething() {
                cout << "Doing something"s << endl;
            }
            ~Object() {
                cout << "Object is destroyed"s << endl;
            }
        };

        // Сконструированный по умолчанию указатель ссылается на nullptr
        ScopedPtr<Object> empty_smart_ptr;
        // Перегруженный оператор приведения к типу bool вернёт false для пустого указателя
        assert(!empty_smart_ptr);

        ScopedPtr<Object> smart_ptr(new Object());
        // Перегруженный оператор bool вернёт true для указателя, ссылающегося на объект
        assert(smart_ptr);

        // Проверка оператора разыменования
        (*smart_ptr).DoSomething();
        // Проверка оператора доступа к членам класса
        smart_ptr->DoSomething();
    }
}

    
    
    
