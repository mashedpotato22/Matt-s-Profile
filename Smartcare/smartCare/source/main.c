#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "alarm.h"
#include "utility.h"
#include "button.h"
#include "segDisplay.h"
#include "displayTime.h"
#include "babyCryDetector.h"

enum State
{
    ALARM,
    TIMEDISPLAY,
    NONE
};

int main()
{
    // configure green button
    runCommand("config-pin p9.24 gpio");
    writeFile("/sys/class/gpio/gpio15/direction", "in");

    // configure white button
    runCommand("config-pin p9.41 gpio");
    writeFile("/sys/class/gpio/gpio20/direction", "in");

    enum State currentState = TIMEDISPLAY;
    bool isAlarmThreadInitialized = false, isDisplayTimeThreadInitialized = false;
    int alarmHour, alarmMinute;
    int currentHour, currentMinute;
    bool alarmOn = false;
    bool alarmChanged = 0;

    char alarmButtonValue;
    char stateButtonValue;

    bool babyIsCrying = 0;

    initializebabyCryDetector();

    runCommand("echo out > /sys/class/gpio/gpio71/direction");
    runCommand("echo 0 > /sys/class/gpio/gpio71/value");

    runCommand("echo out > /sys/class/gpio/gpio70/direction");
    runCommand("echo 0 > /sys/class/gpio/gpio70/value");

    while (1)
    {
        stateButtonValue = readGPIO("20", "/sys/class/gpio/gpio20/value"); // white button
        alarmButtonValue = readGPIO("15", "/sys/class/gpio/gpio15/value"); // green button
        printf("stateButtonValue = %c \n", stateButtonValue);
        printf("alarmButtonValue = %c \n", alarmButtonValue);

        if (stateButtonValue == '0')
        { // state Button is not pressed
            if (currentState == ALARM)
            {
                pauseBabyCryDetectorThread();
                runCommand("echo 0 > /sys/bus/iio/devices/iio:device0/buffer/enable");
                if (!isAlarmThreadInitialized)
                {
                    initializeAlarmThreads();
                    isAlarmThreadInitialized = true;
                    printf("initialized alarm thread.\n");
                }
                printf("alarm thread.\n");
            }
            else if (currentState == TIMEDISPLAY)
            {
                runCommand("echo 1 > /sys/bus/iio/devices/iio:device0/buffer/enable");
                resumeBabyCryDetectorThread();
                if (!isDisplayTimeThreadInitialized)
                {
                    initializeTimeDisplayThread();
                    isDisplayTimeThreadInitialized = true;
                    printf("initialized time display thread.\n");
                }

                printf("display time thread.\n");

                // check whether need to play alarm
                currentHour = getCurrentHour();
                currentMinute = getCurrentMinute();
                alarmHour = getAlarmHour();
                alarmMinute = getAlarmMinute();
                if (alarmOn && currentHour == alarmHour && currentMinute == alarmMinute)
                {
                    runCommand("mpg123 alarm.mp3");
                    printf("Alarm Time!\n");
                }

                if (alarmButtonValue == '1')
                { // alarm Button is pressed
                    if (alarmOn == 1)
                    {
                        printf("alarmOn = 0;. \n");
                        alarmOn = 0;
                        alarmChanged = 1;
                    }
                    else
                    {
                        printf("alarmOn = 1; \n");
                        alarmOn = 1;
                        alarmChanged = 1;
                    }
                }

                if (alarmOn == 1 && alarmChanged)
                {
                    printf("turn on green led. \n");
                    runCommand("echo out > /sys/class/gpio/gpio70/direction");
                    runCommand("echo 1 > /sys/class/gpio/gpio70/value");
                    alarmChanged = 0;
                }

                else if (alarmOn == 0 && alarmChanged)
                {
                    printf("turn off green led. \n");
                    runCommand("echo out > /sys/class/gpio/gpio70/direction");
                    runCommand("echo 0 > /sys/class/gpio/gpio70/value");
                    alarmChanged = 0;
                }
            }
        }
        // if pressed state buttom
        else
        {
            printf("Pressed state Buttom\n");
            if (currentState == TIMEDISPLAY)
            {
                pauseDisplayThread();
                currentState = ALARM;
                unpauseAlarmThread();
                printf("State changed to ALARM\n");
            }
            else if (currentState == ALARM)
            {
                pauseAlarmThread();
                currentState = TIMEDISPLAY;
                unpauseDisplayThread();
                printf("State changed to timeDisplay\n");
            }
        }
        sleep(2);

        if (getBabyIsCrying())
        {
            runCommand("echo out > /sys/class/gpio/gpio71/direction");
            runCommand("echo 1 > /sys/class/gpio/gpio71/value");

            runCommand("mpg123 lullaby.mp3");

            runCommand("echo out > /sys/class/gpio/gpio71/direction");
            runCommand("echo 0 > /sys/class/gpio/gpio71/value");
        }
    }

    return 0;
}