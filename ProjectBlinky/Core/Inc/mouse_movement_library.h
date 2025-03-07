/*
 * mouse_movement_library.h
 *
 *  Created on: Mar 4, 2025
 *      Author: tysen
 */

#ifndef INC_MOUSE_MOVEMENT_LIBRARY_H_
#define INC_MOUSE_MOVEMENT_LIBRARY_H_


int getXPos();
int getYPos();
int getDirection();

void mouseMove(char movement);

char translateMove(char move);

void feignMove(char movement);

void resetMouse(int trueDirection);


#endif /* INC_MOUSE_MOVEMENT_LIBRARY_H_ */
