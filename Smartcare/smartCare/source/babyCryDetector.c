#include "babyCryDetector.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "edge-impulse-sdk/dsp/numpy_types.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/classifier/ei_classifier_types.h"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "stdint.h"
#include <time.h>
#include <errno.h>
#include "getSound.hpp"

static pthread_t threadBabyCryDetector;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int pauseFlag = 0;
static bool babyIsCrying = 0;
static pthread_mutex_t babyIsCryingmutex = PTHREAD_MUTEX_INITIALIZER;

#define AUDIO_READ_BUFFER_SIZE 200
#define AUDIO_BUFFER_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE
#define BUFFER_PATH “/ sys / bus / iio / devices / iio : device0 / buffer / enable”

EI_IMPULSE_ERROR run_classifier(signal_t *, ei_impulse_result_t *, bool);

// millisecond sleep
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void audioClassifier()
{
    printf("Inside audioClassifier.\n");
    signal_t signal;
    static ei_impulse_result_t result;

    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &getSound;

    printf("start run_classifier_continuous.\n");
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

    printf("%s: %f\n", result.classification[0].label, result.classification[0].value);
    printf("%s: %f\n", result.classification[1].label, result.classification[1].value);
    printf("%s: %f\n", result.classification[2].label, result.classification[2].value);
    float babyCryValue = result.classification[0].value;
    if (babyCryValue > 0.7)
    {
        printf("Baby cry detected!\n");
        pthread_mutex_lock(&babyIsCryingmutex);
        babyIsCrying = 1;
        pthread_mutex_unlock(&babyIsCryingmutex);
    }
    else
    {
        printf("No baby cry detected!\n");
        pthread_mutex_lock(&babyIsCryingmutex);
        babyIsCrying = 0;
        pthread_mutex_unlock(&babyIsCryingmutex);
    }
}

void *run(void *arg)
{
    printf("Inside run method.\n");
    int16_t buffer[AUDIO_READ_BUFFER_SIZE];
    int fd = open("/dev/iio:device0", O_RDONLY | O_NONBLOCK);
    if (fd == -1)
    {
        printf("error");
        exit(1);
    }

    int count = 0;
    printf("init classifier.\n");
    while (1)
    {
        read(fd, buffer, AUDIO_READ_BUFFER_SIZE * sizeof(uint16_t));
        pthread_mutex_lock(&mutex);
        if (pauseFlag)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        for (int i = 0; i < AUDIO_READ_BUFFER_SIZE; i++)
        {
            if (count >= AUDIO_BUFFER_SIZE)
            {
                count = 0;
                audioClassifier();
            }
            sound[count++] = buffer[i] * 1.7;
        }
        msleep(25);
    }
    return NULL;
}

void initializebabyCryDetector()
{
    pthread_create(&threadBabyCryDetector, NULL, run, NULL);
}

void pauseBabyCryDetectorThread()
{
    pthread_mutex_lock(&mutex);
    pauseFlag = 1;
    pthread_mutex_unlock(&mutex);
}

void resumeBabyCryDetectorThread()
{
    pthread_mutex_lock(&mutex);
    pauseFlag = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void stopBabyCryDetectorThread()
{

    pthread_join(threadBabyCryDetector, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&babyIsCryingmutex);
    pthread_cond_destroy(&cond);
}

bool getBabyIsCrying()
{
    pthread_mutex_lock(&babyIsCryingmutex);
    bool temp = babyIsCrying;
    pthread_mutex_unlock(&babyIsCryingmutex);
    return temp;
}
