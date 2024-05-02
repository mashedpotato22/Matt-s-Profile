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

void clearLedDisplay(char *filePath)
{
    unsigned char clearData[8] = {0};
    int i2cFileDesc = initI2cBus(filePath, I2C_DEVICE_ADDRESS);
    displayLedPattern(i2cFileDesc, clearData);
}

void lightUpLEDs(int number, char *filePath)
{

    unsigned char ledData[8];
    initializeLedDataInt(ledData, number);

    int i2cFileDesc = initI2cBus(filePath, I2C_DEVICE_ADDRESS);

    writeI2cReg(i2cFileDesc, 0x21, 0x00);
    writeI2cReg(i2cFileDesc, 0x81, 0x00);

    for (int i = 0; i < 16; i += 2)
    {
        writeI2cReg(i2cFileDesc, i, ledData[i / 2]);
        writeI2cReg(i2cFileDesc, i + 1, 0x00);
    }

    close(i2cFileDesc);
}

static unsigned char patterns[10][8] = {
    // Pattern for 0
    {0x07, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x07},
    // Pattern for 1
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
    // Pattern for 2
    {0x07, 0x04, 0x04, 0x07, 0x01, 0x01, 0x01, 0x07},
    // Pattern for 3
    {0x07, 0x04, 0x04, 0x07, 0x04, 0x04, 0x04, 0x07},
    // Pattern for 4
    {0x05, 0x05, 0x05, 0x07, 0x04, 0x04, 0x04, 0x04},
    // Pattern for 5
    {0x07, 0x01, 0x01, 0x07, 0x04, 0x04, 0x04, 0x07},
    // Pattern for 6
    {0x07, 0x01, 0x01, 0x07, 0x05, 0x05, 0x05, 0x07},
    // Pattern for 7
    {0x07, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x04},
    // Pattern for 8
    {0x07, 0x05, 0x05, 0x07, 0x05, 0x05, 0x05, 0x07},
    // Pattern for 9
    {0x07, 0x05, 0x05, 0x07, 0x04, 0x04, 0x04, 0x07}};

static unsigned char patternsFloat[10][8] = {
    // Pattern for 0
    {0x07, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x0F},
    // Pattern for 1
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0A},
    // Pattern for 2
    {0x07, 0x04, 0x04, 0x07, 0x01, 0x01, 0x01, 0x0F},
    // Pattern for 3
    {0x07, 0x04, 0x04, 0x07, 0x04, 0x04, 0x04, 0x0F},
    // Pattern for 4
    {0x05, 0x05, 0x05, 0x07, 0x04, 0x04, 0x04, 0x0C},
    // Pattern for 5
    {0x07, 0x01, 0x01, 0x07, 0x04, 0x04, 0x04, 0x0F},
    // Pattern for 6
    {0x07, 0x01, 0x01, 0x07, 0x05, 0x05, 0x05, 0x0F},
    // Pattern for 7
    {0x07, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x0C},
    // Pattern for 8
    {0x07, 0x05, 0x05, 0x07, 0x05, 0x05, 0x05, 0x0F},
    // Pattern for 9
    {0x07, 0x05, 0x05, 0x07, 0x04, 0x04, 0x04, 0x0F}};

void displayLedPattern(int i2cFileDesc, unsigned char ledData[8])
{
    for (int i = 0; i < 16; i += 2)
    {
        writeI2cReg(i2cFileDesc, i, ledData[i / 2]);
        writeI2cReg(i2cFileDesc, i + 1, 0x00);
    }
}

void initializeLedDataInt(unsigned char ledData[8], int number)
{
    int tens = number / 10;
    int ones = number % 10;
    for (int row = 0; row < 8; row++)
    {
        ledData[row] = patterns[tens][row];
    }
    for (int i = 0; i < 8; i++)
    {
        ledData[i] = (patterns[ones][i] << 4) | patterns[tens][i];
    }
}

void initializeLedDataFloat(unsigned char ledData[8], float f)
{
    int number = (int)(f * 10);
    int tens = number / 10;
    int ones = number % 10;
    for (int row = 0; row < 8; row++)
    {
        ledData[row] = patternsFloat[tens][row];
    }
    for (int i = 0; i < 8; i++)
    {
        ledData[i] = (patternsFloat[ones][i] << 4) | patternsFloat[tens][i];
    }
}

int initI2cBus(char *bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    if (i2cFileDesc < 0)
    {
        printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
        perror("Error is:");
        exit(-1);
    }

    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0)
    {
        perror("Unable to set I2C device to slave address.");
        exit(-1);
    }
    return i2cFileDesc;
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2)
    {
        perror("Unable to write i2c register");
        exit(-1);
    }
}
