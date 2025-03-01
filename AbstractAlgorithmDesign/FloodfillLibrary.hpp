#pragma once


struct mazeCell{
    bool isExplored;
    int wallConfig;
    int toGoalDistance;
    bool isGoal;
    bool floodfillChecked;
};

extern mazeCell maze[16][16];

bool isWall(int x, int y, int sideDirection);

void floodfillUpdate(bool reachedCenter);

void updateAdjacentWalls(mazeCell* cell, int wallBit, bool hasWall);

void surveyCell(int xPos, int yPos, int currDirect);