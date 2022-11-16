cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(Pico-Clock-Green)

pico_sdk_init()


add_executable(Pico-Clock-Green
       Pico-Clock-Green.c
       Ds3231.c Ds3231.h)


# Send Pico's output to USB instead of UART (for debug purposes).
pico_enable_stdio_uart(Pico-Clock-Green 0)
pico_enable_stdio_usb(Pico-Clock-Green 1)


# Create map/bin/hex file etc.
pico_add_extra_outputs(Pico-Clock-Green)


# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(Pico-Clock-Green hardware_adc hardware_flash hardware_i2c hardware_pwm hardware_sync pico_stdlib pico_unique_id pico_multicore)


# add url via pico_set_program_url
# example_auto_set_url(Pico-Clock-Green)
