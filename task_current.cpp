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

	// вы можете дописывать необходимые для вашего решения методы
	void MoveDisks(int disks_num, Tower& destination, Tower& buffer) {
		if (destination.GetDisksNum() < disks_num) {
			/*if (GetDisksNum() == 1) {
				if (buffer.GetDisksNum() > 0) {
					destination.AddToTop(buffer.GetDisks().back());  // сначала перекидываю диск с buffer
					buffer.PopBack();    // удаляю диск с 1 башни 
					destination.AddToTop(GetDisks().back()); // перекидываю диск с первой башни 
					PopBack();  // удаляю диск с 1 башни 
				}
				else {
					destination.AddToTop(GetDisks().back());  // перекидываю диск с первой башни
					PopBack();
				}     // удаляю диск с 1 башни 
			}
			else if (GetDisksNum() == 0) {
				if (buffer.GetDisksNum() > 0) {
					destination.AddToTop(buffer.GetDisks().back());  // сначала перекидываю диск с buffer
					buffer.PopBack();    // удаляю диск с 1 башни 
					destination.AddToTop(GetDisks().back()); // перекидываю диск с первой башни 
					PopBack();  // удаляю диск с 1 башни 
				}
				}
			else {*/

				int current_disk = disks_.back(); // беру последний элемент 
				PopBack(); // удаляю последний элемент

				int sec_current_disk = disks_.back(); // беру предпоследний элемент 
				PopBack(); // удаляю предпоследний элемент

				// добавляю верхний диск на буффер а второй диск на destination 
				buffer.AddToTop(current_disk);
				destination.AddToTop(sec_current_disk);

				if (GetDisksNum() > 1) {

					buffer.PopBack(); // удаляю первый элемент из buffer и переношу его обратно на 1 башню 
					AddToTop(current_disk);

					destination.PopBack(); // удаляю предпоследний сверху диск с destination и переношу его на буффер 
					buffer.AddToTop(sec_current_disk);
				}
				else {
					destination.AddToTop(buffer.GetDisks().back());  // сначала перекидываю диск с buffer
					buffer.PopBack();   // удаляю диск с 1 башни 
					destination.AddToTop(GetDisks().back()); // перекидываю диск с первой башни 
					PopBack();  // удаляю диск с 1 башни 
				}

				// в целом тут цикл заканчивается так как в след итерации с 1 башни диск перенесется на буфер, а нижний на destination
				//далее верхний с буфера  перенесется на 1 башню 
			/* }*/
		}
		MoveDisks(GetDisksNum(), destination, buffer);
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
}
