В этой пасте приведен пример моих заголовочных файлов, которые были сданы на дизайн-ревью. 
В них добавлены предполагаемые сигнатуры новых методов программы.

При сдаче вам потребуется также предоставить pdf-диаграммы взаимосвязей в вашем проекте, а именно:

	formula_calculation.pdf
	cyclic_dependencies.pdf
	cache_invalidation.pdf
	new_cell.pdf

Прикрепляю ссылку на примерные pdf-диаграммы для дизайн-ревью: https://github.com/uetto/design-review-pdf

-----------------------------------------------------------------------
cell.h
-----------------------------------------------------------------------
#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:

    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;

    set<Cell*> dependent_cells_;		//ячейки в которых вычисляем (меняем их значения)
    set<Cell*> referenced_cells_;		//ячейки используемые при вычислении (берем их значения)

    mutable std::optional<Value> cache_;      	//вычисленное значение ячейки хранящееся в кеше 

    class Impl {
    public:

        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const = 0;

        virtual ~Impl() = default;
    };
    
    class EmptyImpl : public Impl {
    public:    
  
        Value GetValue() const override;
        std::string GetText() const override;      
    };
    
    class TextImpl : public Impl {
    public:
        
        explicit TextImpl(std::string text); 
        Value GetValue() const override;       
        std::string GetText() const override;
        
    private:
        std::string text_;        
    };
    
    class FormulaImpl : public Impl {
    public:
        
        explicit FormulaImpl(std::string text, const SheetInterface& sheet);
        Value GetValue() const override;
        std::string GetText() const override;

	std::vector<Position> GetReferencedCells() const override;
        
    private:
        std::unique_ptr<FormulaInterface> formula_ptr_; 
    };

    
    bool HasCircularDependency(Impl& impl);
    void InvalidateCache();
    void RefreshCells();
};
-----------------------------------------------------------------------
formula.h
-----------------------------------------------------------------------
#pragma once

#include "common.h"

#include "FormulaAST.h"

#include <memory>
#include <variant>

class FormulaInterface {
public:
    using Value = std::variant<double, FormulaError>;

    virtual Value Evaluate(const SheetInterface& sheet) const = 0;
    virtual std::string GetExpression() const = 0;
    virtual std::vector<Position> GetReferencedCells() const = 0;

    virtual ~FormulaInterface() = default;
};

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);
-----------------------------------------------------------------------
FormulaAST.h
-----------------------------------------------------------------------
#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {
class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(std::function<double(Position* pos)> cell_pos_func) const;
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;

    std::list<Position>& GetCells();

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
    std::list<Position> cells_;
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);
-----------------------------------------------------------------------
sheet.h
-----------------------------------------------------------------------
#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <vector>

using Table = std::vector<std::vector<std::unique_ptr<Cell>>>;

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    Table cells_;
};
