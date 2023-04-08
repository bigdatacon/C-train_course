#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <deque>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <math.h>
#include <regex>
#include <cmath>
#include <unordered_set>
#include "stat_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "input_reader.h"

using namespace std;

/*
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
*/

//Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka
/*
Stop Tolstopaltsevo 3900m to Marushkino
Stop Marushkino: 9900m to Rasskazovka, 100m to Marushkino
Stop Rasskazovka: 9500m to Marushkino

туда : 3900 + 100+9900 = 13 900
обратно: 9500 + 100 + 3900 = 13500
Итого: 27 400
Ответ: 27 400
*/




int main()
{
	TransportCatalogue tc;
    InputReader reader(std::cin);
     (void) reader.GetNumUpdateQueries();
    //std::cout << "COUNT : " << count << std::endl;
	reader.FillRequests();

	reader.GetUpdStop(tc);
	reader.GetUpdBus(tc);
    reader.GetUpdStopDist(tc);

	//string b = "750"s;

	//tc.GetAllBusInfo(b);

	StatReader streader(std::cin);
	(void)  streader.GetNumOutQueries();
	streader.FillRequests();
	streader.Output(tc);


	//сравниваю файлы 
    //ifstream file1("file1.txt");
    //ifstream file2("file2.txt");


    // закоментировано сравнение фалов , так как делается отдельно от основного вывода. Чтобы запустить сравнение, нужно закомментировать все до строки 50 и раскомментировать 
    // до строки 82 При этом сравниваются пары файлов, то есть отдельно 1 и 2 и отдельно 11 и 22, соответственно нужно или чтобы были закомментированы 50,51 или 57,58

    /*ifstream file1("file11.txt");
    ifstream file2("file22.txt");

    string line1, line2;
    int line_num = 1;

    while (getline(file1, line1) && getline(file2, line2)) {
        if (line1 != line2) {
            cout << "Files differ at line " << line_num << endl;
            cout << "line first ff : " << line1 << endl;
            cout << "line secon ff : " << line2 << endl;
            return 0;
        }
        line_num++;
    }

    if (file1.eof() != file2.eof()) {
        cout << "Files differ in size" << endl;
        return 0;
    }

    cout << "Files are identical" << endl;*/



    /*cout << "/////////////////////////////////////// READ File1 " << endl;

    std::ifstream file("file1.txt");  // открываем файл для чтения
    std::string line;
    while (std::getline(file, line)) {  // читаем строки по очереди
        std::cout << line << std::endl;  // выводим каждую строку на экран
    }
     
    cout << "/////////////////////////////////////// READ File2 " << endl;
    std::ifstream file_s("file2.txt");  // открываем файл для чтения
    std::string line_s;
    while (std::getline(file_s, line2)) {  // читаем строки по очереди
        std::cout << line2 << std::endl;  // выводим каждую строку на экран
    }

    cout << "/////////////////////////////////////// READ File11 " << endl;
    std::ifstream file_t("file11.txt");  // открываем файл для чтения
    std::string line3;
    while (std::getline(file_t, line3)) {  // читаем строки по очереди
        std::cout << line3 << std::endl;  // выводим каждую строку на экран
    }


    cout << "/////////////////////////////////////// READ File22 " << endl;
    std::ifstream file_f("file22.txt");  // открываем файл для чтения
    std::string line4;
    while (std::getline(file_f, line4)) {  // читаем строки по очереди
        std::cout << line4 << std::endl;  // выводим каждую строку на экран
    }*/

    return 0;
}