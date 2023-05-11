#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <numeric>

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
        quant_users_to_page_.resize(12);
    };
    void ReadInput(istream&  cin) {
        int n;
        cin >> n;
        user_and_page_.resize(n + 1);
        for (int i = 1; i <= n; i++) {
            string op;
            cin >> op;
            if (op == "CHEER") {
                int user_id;
                cin >> user_id;

                // Проверка наличия элемента в векторе
                if (!users_.count(user_id)) {
                    cout << 0 << endl;
                }
                else {
                    int current_page = user_and_page_[user_id];
                    if (current_page != 0) {
                        std::vector<int> slice(quant_users_to_page_.begin(), quant_users_to_page_.begin() + current_page);

                        // вычисление суммы элементов среза
                        double sum = std::accumulate(slice.begin(), slice.end(), 0);

                        double part = sum / (users_.size()-1);
                        if (users_.size() == 1) { cout << 1 << endl; }
                        else {

                            cout << part << endl;
                        }
                    }
                    else {

                        cout << 0 << endl;
                    }

                }
            }
            else if (op == "READ") {
                    int user_id, num_page;
                    cin >> user_id >> num_page;
                    // отнимаю юзера от предыдущей страницы чтобы не было задвоения 
                    if (user_and_page_[user_id]!=0) {
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

int main() {
#ifdef _DEBUG
    if (freopen("input.txt.txt", "r", stdin) == nullptr) {
        puts("can't open input.txt.txt");
        return 1;
    }
#endif 

    BookManager bm;
    bm.ReadInput(cin);

    return 0;
}
