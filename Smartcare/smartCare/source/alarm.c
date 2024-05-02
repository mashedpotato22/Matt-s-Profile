#include "alarm.h"
#include "joystick.h"
#include "segDisplay.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

enum TimeComponent
{
    HOUR,
    MINUTE,
    NONE
};
#define FLASH_INTERVAL_MS 500

// Shared variables for time, protected by a mutex
int hours = 0, minutes = 0;
pthread_mutex_t timeMutex = PTHREAD_MUTEX_INITIALIZER;
bool isFirstTime = 1;
pthread_mutex_t isFirstTimeMutex = PTHREAD_MUTEX_INITIALIZER;

// Shared variable to control which component is flashing
enum TimeComponent currentFlash = MINUTE;
pthread_mutex_t flashMutex = PTHREAD_MUTEX_INITIALIZER;

// Thread identifiers
pthread_t threadHour, threadMinute, threadJoystick;

// Global variables
int pauseAlarmFlag = 0;
pthread_mutex_t pauseAlarmMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_pauseAlarm;

static void flashHour(int hours)
{
    lightUpLEDs(hours, I2CDRV_LINUX_BUS1);
    sleepForMs(FLASH_INTERVAL_MS);
    clearLedDisplay(I2CDRV_LINUX_BUS1);
    sleepForMs(FLASH_INTERVAL_MS);
}

static void flashMinute(int minutes)
{
    lightUpLEDs(minutes, I2CDRV_LINUX_BUS2);
    sleepForMs(FLASH_INTERVAL_MS);
    clearLedDisplay(I2CDRV_LINUX_BUS2);
    sleepForMs(FLASH_INTERVAL_MS);
}

static void *flashTime(void *arg)
{
    enum TimeComponent *component = (enum TimeComponent *)arg;
    while (1)
    {
        printf("Inside flashTime for %d.\n", *((int *)arg));
        // check if thread is paused
        pthread_mutex_lock(&pauseAlarmMutex);
        if (pauseAlarmFlag)
        {
            pthread_mutex_unlock(&pauseAlarmMutex);
            pthread_cond_wait(&cond_pauseAlarm, &pauseAlarmMutex);
        }
        pthread_mutex_unlock(&pauseAlarmMutex);

        pthread_mutex_lock(&flashMutex);

        if (*component == currentFlash)
        {
            pthread_mutex_unlock(&flashMutex);

            if (*component == HOUR)
            {
                if (isFirstTime)
                    lightUpLEDs(minutes, I2CDRV_LINUX_BUS2);

                isFirstTime = 0;
                pthread_mutex_lock(&timeMutex);
                flashHour(hours);
                pthread_mutex_unlock(&timeMutex);
            }
            else if (*component == MINUTE)
            {
                if (isFirstTime)
                    lightUpLEDs(hours, I2CDRV_LINUX_BUS1);

                isFirstTime = 0;
                pthread_mutex_lock(&timeMutex);
                flashMinute(minutes);
                pthread_mutex_unlock(&timeMutex);
            }
        }
        else
        {
            pthread_mutex_unlock(&flashMutex);
            sleepForMs(400);
        }
    }
    return NULL;
}

static void *readJoystickInput(void *arg)
{
    double x, y;

    while (1)
    {
        printf("Inside readJoystickInput.\n");
        // check if thread is paused
        pthread_mutex_lock(&pauseAlarmMutex);
        if (pauseAlarmFlag)
        {
            pthread_mutex_unlock(&pauseAlarmMutex);
            pthread_cond_wait(&cond_pauseAlarm, &pauseAlarmMutex);
        }
        pthread_mutex_unlock(&pauseAlarmMutex);

        Joystick_readXY(&x, &y);
        printf("x: %f\n", x);
        printf("y: %f\n", y);

        // Determine which component to flash
        if (x < -0.5)
        {
            pthread_mutex_lock(&isFirstTimeMutex);
            isFirstTime = 1;
            pthread_mutex_unlock(&isFirstTimeMutex);

            pthread_mutex_lock(&timeMutex);
            pthread_mutex_lock(&flashMutex);
            currentFlash = MINUTE;
            pthread_mutex_unlock(&timeMutex);
            pthread_mutex_unlock(&flashMutex);
        }
        else if (x > 0.5)
        {
            pthread_mutex_lock(&isFirstTimeMutex);
            isFirstTime = 1;
            pthread_mutex_unlock(&isFirstTimeMutex);

            pthread_mutex_lock(&timeMutex);
            pthread_mutex_lock(&flashMutex);
            currentFlash = HOUR;
            pthread_mutex_unlock(&timeMutex);
            pthread_mutex_unlock(&flashMutex);
        }

        pthread_mutex_lock(&flashMutex);
        enum TimeComponent component = currentFlash;
        pthread_mutex_unlock(&flashMutex);

        // Adjust time based on joystick up and down movements
        if (y > 0.5)
        {
            if (component == HOUR)
            {
                pthread_mutex_lock(&timeMutex);
                hours = (hours + 1) % 24;
                pthread_mutex_unlock(&timeMutex);
            }
            else if (component == MINUTE)
            {
                pthread_mutex_lock(&timeMutex);
                minutes = (minutes + 1) % 60;
                pthread_mutex_unlock(&timeMutex);
            }
        }
        else if (y < -0.5)
        {
            if (component == HOUR)
            {
                pthread_mutex_lock(&timeMutex);
                hours = (hours - 1 + 24) % 24;
                pthread_mutex_unlock(&timeMutex);
            }
            else if (component == MINUTE)
            {
                pthread_mutex_lock(&timeMutex);
                minutes = (minutes - 1 + 60) % 60;
                pthread_mutex_unlock(&timeMutex);
            }
        }

        sleepForMs(200); // Debounce the joystick input
    }
    return NULL;
}

void initializeAlarmThreads()
{

    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    runCommand("config-pin P9_21 i2c");
    runCommand("config-pin P9_22 i2c");

    enum TimeComponent *hourComponent = malloc(sizeof(enum TimeComponent));
    enum TimeComponent *minuteComponent = malloc(sizeof(enum TimeComponent));
    *hourComponent = HOUR;
    *minuteComponent = MINUTE;

    if (pthread_create(&threadHour, NULL, flashTime, hourComponent) != 0)
    {
        perror("Failed to create hour thread");
        exit(-1);
    }

    if (pthread_create(&threadMinute, NULL, flashTime, minuteComponent) != 0)
    {
        perror("Failed to create minute thread");
        exit(-1);
    }

    if (pthread_create(&threadJoystick, NULL, readJoystickInput, NULL) != 0)
    {
        perror("Failed to create joystick thread");
        exit(-1);
    }
}

void joinAlarmThreads()
{
    pthread_join(threadHour, NULL);
    pthread_join(threadMinute, NULL);
    pthread_join(threadJoystick, NULL);
}

// Getter functions
int getAlarmHour()
{
    pthread_mutex_lock(&timeMutex);
    int temp = hours;
    pthread_mutex_unlock(&timeMutex);

    return temp;
}

int getAlarmMinute()
{
    pthread_mutex_lock(&timeMutex);
    int temp = minutes;
    pthread_mutex_unlock(&timeMutex);

    return temp;
}

void pauseAlarmThread()
{
    pthread_mutex_lock(&pauseAlarmMutex);
    pauseAlarmFlag = 1;
    pthread_mutex_unlock(&pauseAlarmMutex);
}

void unpauseAlarmThread()
{
    pthread_mutex_lock(&pauseAlarmMutex);
    pauseAlarmFlag = 0;
    pthread_mutex_unlock(&pauseAlarmMutex);
    pthread_cond_signal(&cond_pauseAlarm);
    pthread_cond_signal(&cond_pauseAlarm);
    pthread_cond_signal(&cond_pauseAlarm);
}
