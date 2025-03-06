#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"
#ifndef MouseMovementLibrary_H_
#define MouseMovementLibrary_H_

namespace movement{
    enum Direction {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3};
    int getXPos();
    int getYPos();
    Direction getDirection();

    void mouseMove(char movement);

    char translateMove(char move);

    void feignMove(char movement);

    void resetMouse(int trueDirection);
}
#endif