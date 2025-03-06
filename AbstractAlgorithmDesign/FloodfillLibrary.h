#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"
#ifndef FloodfillLibrary_H_
#define FloodfillLibrary_H_
namespace Floodfill{
    bool isCellExplored(int x, int y);
    int getCellDistance(int x, int y);
    int getWallConfig(int x, int y);

    bool isWall(int x, int y, int sideDirection);

    void floodfillUpdate(bool reachedCenter);

    void surveyCell(int xPos, int yPos, int currDirect);

    void markCell(int xPos, int yPos, int thisWallConfig = 0, bool firstCall = true);

    void floodfillReset();
}
#endif