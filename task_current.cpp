#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

class Tower {
public:
	// конструктор и метод SetDisks нужны, чтобы правильно создать башни
	Tower(int disks_num) {
		FillTower(disks_num);
	}

	int GetDisksNum() const {
		return disks_.size();
	}

	void SetDisks(int disks_num) {
		FillTower(disks_num);
	}

	vector<int> GetDisks() {
		return  disks_;
	}

	void PopBack() {
		disks_.pop_back();

	}

	ostream& operator << (ostream& os)
	{	
		for (auto el : disks_) { cout << el << " "; }
		return os;
	}

	void PrintDisks() { for (auto el : disks_) { cout << el << " "; } }


	// добавляем диск на верх собственной башни
	// обратите внимание на исключение, которое выбрасывается этим методом
	void AddToTop(int disk) {
		int top_disk_num = disks_.size() - 1;
		if (0 != disks_.size() && disk >= disks_[top_disk_num]) {
			throw invalid_argument("Невозможно поместить большой диск на маленький");
		}
		else {
			// допишите этот метод и используйте его в вашем решении
			disks_.push_back(disk);
		}
	}

	// 3:2:1  0   0
	//3:2   0  1
	//3   2   1  // выполняетяся 1 раз раскидываение 2 дисков 

	//3  2:1  0  // точка при которой на buffer n-1

	//0  2:1  3
	// 1  2  3
	//1 0  3:2
	//0  0  3:2:1







	// вы можете дописывать необходимые для вашего решения методы
	void MoveDisks(int disks_num, Tower& destination, Tower& buffer) {
		if (disks_num > 1) {

			while (buffer.GetDisksNum() < disks_num - 1) {
				int current_disk = disks_.back(); // беру последний элемент 
				PopBack(); // удаляю последний элемент

				int sec_current_disk = disks_.back(); // беру предпоследний элемент 
				PopBack(); // удаляю предпоследний элемент

				// добавляю верхний диск на буффер а второй диск на destination 
				destination.AddToTop(current_disk);
				buffer.AddToTop(sec_current_disk);
				//3   2   1 

				destination.PopBack(); // удаляю диск с destination и переношу его на буффер 
				buffer.AddToTop(current_disk);
				///3  2:1  0
			}
			//тут условие когда на source остался 1 диск 
			int current_disk = disks_.back(); // беру последний элемент 
			PopBack(); // удаляю последний элемент

			int buffer_disk = buffer.GetDisks().back(); // беру крайний элемент с buffer
			buffer.PopBack(); // удаляю предпоследний элемент

			destination.AddToTop(current_disk);
			//0  2:1  3

			AddToTop(buffer_disk);
			//1  2  3

			destination.AddToTop(buffer.GetDisks().back());  // сначала перекидываю диск с buffer
			buffer.PopBack();    // удаляю диск с 1 башни 

			//1  0 3:2

			destination.AddToTop(GetDisks().back()); // перекидываю диск с первой башни 
			PopBack();  // удаляю диск с 1 башни 
			//0 0 3:2:1



		}
		else {
			//1 0 0
			destination.AddToTop(GetDisks().back()); // перекидываю диск с первой башни 
			PopBack();  // удаляю диск с 1 башни }
			//0 0 1
		}
		//MoveDisks(GetDisksNum(), destination, buffer);
	}
private:
	vector<int> disks_;

	// используем приватный метод FillTower, чтобы избежать дубликации кода
	void FillTower(int disks_num) {
		for (int i = disks_num; i > 0; i--) {
			disks_.push_back(i);
		}
	}
};


void SolveHanoi(vector<Tower>& towers) {
	int disks_num = towers[0].GetDisksNum();

	// допишите функцию, чтобы на towers[0] было 0 дисков,
	// на towers[1] 0 дисков,
	// и на towers[2] было disks_num дисков
	// запускаем рекурсию
	 // просим переложить все диски на последнюю башню
	 // с использованием средней башни как буфера
	towers[0].MoveDisks(disks_num, towers[2], towers[1]);
}


int main() {
	//int towers_num = 5;
	//int disks_num = 5;
	int towers_num = 3;
	int disks_num = 3;
	vector<Tower> towers;
	// добавим в вектор три пустые башни
	for (int i = 0; i < towers_num; ++i) {
		towers.push_back(0);
	}
	// добавим на первую башню три кольца
	towers[0].SetDisks(disks_num);
	SolveHanoi(towers);

	//cout << towers[0] << endl;  -- почему этот оператор не работает 
	towers[0].PrintDisks();
	towers[1].PrintDisks();
	towers[2].PrintDisks();

}
