#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include "segDisplay.h"
#include <time.h>
#include "displayTime.h"
#include "utility.h"

pthread_t displayTimeThread;
pthread_mutex_t pauseFlagMutex;
pthread_cond_t cond_pauseDisplayTime;
int pauseFlag = 0;
pthread_t displayTimeThread;

// Global time variables
int currentHour = 0;
int currentMinute = 0;

// Mutex for time variables
pthread_mutex_t currentTimeMutex = PTHREAD_MUTEX_INITIALIZER;

static void updateTimeVariables()
{
    time_t now;
    struct tm *now_tm;

    now = time(NULL);
    now_tm = localtime(&now);

    pthread_mutex_lock(&currentTimeMutex);
    currentHour = now_tm->tm_hour;
    currentMinute = now_tm->tm_min;
    pthread_mutex_unlock(&currentTimeMutex);
}

static void displayCurrentTime()
{
    pthread_mutex_lock(&currentTimeMutex);
    int hours = currentHour;
    int minutes = currentMinute;
    pthread_mutex_unlock(&currentTimeMutex);

    lightUpLEDs(hours, I2CDRV_LINUX_BUS1);
    lightUpLEDs(minutes, I2CDRV_LINUX_BUS2);
}

void *displayCurrentTimeWrapper(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&pauseFlagMutex);

        if (pauseFlag)
        {
            pthread_cond_wait(&cond_pauseDisplayTime, &pauseFlagMutex);
        }

        pthread_mutex_unlock(&pauseFlagMutex);

        updateTimeVariables();
        displayCurrentTime();
    }
    return NULL;
}

void initializeTimeDisplayThread()
{
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    runCommand("config-pin P9_21 i2c");
    runCommand("config-pin P9_22 i2c");

    pthread_mutex_init(&pauseFlagMutex, NULL);
    pthread_cond_init(&cond_pauseDisplayTime, NULL);

    if (pthread_create(&displayTimeThread, NULL, displayCurrentTimeWrapper, NULL))
    {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
}

void pauseDisplayThread()
{
    pthread_mutex_lock(&pauseFlagMutex);
    pauseFlag = 1;
    pthread_mutex_unlock(&pauseFlagMutex);
}

void unpauseDisplayThread()
{
    pthread_mutex_lock(&pauseFlagMutex);
    pauseFlag = 0;
    pthread_mutex_unlock(&pauseFlagMutex);
    pthread_cond_signal(&cond_pauseDisplayTime);
}

int getCurrentHour()
{
    pthread_mutex_lock(&currentTimeMutex);
    int hour = currentHour;
    pthread_mutex_unlock(&currentTimeMutex);
    return hour;
}

int getCurrentMinute()
{
    pthread_mutex_lock(&currentTimeMutex);
    int minute = currentMinute;
    pthread_mutex_unlock(&currentTimeMutex);
    return minute;
}