cmake_minimum_required(VERSION 3.12)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(PicoKernel C CXX ASM)
pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(hello_multicore
        pOS_main.cpp
        pOS_scheduler.cpp
        pOS_task.cpp
        pOS_thread.cpp
        pOS_kernel.S
        pOS_gpio.cpp
        pOS_memory.cpp
        pOS_critical_section.cpp
        pOS_semaphore.cpp
        pOS_mutex.cpp
        pOS_communication.cpp
        pOS_multicore.cpp
        pOS_memory_protection.cpp
        pOS_utilities.cpp
        pOS_display.cpp)

# Add pico_multicore which is required for multicore functionality
target_link_libraries(hello_multicore
        pico_stdlib
        pico_multicore
        hardware_adc
        hardware_pwm
        hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_multicore)

# add url via pico_set_program_url
# example_auto_set_url(hello_multicore)
