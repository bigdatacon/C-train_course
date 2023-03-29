#include <array>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

class VehiclePlate {
private:
	auto AsTuple() const {
		return tie(letters_, digits_, region_);
	}

public:
	bool operator==(const VehiclePlate& other) const {
		return AsTuple() == other.AsTuple();
	}

	VehiclePlate(char l0, char l1, int digits, char l2, int region)
		: letters_{ l0, l1, l2 }
		, digits_(digits)
		, region_(region) {
	}

	string ToString() const {
		ostringstream out;
		out << letters_[0] << letters_[1];
		out << setfill('0') << right << setw(3) << digits_;
		out << letters_[2] << setw(2) << region_;
		return out.str();
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

class VehiclePlateHasher {
public:
	size_t operator()(const VehiclePlate& plate) const {
		return hasher_(plate.ToString());
	}
private:
	hash<string> hasher_;
};

struct ParkingException {};

template <typename Clock>
class Parking {
	using Duration = typename Clock::duration;
	using TimePoint = typename Clock::time_point;

public:
	Parking(int cost_per_second) : cost_per_second_(cost_per_second) {}
	void Park(VehiclePlate car) {
		if (now_parked_.count(car)) {
			throw ParkingException();
		}
		else {
			now_parked_[car] = Clock::now();

		}
	}

	void Withdraw(const VehiclePlate& car) {
		if (now_parked_.count(car)==0) {
			throw ParkingException();
		}
		else {
			complete_parks_[car] += Clock::now()- now_parked_.at(car);
			now_parked_.erase(car); // удаление элемента по ключу

		}

	}

	// получить счёт за конкретный автомобиль
	int64_t GetCurrentBill(const VehiclePlate& car) const {
		// место для вашей реализации
		auto it = now_parked_.find(car);
		if (it != now_parked_.end()) {
			// Преобразование продолжительности времени в секунды
			auto end_time = Clock::now();
			auto duration_time = end_time - now_parked_.at(car);
			if (complete_parks_.count(car)) {
				duration_time += complete_parks_[car];
			}
			auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration_time);
			// Получение количества секунд
			auto seconds_count = seconds.count();
			int64_t money = cost_per_second_ * (seconds_count);
			return money;

		}
		else {
			auto it = complete_parks_.find(car); // ищу машину в завершенных парковках
			if (it != complete_parks_.end()) {
				auto seconds = std::chrono::duration_cast<std::chrono::seconds>(complete_parks_.at(car));
				// Получение количества секунд
				auto seconds_count = seconds.count();
				int64_t money = cost_per_second_ * (seconds_count);
				return money;
			}
			else {

				return 0;
			}
		}
	}

	unordered_map<VehiclePlate, int64_t, VehiclePlateHasher> EndPeriodAndGetBills() {
		// место для вашей реализации
		unordered_map<VehiclePlate, int64_t, VehiclePlateHasher> res;
		for (auto& pair : now_parked_) {
			int64_t money = GetCurrentBill(pair.first);
			if (money != 0) {
				res[pair.first] = money; //в итоговый map записываю сумму денег
			}
			now_parked_[pair.first] = Clock::now(); //обнуляю время для текущей машины что на стоянке

		}

		for (auto& pair : complete_parks_) {

			if (!res.count(pair.first)) {//если для машины не посчиаты деньги в цикле выше 
				int64_t money = GetCurrentBill(pair.first);
				if (money != 0) {
					res[pair.first] = money;; //в итоговый map записываю сумму денег
				}

			}
		}
		complete_parks_.clear(); // обнуляю complete_parks_
		return  res;
	}


	// не меняйте этот метод
	auto& GetNowParked() const {
		return now_parked_;
	}

	// не меняйте этот метод
	auto& GetCompleteParks() const {
		return complete_parks_;
	}

private:
	int cost_per_second_;
	unordered_map<VehiclePlate, TimePoint, VehiclePlateHasher> now_parked_;
	unordered_map<VehiclePlate, Duration, VehiclePlateHasher> complete_parks_;
};

// эти часы удобно использовать для тестирования
// они покажут столько времени, сколько вы задали явно
class TestClock {
public:
	using time_point = chrono::system_clock::time_point;
	using duration = chrono::system_clock::duration;

	static void SetNow(int seconds) {
		current_time_ = seconds;
	}

	static time_point now() {
		return start_point_ + chrono::seconds(current_time_);
	}

private:
	inline static time_point start_point_ = chrono::system_clock::now();
	inline static int current_time_ = 0;
};

int main() {
	Parking<TestClock> parking(10);

	TestClock::SetNow(10);
	parking.Park({ 'A', 'A', 111, 'A', 99 });

	TestClock::SetNow(20);
	parking.Withdraw({ 'A', 'A', 111, 'A', 99 });
	parking.Park({ 'B', 'B', 222, 'B', 99 });

	TestClock::SetNow(40);
	assert(parking.GetCurrentBill({ 'A', 'A', 111, 'A', 99 }) == 100);
	assert(parking.GetCurrentBill({ 'B', 'B', 222, 'B', 99 }) == 200);
	parking.Park({ 'A', 'A', 111, 'A', 99 });

	TestClock::SetNow(50);
	assert(parking.GetCurrentBill({ 'A', 'A', 111, 'A', 99 }) == 200);
	assert(parking.GetCurrentBill({ 'B', 'B', 222, 'B', 99 }) == 300);
	assert(parking.GetCurrentBill({ 'C', 'C', 333, 'C', 99 }) == 0);
	parking.Withdraw({ 'B', 'B', 222, 'B', 99 });

	TestClock::SetNow(70);
	{
		// проверим счёт
		auto bill = parking.EndPeriodAndGetBills();

		// так как внутри макроса используется запятая,
		// нужно заключить его аргумент в дополнительные скобки
		assert((bill
			== unordered_map<VehiclePlate, int64_t, VehiclePlateHasher>{
				{ {'A', 'A', 111, 'A', 99}, 400},
				{ {'B', 'B', 222, 'B', 99}, 300 },
		}));
	}

	TestClock::SetNow(80);
	{
		// проверим счёт
		auto bill = parking.EndPeriodAndGetBills();

		// так как внутри макроса используется запятая,
		// нужно заключить его аргумент в дополнительные скобки
		assert((bill
			== unordered_map<VehiclePlate, int64_t, VehiclePlateHasher>{
				{ {'A', 'A', 111, 'A', 99}, 100},
		}));
	}

	try {
		parking.Park({ 'A', 'A', 111, 'A', 99 });
		assert(false);
	}
	catch (ParkingException) {
	}

	try {
		parking.Withdraw({ 'B', 'B', 222, 'B', 99 });
		assert(false);
	}
	catch (ParkingException) {
	}

	cout << "Success!"s << endl;
}
