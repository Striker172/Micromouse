/*
 * floodfill_library.cpp
 *
 *  Created on: Mar 4, 2025
 *      Author: tysen
 */

#include "floodfill_library.h"

#include <queue>
#include <array>

#include "API.h"


#define intNORTH 0
#define intEAST 1
#define intSOUTH 2
#define intWEST 3


//Queue for the floodfill algorthim
std::queue<std::array<int, 3>> floodfillQueue;

/*
    Stores information about the cell in the current maze
*/
struct mazeCell
{
    bool isExplored = false; //has the cell been explored yet
    int wallConfig = 0b0000; //each bit represents a side of the cell, 1 meaning a wall is present, 0 meaning not/unknown. bits in clockwise order beginning from left: LEFT,TOP,RIGHT,BOTTOM
    int toGoalDistance; //how many cell units long is the shortest path to the goal from this cell
    bool isGoal = false;

    bool floodfillChecked = false; //has the cell been checked by the floodfill aglorithm
};

//2-D array to represent the maze
mazeCell maze[16][16];



/*
    Determines whether a cell has been explored
    @param x(int) : The x coordinate for the cell
    @param y(int) : The y coordinate for the cell
    @return int : returns whether or not the cell has previously been explored, aka has been surveyed
*/
bool isCellExplored(int x, int y){
    return maze[x][y].isExplored;
}


/*
    Getter for a cells currently known wall configuration
    @param x(int) : The x coordinate for the cell
    @param y(int) : The y coordinate for the cell
    @return int : returns the bit representation of the cells wallConfig
*/
int getWallConfig(int x, int y){
    return maze[x][y].wallConfig;
}


/*
    Gets a cells toGoalDistance
    @param x(int) : The x coordinate for the cell
    @param y(int) : The y coordinate for the cell
    @return int : returns the cells current estimated distance from the goal
*/
int getCellDistance(int x, int y){
    return maze[x][y].toGoalDistance;
}


/*
    Determines if the cell has a wall or not in it
    @param x(int) : The x coordinate for the cell
    @param y(int) : The y coordinate for the cell
    @param sideDirection(int) : the direction you want to check. (0:L, 1:T, 2:R, 3:B)
    @return bool : returns if the checked direction has a wall or not
*/
bool isWall(int x, int y, int sideDirection) {
    //This basically uses a bit mask to determine the specific configuration of it and returns true if its a wall
    return (maze[x][y].wallConfig & (1 << (3 - sideDirection))) != 0;
}



/*
    This performs the floodfill algorithm to check the wall configuration and get the distance from a specific spot
    As long as the spot was marked as the goal previously, recursively
    @param x(int) : The x coordinate for the cell to be checked
    @param y(int) : The y coordinate for the cell to be checked
    @param currDistance(int) : this sets the cell current distance from the goal
*/
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

    //add adjacent cells without a wall in between to the queue for processing
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
/*
    This is used for the beginning steps of the maze that basically sets the center as the goal
    Once the reached center flag has been to turn to true, the mouses goal is the beginning of the maze
*/
void floodfillUpdate(bool reachedCenter) {
    if(!reachedCenter){
        //set center goal cells
        for (int i: {7, 8}) {
            for (int j: {7, 8}) {
                maze[i][j].toGoalDistance = 0;
                maze[i][j].floodfillChecked = false;
                maze[i][j].isGoal = true;
                floodfillQueue.push({i,j,0});
            }
        }

    } else {
        //Sets the beginning as the goal when it has reached the center
        maze[0][0].toGoalDistance = 0;
        maze[0][0].floodfillChecked = false;
        maze[0][0].isGoal = true;
        floodfillQueue.push({0,0,0});
}
    floodfillUtil(floodfillQueue.front()[0],floodfillQueue.front()[1],floodfillQueue.front()[2]); //begin recursion

    //reset floodfillChecked status
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            maze[i][j].floodfillChecked = false;
        }
    }
}

/*
    Updates the other walls near one of the cells
    @param cell(mazeCell) : the current cell that needs to be updated
    @param wallBit(int) : The bit position to update in the wall configuration of the cell at hand
    @param hasWall(bool) : If it actually even has a wall
*/
void updateAdjacentWalls(mazeCell* cell, int wallBit, bool hasWall){
    //Basically so it doesn't cause errors
    if(cell != nullptr){
        cell->wallConfig |= (hasWall << wallBit);
    }
}


/*
    Survey's the current cell, updating it and the adjecent cells
*/
void surveyCell(int xPos, int yPos, int currDirect) {

    // store pointers to cells for readability
    //If the cell is somehow out of bounds the pointer will be set to null so it won't be considered
    mazeCell *curCell = &maze[xPos][yPos];
    mazeCell *westCell = (xPos > 0) ? &maze[xPos-1][yPos] : nullptr;
    mazeCell *northCell = (yPos < 15) ? &maze[xPos][yPos+1] : nullptr;
    mazeCell *eastCell = (xPos < 15) ? &maze[xPos+1][yPos] : nullptr;
    mazeCell *southCell = (yPos > 0) ? &maze[xPos][yPos-1] : nullptr;
    curCell->isExplored = true;

    //check for walls in the current cell in front of and to the sides of the mouse, behind the mouse is irrelevant as we would only survey upon entering a new cell, in which we would know that the way we from which we came (behind the mouse) has no wall
    int front = static_cast<int>(wallFront());
    int left = static_cast<int>(wallLeft());
    int right = static_cast<int>(wallRight());

    // based on the direction of the mouse and its surveyings, update the walls of the current cell and the connected cells
    switch (currDirect)
    {
    case intWEST: //if facing west/left
        curCell->wallConfig |= (front << 3) + (left) + (right << 2);
        updateAdjacentWalls(westCell,1,front);
        updateAdjacentWalls(northCell,0,right);
        updateAdjacentWalls(southCell,2,left);
        break;
    case intNORTH: //if facing north/up
        curCell->wallConfig |= (front << 2) + (left << 3) + (right << 1);
        updateAdjacentWalls(northCell,0,front);
        updateAdjacentWalls(westCell,1,left);
        updateAdjacentWalls(eastCell,3,right);
        break;

    case intEAST: //if facing east/right
        curCell->wallConfig |= (front << 1) + (left << 2) + (right);
        updateAdjacentWalls(eastCell,3,front);
        updateAdjacentWalls(northCell,0,left);
        updateAdjacentWalls(southCell,2,right);
        break;

    case intSOUTH: //if facing south/down
        curCell->wallConfig |= (front) + (left << 1) + (right << 3);
        updateAdjacentWalls(southCell,2,front);
        updateAdjacentWalls(westCell,1,right);
        updateAdjacentWalls(eastCell,3,left);
        break;

    default:
        break;
    }
}



void floodfillReset(){
    //Sets all the cells to false for whether its the goal or not
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            maze[i][j].isGoal = false;
        }
    }
}
