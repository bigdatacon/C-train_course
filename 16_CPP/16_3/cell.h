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
    
    class Impl {
    public:
        
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        
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
        
        explicit FormulaImpl(std::string text);
        Value GetValue() const override;
        std::string GetText() const override;
        
    private:
        std::unique_ptr<FormulaInterface> formula_ptr_;        
    };
    
    std::unique_ptr<Impl> impl_;
};
