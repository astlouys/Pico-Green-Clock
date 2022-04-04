# Next line added
cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

# Next line added
project(Pico-Clock-Green)

pico_sdk_init()


add_executable(Pico-Clock-Green
       Pico-Clock-Green.c
       Ds3231.c Ds3231.h)


# create map/bin/hex file etc.
pico_add_extra_outputs(Pico-Clock-Green)


# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(Pico-Clock-Green pico_stdlib hardware_i2c hardware_adc)


# add url via pico_set_program_url
# example_auto_set_url(Pico-Clock-Green)
