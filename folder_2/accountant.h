#pragma once
//#include "square_calculation.h"   -- это то что раньше было 
//-- тут убираю по аналогии с carpenter.h то есть функции 
//которые используются оставляю только в определяемом файле 

#include "wall.h"

class Accountant {
public:
    double CalcPaintNeeded(const Wall& wall) const ;
    double CalcBricksNeeded(const Wall& wall) const ;
    
    // это то что раньше было 
    /*
    double CalcPaintNeeded(const Wall& wall) const {
        double height = wall.GetHeight();
        double width = wall.GetWidth();
        return CalcSquare(width, height) * 0.4;
    }

    double CalcBricksNeeded(const Wall& wall) const {
        double height = wall.GetHeight();
        double width = wall.GetWidth();
        return CalcSquare(width, height) * 5;
    }*/
};
