#include "MazeAPI.h"
#include "Arduino.h"
void MazeAPI::mazeUpdate() {
	//Implement flood fill 
}

MazeAPI::MazeAPI() {
	for (int row = 0; row < 16; row++) {
		for (int col = 0; col < 16; col++) {
			if (row == 0) {
				MazeAPI::Maze[row][col] = {8,0,row,col}; // Wall up
			} else if (row == 15) {
				MazeAPI::Maze[row][col] = {4,0,row,col}; // Wall down
			} else if (col == 0) {
				MazeAPI::Maze[row][col] = {1,0,row,col}; // Wall left
			} else if (col == 15) {
				MazeAPI::Maze[row][col] = {2,0,row,col}; // Wall right
			} else {
				MazeAPI::Maze[row][col] = {0,0,row,col}; // No wall
			}
		}
	}
}
String MazeAPI::getWalls(unsigned short int walls) {
    if(walls <= 15 && walls >= 8){
      return "U" + getWalls(walls-8);
    }else if(walls <= 7 && walls >= 4){
      return "D" + getWalls(walls-4);
    }else if(walls == 3 || walls == 2){
      return "R" + getWalls(walls-2);
    } else if(walls == 1){
      return "L" + getWalls(walls-1);
    }
    return "";
}

unsigned int MazeAPI::getDistance(int row, int col){
    return MazeAPI::Maze[row][col].distance;
}
//Make this recursive because it's really easy.
void MazeAPI::addWalls(int x, int y, unsigned int direction) {
	if (direction > 15 || direction < 0) {
		MazeAPI::Maze[x][y].walls = 0;
	}
	else {
		MazeAPI::Maze[x][y].walls = direction;
	}
}

