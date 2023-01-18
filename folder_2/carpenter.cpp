// Определите методы класса здесь

#include "wall.h"  
#include "square_calculation.h"  

class Carpenter{
public:
    int CalcShelves(const Wall& wall) const {
        double height = wall.GetHeight();
        double width = wall.GetWidth();
        return CalcSquare(width, height) / 2  ;
    }
};
