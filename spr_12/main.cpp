#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <numeric>
#include <iomanip>
#include <fstream> // добавленный заголовочный файл



using namespace std;

/*
12
CHEER 5
READ 1 10
CHEER 1
READ 2 5
READ 3 7
CHEER 2
CHEER 3
READ 3 10
CHEER 3
READ 3 11
CHEER 3
CHEER 1
*/

/*
0
1
0
0.5
0.5
1
0.5
*/

class BookManager {

public:
	BookManager() {
		quant_users_to_page_.resize(1025); //  с учетом максимальных значений данных в задании
		user_and_page_.resize(100001);
	};
	void ReadInput(istream& cin) {
		int n;
		cin >> n;
		//user_and_page_.resize(n + 1);
		std::ofstream fout("from_func.txt");
		for (int i = 1; i <= n; i++) {
			string op;
			cin >> op;
			if (op == "CHEER") {
				int user_id;
				cin >> user_id;

				int current_page = user_and_page_[user_id];
				if (current_page != 0) {

					if (users_.size() == 1) {
						fout << 1 << endl;
					}

					// вычисление суммы элементов среза
					else {
						std::vector<int> slice(quant_users_to_page_.begin(), quant_users_to_page_.begin() + current_page);
						double sum = std::accumulate(slice.begin(), slice.end(), 0);
						double result = sum / (users_.size() - 1);

						fout << std::fixed << std::setprecision(6) << result << std::endl;
					}
				}
				else {
					//fout << fixed << setprecision(6) << (double)0 << endl;
					fout << 0 << endl;
				}

			}
			else if (op == "READ") {
				int user_id, num_page;
				cin >> user_id >> num_page;
				// отнимаю юзера от предыдущей страницы чтобы не было задвоения 
				if (user_and_page_[user_id] != 0) {
					quant_users_to_page_[user_and_page_[user_id]] -= 1;

				}

				// добавляю новые данные
				user_and_page_[user_id] = num_page; // по индексу страницы записываю юзера 
				quant_users_to_page_[num_page] += 1; // увеличиваю количество дочитавших до стр на 1
				if (!users_.count(user_id)) {
					users_.insert(user_id);
				}
			}
		}

	}


private:
	vector<int> user_and_page_;
	vector<int> quant_users_to_page_;
	set<int> users_;
};

bool CompareFiles(const std::string& file1, const std::string& file2)
{
	std::ifstream f1(file1);
	std::ifstream f2(file2);

	if (!f1.is_open() || !f2.is_open()) {
		std::cerr << "Unable to open files." << std::endl;
		return false;
	}

	std::string line1, line2;
	int lineNum = 1;
	bool filesMatch = true;

	while (getline(f1, line1) && getline(f2, line2))
	{
		if (line1 != line2)
		{
			std::cout << "Line " << lineNum << " does not match." << std::endl;
			std::cout << "File1_test: " << line1 << std::endl;
			std::cout << "File2_my: " << line2 << std::endl;
			filesMatch = false;
		}

		lineNum++;
	}

	if (!filesMatch) {
		std::cout << "Files do not match." << std::endl;
	}
	else {
		std::cout << "Files match." << std::endl;
	}

	return filesMatch;
}





int main() {
#ifdef _DEBUG
	if (freopen("input.txt.txt", "r", stdin) == nullptr) {
		puts("can't open input.txt.txt");
		return 1;
	}
#endif 

	BookManager bm;
	bm.ReadInput(cin);

	std::string filename1 = "out.txt";
	std::string filename2 = "from_func.txt";
	if (CompareFiles(filename1, filename2)) {
		std::cout << "The files are identical" << std::endl;
	}
	else {
		std::cout << "The files are different" << std::endl;
	}
	return 0;
}
