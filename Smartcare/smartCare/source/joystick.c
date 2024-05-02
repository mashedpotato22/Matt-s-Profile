#include "joystick.h"
#include "utility.h"
#include <stdlib.h>
#define PATH_JOYSTICK_X "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define PATH_JOYSTICK_Y "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"

void Joystick_readXY(double *x, double *y)
{
    if (x == NULL || y == NULL)
    {
        return;
    }

    const int MAX_LENGTH = 1024;
    char buff1[MAX_LENGTH];
    char buff2[MAX_LENGTH];

    readFile(PATH_JOYSTICK_X, buff1, MAX_LENGTH);
    readFile(PATH_JOYSTICK_Y, buff2, MAX_LENGTH);

    int rawX = atoi(buff1), rawY = atoi(buff2);

    *x = (double)rawX / 4095.0 * 2.0 - 1.0;
    *y = (double)rawY / 4095.0 * 2.0 - 1.0;
}
