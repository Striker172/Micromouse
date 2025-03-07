/*
 * navigation_algorithm.h
 *
 *  Created on: Mar 6, 2025
 *      Author: tysen
 */

#ifndef INC_NAVIGATION_ALGORITHM_H_
#define INC_NAVIGATION_ALGORITHM_H_

void beginExplorationMode();
void beginSpeedMode();


bool queueBestPath();
void clearSpeedQueue();


#endif /* INC_NAVIGATION_ALGORITHM_H_ */
