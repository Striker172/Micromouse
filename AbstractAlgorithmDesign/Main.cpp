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
enum Direction {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3};
Direction currDirect;
bool reachedCenter = false;
//always call after moveForward, must be later implemented into firmware API
//Own file
void updatePos() {
 switch(currDirect) {
            case NORTH: yPos += 1; break;
            case EAST:  xPos += 1; break;
            case SOUTH: yPos -= 1; break;
            case WEST:  xPos -= 1; break;
        }
}

//update orientation of mouse, must be later implemented in firmware, 0:turn left, 1: turn right. Call after every turn
//Own file 
void changeDirect(char turn) {
    if(turn == 'R') {
        currDirect = Direction((currDirect + 1) % 4);
    } else if(turn == 'L') {
        currDirect = Direction((currDirect + 3) % 4);
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
//Own File
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
//Own File
void floodfillUpdate() {
    //set center goal cells
    if(!reachedCenter){
        cout << reachedCenter << endl;
        for (int i: {7, 8}) { // TODO!! MOVE LOOP TO MAIN METHOD
            for (int j: {7, 8}) {
                maze[i][j].toGoalDistance = 0; 
                maze[i][j].floodfillChecked = false;
                maze[i][j].isGoal = true;
                floodfillQueue.push({i,j,0});
            }
        }  
    } else {
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
            API::setText(i,j,std::to_string(maze[i][j].toGoalDistance));
        }
    }

    //print maze cell distances for testing/debugging
}
//Own File
void updateAdjacentWalls(mazeCell* cell, int wallBit, bool hasWall){
    if(cell != nullptr){
        cell->wallConfig |= (hasWall << wallBit);
    }
}
//mouse will mark the current cell as explored, and determine it's wall configuration
//Own File
void surveyCell() {

    // store pointers to cells for readability
    mazeCell *curCell = &maze[xPos][yPos];
    mazeCell *westCell = (xPos > 0) ? &maze[xPos-1][yPos] : nullptr;
    mazeCell *northCell = (yPos < 15) ? &maze[xPos][yPos+1] : nullptr;
    mazeCell *eastCell = (xPos < 15) ? &maze[xPos+1][yPos] : nullptr;
    mazeCell *southCell = (yPos > 0) ? &maze[xPos][yPos-1] : nullptr;
    curCell->isExplored = true;

    //check for walls in the current cell in front of and to the sides of the mouse, behind the mouse is irrelevant as we would only survey upon entering a new cell, in which we would know that the way we from which we came (behind the mouse) has no wall
    int front = static_cast<int>(API::wallFront());
    int left = static_cast<int>(API::wallLeft());
    int right = static_cast<int>(API::wallRight());

    // based on the direction of the mouse and its surveyings, update the walls of the current cell and the connected cells
    switch (currDirect)
    {
    case WEST: //if facing west/left
        curCell->wallConfig |= (front << 3) + (left) + (right << 2);
        updateAdjacentWalls(westCell,1,front);
        updateAdjacentWalls(northCell, 0, right);
        updateAdjacentWalls(southCell,2,left);
        break;
    case NORTH: //if facing north/up
        curCell->wallConfig |= (front << 2) + (left << 3) + (right << 1);
        updateAdjacentWalls(northCell,0,front);
        updateAdjacentWalls(westCell,1,left);
        updateAdjacentWalls(eastCell,3,right);
        break;
    
    case EAST: //if facing east/right
        curCell->wallConfig |= (front << 1) + (left << 2) + (right);
        updateAdjacentWalls(eastCell,3,front);
        updateAdjacentWalls(northCell,0,left);
        updateAdjacentWalls(southCell,2,right);
        break;
    
    case SOUTH: //if facing south/down
        curCell->wallConfig |= (front) + (left << 1) + (right << 3);
        updateAdjacentWalls(southCell,2,front);
        updateAdjacentWalls(westCell,1,right);
        updateAdjacentWalls(eastCell,3,left);
        break;
    
    default:
        break;
    }
}
//Arduino 
void Move(char movement) {
    switch(movement) {
        case 'F':
            API::moveForward();
            break;
        case 'B':
            API::turnLeft();
            API::turnLeft();
            currDirect = static_cast<Direction>((currDirect + 2) % 4); // Update direction after 180° turn
            API::moveForward();
            break;
        case 'L':
            API::turnLeft();
            changeDirect('L'); // Update direction
            API::moveForward();
            break;
        case 'R':
            API::turnRight();
            API::moveForward();
            changeDirect('R'); // Update direction
            break;
    }
}
//Own File
// 0:F, 1:B, 2:L, 3:R
char translateMove(char move) {
    const char translatedMoves[4][4] = {
        {'F','B','L','R'}, //N
        {'L','R','B','F'}, //E
        {'B','F','R','L'}, //S
        {'R','L','F','B'}  //W
    };
    move = (move == 'F') ? 0 : (move == 'B') ? 1 : (move == 'L') ? 2 : 3;
   return translatedMoves[currDirect][move];
}
void getWalls(int wallConfig,string& walls) {
    if(wallConfig < 15 && wallConfig >= 8){
        walls +='L';
        getWalls(wallConfig-8,walls);
    } else if (wallConfig <= 7 && wallConfig >= 4){
        API::setWall(xPos,yPos,'n');
        walls +='U';
        getWalls(wallConfig-4,walls);
    }else if(wallConfig == 3 || wallConfig == 2){
        walls += 'R';
        getWalls(wallConfig-2,walls);
    } else if(wallConfig == 1){
        walls += 'D';
        getWalls(wallConfig-1,walls);
    }
    return;
    }
//Change code to have it use the sensors
//isn't required for arduino code 
 void markCell(int wallConfig) {
    if(wallConfig < 15 && wallConfig >= 8){
        API::setWall(xPos,yPos,'w');
        markCell(wallConfig-8);
    } else if (wallConfig <= 7 && wallConfig >= 4){
        API::setWall(xPos,yPos,'n');
        markCell(wallConfig-4);
    }else if(wallConfig == 3 || wallConfig == 2){
        API::setWall(xPos,yPos,'e');
        markCell(wallConfig-2);
    } else if(wallConfig == 1){
        API::setWall(xPos,yPos,'s');
        markCell(wallConfig-1);
    }
    return;
    }

int main(int argc, char* argv[]) {
    log("Running..."); //DO NOT TOUCH
    API::setColor(0, 0, 'G'); //DO NOT TOUCH
    API::setText(0, 0, "abc"); //DO NOT TOUCH
    floodfillUpdate();
    markCell(maze[xPos][yPos].wallConfig);
    surveyCell();
    char bestMove = 'X';  // X means no move found
    currDirect = NORTH;
    int bestDistance = maze[xPos][yPos].toGoalDistance;
    //the mouses run loop
    while (1) {
    string Walls = "";
    //Loads in the walls into the string to let the thing decided which path to go
    //Checks the individual if statement to make sure that its infact the best move to do
    getWalls(maze[xPos][yPos].wallConfig,Walls);
    if(yPos < 15 && Walls.find('U') == string::npos && maze[xPos][yPos+1].toGoalDistance < bestDistance ){
        bestMove = 'F';
        bestDistance = maze[xPos][yPos+1].toGoalDistance;
    }
    if(yPos > 0 &&  Walls.find('D') == string::npos && maze[xPos][yPos-1].toGoalDistance < bestDistance){
        bestMove = 'B';
        bestDistance = maze[xPos][yPos-1].toGoalDistance;
    } 
    if(xPos < 15 && Walls.find('R') == string::npos && maze[xPos+1][yPos].toGoalDistance < bestDistance){
        bestMove = 'R';
        bestDistance = maze[xPos+1][yPos].toGoalDistance;
    }
    if(xPos > 0 && Walls.find('L') == string::npos && maze[xPos-1][yPos].toGoalDistance < bestDistance){
        bestMove = 'L';
        bestDistance = maze[xPos-1][yPos].toGoalDistance;
    }
    if(xPos > 0 && Walls.find("LUR") != string::npos && maze[xPos][yPos-1].toGoalDistance > bestDistance){
        bestMove = 'B';
        bestDistance = maze[xPos][yPos-1].toGoalDistance;
    }

// Execute best move if one was found
//Own file 
if(bestMove != 'X'){
    // cout << "Current Direction: " << currDirect << endl;
    // cout << "Curr: " << bestMove << " Translated: " << translateMove(bestMove)<<endl;
    //Translates the move with the mouses current direction and moves
    Move(translateMove(bestMove));
    updatePos();
    //resets the best move
    bestMove = 'X';
    bestDistance = maze[xPos][yPos].toGoalDistance;
    //Once the mouse reaches the center, it goes back to the start to collect more wall data on the maze.
    //Basically a second run and stuff
    if(bestDistance == 0 && !reachedCenter){
        reachedCenter = true;
        floodfillUpdate();
        for (int i = 0; i < 16; ++i) { 
            for (int j = 0; j < 16; ++j) {
                maze[i][j].isGoal = false;
        }
    }

    }
    // Survey new cell if unexplored
    if (!maze[xPos][yPos].isExplored) {
        surveyCell();
        markCell(maze[xPos][yPos].wallConfig);
        floodfillUpdate();
        bestDistance = maze[xPos][yPos].toGoalDistance;
    }
    // cout << "Best Distance: " <<bestDistance << endl;
    // cout << "X: " << xPos << " Y: " << yPos << endl;
}
}
}