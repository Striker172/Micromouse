#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"
using namespace std;
void log(const std::string& text) { //DO NOT TOUCH
    std::cerr << text << std::endl; //DO NOT TOUCH
}

//position of mouse in the maze
int xPos;
int yPos;
int mouseDir = 1; //direction the mouse is facing, 0:WEST, 1:NORTH, 2:EAST, 3:SOUTH

//always call after moveForward, must be later implemented into firmware API
void updatePos() {
    switch (mouseDir)
    {
    case 0:
        --xPos;
        break;
    
    case 1:
        ++yPos;
        break;
    
    case 2:
        ++xPos;
        break;
    
    case 3:
        --yPos;
        break;
    
    default:
        break;
    }
}

//update orientation of mouse, must be later implemented in firmware, 0:turn left, 1: turn right. Call after every turn 
void updateDir(int direction) {
    if (direction == 0) {
        --mouseDir;
    } if (direction == 1) {
        ++mouseDir;
    }
    if (mouseDir == 4) {
        mouseDir = 0;
    } if (mouseDir == -1) {
        mouseDir = 3;
    }
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

//does a wall exist/is known for a cell in a specific direction
bool isWall(int x, int y, int sideDirection) { //for sideDirection: 0: Left, 1: Top, 2: Right, 3: Bottom
    return (maze[x][y].wallConfig & (1 << (3 - sideDirection))) != 0;
}

//recursive function for floodfill
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
    maze[x][y].floodfillChecked = true; //this cell has now been checked
    maze[x][y].toGoalDistance = curDistance; //set this cells distance

    //add adjacent cells without a wall inbetween to the queue for processing
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

    //recursively process the queue until it is empty
    while (!floodfillQueue.empty()) {
        floodfillUtil(floodfillQueue.front()[0],floodfillQueue.front()[1],floodfillQueue.front()[2]);
    }


}

void floodfillUpdate() {

    //set center goal cells
    for (int i: {7, 8}) { // TODO!! MOVE LOOP TO MAIN METHOD
        for (int j: {7, 8}) {
            maze[i][j].toGoalDistance = 0; 
            maze[i][j].floodfillChecked = false;
            maze[i][j].isGoal = true;
            }
    }
    for (int i : {6,7,8,9}) {
        for (int j : {6,7,8,9}) {
            if((i != 6 && i != 9)||(j != 6 && j != 9)){
               floodfillQueue.push({i,j,1}); 
            }
        }
    }
    
    floodfillUtil(floodfillQueue.front()[0],floodfillQueue.front()[1],floodfillQueue.front()[2]);
    //reset floodfillChecked status
    for (int i = 0; i < 16; ++i) { 
        for (int j = 0; j < 16; ++j) {
            maze[i][j].floodfillChecked = false;
        }
    }

}

//mouse will mark the current cell as explored, and determine it's wall configuration
void surveyCell() {

    mazeCell *curCell = &maze[xPos][yPos]; // store pointer to current cell for readability

    //check for walls in the current cell in front of and to the sides of the mouse, behind the mouse is irrelevant as we would only survey upon entering a new cell, in which we would know that the way we from which we came (behind the mouse) has no wall
    int front = static_cast<int>(API::wallFront());
    int left = static_cast<int>(API::wallLeft());
    int right = static_cast<int>(API::wallRight());

    switch (mouseDir)
    {
    case 0: //if facing west/left
        curCell->wallConfig = curCell->wallConfig + (front << 3) + (left) + (right << 2);
        break;
    
    case 1: //if facing north/up
        curCell->wallConfig = curCell->wallConfig + (front << 2) + (left << 3) + (right << 1);
        break;
    
    case 2: //if facing east/right
        curCell->wallConfig = curCell->wallConfig + (front << 1) + (left << 2) + (right);
        break;
    
    case 3: //if facing south/down
        curCell->wallConfig = curCell->wallConfig + (front) + (left << 1) + (right << 3);
        break;
    
    default:
        break;
    }
}

int main(int argc, char* argv[]) {
    log("Running..."); //DO NOT TOUCH
    API::setColor(0, 0, 'G'); //DO NOT TOUCH
    API::setText(0, 0, "abc"); //DO NOT TOUCH

    

    floodfillUpdate();

    //print maze cell distances for testing/debugging
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            API::setText(i,j,std::to_string(maze[i][j].toGoalDistance));
        }
    }

    //the mouses run loop
    int i;
    while (true) {

        if (!API::wallLeft()) {
            API::turnLeft();
        }
        while (API::wallFront()) {
            API::turnRight();
        }
        API::moveForward();
        updatePos();
        surveyCell();

    }

    // const int LENGTH = 16;
    // std::queue<short> floodFillAlg;
    // int horiWalls[LENGTH][LENGTH] = {};
    // int verWalls[LENGTH][LENGTH] = {};
    // int distances[2 * (LENGTH - 2)][2 * (LENGTH - 2)] = {};
    // typedef struct Node{
    //     short floodval;
    //     short row;
    //     short column;
    //     short visited;

    //     /* pointers to neighbors */
    //     struct Node *left;
    //     struct Node *right;
    //     struct Node *up;
    //     struct Node *down;
    // } Node;

    // typedef struct Maze {
    //     Node *map[LENGTH][LENGTH];
    // } Maze;

}


