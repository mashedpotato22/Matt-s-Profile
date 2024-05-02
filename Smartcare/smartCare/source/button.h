#ifndef BUTTON_H
#define BUTTON_H

char* readButtom();
void configureAndInitializeGPIO(const char* gpioNumber, const char* gpioPath, const char* direction);
char readGPIO(const char* gpioNumber, const char* gpioValuePath);



#endif