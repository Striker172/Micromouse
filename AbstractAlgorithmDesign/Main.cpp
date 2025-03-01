#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"
#include "MouseMovementLibrary.cpp"
#include "FloodfillLibrary.cpp" // using header file not working?? must include cpp source files directly

using namespace std;

void log(const std::string& text) { //DO NOT TOUCH
    std::cerr << text << std::endl; //DO NOT TOUCH
}


//Determines if the mouse has reached the center for it to return to the beginning
bool reachedCenter = false;

//Determines if the mouse is currently in exploration mode, or else speed mode
bool explorationMode = true;

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
        API::setWall(getXPos(),getYPos(),'n');
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



// mouse running function
int main(int argc, char* argv[]) {
    log("Running..."); //DO NOT TOUCH
    API::setColor(0, 0, 'G'); //DO NOT TOUCH
    API::setText(0, 0, "abc"); //DO NOT TOUCH
    surveyCell(getXPos(), getYPos(), getDirection());
    floodfillUpdate(reachedCenter);
    markCell(getXPos(), getYPos());
    char bestMove = 'X';  // X means no move found
    int bestDistance = getCellDistance(getXPos(), getYPos());
    //the mouses run loop
    while (1) {
        string Walls = "";
        //Loads in the walls into the string to let the thing decided which path to go
        //Checks the individual if statement to make sure that its infact the best move to do
        getWalls(maze[getXPos()][getYPos()].wallConfig,Walls); //should use getter instead, but causes crashing for unknown reason
        //Checks certain situtions if its the best move for the mouse to make, pretty basic if i'm going to be honest
        //Its pretty readable, as it finds the current wall to see if it could possible move forward and such
        //I don't really need to comment much about it.
        if(getYPos() < 15 && Walls.find('U') == string::npos && getCellDistance(getXPos(), getYPos()+1) < bestDistance ){
            bestMove = 'F';
            //This needs to happen otherwise it can't judge the other moves it tries to make
            bestDistance = getCellDistance(getXPos(), getYPos()+1);
        }
        if(getYPos() > 0 &&  Walls.find('D') == string::npos && getCellDistance(getXPos(), getYPos()-1) < bestDistance){
            bestMove = 'B';
            bestDistance = getCellDistance(getXPos(), getYPos()-1);
        } 
        if(getXPos() < 15 && Walls.find('R') == string::npos && getCellDistance(getXPos()+1, getYPos()) < bestDistance){
            bestMove = 'R';
            bestDistance = getCellDistance(getXPos()+1, getYPos());
        }
        if(getXPos() > 0 && Walls.find('L') == string::npos && getCellDistance(getXPos()-1, getYPos()) < bestDistance){
            bestMove = 'L';
            bestDistance = getCellDistance(getXPos()-1, getYPos());
        }
        if(getXPos() > 0 && Walls.find("LUR") != string::npos && getCellDistance(getXPos(), getYPos()-1) > bestDistance){
            bestMove = 'B';
            bestDistance = getCellDistance(getXPos(), getYPos()-1);
        }

        //Once the best move has been found it translates and makes the move 
        if(bestMove != 'X'){
            //Debug stuff
            // cout << "Current Direction: " << currDirect << endl;
            // cout << "Curr: " << bestMove << " Translated: " << translateMove(bestMove)<<endl;

            //Translates the move with the mouses current direction and moves
            mouseMove(translateMove(bestMove));
            //resets the best move to be made and the cells current distance from the goal
            bestMove = 'X';
            bestDistance = getCellDistance(getXPos(), getYPos());
            //Once the mouse reaches the center, it goes back to the start to collect more wall data on the maze.
            //To see if it made the best choices
            if(bestDistance == 0){
                if(reachedCenter){
                    reachedCenter = false;
                    mouseMove('T');
                } else {
                    reachedCenter = true;
                }
                floodfillReset();
                floodfillUpdate(reachedCenter);
            }
            //If the mouse moves to an unexplored cell, then it will explore it and update the floodfill to account for it
            if (!isCellExplored(getXPos(), getYPos())) {
                surveyCell(getXPos(), getYPos(), getDirection());
                markCell(getXPos(), getYPos());
                floodfillUpdate(reachedCenter);
                bestDistance = getCellDistance(getXPos(), getYPos());
            }
        }
    }
}