#include "wall.h"

class Painter {
public:
    // Напишите класс Painter
    void Paint(const Wall& wall, Wall::Color color){
        wall_color_ = wall.GetColor()::color;
        
    }
private:
    Color wall_color_;


};
