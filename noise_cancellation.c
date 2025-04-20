#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define FILTER_LENGTH 128
#define MU 0.01f

typedef struct {
    float w[FILTER_LENGTH];  // 滤波器系数
    float x[FILTER_LENGTH];  // 延迟线缓存
    uint32_t index;          // 当前写入位置
} LMSFilter;

// 初始化LMS滤波器
void lms_init(LMSFilter *filter) {
    for(int i=0; i<FILTER_LENGTH; i++) {
        filter->w[i] = 0.0f;
        filter->x[i] = 0.0f;
    }
    filter->index = 0;
}

// 执行噪声消除
// 添加头文件
#include <stdbool.h>

// 添加全局变量存储噪声特征
static float adc_noise_rms = 0.0f;
static bool noise_calibrated = false;

// 添加噪声测量函数
// Add this macro definition before the functions
#define NOISE_SAMPLES 1000

// Change the first function name to avoid conflict
void calibrate_adc_noise(int calibration_samples) {
    float sum = 0.0f;
    
    for(int i=0; i<calibration_samples; i++) {
        float sample = 0.0f; // Replace with actual ADC read
        sum += sample * sample;
    }
    
    adc_noise_rms = sqrtf(sum / calibration_samples);
    noise_calibrated = true;
}

// Add function prototype for ADC reading
float read_adc_input(); // Should be implemented in your hardware layer

// Keep the measurement function
float measure_adc_noise() {
    float noise[NOISE_SAMPLES];
    float sum = 0.0f;
    
    for(int i=0; i<NOISE_SAMPLES; i++) {
        noise[i] = read_adc_input();
        sum += noise[i] * noise[i];
    }
    return sqrt(sum / NOISE_SAMPLES);
}

// 修改噪声消除函数
float noise_cancellation(LMSFilter *filter, float primary, float reference) {
    // 更新参考信号延迟线
    filter->x[filter->index] = reference;
    
    // 计算滤波器输出
    float y = 0.0f;
    int tap = filter->index;
    for(int i=0; i<FILTER_LENGTH; i++) {
        y += filter->w[i] * filter->x[tap];
        tap = (tap == 0) ? FILTER_LENGTH-1 : tap-1;
    }

    // 计算误差信号（期望信号）
    float error = primary - y;

    // 添加噪声阈值处理
    if(noise_calibrated && fabsf(error) < adc_noise_rms * 3.0f) {
        error = 0.0f;  // 忽略小于3倍本底噪声的信号
    }
    // 更新滤波器系数
    tap = filter->index;
    for(int i=0; i<FILTER_LENGTH; i++) {
        filter->w[i] += MU * error * filter->x[tap];
        tap = (tap == 0) ? FILTER_LENGTH-1 : tap-1;
    }

    // 更新延迟线索引
    filter->index = (filter->index + 1) % FILTER_LENGTH;

    return error;
}

// 双通道噪声消除接口
void process_audio_frame(float *main_mic, float *ref_mic, float *output, uint32_t frame_size) {
    static LMSFilter noise_filter;
    static uint8_t initialized = 0;
    
    if(!initialized) {
        lms_init(&noise_filter);
        calibrate_adc_noise(1000);  // Add calibration during initialization
        initialized = 1;
    }

    for(uint32_t i=0; i<frame_size; i++) {
        output[i] = noise_cancellation(&noise_filter, main_mic[i], ref_mic[i]);
    }
}

// 生成典型ADC噪声模型（白噪声+1/f噪声）
float generate_adc_noise(float time) {
    static float last_noise = 0.0f;
    float white_noise = (float)rand()/RAND_MAX * 2.0f - 1.0f;
    float pink_noise = last_noise * 0.9f + white_noise * 0.1f; 
    last_noise = pink_noise;
    return white_noise * 0.7f + pink_noise * 0.3f; // 混合噪声
}

// Add this fake ADC implementation at the end of the file
#ifndef USD_HW_ADC
float read_adc_input() {
    // Simulate ADC noise with random values
    static unsigned int seed = 12345;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return (float)(seed % 1000) / 1000.0f - 0.5f; // Returns -0.5 to +0.5
}
#endif