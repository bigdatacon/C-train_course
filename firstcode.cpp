#include <utility>

class Alibaba;

// Разбойник — это объект шаблонного класса
// Разбойники разных специализаций имеют разные шаблонные параметры
template <typename LootType>
class Thief {
public:
    void GreetAlibaba(Alibaba& alibaba);

    LootType ConvertMoneyToLoot(int money);

private:
    LootType loot_;
};

class Alibaba {
    // Али-Баба дружит с разбойниками всех специализаций
    // Конструкцию friend делаем шаблонной
    template <typename T>
    friend class Thief;

private:
    int money_ = 100;
};

template <typename T>
void Thief<T>::GreetAlibaba(Alibaba& alibaba) {
    int stolen = 0;
    // Благодаря дружбе с Али-Бабой разбойник получил доступ к 
    // его приватному полю и обнулил это поле
    std::swap(stolen, alibaba.money_);
    loot_ += ConvertMoneyToLoot(stolen);
}
