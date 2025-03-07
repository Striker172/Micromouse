/*
 * API.cpp
 *
 *  Created on: Mar 6, 2025
 *      Author: tysen
 */

#include "main.h"
#include "tim.h"
#include "gpio.h"

#define SPEED 75


volatile int driveValues[4] = {0,0,0,0};


void writePWM(){
	htim1.Instance-> CCR1 = ((driveValues[0])*65535)/100;//Forward Left
	htim1.Instance-> CCR2 = ((driveValues[1])*65535)/100;//Backward Left
	htim1.Instance-> CCR3 = ((driveValues[2])*65535)/100;//Forward Right
	htim1.Instance-> CCR4 = ((driveValues[3])*65535)/100;//Backward Right
}


void drive(char D){
	switch(D){
//	case 'F':
//		HAL_GPIO_WritePin(LeftsideF, LeftsideF, SPEED);
//		HAL_GPIO_WritePin(RightsideF, RightsideF, SPEED);
//		break;
//	case 'B':
//		HAL_GPIO_WritePin(RightsideB, RightsideB, SPEED);
//		HAL_GPIO_WritePin(LeftsideB, LeftsideB, SPEED);
//		break;
//	case 'L':
//		HAL_GPIO_WritePin(LeftsideF, LeftsideF, SPEED);
//		HAL_GPIO_WritePin(RightsideF, RightsideF, SPEED/2);
//		break;
//	case 'R':
//		HAL_GPIO_WritePin(LeftsideF, LeftsideF, SPEED/2);
//		HAL_GPIO_WritePin(RightsideF, RightsideF, SPEED);
//		break;
//	default:
//		HAL_GPIO_WritePin(RightsideF, RightsideF, 0);
//		HAL_GPIO_WritePin(RightsideB, RightsideB, 0);
//		HAL_GPIO_WritePin(LeftsideB, LeftsideB, 0);
//		HAL_GPIO_WritePin(LeftsideF, LeftsideF, 0);
		break;
	}
}


void moveForward(int distance){
	//Check the hall effect sensor on the actual motors to see how far you have moved
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
	driveValues[2] = SPEED/2;
	driveValues[0] = SPEED;
	driveValues[1] = 0;
	driveValues[3] = 0;
	writePWM();
}

void turnRight(){
	driveValues[1] = SPEED/2;
	driveValues[3] = SPEED;
	driveValues[2] = 0;
	driveValues[0] = 0;
	writePWM();
}
