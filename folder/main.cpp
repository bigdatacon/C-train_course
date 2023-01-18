#include "builder.h"
#include "wall.h"
#include <iostream>


int main() {
    Builder tom;
    double wall_height = 3, wall_width = 2;
    Wall wall(wall_height, wall_width );
    // Попросите Тома посчитать кирпичи для стены 3.5 х 2.45
 
    const auto num_of_bricks = tom.CalcBricksNeeded(wall);
    cout << num_of_bricks;
}
