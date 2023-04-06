// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"
#include "transport_catalogue.h"



int main()
{
    InputReader reader(std::cin);
    int count = reader.getNumUpdateQueries();
    std::cout << "COUNT : " << count << std::endl;
    for (int i = 0; i < count; i++) {
        UpdateQuery q = reader.getUpdateQuery();
        std::cout << "Q : " << q.toString() << std::endl;
    }
    return 0;
}
