#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"
#include "FloodfillLibrary.cpp" // using header file not working?? must include cpp source file directly

using namespace std;

void log(const std::string& text) { //DO NOT TOUCH
    std::cerr << text << std::endl; //DO NOT TOUCH
}

//position of mouse in the maze
int xPos;
int yPos;

//current direction the mouse is facing (north, east, west, south)
Direction currDirect;

//Determines if it has reached the center for it to return to the beginning
bool reachedCenter = false;

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
    surveyCell(xPos, yPos, currDirect);
    floodfillUpdate(reachedCenter);
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
        floodfillUpdate(reachedCenter);
        //Sets all the cells to false for whether its the goal or not
        for (int i = 0; i < 16; ++i) { 
            for (int j = 0; j < 16; ++j) {
                maze[i][j].isGoal = false;
        }
    }

    }
    //If the mouse moves to an unexplored cell, then it will explore it and update the floodfill to account for it
    if (!maze[xPos][yPos].isExplored) {
        surveyCell(xPos, yPos, currDirect);
        markCell(maze[xPos][yPos].wallConfig);
        floodfillUpdate(reachedCenter);
        bestDistance = maze[xPos][yPos].toGoalDistance;
    }
}
}
}