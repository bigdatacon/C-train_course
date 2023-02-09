
#include <cassert>
#include <vector>
#include <list>
#include <iostream>
#include <iterator>

using namespace std;

/*
Вам понадобятся два списка: один для хранения текста, а другой — для буфера вставки. Итератор — удобное решение для хранения текущей позиции курсора.
std::list<int>::iterator range_begin = c.begin();
*/
class Editor {
public:
    Editor(){};
    // сдвинуть курсор влево
    void Left() {
    
        if (it_ != text_base_.begin()) {
            --it_;
        }
    }
    void Right() {
        // сдвинуть курсор вправо
    
        if (it_ != text_base_.end()) {
            ++it_;
        }
    }

    
    void Insert(char token) {
        // вставить символ token
        text_base_.insert(it_, token);
        //Right();
    }
        /*
    Курсор не смещается ни при копировании, ни при вырезании текста. Например, после вырезания из текста ab|cdef фрагмента из трёх символов получим текст ab|f.
    */
    
    void Cut(size_t tokens = 1) {
        for (size_t i = 1; i <= tokens && it_ != text_base_.end(); ++i) {
            text_buff_.push_back(*it_);
            //text_base_.erase(it_);
            it_ = text_base_.erase(it_);
            //Left();
            cout << "Текущее состояние редактора в CUT : " << GetText() << endl;
        }
    }


    void Copy(size_t tokens = 1) {
        text_buff_.clear();
        for (size_t i = 1; i <= tokens; ++i) {
            text_buff_.push_back(*it_);
            //Right();
        }
    }

    // вставить содержимое буфера в текущую позицию курсора
    void Paste() {
        for (char el: text_buff_) {
            text_base_.insert(it_, el);
        }
    }


    // получить текущее содержимое текстового редактора
    string GetText() const {
        string res=""s;
        for (char el : text_base_) {
            res +=el;
        }
        return res;
    }


private:
    list<char> text_base_; //один для хранения текста
    list<char> text_buff_; //а другой — для буфера вставки
    std::list<char>::iterator it_= text_base_.begin(); //Итератор — удобное решение для хранения текущей позиции курсора. Делаю на 1 позицию по умолчанию
};
int main() {
    Editor editor;
    const string text = "hello, world"s;
    for (char c : text) {
        editor.Insert(c);
    }
    // Текущее состояние редактора: `hello, world|`
    cout << "Текущее состояние редактора: `hello, world|` : " << editor.GetText() << endl;
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    // Текущее состояние редактора: `|hello, world`
    cout << "Текущее состояние редактора: `|hello, world` : " << editor.GetText() << endl;
    editor.Cut(7);
    // Текущее состояние редактора: `|world`
    cout << "Текущее состояние редактора: `|world` : " << editor.GetText() << endl;
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    // Текущее состояние редактора: `world|`
    cout << "Текущее состояние редактора до insert: `world|` : " << editor.GetText() << endl;
    editor.Insert(',');
    editor.Insert(' ');
    // Текущее состояние редактора: `world, |`
    cout << "Текущее состояние редактора: `world, |` :    " << endl;
    editor.Paste();
    // Текущее состояние редактора: `world, hello, |`
 
    cout << "Текущее состояние редактора: `world, |` :    " << endl;
    editor.Left();
    editor.Left();
    //Текущее состояние редактора: `world, hello|, `
    cout << "Текущее состояние редактора: `world, hello|, ` :    " << endl;
    editor.Cut(3);  // Будут вырезаны 2 символа
    // Текущее состояние редактора: `world, hello|`
    cout << "Текущее состояние редактора: `world, hello|`   " << endl;
    cout << editor.GetText();
    return 0;
}

/*
int main() {

	list<int> numbers = {1, 2, 3, 4};
    list<char> chars = { 's', 'r'};
	 std::list<int>::iterator it = find(numbers.begin(), numbers.end(), 3);
    std::list<char>::iterator cht = find(chars.begin(), chars.end(), 's');
	numbers.erase(it);
	cout << "end " << endl;
    
}
*/
