#include "MazeAPI.h"
#include "Arduino.h"
#include <queue>
std::queue<std::array<int, 3>> floodfillQueue;
 void MazeAPI::updatePos() {
  switch(currDirect) {
      case NORTH: yPos += 1; break;
      case EAST:  xPos += 1; break;
      case SOUTH: yPos -= 1; break;
      case WEST:  xPos -= 1; break;
      }
}
void MazeAPI::changeDirect(char turn) {
    if(turn == 'R') {
        currDirect = Direction((currDirect + 1) % 4);
    } else if(turn == 'L') {
        currDirect = Direction((currDirect + 3) % 4);
    }
}
bool MazeAPI::isWall(int x, int y, int sideDirection) { //for sideDirection: 0: Left, 1: Top, 2: Right, 3: Bottom
    return (maze[x][y].wallConfig & (1 << (3 - sideDirection))) != 0;
}
void MazeAPI::floodfillUtil(int x, int y, int curDistance) {
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
void MazeAPI::floodfillUpdate() {

    //set center goal cells
    for (int i: {7, 8}) { // TODO!! MOVE LOOP TO MAIN METHOD
        for (int j: {7, 8}) {
            maze[i][j].toGoalDistance = 0; 
            maze[i][j].floodfillChecked = false;
            maze[i][j].isGoal = true;
            floodfillQueue.push({i,j,0});
            }
        
    }  
    floodfillUtil(floodfillQueue.front()[0],floodfillQueue.front()[1],floodfillQueue.front()[2]); //begin recursion

    //reset floodfillChecked status
    for (int i = 0; i < 16; ++i) { 
        for (int j = 0; j < 16; ++j) {
            maze[i][j].floodfillChecked = false;
        }
    }

    //print maze cell distances for testing/debugging
}
void MazeAPI::updateAdjacentWalls(mazeCell* cell, int wallBit, bool hasWall){
    if(cell != nullptr){
        cell->wallConfig |= (hasWall << wallBit);
    }
}
void MazeAPI::surveyCell(bool& wallFront,bool& wallLeft,bool& wallRight){
  
    // store pointers to cells for readability
    mazeCell *curCell = &maze[xPos][yPos];
    mazeCell *westCell = (xPos > 0) ? &maze[xPos-1][yPos] : nullptr;
    mazeCell *northCell = (yPos < 15) ? &maze[xPos][yPos+1] : nullptr;
    mazeCell *eastCell = (xPos < 15) ? &maze[xPos+1][yPos] : nullptr;
    mazeCell *southCell = (yPos > 0) ? &maze[xPos][yPos-1] : nullptr;
    curCell->isExplored = true;

    //check for walls in the current cell in front of and to the sides of the mouse, behind the mouse is irrelevant as we would only survey upon entering a new cell, in which we would know that the way we from which we came (behind the mouse) has no wall
    int front = static_cast<int>(wallFront);
    int left = static_cast<int>(wallLeft);
    int right = static_cast<int>(wallRight);

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
char MazeAPI::translateMove(char move) {
    const char translatedMoves[4][4] = {
        {'F','B','L','R'}, //N
        {'L','R','B','F'}, //E
        {'B','F','R','L'}, //S
        {'R','L','F','B'}  //W
    };
    move = (move == 'F') ? 0 : (move == 'B') ? 1 : (move == 'L') ? 2 : 3;
   return translatedMoves[currDirect][move];
}
void MazeAPI::getWalls(int wallConfig,String& walls) {
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
  void MazeAPI::makeMove(char& bestMove){
    //Own File
    int bestDistance = maze[xPos][yPos].toGoalDistance;
    String Walls = "";
    bestMove = 'X';
    getWalls(maze[xPos][yPos].wallConfig,Walls);
    if(yPos < 15 && Walls.indexOf('U') == -1 && maze[xPos][yPos+1].toGoalDistance < bestDistance ){
        bestMove = 'F';
        bestDistance = maze[xPos][yPos+1].toGoalDistance;
    }
    if(yPos > 0 &&  Walls.indexOf('D') == -1 && maze[xPos][yPos-1].toGoalDistance < bestDistance){
        bestMove = 'B';
        bestDistance = maze[xPos][yPos-1].toGoalDistance;
    } 
    if(xPos < 15 && Walls.indexOf('R') == -1 && maze[xPos+1][yPos].toGoalDistance < bestDistance){
        bestMove = 'R';
        bestDistance = maze[xPos+1][yPos].toGoalDistance;
    }
    if(xPos > 0 && Walls.indexOf('L') == -1 && maze[xPos-1][yPos].toGoalDistance < bestDistance){
        bestMove = 'L';
        bestDistance = maze[xPos-1][yPos].toGoalDistance;
    }
    if(xPos > 0 && Walls.indexOf("LUR") != -1 && maze[xPos][yPos-1].toGoalDistance > bestDistance){
        bestMove = 'B';
        bestDistance = maze[xPos][yPos-1].toGoalDistance;
    }

// Execute best move if one was found
//Own file 
if(bestMove != 'X'){
    bestMove = translateMove(bestMove);
    updatePos();
    bestDistance = maze[xPos][yPos].toGoalDistance;
    // Survey new cell if unexplored
    if (!maze[xPos][yPos].isExplored) {
        surveyCell();
        markCell(maze[xPos][yPos].wallConfig);
        floodfillUpdate();
        bestDistance = maze[xPos][yPos].toGoalDistance;
    }
    return;
}
  }