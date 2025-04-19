#ifndef NOISE_CANCELLATION_H
#define NOISE_CANCELLATION_H

#include <stdint.h>

#define FILTER_LENGTH 128

typedef struct {
    float w[FILTER_LENGTH];
    float x[FILTER_LENGTH];
    uint32_t index;
} LMSFilter;

void process_audio_frame(float *main_mic, float *ref_mic, float *output, uint32_t frame_size);

#endif