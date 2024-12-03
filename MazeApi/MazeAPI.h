#ifndef MAZEAPI_H
#define MAZEAPI_H
//#include "Arduino.h"
class MazeAPI {
public:
	void mazeUpdate();
	MazeAPI();
	
private:
	const int objecX = 8;
	const int objecY = 8;
	struct cell{
		short unsigned int walls;
		unsigned int distance;
		int x, y;
	};
	cell Maze[16][16];
};

#endif
