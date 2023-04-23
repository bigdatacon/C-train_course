#include "stat_reader.h"
#include "geo.h"
#include "transport_catalogue.h"
#include "input_reader.h"
#include "json_reader.h"

using namespace std;

/*
int main() {
	
	 * Примерная структура программы:
	 *
	 * Считать JSON из stdin
	 * Построить на его основе JSON базу данных транспортного справочника
	 * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
	 * с ответами.
	 * Вывести в stdout ответы в виде JSON
	 
}
*/

int main()
{
	ReadInputJsonRequest();
}



/*
int main() {
    std::ifstream inFile("input.json.txt"); // открытие файла для чтения
    if (!inFile) { // проверка успешности открытия файла
        std::cerr << "Ошибка при открытии файла "<< std::endl;
        return 1;
    }

    std::string full_line;
    std::string line;
    while (std::getline(inFile, line)) { 
        full_line += line;
    }
    
    inFile.close(); // закрытие файла
    std::cout << "eto fulL_line : " << full_line << std::endl;

    return 0;
}*/
