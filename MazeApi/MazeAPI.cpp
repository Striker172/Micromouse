#include "MazeAPI.h"

void MazeAPI::mazeUpdate() {
	//Implement flood fill 
}

MazeAPI::MazeAPI() {
	for (int row = 0; row < sizeof(MazeAPI::Maze); row++) {
		for (int col = 0; col < sizeof(MazeAPI::Maze); col++) {
			MazeAPI::Maze[row][col] = {0,0,row,col};
		}
	}

};
