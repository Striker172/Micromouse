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
//Data type for the direction, basically acts like an array
enum Direction {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3};
Direction currDirect;
//Determines if it has reached the center for it to return to the beginning
bool reachedCenter = false;
//2-D array to represent the maze
mazeCell maze[16][16];
//Queue for the floodfill algorthim
std::queue<std::array<int, 3>> floodfillQueue;
/*
 Updates the mouses direction when it moves
*/
void updatePos() {
 switch(currDirect) {
            case NORTH: yPos += 1; break;
            case EAST:  xPos += 1; break;
            case SOUTH: yPos -= 1; break;
            case WEST:  xPos -= 1; break;
        }
}

/*
    Changes the direction of the mouse depending on what direction it turns
    @param Turn(char): takes in a character either 'L' or 'R' to indicate if it has taken a left or right.  
*/ 
void changeDirect(char turn) {
    if(turn == 'R') {
        currDirect = Direction((currDirect + 1) % 4);
    } else if(turn == 'L') {
        currDirect = Direction((currDirect + 3) % 4);
    } else{
        cout << "Wrong Input " << endl;
    }
}
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
    As long as the spot was marked as the goal previvously, recusively 
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
/*
    This is used for the beginning steps of the maze that basically sets the center as the goal
    Once the reached center flag has been to turn to true, the mouses goal is the beginning of the maze
*/
void floodfillUpdate() {
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
            //This is only for the simulator, remove when porting
            API::setText(i,j,std::to_string(maze[i][j].toGoalDistance));
        }
    }
}
/*
    Updates the other walls near one of the cells
    @param cell(mazeCell) : the current cell that needs to be updated
    @param wallBit(int) : The wall configuration of the current cell, this depends on the mouse's direction
    @param hasWall(bool) : If it actually even has a wall
*/
void updateAdjacentWalls(mazeCell* cell, int wallBit, bool hasWall){
    //Basically so it doesn't cause errors
    if(cell != nullptr){
        cell->wallConfig |= (hasWall << wallBit);
    }
}
/*
    Survey's the current cell and the adjecent cells
*/
void surveyCell() {

    // store pointers to cells for readability
    //If the cell is somehow out of bounds the pointer will be set to null so it won't be considered 
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
/*
    Function does the movement for the API, this only works for the API and thus shouldn't be ported
    Needs to be modified if its going to be ported, untranslated
    @param movement(char) : a char that handles the movement, (F:Forward, B:Backwards, L:Left, R:Right)
*/
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
        case 'T':
            API::turnLeft();
            API::turnLeft();
            changeDirect('L'); // Update direction
            changeDirect('L');
            break;
    }
}
/*
    Translates the movement when it a specific direction 
    @param movement(char) : a char that handles the movement, (F:Forward, B:Backwards, L:Left, R:Right)
    @returns char : returns the translated movement depending on what direction the mouse is facing
*/
char translateMove(char move) {
    //Stores the translated moves into a 2d matrix where the direction controls the row and the move controls the column
    const char translatedMoves[4][4] = {
        {'F','B','L','R'}, //N
        {'L','R','B','F'}, //E
        {'B','F','R','L'}, //S
        {'R','L','F','B'}  //W
    };
    move = (move == 'F') ? 0 : (move == 'B') ? 1 : (move == 'L') ? 2 : 3;
   return translatedMoves[currDirect][move];
}
/*
    Gets the current wall configuration and translates it into a string to be compared
    @param wallConfig(int) : the cell's current wall configuration
    @param walls(string) : what the cell's wall looks like passed by reference
*/
void getWalls(int wallConfig,string& walls) {
    //This could honestly be removed or shorted with a bit mask or something like that. 
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
/*
    Marks the walls of the current cell the mouse is in (API only)
    @param wallConfig(int) : the cell's current wall configuration
*/
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
    surveyCell();
    floodfillUpdate();
    markCell(maze[xPos][yPos].wallConfig);
    char bestMove = 'X';  // X means no move found
    currDirect = NORTH;
    int bestDistance = maze[xPos][yPos].toGoalDistance;
    //the mouses run loop
    while (1) {
    string Walls = "";
    //Loads in the walls into the string to let the thing decided which path to go
    //Checks the individual if statement to make sure that its infact the best move to do
    getWalls(maze[xPos][yPos].wallConfig,Walls);
    //Checks certain situtions if its the best move for the mouse to make, pretty basic if i'm going to be honest
    //Its pretty readable, as it finds the current wall to see if it could possible move forward and such
    //I don't really need to comment much about it.
    if(yPos < 15 && Walls.find('U') == string::npos && maze[xPos][yPos+1].toGoalDistance < bestDistance ){
        bestMove = 'F';
        //This needs to happen otherwise it can't judge the other moves it tries to make
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

//Once the best move has been found it translates and makes the move 
if(bestMove != 'X'){
    //Debug stuff
    // cout << "Current Direction: " << currDirect << endl;
    // cout << "Curr: " << bestMove << " Translated: " << translateMove(bestMove)<<endl;

    //Translates the move with the mouses current direction and moves
    Move(translateMove(bestMove));
    //Updates the position
    updatePos();
    //resets the best move to be made and the cells current distance from the goal
    bestMove = 'X';
    bestDistance = maze[xPos][yPos].toGoalDistance;
    //Once the mouse reaches the center, it goes back to the start to collect more wall data on the maze.
    //To see if it made the best choices
    if(bestDistance == 0){
        if(reachedCenter){
            reachedCenter = false;
            Move('T');
        }else {
            reachedCenter = true;
        }
        floodfillUpdate();
        //Sets all the cells to false for whether its the goal or not
        for (int i = 0; i < 16; ++i) { 
            for (int j = 0; j < 16; ++j) {
                maze[i][j].isGoal = false;
        }
    }

    }
    //If the mouse moves to an unexplored cell, then it will explore it and update the floodfill to account for it
    if (!maze[xPos][yPos].isExplored) {
        surveyCell();
        markCell(maze[xPos][yPos].wallConfig);
        floodfillUpdate();
        bestDistance = maze[xPos][yPos].toGoalDistance;
    }
}
}
}