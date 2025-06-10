![image](https://github.com/user-attachments/assets/da7e4c82-f380-4a0c-86c0-5e40cc4aa8cd)

# NOTE: For those looking for a "Network Time Protocol" (NTP) clock with many features, you may want to take a look at my "Pico-RGB-Matrix" repository...

# Pico Green Clock
Firmware Version 9.03 released 09-SEP-2023

NOTE: Source code still shows 9.02. You may check near line 1257... If Spanish month names are there, it means you have Version 9.03.
      Also, be aware that posted ".uf2" versions are still 9.02 (without Spanish support). You'll have to rebuild executables to get Spanish language support for now.

NEW WITH FIRMWARE 9.02 and 9.03:
=======================
- Fix the problem with Wi-Fi connection (CYW43 initialization) with new Pico's SDK library.
- Add Spanish language support. Thanks to George A. Theall ("gatheall" on GitHub) for the translation work !
- Add basic German language support. Thanks to Frank Seidel for the translation !
- Add Czech language support. Thanks to KaeroDot for the excellent work !
- Improve "middle-dot blinking" algorithm (see User Guide for details). Thanks to Frank Seidel for the suggestion and code modification.
- Increase string length for Calendar Events up to 50 characters.
- Fix dates encoded in CalendarEventsGeneric.cpp (examples of Calendar Events).

NOTE: I went through a system crash before the release of version 9.02 and lost a few weeks of work. Also, I had less time for testing / quality control
      before Firmare Version 9.02 release. Please kindly let me know if you find "undocumented features" in the code (read: "bugs").


Firmware Hilights:
==================
- User Guide updated to cover current Firmware Version.
- Support for Network Time Protocol ("NTP"). When using a PicoW, the Green Clock will re-synchronize itself from a time reference server over Internet.
- Add a function to "set-and-save" Wi-Fi credentials to PicoW's flash memory for NTP Wi-Fi access.
- Count-down timer alarm will now ring periodically for 30 minutes or until user presses the top button, whichever happens first.
- Improve clock precision independantly of real-time IC and NTP by better synchronization of callback period time.
- Modify algorithm of the "Chime Hour Count" so that it is less "aggressive" than the original version (thanks to Ewan Harrow for his collaboration).
- Add auto detection of microcontroller type (Pico or PicoW).
- Add an option so that Hourly Chime corresponds to the 12-hour format current value (needs to be turned On in the code).
- Change clock display brightness algorithm. It is now driven by a PWM signal from the Pico.
- Auto Brightness is now smoother and provides hundreds of different levels (Note: if ambient light is dark, clock display becomes **VERY** dim).
- Add a half-hour "Light Chime".
- Add automatic detection of USB CDC connection with an external terminal emulator program (for developers' support).
- Add "snowflakes pixel animation" (remote control required).
- Add Timezone clock parameter allowing proper handling of Summer Time for some countries.
- Timezone is also required for proper NTP handling when adjusting the clock with a time reference from the Internet.
- Automatic handling of "Daylight Saving Time"  /  "Summer Time - Winter Time"  /  "Spring Forward - Fall-Back". Should cover most countries of the world.
- Implement two circular buffers for inter-core communications (core 1 is used for DHT22 support).
- Transfer code for DHT22 support to Pico's second core (core 1).
- Green Clock configuration is now saved to Pico's flash memory and automatically retrieved after a reboot / power-up.
- Nine (9) alarms are now supported with the possibility to configure each of them for as many days-of-week as desired.
- Different sound for each alarm if a passive buzzer has been installed by user or different "train of sounds" if using the integrated active buzzer.
- "QUICK-START" compile time option to skip some device tests during power-up sequence.
- Add a "System Idle-Time Monitor" to get an idea of current system load (remote control required).
- Sound queue allows for different sounds to play "jingles" if user installed a passive buzzer.
- Sound queue allows for easy support of different sound duration and different "trains of sounds" if using the integrated active buzzer.
- Add a sound queue for active buzzer (integrated in Green Clock) and also for an optional passive buzzer to optimize its support. 
- On and Off time (run-time parameters) added for Hourly Chime to make the clock silent during the night.
- Double dots blinking in sequence on the display to know "how deep we are" in the current minute.
- Add more levels of brightness (for display LEDs) with an hysteresis.
- Implement 5 X 7 character set with variable width to allow scrolling of text on clock display (date scrolling uses it for day-of-week and month).
- "Calendar Events" may be configured at compile time. They will scroll on clock display every 30 minutes (for a 24-hour period) during the target date.
- Sequential test of LED matrix (and optional devices if added by user) during power-up sequence.


- Support for those languages:
- Czech - Thanks to KaeroDot for the excellent work on this feature !
- English
- French
- German (basic traslation) - Thanks to Frank Seidel for the translation work !
- Spanish - Thanks to George A. Theall for his translation work !
- NOTE: When a translation is not available for the target language, the English text / message will be used instead.


- DHT22 device support code (DHT22 is an option that must be installed by user).
- BME280 device support code (BME280 is an option that must be installed by user).
- VS1838b infrared sensor support code (VS1838b is an option that must be installed by user).
- Passive buzzer support code (passive buzzer is an option that must be installed by user).


This project provides Firmware for the "Pico Green Clock" product from Waveshare (The clock is available directly from Waveshare website, or from Amazon).
It is based on a Raspberry Pi Pico microcontroller.

Waveshare provides a basic Firmware (Version 1.00) for the Pico Green Clock. Current project provides many enhancements and functionalities to the clock.
You may refer to the User Guide of the most recent Firmware Version (included in this repository) to see the detailed list of changes / enhancements.

Please take note that I'm not a Waveshare employee. This firmware is provided "as is" to help users develop their own code.

## Building from source
Note: The file "pico_sdk_import.cmake" is not included in the repository. I suggest that you make a symbolic link in your project directory:
"ln  -s  /home/pi/pico/pico-sdk/external/pico_sdk_import.cmake"  (assuming that you used the recommended directory names)
This way, as soon as you update your SDK, rebuilding your project will automatically use the latest version.

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
You now have an executable "uf2" that you can transfer to the Pico`s flash memory to run the Pico-Green-Clock.
