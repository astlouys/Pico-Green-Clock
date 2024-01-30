# CMakeLists.txt
# For Pico-Green-Clock
# CMakeLists.txt version for Pico W, with NTP support.
#
#
cmake_minimum_required(VERSION 3.13)
#
#
include(pico_sdk_import.cmake)
#
#
project(Pico-Green-Clock)
#
#
# set (CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set (C_STANDARD 11)
set (CXX_STANDARD 17)
set (PICO_BOARD pico_w)
#
#
pico_sdk_init()
#
#
message("Running makefsdata python script")
execute_process(COMMAND
        py makefsdata.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
#
#
add_executable(Pico-Green-Clock
       Pico-Green-Clock.c
       picow_ntp_client.c
       Ds3231.c Ds3231.h)
#
#
target_include_directories(Pico-Green-Clock PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
#
#
# Send Pico's output to USB and also to UART (for debug purposes).
pico_enable_stdio_uart(Pico-Green-Clock 0)
pico_enable_stdio_usb(Pico-Green-Clock 1)
#
#
# Create map/bin/hex/uf2 files etc.
pico_add_extra_outputs(Pico-Green-Clock)
#
#
# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(Pico-Green-Clock hardware_adc hardware_flash hardware_i2c hardware_pwm hardware_sync pico_stdlib pico_unique_id pico_multicore pico_cyw43_arch_lwip_threadsafe_background pico_lwip_http)
#
#
# add url via pico_set_program_url
# example_auto_set_url(Pico-Green-Clock)
