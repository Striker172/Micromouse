#include <iostream>
#include <string>
#include <queue>

#include "API.h"
const int LENGTH = 16;
void log(const std::string& text) {
    std::cerr << text << std::endl;
}

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    while (true) {
        if (!API::wallLeft()) {
            API::turnLeft();
        }
        while (API::wallFront()) {
            API::turnRight();
        }
        API::moveForward();
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
