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




int main()
{
	/*TransportCatalogue tc;
    InputReader reader(std::cin);
    int count = reader.GetNumUpdateQueries();
    //std::cout << "COUNT : " << count << std::endl;
	reader.FillRequests();

	reader.GetUpdStop(tc);
	reader.GetUpdBus(tc);

	string b = "750"s;

	tc.GetAllBusInfo(b);

	StatReader streader(std::cin);
	int count_out = streader.GetNumOutQueries();
	streader.FillRequests();
	streader.Output(tc);*/


	//сравниваю файлы 
    ifstream file1("file1.txt");
    ifstream file2("file2.txt");

    //ifstream file1("file11.txt");
    //ifstream file2("file22.txt");

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

    cout << "Files are identical" << endl;

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
