/*
 * API.h
 *
 *  Created on: Mar 4, 2025
 *      Author: tysen
 */

#ifndef SRC_API_H_
#define SRC_API_H_
bool wallFront();
bool wallLeft();
bool wallRight();
void writePWM(int forLef,int bacLef,int forRig,int bacRig);
void moveForward(int numCells);
void turnLeft();
void turnRight();



#endif /* SRC_API_H_ */
