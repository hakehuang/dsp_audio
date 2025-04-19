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
        initialized = 1;
    }

    for(uint32_t i=0; i<frame_size; i++) {
        output[i] = noise_cancellation(&noise_filter, main_mic[i], ref_mic[i]);
    }
}