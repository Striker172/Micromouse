#ifndef MAZEAPI_H
#define MAZEAPI_H
#include "Arduino.h"
class MazeAPI {
public:
	int xPos,yPos;
    enum Direction {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3};
  Direction currDirect;
  void makeMove(char& bestMove);
  void updatePos();
  void changeDirect();
private:
  struct mazeCell {
    bool isExplored = false; //has the cell been explored yet
    int wallConfig = 0b0000; //each bit represents a side of the cell, 1 meaning a wall is present, 0 meaning not/unknown. bits in clockwise order beginning from left: LEFT,TOP,RIGHT,BOTTOM
    int toGoalDistance; //how many cell units long is the shortest path to the goal from this cell
    bool isGoal = false;
    bool floodfillChecked = false; //has the cell been checked by the floodfill aglorithm
};
  mazeCell maze[16][16];
  bool isWall(int x, int y, int sideDirection);
  void floodfillUtil(int x, int y, int curDistance);
  void floodfillUpdate();
  void updateAdjacentWalls(mazeCell* cell, int wallBit, bool hasWall);
  void surveyCell(bool& wallFront,bool& wallLeft,bool& wallRight);
  char translateMove(char move);
  void getWalls(int wallConfig,String& walls);
  void changeDirect(char turn);
};

#endif
