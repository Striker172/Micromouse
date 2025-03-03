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


//Queue for the speed mode algorthim
std::queue<char> speedModeQueue;



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

/*
Runs the exploration mode algorithm. Current cell will be surveyed, followed by a floodfill update, and the best imaginable move being made by the mouse. This will repeat until the mouse reaches the center, at which point the mouse will repeat the algorithm on it's return to the start.
*/
void beginExplorationMode(){
    bool isInexplorationMode = true;
    bool reachedCenter = false; // determines if the mouse has reached the center for it to return to the beginning
    floodfillReset();
    floodfillUpdate(reachedCenter);
    surveyCell(getXPos(), getYPos(), getDirection());
    markCell(getXPos(), getYPos());
    char bestMove = 'X';  // X means no move found
    int bestDistance = getCellDistance(getXPos(), getYPos());
    string Walls = "";
    while (isInexplorationMode) {
        bestDistance = getCellDistance(getXPos(), getYPos());
        Walls = "";
        //Loads in the walls into the string to let the thing decided which path to go
        //Checks the individual if statement to make sure that its infact the best move to do
        getWalls(getWallConfig(getXPos(),getYPos()),Walls);
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
                    isInexplorationMode = false;
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


/*
Queues the best imaginable path for speed mode, and checks if it has been entirely explored
@returns bool : true if every single cell which is a part of the queued path has been surveyed/explored, else false
*/
bool QueueBestPath(){
    int mouseInitialDirection = getDirection(); //save mouses initial direction for reset at end
    floodfillReset();
    floodfillUpdate(false); //update with argument of center not reached as mouse begins speed run from start
    
    //firstly the mouse preps the movements of its best determined route into a queue
    bool isPrepared = false; // has mouse finished preparing route
    bool isPathExplored = true; // has the entire path been explored
    char bestMove = 'X';  // X means no move found
    int bestDistance = getCellDistance(getXPos(), getYPos()); //get current distance to compare with neighboring cells
    string Walls = "";
    while (!isPrepared) {
        bestDistance = getCellDistance(getXPos(), getYPos());
        Walls = "";
        //Loads in the walls into the string to let the thing decided which path to go
        //Checks the individual if statement to make sure that its infact the best move to do
        getWalls(getWallConfig(getXPos(),getYPos()),Walls);
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

        //Once the best move has been found it translates and adds the move 
        if(bestMove != 'X'){
            //Translates the move with the mouses current direction and adds move to queue
            speedModeQueue.push(translateMove(bestMove));
            feignMove(translateMove(bestMove)); //do not move mouse, but begin addressing next cell
            isPathExplored &= isCellExplored(getXPos(), getYPos()); //check if cell is unexplored, if not, then path is not entirely explored
            //resets the best move to be made and the cells current distance from the goal
            bestMove = 'X';
            bestDistance = getCellDistance(getXPos(), getYPos());
            //Once the algorithm reaches the center, the mouse has queued all necessary movements
            if(bestDistance == 0){
                isPrepared = true;
            }
        }
    }

    resetMouse(mouseInitialDirection);
    return isPathExplored;
}


/*
Clears the movement queue for speedmode
*/
void clearSpeedQueue(){
    while (!speedModeQueue.empty()){
        speedModeQueue.pop();
    }
}


/*
Executes the queued moves for the speed mode algorithm. Cells are not actively surveyed and the known maze layout is not updated as it is assumed to be sufficiently known.
*/
void beginSpeedMode(){
    while (!speedModeQueue.empty()) {
        mouseMove(speedModeQueue.front());
        speedModeQueue.pop();
    }
}

// mouse running function
int main(int argc, char* argv[]) {
    log("Running..."); //DO NOT TOUCH
    API::setColor(0, 0, 'G'); //DO NOT TOUCH
    API::setText(0, 0, "abc"); //DO NOT TOUCH

    int iterationCount = 0;
    bool speedModeReady = false;

    //the mouses run loop
    while(!speedModeReady){ // loop exploration mode runs until best path imaginable is entirely explored
        clearSpeedQueue();
        ++iterationCount;
        cout << "iteration: #" << iterationCount << endl;
        beginExplorationMode();
        speedModeReady = QueueBestPath();
    }
    beginSpeedMode();
}