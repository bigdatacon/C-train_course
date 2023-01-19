#include "accountant.h"
#include "square_calculation.h" //- тут по идее не нужно раз есть в заголовочном файле 

    double CalcPaintNeeded(const Wall& wall) const {
        double height = wall.GetHeight();
        double width = wall.GetWidth();
        return CalcSquare(width, height) * 0.4;
    }

    double CalcBricksNeeded(const Wall& wall) const {
        double height = wall.GetHeight();
        double width = wall.GetWidth();
        return CalcSquare(width, height) * 5;
    }
