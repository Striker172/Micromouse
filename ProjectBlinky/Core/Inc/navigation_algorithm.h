/*
 * navigation_algorithm.h
 *
 *  Created on: Mar 6, 2025
 *      Author: tysen
 */

#ifndef INC_NAVIGATION_ALGORITHM_H_
#define INC_NAVIGATION_ALGORITHM_H_

#ifdef __cplusplus
extern "C" {
#endif

void beginExplorationMode();
void beginSpeedMode();


bool queueBestPath();
void clearSpeedQueue();

#ifdef __cplusplus
}
#endif



#endif /* INC_NAVIGATION_ALGORITHM_H_ */
