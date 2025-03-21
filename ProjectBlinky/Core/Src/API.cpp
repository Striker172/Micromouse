/*
 * API.cpp
 *
 *  Created on: Mar 6, 2025
 *      Author: tysen
 */

#include "API.h"
#include "tim.h"
#include "gpio.h"

#define SPEED 50




void writePWM(int forLef,int bacLef,int forRig,int bacRig){
	htim1.Instance-> CCR1 = ((forLef)*65535)/100;//Forward Left
	htim1.Instance-> CCR2 = ((bacLef)*65535)/100;//Backward Left
	htim1.Instance-> CCR3 = ((forRig)*65535)/100;//Forward Right
	htim1.Instance-> CCR4 = ((bacRig)*65535)/100;//Backward Right
}




void moveForward(int numCells){
	writePWM(SPEED,0,SPEED,0);
}


bool wallFront(){
	return HAL_GPIO_ReadPin(SensorF_GPIO_Port, SensorF_Pin);
}
bool wallLeft(){
	return HAL_GPIO_ReadPin(SensorL_GPIO_Port, SensorL_Pin);
}
bool wallRight(){
	return HAL_GPIO_ReadPin(SensorR_GPIO_Port, SensorR_Pin);
}


void turnLeft(){
	writePWM(SPEED,0,0,SPEED);
}

void turnRight(){
	writePWM(0,SPEED,SPEED,0);
}
