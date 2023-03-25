#include <future>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
 
#include "test_framework.h"
 
using namespace std;
 
// Реализуйте шаблон Synchronized<T>.
// Метод GetAccess должен возвращать структуру, в которой есть поле T& ref_to_value.
template <typename T>
class Synchronized {
public:
    explicit Synchronized(T initial = T());
 
    /*struct Access {
        T& ref_to_value;
        mutex struct_value_mutex;
    };
    
    struct Access {
    Access() = default;
    Access(mutex value_mutex) {
        std::lock_guard<std::mutex> lock(value_mutex_);
        ref_to_value = value_;
    }
    ~ Access() {
    }
    T& ref_to_value;
    mutex struct_value_mutex*/
    
    
    struct Access {
	Access() = default;

	Access(T value_, mutex value_mutex_)
		: ref_to_value(value_)
		, struct_value_mutex(value_mutex_)
		 {
	}
        
    ~ Access() {
        std::unique_lock<std::mutex> lock(struct_value_mutex);
    }

    T& ref_to_value;
    mutex struct_value_mutex;
};

    
};
  
    /*Тип Access должен быть структурой, в которой есть поле T& ref_to_value, ссылающееся на поле value_ класса Synchronized<T>. Объект класса Access должен вести себя аналогично классу lock_guard<mutex>: захватывать мьютекс в своём конструкторе и освобождать в деструкторе.*/
    Access GetAccess(){ 
        std::lock_guard<std::mutex> lock(acc.struct_value_mutex);
        acc.ref_to_value = value_;
        return acc;
    };
    
 
private:
    T value_;
    mutex value_mutex_;
    Access acc;
};
 
void TestConcurrentUpdate() {
    Synchronized<string> common_string;
 
    const size_t add_count = 50000;
    auto updater = [&common_string, add_count] {
        for (size_t i = 0; i < add_count; ++i) {
            auto access = common_string.GetAccess();
            access.ref_to_value += 'a';
        }
    };
 
    auto f1 = async(updater);
    auto f2 = async(updater);
 
    f1.get();
    f2.get();
 
    ASSERT_EQUAL(common_string.GetAccess().ref_to_value.size(), 2 * add_count);
}
 
vector<int> Consume(Synchronized<deque<int>>& common_queue) {
    vector<int> got;
 
    for (;;) {
        deque<int> q;
 
        {
            // Мы специально заключили эти две строчки в операторные скобки, чтобы
            // уменьшить размер критической секции. Поток-потребитель захватывает
            // мьютекс, перемещает всё содержимое общей очереди в свою
            // локальную переменную и отпускает мьютекс. После этого он обрабатывает
            // объекты в очереди за пределами критической секции, позволяя
            // потоку-производителю параллельно помещать в очередь новые объекты.
            //
            // Размер критической секции существенно влияет на быстродействие
            // многопоточных программ.
            auto access = common_queue.GetAccess();
            q = move(access.ref_to_value);
        }
 
        for (int item : q) {
            if (item > 0) {
                got.push_back(item);
            } else {
                return got;
            }
        }
    }
}
 
void TestProducerConsumer() {
    Synchronized<deque<int>> common_queue;
 
    auto consumer = async(Consume, ref(common_queue));
 
    const size_t item_count = 100000;
    for (size_t i = 1; i <= item_count; ++i) {
        common_queue.GetAccess().ref_to_value.push_back(i);
    }
    common_queue.GetAccess().ref_to_value.push_back(-1);
 
    vector<int> expected(item_count);
    iota(begin(expected), end(expected), 1);
    ASSERT_EQUAL(consumer.get(), expected);
}
 
int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConcurrentUpdate);
    RUN_TEST(tr, TestProducerConsumer);
}
