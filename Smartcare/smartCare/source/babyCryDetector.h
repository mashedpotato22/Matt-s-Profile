#ifndef BABY_CRY_DETECTOR_H
#define BABY_CRY_DETECTOR_H

#include <pthread.h>
#include <stdbool.h>

void initializebabyCryDetector();
void pauseBabyCryDetectorThread();
void resumeBabyCryDetectorThread();
void stopBabyCryDetectorThread();
bool getBabyIsCrying();

#endif 
