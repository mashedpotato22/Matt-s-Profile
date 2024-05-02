
#include "button.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PATH_GPIO72_VALUE "/sys/class/gpio/gpio72/value"

char *readButtom()
{
    const int MAX_LENGTH = 1024;
    char *USERButton = (char *)malloc(MAX_LENGTH * sizeof(char));
    readFile(PATH_GPIO72_VALUE, USERButton, MAX_LENGTH);
    return USERButton;
}

void configureAndInitializeGPIO(const char *gpioNumber, const char *gpioPath, const char *direction)
{
    // Buffer to hold the output of the pin configuration query
    char buffer[128];

    // Query the current configuration of the pin and get the output in pinStatus file pointer
    FILE *pinStatus = popen("config-pin -q p8.43", "r");
    if (pinStatus == NULL)
    {
        fprintf(stderr, "Could not read pin configuration.\n");
        return;
    }

    while (fgets(buffer, sizeof(buffer), pinStatus) != NULL)
    {
        if (buffer[strspn(buffer, " \t\r\n")] == '\0')
        {
            continue;
        }

        if (strstr(buffer, "gpio") == NULL)
        {

            runCommand("config-pin p8.43 gpio");
        }
    }

    pclose(pinStatus);

    char dirPath[256];
    snprintf(dirPath, sizeof(dirPath), "%s/direction", gpioPath);

    // Check if the GPIO is exported
    if (access(dirPath, F_OK) == -1)
    {
        FILE *exportFile = fopen("/sys/class/gpio/export", "w");
        if (exportFile == NULL)
        {
            perror("ERROR OPENING /sys/class/gpio/export FOR WRITING");
            exit(1);
        }
        fprintf(exportFile, "%s", gpioNumber);
        fclose(exportFile);

        sleepForMs(100);

        FILE *dirFile = fopen(dirPath, "w");
        if (dirFile == NULL)
        {
            perror("ERROR OPENING GPIO DIRECTION FILE FOR WRITING");
            exit(1);
        }
        fprintf(dirFile, "%s", direction);
        fclose(dirFile);
    }
}

char readGPIO(const char *gpioNumber, const char *gpioValuePath)
{
    char gpioPath[128];

    snprintf(gpioPath, sizeof(gpioPath), "/sys/class/gpio/gpio%s", gpioNumber);

    FILE *pGPIOFile = fopen(gpioValuePath, "r");

    if (pGPIOFile == NULL)
    {
        perror("ERROR OPENconfigureAndInitializeGPIOING GPIO VALUE FILE FOR READING");
        exit(1);
    }

    char state;
    fscanf(pGPIOFile, "%c", &state);

    if (state != '0' && state != '1')
    {
        fprintf(stderr, "ERROR READING DATA FROM GPIO VALUE FILE\n");
        exit(1);
    }

    fclose(pGPIOFile);

    return state;
}
