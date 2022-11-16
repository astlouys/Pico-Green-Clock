# Pico Green Clock
Firmware Version 7.00 released 16-NOV-2022

Firmware Hilights

- Add UTC (Coordinated Universal Time) clock parameter allowing proper handling of DST for some countries and also as a requirment for future NTP implementation.
- Improve algorithm for automatic handling of Daylight Saving Time / Summer Time - Winter Time / Spring Forward - Fall-Back. Now covers most (if not all) countries of the world.
- Implement two circular buffers for inter-core communications.
- Transfer code for DHT22 support to Pico's second core (core 1).
- Green Clock configuration is now saved to Pico's flash memory and automatically retrieved after a reboot / power-up.
- Nine (9) alarms are now supported with the possibility to configure each of them for as many days-of-week as desired.
- Different sound for each alarm if a passive buzzer has been installed by user or different "train of sounds" if using the integrated active buzzer.
- "QUICK-START" compile time option to skip some device tests during power-up sequence.
- Add a "System Idle-Time Monitor" to get an idea of system load.
- Add a sound queue for active buzzer (integrated in Green Clock) and also for an optional passive buzzer to optimize its support. Different sounds allows for "jingles" to be played if user installed a passive buzzer or different duration and different "trains of sounds" if using the integrated active buzzer.
- On and Off time (run-time parameters) added for hourly chime to make the clock silent during the night.
- Double dots blinking in sequence on the display to know "how deep we are" in the current minute.
- Support for English and French languages (as much as possible, since clock display limits accented letters).
- DHT22 device support code (option that must be installed by user).
- BME280 device support code (option that must be installed by user).
- Infrared sensor support code (option that must be installed by user).
- Passive buzzer support code (option that must be installed by user).
- Add more levels of brightness (for display LEDs) with an hysteresis.
- Add 5 X 7 character set with variable width to allow scrolling of text on clock display (date scrolling takes advantage of it for day-of-week and month).
- "Calendar Events" may be configured at compile time. They will scroll on clock display every 30 minutes (for a 24-hour period) during the target date.
- Sequential test of LED matrix (and optional devices if added by user) during power-up sequence.
- User Guide updated to cover Firmware Version 7.00

This project provides firmware for the "Pico Green Clock" product from Waveshare (available directly from Waveshare website, or from Amazon).
It is based on a Raspberry Pi Pico microcontroller.

Waveshare provides a basic Firmware (Version 1.00) for the Pico Green Clock. Current project provides many enhancements and functionalities to the clock.
You may refer to the User Guide of the most recent Firmware Version (included in this repository) to see the list of changes / enhancements.

Please take note that I'm not a Waveshare employee. This firmware is provided "as is" to help users develop their own code.

## Building from source
1. Follow the instructions for setting up the Pico build environment from https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf
2. In the pico c root directory you want to install the Pico-Green-Clock source from github by running the following commands.
```
$ git clone git@github.com:astlouys/Pico-Green-Clock.git
```
3. Verify that the diretories are the following:
```
$ ls
Pico-Green-Clock        pico-examples           pico-sdk
```
4. Now lets build
```
$ cd Pico-Green-Clock/
$ mkdir build
$ cd build/
$ export PICO_SDK_PATH=../../pico-sdk
$ cmake ..
$ make -j4
$ ls
CMakeCache.txt                  Pico-Clock-Green.dis            Pico-Clock-Green.uf2            pico-sdk
CMakeFiles                      Pico-Clock-Green.elf            cmake_install.cmake             pioasm
Makefile                        Pico-Clock-Green.elf.map        elf2uf2
Pico-Clock-Green.bin            Pico-Clock-Green.hex            generated
```
Perfect you have a built `uf2` binary you can copy over to the clock pico.
