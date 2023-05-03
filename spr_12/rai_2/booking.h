#pragma once

template <typename Provider>
class Booking {
private:
    using BookingId = typename Provider::BookingId;

    Provider* provider_;
    ???

public:
    Booking(Provider* p, ???);
    Booking(const Booking&);
    Booking(Booking&& other);
    Booking& operator=(const Booking&);
    Booking& operator=(Booking&& other);
    ~Booking();

    // Эта функция не требуется в тестах, но в реальной программе она может быть нужна
    BookingId GetId() const;   
};