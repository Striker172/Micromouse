#pragma once


int getXPos();
int getYPos();
int getDirection();

void mouseMove(char movement);

char translateMove(char move);

void feignMove(char movement);

void resetMouse(int trueDirection);