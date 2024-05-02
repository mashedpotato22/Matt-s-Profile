#ifndef UTILITY_H
#define UTILITY_H

#include <sys/time.h>
#include <stddef.h>

long long getTimeInMicros(void);
void sleepForMs(const long long delayInMs);

void readFile(const char *fileName, char* buff, const int MAX_LENGTH);
void writeFile(const char *fileName, const char *content);

void runCommand(const char *command);

#endif
