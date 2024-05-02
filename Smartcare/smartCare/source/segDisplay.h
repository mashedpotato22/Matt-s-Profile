#ifndef SEG_DISPLAY_H
#define SEG_DISPLAY_H

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"
#define I2C_DEVICE_ADDRESS 0x70

int initI2cBus(char* bus, int address);
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
void initializeLedDataInt(unsigned char ledData[8], int number);
void initializeLedDataFloat(unsigned char ledData[8], float f);
void updateLedDisplay(int number);
void displayLedPattern(int i2cFileDesc, unsigned char ledData[8]);
void lightUpLEDs(int number, char* filePath);
void clearLedDisplay(char* filePath);

#endif