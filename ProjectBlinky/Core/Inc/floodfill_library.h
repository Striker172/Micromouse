/*
 * floodfill_library.h
 *
 *  Created on: Mar 4, 2025
 *      Author: tysen
 */

#ifndef INC_FLOODFILL_LIBRARY_H_
#define INC_FLOODFILL_LIBRARY_H_


#pragma once


bool isCellExplored(int x, int y);
int getCellDistance(int x, int y);
int getWallConfig(int x, int y);

bool isWall(int x, int y, int sideDirection);

void floodfillUpdate(bool reachedCenter);

void surveyCell(int xPos, int yPos, int currDirect);

void markCell(int xPos, int yPos, int thisWallConfig = 0, bool firstCall = true);

void floodfillReset();


#endif /* INC_FLOODFILL_LIBRARY_H_ */
