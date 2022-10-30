# Pico Green Clock
Firmware Version 6.00 just released 25-OCT-2022

What's new in this Firmware ?

- Green Clock configuration is now saved to Pico's flash memory and automatically retrieved after a reboot / power-up.
- Nine (9) alarms are now supported with the possibility to configure each of them for as many days-of-week as desired.
- Different sound for each alarm is a passive buzzer is installed by user or different "train of sounds" if using the integrated active buzzer.
- "QUICK-START" compile-time option to skip some device tests during power-up sequence.
- Add a "System Idle-Time Monitor" to get an idea of system load.
- Add a sound queue for active buzzer (integrated in Green Clock) and also for an optional passive buzzer to optimize its support. Different sounds allows for "jingles" to be played if user installed a passive buzzer or different duration and different "trains of sounds" if using the integrated active buzzer.
- On and Off time run-time parameters added for hourly chime to make the clock silent during the night.
- Double dots blinking in sequence on the display to know "how deep we are" in the current minute.
- Support for English and French languages (as much as possible, since clock display limits accented letters).
- DHT22 device support code.
- BME280 device support code.
- Infrared sensor support code.
- Passive buzzer support code.
- Add more levels of brightness with an hysteresis.
- 5 X 7 character set with variable width allows scrolling of text on clock display (date scrolling takes advantage of it for day-of-week and month).
- "Calendar Events" may be configured at compile time. They will scroll on clock display every 30 minutes during the tagged date (for a 24-hour period).
- Automatic support for Daylight Saving Time (compliant with North American standard, but could be bonified by user for other standards).
- Sequential test of LED matrix during power-up sequence.
- The User Guide has now been updated for Firmware Version 6.00

This project provides firmware for the "Pico Green Clock" product from Waveshare (available directly from Waveshare website, or from Amazon).
It is based on a Raspberry Pi Pico microcontroller.

Waveshare provides a basic Firmware (Version 1.00) for the Pico Green Clock. Current project provides many enhancements and functionalities to the clock.
You may refer to the User Guide of the most recent firmware version (included in this repository) to see the list of changes / enhancements.

Please take note that I'm not a Waveshare employee. This firmware is provided "as is" to help users develop their own code.
