# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
# CMakeLists.txt
# For Pico-Green-Clock
# CMakeLists.txt version for Pico W, with NTP support.
#
set (PICO_BOARD pico_w)
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
#
#
pico_sdk_init()
#
#
message("Running makefsdata python script")
execute_process(COMMAND
        python makefsdata.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
#
#
add_executable(Pico-Green-Clock
       Pico-Green-Clock.c
       picow_ntp_client.c
       Ds3231.c Ds3231.h
       ssi.c cgi.c
       )
#
#
target_include_directories(Pico-Green-Clock PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include # for the WiFi hardware
        )
#
#
# Send Pico's output to USB and also to UART (for debug purposes).
pico_enable_stdio_uart(Pico-Green-Clock 1)
pico_enable_stdio_usb(Pico-Green-Clock 1)
#
#
# Create map/bin/hex/uf2 files etc.
pico_add_extra_outputs(Pico-Green-Clock)
#
#
# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(Pico-Green-Clock hardware_adc hardware_flash hardware_i2c hardware_pwm hardware_sync pico_stdlib pico_unique_id pico_multicore pico_cyw43_arch_lwip_threadsafe_background pico_lwip_http pico_cyw43_arch)
#
#
# add url via pico_set_program_url
# example_auto_set_url(Pico-Green-Clock)
