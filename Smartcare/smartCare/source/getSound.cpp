#include "getSound.hpp"
#include "edge-impulse-sdk/dsp/numpy.hpp"

int getSound(size_t offset, size_t length, float *out_ptr) {
    return ei::numpy::int16_to_float(sound + offset, out_ptr, length);
}


