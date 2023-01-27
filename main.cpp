int EffectiveCount(const vector<int>& v, int n, int i) {
    // место для вашего решения
    //1. нахожу номер позиции числа большего i
    auto pos_if = find_if( v.begin(), v.end(),  [i](int el) {return i < el;}) - v.begin();
    //auto pos = find( v.begin(), v.end(),  i);
    //cout << pos<< end;
    cout << pos_if<< endl;
    
    
    //2 определяю какой алгоритм выбрать  - для худшего или хорошего случая 
    auto good  = (pos_if <= log2(v.size())) || (pos_if <= static_cast<int64_t>(v.size())*(i + 1)/(n + 1)) ;
    if (good) {
        
    cout << "Using find_if"; return pos_if;}
    else {
        cout << "Using upper_bound";
        return upper_bound( v.begin(), v.end(),  i) - v.begin();
    }
    ///return *pos;     
}
