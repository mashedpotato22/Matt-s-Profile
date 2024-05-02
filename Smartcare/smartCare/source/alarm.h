#ifndef ALARM_H
#define ALARM_H

void initializeAlarmThreads();
void joinAlarmThreads();
int getAlarmHour();
int getAlarmMinute();
void pauseAlarmThread();
void unpauseAlarmThread();

#endif