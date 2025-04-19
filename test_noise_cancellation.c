#include "noise_cancellation.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>  // For memcpy

#define TEST_FRAME_SIZE 256
#define SAMPLE_RATE 16000

// Add timestamp calculation
#define DELAY_MS 5  // Simulate 5ms system delay
#define DELAY_SAMPLES (DELAY_MS * SAMPLE_RATE / 1000)

static float generate_sine_wave(float freq, float time) {
    return sin(2 * M_PI * freq * time);
}

static float generate_correlated_noise(float time) {
    // Safe type conversion using memcpy
    unsigned int seed;
    memcpy(&seed, &time, sizeof(seed));
    srand(seed); 
    return (float)rand()/RAND_MAX * 2.0f - 1.0f;
}

void test_basic_noise_cancellation() {
    float main_mic[TEST_FRAME_SIZE];
    float ref_mic[TEST_FRAME_SIZE];
    float output[TEST_FRAME_SIZE];
    float ref_noise_buffer[DELAY_SAMPLES] = {0};
    uint32_t buffer_index = 0;

    for(int i=0; i<TEST_FRAME_SIZE; i++) {
        float time = (float)i / SAMPLE_RATE;
        
        // Generate reference noise with current timestamp
        float current_ref_noise = generate_correlated_noise(time) * 0.5f;
        ref_mic[i] = current_ref_noise;

        // Main signal uses delayed reference noise + sine
        // Removed unused delayed_time variable
        main_mic[i] = generate_sine_wave(1000, time) * 0.8f + 
                      ref_noise_buffer[buffer_index] * 0.2f;

        // Update noise buffer with current reference
        ref_noise_buffer[buffer_index] = current_ref_noise;
        buffer_index = (buffer_index + 1) % DELAY_SAMPLES;
    }

    process_audio_frame(main_mic, ref_mic, output, TEST_FRAME_SIZE);

    // 验证输出信号RMS值
    float rms = 0.0f;
    for(int i=0; i<TEST_FRAME_SIZE; i++) {
        rms += output[i] * output[i];
    }
    rms = sqrt(rms/TEST_FRAME_SIZE);
    
    printf("Noise cancellation test RMS: %.4f\n", rms);
}

int main() {
    test_basic_noise_cancellation();
    return 0;
}