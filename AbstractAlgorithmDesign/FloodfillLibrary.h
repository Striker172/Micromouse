#pragma once


struct mazeCell{
    bool isExplored;
    int wallConfig;
    int toGoalDistance;
    bool isGoal;
    bool floodfillChecked;
};

bool isCellExplored(int x, int y);
int getCellDistance(int x, int y);
int getWallConfig(int x, int y); //using this function instead of directly accessing value causes a crash for unknown reason

bool isWall(int x, int y, int sideDirection);

void floodfillUpdate(bool reachedCenter);

void updateAdjacentWalls(mazeCell* cell, int wallBit, bool hasWall);

void surveyCell(int xPos, int yPos, int currDirect);

void markCell(int xPos, int yPos, int thisWallConfig = 0, bool firstCall = true);

void floodfillReset();