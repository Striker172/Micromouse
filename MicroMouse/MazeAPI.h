#ifndef MAZEAPI_H
#define MAZEAPI_H
#include "Arduino.h"

class MazeAPI {
public:
	void mazeUpdate();
	MazeAPI();
	void addWalls(int x, int y, unsigned int direction);
	unsigned int getDistance(int row, int col);
	String getWalls(unsigned short int walls);
private:
	struct cell {
		short unsigned int walls;
		unsigned int distance;
		int x, y;
	};
	cell Maze[16][16];
};

#endif
