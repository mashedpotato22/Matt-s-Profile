#ifndef GET_SOUND_HPP
#define GET_SOUND_HPP
#include <stddef.h> 
#include "edge-impulse-sdk/dsp/numpy_types.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/classifier/ei_classifier_types.h"

short sound[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
size_t feature_ix = 0;

#ifdef __cplusplus
extern "C" {
#endif

int getSound(size_t offset, size_t length, float *out_ptr);

#ifdef __cplusplus
}
#endif

#endif // GET_SOUND_HPP