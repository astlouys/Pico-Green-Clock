# CMakeLists.txt
# For Pico-Green-Clock
# CMakeLists.txt version for Pico, without NTP support.
#
#
cmake_minimum_required(VERSION 3.13)
#
#
include(pico_sdk_import.cmake)
#
#
project(Pico-Green-Clock LANGUAGES C CXX ASM)
#
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico)
#
#
pico_sdk_init()
#
#
add_executable(Pico-Green-Clock
	Pico-Green-Clock.c
	Ds3231.c Ds3231.h
	)
#
#
# Send Pico's output to USB and UART (for debug purposes).
pico_enable_stdio_uart(Pico-Green-Clock 1)
pico_enable_stdio_usb(Pico-Green-Clock  1)
#
#
# Pull in our pico_stdlib which pulls in commonly used features.
target_link_libraries(Pico-Green-Clock pico_stdlib hardware_adc hardware_i2c hardware_pwm pico_multicore)
#
#
pico_add_extra_outputs(Pico-Green-Clock)

