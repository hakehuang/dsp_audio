

```markdown:z:\audio_dsp\dsp_audio\README.md
# 实时噪声消除系统

基于LMS自适应滤波器的实时音频降噪解决方案

## 🛠 编译指南

### Linux/Unix 系统
```bash
# 创建构建目录
mkdir -p build && cd build

# 生成构建系统
cmake ..

# 编译可执行文件
make

# 运行测试程序
./test_noise_cancellation
```

### Windows 系统（使用MinGW）
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
test_noise_cancellation.exe
```

## 📂 项目结构
```
.
├── CMakeLists.txt          # CMake构建配置
├── noise_cancellation.h    # 滤波器头文件（结构体定义/接口）
├── noise_cancellation.c    # LMS算法核心实现
└── test_noise_cancellation.c # 自动化测试套件
```

## ✨ 功能特性
- 128阶LMS自适应滤波器
- 实时音频帧处理（16kHz采样率）
- 白噪声消除测试场景
- 信号延迟自动补偿（5ms）
- RMS性能评估指标
- 跨平台支持（Linux/Windows）

## 🧪 测试验证
```bash
# 预期输出示例
Noise cancellation test RMS: 0.5615
[延迟验证] 配置延迟:5ms 实际测量延迟:5ms
```

## ⚙️ 参数配置
```c
// test_noise_cancellation.c
#define TEST_FRAME_SIZE 256    // 测试帧长度 
#define SAMPLE_RATE 16000      // 采样率
#define DELAY_MS 5             // 系统延迟

// noise_cancellation.c
#define FILTER_LENGTH 128     // 滤波器阶数
#define MU 0.01f              // 自适应步长因子
```

## 📝 注意事项
1. 依赖环境：
   - C编译器（GCC/Clang ≥8.0 或 MSVC ≥2019）
   - CMake ≥3.10
   - Math库（自动链接）

2. Ubuntu/Debian依赖安装：
```bash
sudo apt install build-essential cmake
```

3. 测试信号参数调整后需要重新编译生效

## 📊 结果分析
- **RMS值 ≈ 0.56**：接近纯净信号理论值（0.8/√2≈0.5657），表明噪声被有效消除
- **RMS值 > 0.6**：可能表示算法收敛不足，建议：
  1. 增加滤波器阶数（FILTER_LENGTH）
  2. 减小步长因子（MU）
  3. 延长测试时长

## 开源协议
本项目采用 MIT License 开源，详情见 LICENSE 文件
```