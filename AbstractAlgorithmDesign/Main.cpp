#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"
#include "MouseMovementLibrary.cpp"
#include "FloodfillLibrary.cpp" // using header file not working?? must include cpp source files directly

using namespace std;


//Queue for the speed mode algorthim
std::queue<char> speedModeQueue;


/*
Gets the mouses best move by comparing all accessible neighboring cells distances to the goal.
@return char : Returns a char representing the direction of the mouses next best move, independent of the mouses current facing direction. ('F' : North, 'R' : East, 'B' : South, 'L' : West)
*/
char getBestMove() {

    char bestMove = 'X'; // best move to output, X means no move found
    int bestDistance = getCellDistance(getXPos(), getYPos()); //get to goal distance of current cell to compare with neighboring cells
    int curWallConfig = getWallConfig(getXPos(),getYPos()); //get the wall configuration of the current cell in order to only check accessible neighboring cells

    //If move is in bounds, and there is no wall restricting that cell, check if that cells distance to the goal is less than the currently known best move
    if(getYPos() < 15 && !(curWallConfig & 0b0100) && getCellDistance(getXPos(), getYPos()+1) < bestDistance ){
        bestMove = 'F';
        bestDistance = getCellDistance(getXPos(), getYPos()+1);
    }
    if(getYPos() > 0 &&  !(curWallConfig & 0b0001) && getCellDistance(getXPos(), getYPos()-1) < bestDistance){
        bestMove = 'B';
        bestDistance = getCellDistance(getXPos(), getYPos()-1);
    } 
    if(getXPos() < 15 && !(curWallConfig & 0b0010) && getCellDistance(getXPos()+1, getYPos()) < bestDistance){
        bestMove = 'R';
        bestDistance = getCellDistance(getXPos()+1, getYPos());
    }
    if(getXPos() > 0 && !(curWallConfig & 0b1000) && getCellDistance(getXPos()-1, getYPos()) < bestDistance){
        bestMove = 'L';
        bestDistance = getCellDistance(getXPos()-1, getYPos());
    }

    return bestMove;
}

/*
Runs the exploration mode algorithm. Current cell will be surveyed, followed by a floodfill update, and the best imaginable move being made by the mouse. This will repeat until the mouse reaches the center, at which point the mouse will repeat the algorithm on it's return to the start.
*/
void beginExplorationMode(){
    bool isInexplorationMode = true;
    bool reachedCenter = false; // determines if the mouse has reached the center for it to return to the beginning
    floodfillReset();

    char bestMove;  // X means no move found
    while (isInexplorationMode) {

        //If the mouse moves to an unexplored cell, then it will explore it and update the floodfill to account for it
        if (!isCellExplored(getXPos(), getYPos())) {
            surveyCell(getXPos(), getYPos(), getDirection());
            markCell(getXPos(), getYPos());
            floodfillUpdate(reachedCenter);
        }

        bestMove = getBestMove();

        //Once the best move has been found it translates and makes the move 
        if(bestMove != 'X'){
            
            //Translates the move with the mouses current direction and moves
            mouseMove(translateMove(bestMove));
           
            //Once the mouse reaches the center, it goes back to the start to collect more wall data on the maze.
            //To see if it made the best choices
            if(getCellDistance(getXPos(), getYPos()) == 0){
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
            
        } else cerr << "Error: No Next Move Found" << endl;
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
    char bestMove;  // X means no move found
    while (!isPrepared) {
        
        bestMove = getBestMove();

        //Once the best move has been found it translates and adds the move 
        if(bestMove != 'X'){
            //Translates the move with the mouses current direction and adds move to queue
            speedModeQueue.push(translateMove(bestMove));
            feignMove(translateMove(bestMove)); //do not move mouse, but begin addressing next cell
            isPathExplored &= isCellExplored(getXPos(), getYPos()); //check if cell is unexplored, if not, then path is not entirely explored
            
            //Once the algorithm reaches the center, the mouse has queued all necessary movements
            if(getCellDistance(getXPos(), getYPos()) == 0){
                isPrepared = true;
            }
        } else cerr << "Error: No Next Move Found" << endl;
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
    int iterationCount = 0;
    bool speedModeReady = false;

    //the mouses run loop
    while(!speedModeReady){ // loop exploration mode runs until best path imaginable is entirely explored
        clearSpeedQueue();
        ++iterationCount;
        cerr << "Exploration iteration: #" << iterationCount << endl;
        beginExplorationMode();
        speedModeReady = QueueBestPath();
    }
    cerr << "*Beginning speed mode*" << endl;
    beginSpeedMode();
}