#include "cell.h"
#include "sheet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell(Sheet& sheet) : impl_(std::make_unique<EmptyImpl>()),
                           sheet_(sheet) {}
Cell::~Cell() = default;

void Cell::Set(std::string text, Position pos) {
    std::unique_ptr<Impl> iterim;
    pos_ = pos;


    Position pos_for_ref_cell;

    try {
        // Попытка отрезать первый символ
        pos_for_ref_cell  = Position::FromString(text.substr(1));
    } catch (const std::out_of_range& e) {
        // Обработка исключения, если указанный индекс находится за пределами строки
        std::cout << "Ошибка: " << e.what() << std::endl;
    }

    if (text.empty()) {
        iterim = std::make_unique<EmptyImpl>();
    } else if (text.size() >= 2 && text[0] == FORMULA_SIGN) {
        iterim = std::make_unique<FormulaImpl>(std::move(text), sheet_);
        auto new_iterim_formula = iterim->GetText();
    } else {
        iterim= std::make_unique<TextImpl>(std::move(text));
    }

    if (CheckCircularDependencies(*iterim, pos_for_ref_cell)) {
        throw CircularDependencyException("circular dependency detected");
    }

    // Обновление зависимостей
    //std::cout << "OBNIVLYAEM ZAVISIMOSTI DLYA : " << this->pos_.ToString() << std::endl;
    auto second_st = this->pos_.ToString();

    impl_ = std::move(iterim);

    for (Cell* used : using_cells_) {
        //std::cout << "USED : " << used->pos_.ToString() << std::endl;
        auto third_st = used->pos_.ToString();
        used->calculated_cells_.erase(this);
    }

    using_cells_.clear();

    auto iterim_st = impl_->GetText();
    for (const auto& pos : impl_->GetReferencedCells()) {
        //std::cout << "REFERENCED CELLAS  : " << pos_.ToString() << std::endl;
        auto four_st = pos_.ToString();
        Cell* used = sheet_.Get_Cell(pos);
        if (!used ) {
            sheet_.SetCell(pos, "");
            used  = sheet_.Get_Cell(pos);
        }
        auto five_st = used->pos_.ToString();
        auto six_st = used->GetValue();
        using_cells_.insert(used );
        used ->calculated_cells_.insert(this);
    }

    //impl_ = std::move(iterim);

    // Инвалидация кеша
    ResetCache(true);
}


bool Cell::CheckNumberInVector(const std::vector<int>& numbers, int n) const {
    while (true) {
        if (n < 10) {
            return false;
        }

        int lastDigit = n % 10;
        n = (n - lastDigit) / 10;

        if (std::find(numbers.begin(), numbers.end(), n) != numbers.end()) {
            return true;
        }
    }

    return false;
}

bool Cell::FindPairsInCalc(const Cell* dependent, const std::set<std::pair<const Cell*, int>>& calc_, int dependent_level) const {
    std::vector<int> matchingPairs;

    for (const auto& pair : calc_) {
        auto f_1_0 =  pair.first->pos_.col;
        auto f_1_1 =  pair.first->pos_.row;
        auto f_2_0 =  dependent->pos_.col;
        auto f_2_1 =  dependent->pos_.row;

        auto rr = pair.first->pos_==dependent->pos_;

        if (pair.first->pos_ == dependent->pos_) {
            matchingPairs.push_back(pair.second);
        }
    }
    return CheckNumberInVector(matchingPairs, dependent_level);
}

bool Cell::CheckCircularDependencies(const Impl& new_impl, Position pos_for_ref_cell) const {
    const auto& new_ref_cells = new_impl.GetReferencedCells();
    auto value_in = new_impl.GetValue();
    auto text_in = new_impl.GetText();


    if (!new_ref_cells.empty()) {
        std::set<std::pair<const Cell*, int>> calc_;
        std::vector<std::pair<const Cell*, int>> insert_;
        std::set<const Cell*> using_;
        std::vector<int> matching_cells;

        for (const auto& position : new_ref_cells) {
            auto pos_to_string = position.ToString();
            auto eight_st = position.ToString();
            //std::cout << "Ref CElls CYCLE DEPENDENT CELL FOR   : "<< text_in <<" : " << position.ToString() << std::endl;
            const Cell* ref_cell = sheet_.Get_Cell(position);
            if (ref_cell) {
                using_.insert(ref_cell);
            } else {
                sheet_.SetCell(position, "");
                using_.insert(sheet_.Get_Cell(position));
            }
        }
        //std::cout << "This Get Pos : " << this->pos_.ToString() << std::endl;
        auto ten_st = this->pos_.ToString();
        insert_.push_back(std::make_pair(this, 0));
        //alc_.insert(std::make_pair(new_impl, 0));  // add

        while (!insert_.empty()) {
            const Cell* current = insert_.back().first;
            int current_level = insert_.back().second;

            insert_.pop_back();
            calc_.insert(std::make_pair(current, current_level ));
            int i = 0;
            for (const Cell* dependent : current->calculated_cells_) {
                auto st_eleven = dependent->pos_.ToString();
                i +=1;
                int dependent_level = current_level*10+i;

                if (dependent->pos_ == pos_for_ref_cell){return true;}  // Найдена циклическая зависимость}

                if (!FindPairsInCalc( dependent,  calc_, dependent_level)) {
                    insert_.push_back(std::make_pair(dependent, dependent_level) );

                } else {
                    return true; // Найдена циклическая зависимость
                }
            }
        }
    }

    return false; // Циклических зависимостей не найдено
}




void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {return impl_->GetValue();}
std::string Cell::GetText() const {return impl_->GetText();}
std::vector<Position> Cell::GetReferencedCells() const {return impl_->GetReferencedCells();}
bool Cell::IsReferenced() const {return !calculated_cells_.empty();}

void Cell::ResetCache(bool flag = false) {
    if (impl_->HasCache() || flag) {
        impl_->ResetCache();

        for (Cell* dependent : calculated_cells_) {
            dependent->ResetCache();
        }
    }
}

std::vector<Position> Cell::Impl::GetReferencedCells() const {return {};}
bool Cell::Impl::HasCache() {return true;}
void Cell::Impl::ResetCache() {}

Cell::Value Cell::EmptyImpl::GetValue() const {return "";}
std::string Cell::EmptyImpl::GetText() const {return "";}

Cell::TextImpl::TextImpl(std::string text) : text_(std::move(text)) {}

Cell::Value Cell::TextImpl::GetValue() const {

    if (text_.empty()) {
        throw FormulaException("it is empty impl, not text");

    } else if (text_.at(0) == ESCAPE_SIGN) {
        return text_.substr(1);

    } else {
        return text_;
    }
}

std::string Cell::TextImpl::GetText() const {return text_;}

Cell::FormulaImpl::FormulaImpl(std::string text, SheetInterface& sheet) : formula_ptr_(ParseFormula(text.substr(1)))
        , sheet_(sheet) {}

Cell::Value Cell::FormulaImpl::GetValue() const {

    if (!cache_) {cache_ = formula_ptr_->Evaluate(sheet_);}
    return std::visit([](auto& helper){return Value(helper);}, *cache_);
}

std::string Cell::FormulaImpl::GetText() const {return FORMULA_SIGN + formula_ptr_->GetExpression();}
std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {return formula_ptr_->GetReferencedCells();}
bool Cell::FormulaImpl::HasCache() {return cache_.has_value();}
void Cell::FormulaImpl::ResetCache() {cache_.reset();}