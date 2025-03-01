#include <iostream>
#include <string>
#include <queue>
#include <array>

#include "API.h"

using namespace std;

//position of mouse in the maze
int xPos;
int yPos;

//Data type for the direction, basically acts like an array
enum Direction {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3};
//current direction the mouse is facing (north, east, west, south)
Direction currDirect = NORTH;


// Simple getter functions
/*
 Gets the mouses x coordinate
*/
int getXPos(){return xPos;}
/*
 Gets the mouses y coordinate
*/
int getYPos(){return yPos;}
/*
 Gets the mouses current facing direction
*/
Direction getDirection(){return currDirect;}


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
    Function does the movement for the API, to port this function to hardware, it must use the custom hardware implemented movement API instead
    @param movement(char) : a char that handles the movement, (F:Forward, B:Backwards, L:Left, R:Right)
*/
void mouseMove(char movement) {
    switch(movement) {
        case 'F':
            API::moveForward();
            updatePos(); // Update mouse position
            break;
        case 'B':
            API::turnLeft();
            API::turnLeft();
            currDirect = static_cast<Direction>((currDirect + 2) % 4); // Update direction after 180° turn
            API::moveForward();
            updatePos(); // Update mouse position
            break;
        case 'L':
            API::turnLeft();
            changeDirect('L'); // Update direction
            API::moveForward();
            updatePos(); // Update mouse position
            break;
        case 'R':
            API::turnRight();
            API::moveForward();
            changeDirect('R'); // Update direction
            updatePos(); // Update mouse position
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