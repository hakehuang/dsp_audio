cmake_minimum_required(VERSION 3.10)
project(NoiseCancellation C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -O3")

add_executable(test_noise_cancellation 
    test_noise_cancellation.c 
    noise_cancellation.c
)

target_link_libraries(test_noise_cancellation m)