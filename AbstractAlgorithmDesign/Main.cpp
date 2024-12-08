#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"

void log(const std::string& text) { //DO NOT TOUCH
    std::cerr << text << std::endl; //DO NOT TOUCH
}

struct mazeCell
{
    bool isExplored = false; //has the cell been explored yet
    int wallConfig = 0b0000; //each bit represents a side of the cell, 1 meaning a wall is present, 0 meaning not/unknown. bits in clockwise order beginning from left: LEFT,TOP,RIGHT,BOTTOM
    int toGoalDistance; //how many cell units long is the shortest path to the goal from this cell
    bool isGoal = false;

    bool floodfillChecked = false; //has the cell been checked by the floodfill aglorithm
};

mazeCell maze[16][16];
std::queue<std::array<int, 3>> floodfillQueue;

bool isWall(int x, int y, int sideDirection) { //for sideDirection: 0: Left, 1: Top, 2: Right, 3: Bottom
    return (maze[x][y].wallConfig & (1 << (3 - sideDirection))) != 0;
}

void floodfillUtil(int x, int y, int curDistance) {
    floodfillQueue.pop(); //remove this element from the queue

    if (x < 0 || y < 0 || x >= 16 || y >= 16) { //return if out of bounds
        return;
    }
    if (maze[x][y].floodfillChecked == true) { //return if the cell has been updated already
        return;
    }
    if (maze[x][y].isGoal) { //goal is distance 0 from itself
        curDistance = 0;
    }
    maze[x][y].floodfillChecked = true;
    maze[x][y].toGoalDistance = curDistance;

    if (!isWall(x, y, 0)) {
        floodfillQueue.push({x-1, y, curDistance+1});
    }
    if (!isWall(x, y, 1)) {
        floodfillQueue.push({x, y+1, curDistance+1});
    }
    if (!isWall(x, y, 2)) {
        floodfillQueue.push({x+1, y, curDistance+1});
    } 
    if (!isWall(x, y, 3)) {
        floodfillQueue.push({x, y-1, curDistance+1});
    }


    while (!floodfillQueue.empty()) {
        floodfillUtil(floodfillQueue.front()[0],floodfillQueue.front()[1],floodfillQueue.front()[2]);
    }


}

void floodfillUpdate() {

    //set center goal cells
    for (int i: {7, 8}) {
        for (int j: {7, 8}) {
            maze[i][j].toGoalDistance = 0; 
            maze[i][j].floodfillChecked = false;
            maze[i][j].isGoal = true;
            }
    }


    floodfillUtil(7, 7, 0);

    //reset floodfillChecked status
    for (int i = 0; i < 16; ++i) { 
        for (int j = 0; j < 16; ++j) {
            maze[i][j].floodfillChecked = false;
        }
    }

}

int main(int argc, char* argv[]) {
    log("Running..."); //DO NOT TOUCH
    API::setColor(0, 0, 'G'); //DO NOT TOUCH
    API::setText(0, 0, "abc"); //DO NOT TOUCH

    

    floodfillUpdate();

    //print maze cell distances
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            std::cout << maze[i][j].toGoalDistance << " ";
        }
        std::cout << std::endl;
    }

    while (true) {
        if (!API::wallLeft()) {
            API::turnLeft();
        }
        while (API::wallFront()) {
            API::turnRight();
        }
        API::moveForward();
    }
}

