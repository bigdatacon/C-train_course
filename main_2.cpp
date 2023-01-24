#include "log_duration.h"

#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace std;

vector<float> ComputeAvgTemp(const vector<vector<float>>& measures) {
    // место для вашего решения
    int q_external = measures.size();  /// количество векторов внутри вектора векторов measures
    int q_internal = measures[0].size();  // количество измерений внутри внутренного вектора 
    vector<float> result(q_internal);  // вектор с измерениями по количечтву измерений в векторе 
    
    for (int j = 0; j < q_internal; ++j){ // начинаю проход по вектору векторов , но сначала определяю индекс 
        // элемента который буду брать это j
        float sum_observ = 0;
        float quant_observ = 0;
        for (int i = 0; i < q_external; ++i){ // тут иду по вектору векторов и суммирую measures[i][j]
            if (measures[i][j] > 0){sum_observ+=measures[i][j]; ++quant_observ; }     
        }
        // тут закончен цикл и у каждого вложенного вектора проверен и учтен элемен measures[i][j],
        // перед переходом к j+1 обрабатываю полученные данные и записываю из в вектор
        if (quant_observ >0) {result.push_back(sum_observ /quant_observ );}
        else {result.push_back(0.0);}
        
    }
    return result;
    
}

vector<float> GetRandomVector(int size) {
    static mt19937 engine;
    uniform_real_distribution<float> d(-100, 100);

    vector<float> res(size);
    for (int i = 0; i < size; ++i) {
        res[i] = d(engine);
    }

    return res;
}

int main() {
    vector<vector<float>> data;
    data.reserve(5000);

    for (int i = 0; i < 5000; ++i) {
        data.push_back(GetRandomVector(5000));
    }

    vector<float> avg;
    {
        LOG_DURATION("ComputeAvgTemp"s);
        avg = ComputeAvgTemp(data);
    }

    cout << "Total mean: "s << accumulate(avg.begin(), avg.end(), 0.f) / avg.size() << endl;
}
