/* ================================================================== *\
   Pico-Clock-Green.c
   St-Louys, Andre - February 2022
   astlouys@gmail.com
   Revision 01-JUN-2023
   Compiler: arm-none-eabi-gcc 7.3.1
   Version 10.00

   Raspberry Pi Pico firmware to drive the Waveshare Pico-Green-Clock.
   From an original software version 1.00 by Waveshare
   Released under 3-Clause BSD License.

   NOTE:
   THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
   WITH CODING INFORMATION REGARDING THEIR PRODUCT IN ORDER FOR THEM TO SAVE
   TIME. AS A RESULT, WAVESHARE OR THE AUTHOR SHALL NOT BE HELD LIABLE FOR ANY
   DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
   FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
   CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS. ALSO
   NOTE THAT THE AUTHOR IS NOT A WAVESHARE EMPLOYEE.



   REVISION HISTORY:
   =================
   04-FEB-2022  1.00 - Initial release from Waveshare.

   10-FEB-2022  2.00 - Global code reformatting and rework.
                     - Fix a bug allowing "DayOfMonth" to be set to 0.
                     - Fix a bug allowing "DayOfMonth" to go higher than the maximum of 28, 29, 30 or 31 (dependingget_day_of_week of the  month).
                     - Fix a bug and make sure the "count down" indicator is turned off when count-down timer reaches 00m00s.
                     - Fix a bug when the clock is set for 12-hours time display and is displaying 00h00 AM instead of 12h00 AM.
                     - Add a "test" section to put many chunks of code for testing and debugging purposes.
                     - Add "FRENCH" option so that the date can be displayed in the corresponding format (compile-time option).
                       (It will be easy to add other languages if the programmer makes a search for "ENGLISH" and / of "FRENCH" in the code...
                       ...and assuming the characters required are standard ASCII characters, similar to English).
                     - Implement 5 X 7 character set with variable character width.
                     - Implement a reverse_bits() function allowing the bitmap of the 5 X 7 characters to be more intuitively defined.
                     - Add a generic "scroll_string()" function and a fill_display_buffer_5X7() function to easily handle 5 X 7 characters.
                     - Change the name of many functions to make them more representative of what they do.
                     - Make the scroll_string() function so that it can accept a string longer than what can be handled by the available
                       framebuffer. The function will wait until the framebuffer gets some free space to transfer next chunk of the string.
                     - Add a specific section at the top of source code to select many default clock options at compile time.
                     - Implement different tone types with different number of tones and duration for different events.
                       (Note: Tone frequency can't be changed since the oscillator is integrated in the clock active buzzer).
                     - Clock display will blink the two center dots according to the number of seconds passed since the last minute change.
                     - Add automatic handling of "Daylight Saving Time" for most northern hemisphere countries.
                       (Provision has been made to add different schemes of daylight saving time for other areas of the world).
                     - Add a new option for hourly chime. It may be On, Off, or "Day" (OI for "On, Intermittent"). If set to "Day",
                       chime will sound only during day hours, as defined between CHIME_TIME_ON and CHIME_TIME_OFF set during clock setup.
                       (They are 9h00 and 21h00 by default).
                     - Add handling of "Calendar events" that can be added by user at compile time. During the specified day of the year,
                       the string defined by the user will scroll on clock display twice an hour, at xxh05 and xxh35. Moreover, a special sound
                       will be heard when the message start scrolling during daytime as defined between CHIME_TIME_ON and CHIME_TIME_OFF.
                     - Add an option in the clock setup to configure the Chime Time On and Chime Time Off.

   24-MAR-2022  3.00 - More code rework and optimisation.
                     - When powering up the clock, replace DST = 0xFF (hex value) by "DST = On" (and replace "DST = 0x00" by "DST = Off").
                     - Add logic for 2 different "repeat counts" for sounds, to add versatility and more possibilities for different sound codes.
                     - Day-of-month will change automatically if it becomes out-of-bound while setting up current month
                       (for example, day-of-month will change automatically from 31 to 28 while we change month from 3 (March) to 2 (February).
                     - Add suffix to day-of-month when scrolling the date in English (now "...March 31st...").
                     - The setting of Chime time On and Chime time Off now comply with 12 or 24-hours time format current clock setting.
                     - Fix a bug allowing Hourly Chime sound to be heard one hour later than the Chime time Off.
                     - Fix a bug allowing Hourly Chime to sound sometimes while doing clock setup.
                     - The setting of alarms now comply with 12 or 24-hours time format current clock setting.
                     - To help knowing which alarm is On (0 or 1), when Alarm 0 is On, Left Alarm LED is On in the
                       alarm indicator and when Alarm 1 is On, Right Alarm LED is On in the indicator.
                       NOTE: On power up, both alarms are set to OFF (this is also true in case of power failure), since there
                             is no backed-up RAM available in the RTC IC to save such variables in case of power failure.
                     - Change the logic so that each alarm (0 and 1) are now checked individually in the RTC IC.
                     - Add the logic for both alarms (0 and 1) to have a distinct (different) alarm sound.
                     - When setting up alarms, current alarm parameters saved in RTC IC are now proposed to user as default values.
                       Note: As mentioned above, alarm status (On or Off) is lost on power failure (and both alarms restart as Off),
                             but other alarm parameters (Hour, Minute, Day-of-week) are kept in RTC IC with battery back-up.
                     - Now blink the day-of-week while setting up day-of-week in alarm setup.
                     - Language selection is now a run-time option. English and French are the available languages for now.
                     - Hourly chime: add logic for "nighttime workers". If Chime time On is greater than Chime time Off
                       (as opposed to what we would normally expect), we assume that we want sounds to be heard during
                       nighttime and not heard during daytime. Hourly chime will then sounds after Chime time On (in the evening)
                       and before Chime time Off (in the morning).
                     - Add support for DHT22 (humidity and temperature sensor - compile time option). To implement this option, it must
                       be "#define" in the code, and a 3-wire cable must be installed between GPIO8, 3.3V and GND, and a DHT22
                       can be installed outdoor.
                     - Add Daily saving time as run-time parameter. User can select 0 (no support for DST) or 1 (North-America-like DST support).
                     - Add some of the changes already made by David Ruck (David is also on GitHub) to fine tune auto brightness.
                     - When time display format is H12 (12-hours format), do not print a leading 0 to Hour to comply with the standard.

   04-APR-2022  4.00 - Some code optimization (there is much more to do !).
                     - Major cleanup in the "setup flags" to make it more straightforward to add options to the clock setup list of parameters.
                     - Transferred "Temperature unit" setup to the list of clock parameters setup.
                     - Transferred "Auto-brightness" setup to the list of clock parameters setup.
                       (NOTE: A copy of the "auto-brightness" function code has been left on the "bottom button quick press" for faster access if required).
                     - Implement new "Night light" feature: use the two white LEDs inside the clock as a night light. It can be
                       turned On, Off, Automatic or OI ("On, Intermittent" - see Hourly chime for explanation about "OI" setting).
                     - Since temperature unit setting has been transferred to the general clock setup, "Up" (middle) button quick press
                       will now display outside temperature and humidity (if user installed a DHT22).
                     - To be more deterministic, instead of scrolling the date every xxx minutes, scrolling will now occurs when reaching
                       an integer number of times the period specified. So, if we scroll the date every 5 minutes, scrolling will occur
                       at xh00, xh05, xh10, xh15 and so on. This way, we know how long we have to wait until next scroll.
                     - Add support for a remote control (see User guide for details). Infrared sensor (VS1838B-type) must be bought and installed
                       by user to enable this option.
                     - Time checking of calendar events has been changed from xxh05 and xxh35 to xxh14 and xxh44. Those values are unlikely to be a
                       multiple integer value of the time period to display date and then, those two actions (date display and calendar events) will
                       not interfere with one another.
                     - Implement many functions that are accessible only with remote control (see User guide for details):
                     - Emulate the three clock buttons on the remote control so that we can achieve a similar behavior remotely.
                     - Fix a bug in reading VSYS (power supply voltage).
                     - Add some pixel twinkling when software starts (just for fun...)
                     - Add column-by-column LED matrix test to make it easy to see if all LEDs work fine. Idem for all clock display indicators.
                     - Add firmware support for an optional passive piezo so that sound frequency may be changed / modulated by software.
                       PWM has been selected to drive the passive piezo. If we would use a completely "software" oscillator, we would
                       hear hiccups in the sound when the other ISR / callback functions run (this processing would interfere
                       with the regularity of the sound). Similar to the scroll queue, a sound queue (circular buffer) has been implemented
                       to handle the sounds of the passive piezo.
                     - Add the structure member "Jingle" to the calendar Event structure. If it is not "0", it identifies a jingle (quick music)
                       that will play when the calendar event is scrolled on the display. (For example, we could ask to play "Jingle bell",
                       along with the message "Merry Christmas" on December 25th. (NOTE: Passive piezo must have been bought and installed by user
                       to support this option).
                     - Add Pico internal temperature display (compliant to the temperature unit - C or F - that has been selected for other temperatures).
                     - Add programmable "silence period" to temporarily turn off hourly chime and calendar event sounds (available with remote control).
                     - Add "dice rolling" to randomly display 2 dice values (from 1 to 6) on display (available with remote control).
                     - Modify adc_read_light() function so that light value returned is intuitive: higher number means more light. Auto-brightness and
                       automatic night-light functions have been adapted accordingly.

   19-JUN-2022  5.00 - Uncomment matrix_test() and pixel_twinkling() during power-up sequence that were left mistakenly commented-out in release 4.00.
                     - Add scrolling of firmware version number during power-up and in display function.
                     - Scroll Pico's "unique number" ("serial number") during power-up and in display function.
                     - When hour changes (from xh59 to xh00), date scrolling will now begin only 5 seconds later to let the time to look at the clock
                       display to see current time when we hear the hourly chime. This is true only from xh59 to xh00 and not for other integer number
                       of times the specified period (for example from xh04 to xh05 if date scrolling period is set to 5 minutes).
                     - Command "Display auto-bright" (with remote control generic display): logic has been fixed for French message.
                     - Modify the logic behind "automatic night light". There is now a "twilight" period during which the night light status does not
                       change. This is to prevent a period of "On/Off" toggling when light level is just around the value to change night light status.
                     - Modify the logic behind "clock auto-light" (or automatic brightness), so that clock display does not dim at all when ambient
                       light intensity is high enough.
                     - Also, slow down the frequency for reading ambient light (from one second to five seconds). Auto-brightness Will now take a few
                       seconds to react when changed with remote control.
                     - Adjust / change the ambient light levels for clock display dim intensities.
                     - Since the auto-brightness of the clock display has been fine-tuned, the default configuration when the clock is powered on
                       is now auto-brightness On.
                     - Add UART configuration to allow sending data to a VT101-type monitor connected to Pico's uart1.
                       >>> NOTE: be careful to adjust voltage logic levels before connecting an external monitor.
                           NOTE: see user guide on how to receive and display data from a Pico to a PC screen through USB port.
                     - Improve / optimize the logic of the sound circular buffer and sound related functions for passive buzzer.
                     - Add support for BME280 sensor (temperature, humidity and barometric pressure). The BME280 uses the same I2C channel as the
                       DS3231 real-time clock.
                     - Add calculation of approximate altitude based on barometric pressure. After a few tries, it has been commented out since it
                       changes with pressure variations and it appears to be not very accurate and / or useful.
                     - If a BME280 has been installed, scroll "device ID" and device "unique ID" on clock power-up.
                     - Rework algorithm of LED matrix automatic brightness. Add hysteresis and also more brightness levels.

   25-AUG-2022  6.00 - Add Pico's flash memory read, write and erase functions.
                     - Add CRC16 function to calculate the cyclic redundancy checksum of an array of characters (using the 0x1021 polynom).
                     - Add algorithms to save Green Clock configuration to flash memory and restore it when required.
                     - Improve the "degree" symbol for Celsius and Fahrenheit. Thanks to Eric Escolano for proposing this improvement.
                     - Fix a few "compile bugs" when some options / features are turned Off (remote control, DHT22, BME280, etc...).
                       Thanks to "pjbroad" and "maxromanovsky" on GitHub for bringing this to my attention.
                     - Add a "QUICK_START" compile option to easily allow skipping parts of the power-up sequence if so desired.
                     - Change debug output from UART1 to UART0 for easier transfer to a PC through USB CDC interface.
                     - Add a system idle monitor to get an idea of the load on the microcontroller.
                     - Add a sound queue for the active buzzer (the one integrated with the Pico Green Clock).
                     - Adapt the sound callback function to support both the passive buzzer and the active buzzer.
                     - Implement 9 alarms since we can now save them to flash. Alarm algorithm has been completely reworked
                     - Optimize sound algorithm throughout the code by using the sound active queue.
                     - Tried to improve DHT22 algorithm by replacing the "polling" algorithm by an interrupt-driven function... without success. I'm
                       still getting more or less 75% read errors. Maybe the Pico is too busy with the callback functions to be able to properly
                       handle the DHT22. When I have some extra time, I may take a look with a logic analyzer to see what happens. Whatever the case,
                       I have 100% communications success (no error at all) with the BME280 device that I use (see User guide for more info).

   31-OCT-2022  7.00 - Given all timing problems while trying to read datastream from DHT22, offload that function to Pico's second core (core 1).
                     - Implement two queues for inter-core communications since the SDK documentation recommends not using the fifos for this purpose if at all possible.
                     - Implement a new generic Daylight Saving Time algorithm covering most countries of the world.
                     - Add Universal Time Coordinate to the clock setup and to the flash configuration. It will be required when implementing time update / synchronization
                       via NTP and it also allows to properly handle Daylight Saving Time / Summer Time for those countries who change the time based on UTC.
                     - Add current DST status to flash memory to optimize DST algorithm.
                     - Change algorithm to find DayOfWeek that was sometimes returning an out-of-sync value.

   13-JAN-2023  8.00 - Add "snowflakes" pixel animation (button "Play/Pause" on remote control).
                     - Optimize top indicators update function.
                     - Change clock display brightness configuration and algorithm so that it is now driven by a Pico's PWM signal.
                     - Add features and optimize sound queues handling.
                     - Add a command queue to prevent overrun while adding more and more processing in callback functions.
                     - Prevent a quick press on "set" (top) button to enter setup mode if data is currently scrolling on clock display.
                     - Rework / improve / optimize Daylight Saving Time / Summer Time algorithm.
                     - Make some cleanup and optimization on the analog-to-digital related functions.
                     - Debug hourly chime sometimes sounding at xxh01.
                     - Add automatic detection of CDC USB while in "DEVELOPER_VERSION".
                     - Add automatic detection of microcontroller (Pico or Pico W).
                     - Add a half-hour "light chime" (2 quick beeps).
                     - Add an option where hourly chime sounds a number of beeps being equal to current hour (2 o'clock = 2 beeps / 5 o'clock = 5 beeps, etc...)
                     - Cleanup and optimizations in PWM handling and integration of PWM for passive buzzer and clock display brightness.
                     - Modify checking of clock's flash configuration to prevent a glitch on clock display while disabling interrupts.
                     - Cleanup code for easier configuration and handling of temperature reading from either Pico, DS3231, DHT22 and / or BME280.
                     - Some general code cleanup and optimization.

   19-JAN-2023  8.01 - Fix a bug with blinking days-of-week indicators in Alarm Setup.
                     - Add get_pico_unique_id() function and use it only once before flash operations to prevent potential problems.

   21-JAN-2023  8.02 - Fix a bug with the logic behind "days-of-week". Two different logics were used throughout the code.

   26-JAN-2023  9.00 - Change count-down timer alarm algorithm. Will now ring until reset by user or when reaching maximum ringing duration.
                     - Implement NTP (Network Time Protocol). Green Clock will now resync its time periodically from Internet. (Requires Pico W).
                     - Improve clock precision independently of real-time IC by better synchronization of callback period time.
                     - Add a function to "set-and-save" Wi-Fi credentials to flash.

   01-MAY-2023  9.01 - Restricted availability version to test German translation.

   06-APR-2023  9.02 - Change "middle dots blinking" algorithm. Thanks to Frank Seidel for his suggestion and work on this feature.
                     - Add basic German language support. Thanks to Frank Seidel for his work and translation on this feature !
                       (Note: English is used if / when German translation is not available).
                     - Add functions to easily support and convert tm_time, human_time, unix_time.
                     - Fix a problem when initializing CYW43 with new SDK library.
                     - Increase string length for Calendar Events up to 50 characters.
                     - Begin implementation of "Reminders", giving even more flexibility than Calendar Events (to be completed).
                     - Fix dates encoded in CalendarEventsGeneric.cpp (example Calendar Events).
                     - Add Czech language support. Thanks to KaeroDot for the excellent work and translation on this feature !

   29-JAN-2024 10.00 - Add hostname to the NTP and networking initialisation. Add to flash structure too.
   19-FEB-2024 10.00 - Add web page to control the clock settings. This can change many things. Accessed as http://hostname or IP/index.shtml
                     - Reduce the display minimum dim level by altering the display PWM frequency.
                     - Add a step dimmer on the lower button to switch between auto dimming and 5 levels of manual modes.
                     - Add a web control to set a local level for the minimum light level for maximum dimming. This can vary between clocks. Value stored in flash.
                     - Add a web control to swap the operation of the set key from short press to set clock to short press to set the alarms.
                     - Fix triggering of afternoon alarms when in 12hr display mode.
                     - Release on GitHub.

   25-FEB-2024 10.01 - Correct the flash_config structure in the Pico-W NTP Client code. Now matches the one in the main code.
   06-MAR-2024       - Add more bytes to the alarm structure. This may cause the 10.01 first install to wipe out all flash alarm and WiFi data.
                     - Correct the light ADC initialisation after a firmware 10.00 release voltage reading fix caused more problems than it solves.
                     - Add in a new feature to temporarily disable all alarms for a 4 hour window by pressing any two buttons together. This doesn't affect any settings.
                     - Add in the ability for all of the alarms to drive jingles, a different set of beeps or use the onboard buzzer if the passive Piezo one is fitted.
                       These are controlled on the web page. The default beeps mode has alarm 1 as 1, 2 as 2, 3 as 3, etc. The cascading of multiple alarms has been removed.
                     - Add in support for local reminder, event and WiFi config files that can be pulled in when built without appearing in the git sources.
                     - Tweak daylight savings region web control drop down so that it's now populated correctly rather than having a print out of the region and drop down to change.
                     - Add periodic print out of the Pico-W WiFi connection status and IP address to the UART or USB serial console (will work with putty, etc.). Serial baud rate is 921600.

\* ================================================================== */

/* ================================================================== *\
      NOTE: Your comments - if they are constructive - are welcome:
                         astlouys@gmail.com

      Let me know which version of the firmware you are using, tell
      me if you found bugs, what are the improvements you added /
      integrated and / or what changes you would like to be done
      to the firmware (however, I do not commit to make changes).
\* ================================================================== */

/* ================================================================== *\
    Other features / options that could be added / implemented:
   ==================================================================
    - Make more compile-time options available at run-time by making
      the list of clock setup steps longer. Some options currently
      "compile-time" only could be added to the list of "run-time" options
      (for example: sound On/Off, scroll period, etc...).
    - Use a PIO to support IR burst decoding and / or DHT22 data decoding.
    - If a new version / update of the hardware is ever made, here are a
      few suggestions for improvements:
         > Make the plastic case deeper so that it is possible to add a
           piggy-back module on top of the Pico (for example: ZigBee).
         > Passive piezo to allow changing the frequency of the sound.
         > Add a clear zone in the plastic case (similar to where is
           located the photoresistor used for ambient light reading)
           so that an IR sensor can be added internally (I had to put
           my IR sensor externally).
         > Remote control with buttons specifically configured for the
           Green Clock.
         > Replace the two unused white LEDs near the clock buttons by
           infrared LEDs (installed through "holes" in the plastic case),
           so that they can be used by software to emulate a "remote
           control" to turn On or Off a radio, television, or other
           equipment at a specific time (and / or date). Infrared protocol
           analysis would need to be done "on the side", so that the
           protocol can be reproduced on the Pico-Green-Clock.
           NOTE: These two white LEDs are now used as "night light" on
                 the clock.

    - NOTE: There is a new product from Waveshare that already provides
            some of the improvements above: "Pico-RGB-Matrix-P3-64X32".
            I will give it a try. So far, it seems to have more features
            than the Green Clock:
            - One external socket to put a Pico-compatible module.
            - One IR sensor to receive commands from a remote (a small
              remote control comes with it).
            - Display is 64 X 32 pixels which allows much more data to
              be displayed. Each 5X7 digit is physically smaller. However,
              LEDs are crisp and bright. Not sure what the end result would
              be as for the distance from which we can read it, compare to
              the Green Clock.
    You may want to check on my GitHub repository for an eventual
    release of new firmware for the Pico-RGB-Matrix...
\* ================================================================== */



/* $TITLE=Clock configuration or options. */
/* $PAGE */
/* ================================================================== *\
              ===== CLOCK CONFIGURATION OR OPTIONS =====
     SOME OF THESE ITEMS ARE ADJUSTABLE AT RUNTIME, OTHERS ARE NOT.
     NOTE: Structure "CalendarEvent" should also be initialized
             according to user needs. It is in the file:
                     "CalendarEventsGeneric.cpp".
\* ================================================================== */
/* Firmware version. */
#define FIRMWARE_VERSION "10.01"  ///

/* Select the language for data display. */
#define DEFAULT_LANGUAGE ENGLISH // choices for now are FRENCH, ENGLISH, GERMAN, and SPANISH.

/* While in development mode, we may want to disable NTP update, for example while testing Summer Time handling algorithm. */
// #define NTP_ENABLE  /// WARNING: This #define is not supported for now. Use "#define PICO_W" below instead to enable NTP for now.
#define PICO_HOSTNAME    "PicoW"          /// for those with a development environment, you can enter your hostname, SSID
#define NETWORK_NAME     "MyNetworkName"  /// and password below, run the Firmware until the first date scrolling (credentials
#define NETWORK_PASSWORD "MyPassword"     /// will be saved to flash), then erase the credentials and put comment on both lines.

/* If a Pico W is used, librairies for Wi-Fi and NTP synchronization will be merged in the executable. If PICO_W is not defined, NTP is automatically disabled. */
#define PICO_W  ///

/* Flag to handle automatically the daylight saving time. List of countries are given in the User Guide. */
#define DST_COUNTRY DST_NORTH_AMERICA

/* Release or Developer Version: Make selective choices or options. */
#define RELEASE_VERSION  ///

/* Load custom calendar and reminder files for Andre*/
// This is used when creating developer and debug builds
// #define DEVELOPER_IS_ANDRE

/* ---------------------------------------------------------------------------------- *\
                          Options that can be installed by user.
\* ---------------------------------------------------------------------------------- */

/* Support for an optional passive buzzer. This buzzer must be provided by user and is not included with the Green Clock.
   It allows variable frequency sounds on the clock.
   If you did install one, cut this block and paste is outside of the "#ifdef DEVELOPER_VERSION" and "#endif" to enable the "#define PASSIVE_PIEZO_SUPPORT". */
#define PASSIVE_PIEZO_SUPPORT  /// if a passive buzzer has been installed by user.
#ifdef PASSIVE_PIEZO_SUPPORT
#warning Built with PASSIVE_PIEZO support
#endif  // PASSIVE_PIEZO_SUPPORT


/* Support of an optional (external) temperature and humidity sensor (DHT22) or temperature, humidity and atmospheric pressure sensor (BME280)
   to read and display those parameters. The sensors must be bought and installed by user. They are not included with the Pico Green Clock.
   If you did install one, cut this block and paste it outside of the "#ifdef DEVELOPER_VERSION" and "#endif" to enable the "#define DHT_SUPPORT and / or
   "#define BME280_SUPPORT". */
// #define DHT_SUPPORT  /// if a DHT22 temperature and humidity sensor has been installed by user.
#ifdef DHT_SUPPORT
#warning Built with DHT22 support
#endif  // DHT_SUPPORT

// #define BME280_SUPPORT  /// if a BME280 temperature, humidity and barometric pressure sensor has been installed by user.
#ifdef BME280_SUPPORT
#warning Built with BME280 support
#endif  // BME280_SUPPORT


/* Support of an optional remote control to interact with the clock remotely, for example when the clock is installed
   too high and is out of reach. There is no remote control provided with the clock. It must be bought by the user.
   Current support is for a Memorex remote control, model MCR 5221 that I had readily available. If another brand of
   remote control is to be used, user will have to decode its protocol and implement it in the Green Clock firmware.
   (a file similar to "memorex.cpp" must be created to support the new remote control).
   If you did install an infrared sensor, cut this block and paste it outside of the "#ifdef DEVELOPER_VERSION" and "#endif" to enable the "#define IR_SUPPORT".
   You also need to replace the default "REMOTE_FILENAME" by the filename you created containing the infrared timing / codes corresponding to your remote control.
   You may want to check the Pico-Remote-Analyzer utility in one of my repositories. */
// #define IR_SUPPORT // if an infrared sensor (VS1838B-type) has been installed by the user and IR protocol of the remote control has been analyzed and implemented.

/* Specify the file containing the remote control protocol to be used. */
#define REMOTE_FILENAME "memorex.cpp"

/* Silence period request unit (in minutes). Needs remote control. */
#define SILENCE_PERIOD_UNIT 30
#ifdef IR_SUPPORT
#warning Built with INFRARED support
#endif  // IR_SUPPORT

/* ----------------------------------------------------------------------------------------------------------------------- *\
                                         End of Options that can be installed by user.
\* ----------------------------------------------------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------------------------------------------------- *\
                                             Setup specific to RELEASE version.
\* ----------------------------------------------------------------------------------------------------------------------- */
#ifdef RELEASE_VERSION
#warning Built as RELEASE_VERSION
#endif  // RELEASE_VERSION
/* ----------------------------------------------------------------------------------------------------------------------- *\
                                           End of setup specific to RELEASE version.
\* ----------------------------------------------------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------------------------------------------------- *\
                                               Setup specific to DEVELOPER version.
   NOTE: Developer version requires a USB CDC communication to start. Refer to User Guide for details.
\* ----------------------------------------------------------------------------------------------------------------------- */
#ifndef RELEASE_VERSION
#define DEVELOPER_VERSION
#warning Built as DEVELOPER_VERSION

/* Conditional compile to allow a quicker power-up sequence by-passing some device tests. */
#define QUICK_START  ///
#ifdef QUICK_START
#warning Built with QUICK_START
#endif  // QUICK_START

/* If SOUND_DISABLED is commented out below, it allows turning Off <<< ABSOLUTELY ALL SOUNDS >>> from the Green Clock.
   (For example, during development phase, if your wife is sleeping while you work late in the night as was my case - smile). */
// #define SOUND_DISABLED  ///
#ifdef SOUND_DISABLED
#warning Built with SOUND DISABLED
#endif  // SOUND_DISABLED

/* Flag to include test code in the executable (conditional compile). Tests can be run before falling in normal clock ("time display") mode.
   Keeping the comment sign on the #define below will exclude test code and make the executable smaller. */
#define TEST_CODE

/* Loop at the beginning of the code until a USB CDC connection has been established. Quick beeps will be heard during waiting so that user
   is aware of what's going on. */
// #define USB_CONNECTION  ///
#ifdef USB_CONNECTION
#warning Built with USB_CONNECTION
#endif  // USB_CONNECTION

#endif  // DEVELOPER_VERSION
/* ----------------------------------------------------------------------------------------------------------------------- *\
                                            End of setup specific to DEVELOPER version
\* ----------------------------------------------------------------------------------------------------------------------- */



/* Determine if date scrolling will be enable by default when the clock starts. */
#define SCROLL_DEFAULT FLAG_ON  // choices are FLAG_ON / FLAG_OFF

/* Scroll one dot to the left every such milliseconds (80 is a good start point. lower = faster). */
#define SCROLL_DOT_TIME 66      // this is a UINT8 (must be between 0 and 255).

/* Date, temperature and other options will scroll at this frequency
   (in minutes - we must leave enough time for the previous scroll to complete). */
#define SCROLL_PERIOD_MINUTE 5

/* Default temperature unit to display on the clock. */
#define TEMPERATURE_DEFAULT CELSIUS  // choices are CELSIUS and FAHRENHEIT.

/* Time will be displayed in 24-hours format by default. */
#define TIME_DISPLAY_DEFAULT H24     // choices are H12 and H24.

/* Exit setup mode after this period of inactivity (in seconds). (for "clock setup", "alarm setup" or "timer setup"). */
#define TIME_OUT_PERIOD 20

/* Maximum number of seconds an alarm will ring. It will be automatically shut Off after this period in seconds
   if user has not pressed yet the "Set" (top) button. */
#define MAX_ALARM_RING_TIME 3600 // (in seconds) alarm will be automatically shut Off after ringing for one hour (3600 seconds).

/* Night light default values. "AUTO" is based on ambient light reading to turn night light On or Off. */
#define NIGHT_LIGHT_DEFAULT   NIGHT_LIGHT_AUTO  // choices are NIGHT_LIGHT_OFF / NIGHT_LIGHT_ON / NIGHT_LIGHT_NIGHT / NIGHT_LIGHT_AUTO
#define NIGHT_LIGHT_TIME_ON   21                // if "NIGHT_LIGHT_NIGHT", LEDs will turn On  at this time (in the evening).
#define NIGHT_LIGHT_TIME_OFF   8                // if "NIGHT_LIGHT_NIGHT", LEDs will turn Off at this time (in the morning).

/* Hourly chime default values. */
#define CHIME_DEFAULT  CHIME_DAY         // choices are CHIME_OFF / CHIME_ON / CHIME_DAY
#define CHIME_TIME_ON          9         // if "CHIME_DAY", "hourly chime" and "calendar event" will sound beginning at this time (in the morning).
#define CHIME_TIME_OFF        21         // if "CHIME_DAY", "hourly chime" and "calendar event" will sound for the last time at this time (in the evening).
#define CHIME_HALF_HOUR  FLAG_ON         // if "FLAG_ON", will sound a "double-beep" on half-hour (every xxh30), compliant to chime settings above.
#define CHIME_HOUR_COUNT FLAG_OFF        // if "FLAG_ON", hourly chime will beep a number of times equivalent to the hour value in 12-hour format.
#define CHIME_HOUR_COUNT_BEEP_DURATION 300  // duration of "hour count" beeps (in msec) when flag above is On.
/* NOTE: See also revision history above (or User guide) about "night time workers" support for hourly chime. */

/* Temporary alarm disable window setting */
#define ALARM_DISABLE_WINDOW   4         // Duration of 2 button press alarm disable window in hours. Minute value is sampled. Set for a long but not excessive period

/* ================================================================== *\
            ===== END OF CLOCK CONFIGURATION OR OPTIONS =====
\* ================================================================== */





/* $TITLE=Definitions and include files. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
                    Definitions and include files
                       (in alphabetical order)
\* ------------------------------------------------------------------ */
// Now moved into Pico-Green-Clock.h

/* Include files. */
#include "bitmap.h"
#include "ctype.h"
#include "debug.h"
#include "Ds3231.h"
#include "errno.h"
#include "fcntl.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/watchdog.h"
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "math.h"
#include "pico/multicore.h"
#include "pico/platform.h"
#include "pico/sync.h"
#include "pico/unique_id.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#ifdef PICO_W
#include "picow_ntp_client.h"
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "cgi.h"
#endif  // PICO_W

#include "Pico-Green-Clock.h"

/* Clock setup step definitions. */
/* They are not categorized as #define since there is a difference in the
   order to handle day-of-month setting and month setting between French and
   English and it appears to be the easiest way to handle this difference.
   "HI_LIMIT" remains a "#define" since it is used as an initialization
   parameter for a vector. */
#define SETUP_CLOCK_LO_LIMIT       0x00
UINT8 SETUP_NONE =                 0x00;
UINT8 SETUP_HOUR =                 0x01;
UINT8 SETUP_MINUTE =               0x02;
UINT8 SETUP_MONTH =                0x03;
UINT8 SETUP_DAY_OF_MONTH =         0x04;
UINT8 SETUP_YEAR =                 0x05;
UINT8 SETUP_DST =                  0x06;
UINT8 SETUP_UTC =                  0x07;
UINT8 SETUP_KEYCLICK =             0x08;
UINT8 SETUP_SCROLLING =            0x09;
UINT8 SETUP_TEMP_UNIT =            0x0A;
UINT8 SETUP_LANGUAGE =             0x0B;
UINT8 SETUP_TIME_FORMAT =          0x0C;
UINT8 SETUP_HOURLY_CHIME =         0x0D;
UINT8 SETUP_CHIME_TIME_ON =        0x0E;
UINT8 SETUP_CHIME_TIME_OFF =       0x0F;
UINT8 SETUP_NIGHT_LIGHT =          0x10;
UINT8 SETUP_NIGHT_LIGHT_TIME_ON =  0x11;
UINT8 SETUP_NIGHT_LIGHT_TIME_OFF = 0x12;
UINT8 SETUP_AUTO_BRIGHT =          0x13;
#define SETUP_CLOCK_HI_LIMIT       0x14



/* $TITLE=Calendar events. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
       Calendar events have been implemented to be configurable
     by the user. However, they must be configured at compile time
         (they can't be configured once the clock is running).
       As a user may intuitively guess, the parameters are:
       1) Day of month (1 to 31).
       2) Month (English 3-letters: JAN / FEB / MAR / APR / etc...)
       3) Jingle ID (Jingle number to play while scrolling this calendar event, 0 = none) (require passive buzzer to be installed by user).
       4) Text to scroll on clock display (between "double-quotes" in the code)
          (The text is limited to 40 characters. It takes a relatively long time to read 40 characters of text on the clock scrolling display !)

   NOTE: Calendar events are verified twice an hour, at xxh14 and xxh44. The text configured will be scrolled on
         the clock display if the clock is in its "normal" time display mode (that is, the text will not scroll if user is in a setup operation).
         Also, the tone will sound (according to TONE_EVENT_xxx settings) during the 24 hours define by the date of the calendar event, and in
         compliance with the CHIME_xxx settings. Outside hours define by CHIME_xxx settings, the text will scroll on the display, but there will be no sound.
         NOTE: See also the "nighttime workers" information in User Guide.
         NOTE: No validation is done on the date. If an invalid date is set, the event will never be scrolled.
               Also if, for example, 29-FEB is set, the event will be scrolled only every 4 years.
\* ------------------------------------------------------------------ */

/* Events to scroll on clock display at specific dates. Must be setup by user. Some examples are already defined. */
#if defined __has_include && __has_include ("CalendarEventsAndre.cpp")
#include "CalendarEventsAndre.cpp"
#elif defined __has_include && __has_include ("CalendarEventsCustom.cpp")
#include "CalendarEventsCustom.cpp"
#else
#include "CalendarEventsGeneric.cpp"
#endif

/* Events to scroll on clock display at specific dates. Must be setup by user. Some examples are already defined. */
#if defined __has_include && __has_include ("RemindersAndre.cpp")
#include "RemindersAndre.cpp"
#elif defined __has_include && __has_include ("RemindersCustom.cpp")
#include "RemindersCustom.cpp"
#else
#include "RemindersGeneric.cpp"
#endif

// Define data structures for optional hardware.
#ifdef BME280_SUPPORT
/* BME280 calibration parameters computed from data written in the device. */
struct bme280_data
{
  /* Calibration data for the specific BME280 used (stored in BME280's non-volatile memory). */
  UINT8  Bme280CalibData[42];

  /* Temperature-related parameters. */
  UINT16  DigT1;
  int16_t DigT2;
  int16_t DigT3;

  /* Pressure-related parameters. */
  UINT16  DigP1;
  int16_t DigP2;
  int16_t DigP3;
  int16_t DigP4;
  int16_t DigP5;
  int16_t DigP6;
  int16_t DigP7;
  int16_t DigP8;
  int16_t DigP9;

  /* Humidity-related parameters. */
  UCHAR   DigH1;
  int16_t DigH2;
  UCHAR   DigH3;
  int16_t DigH4;
  int16_t DigH5;
  char    DigH6;

  /* Return values. */
  float TemperatureC;
  float TemperatureF;
  float Humidity;
  float Pressure;
  float Altitude; // not trusted value.
  float Humidex;  // wrong value for now.

  /* Generic parameters. */
  UINT32 Bme280Errors;      // cumulative number of errors while trying to read BME280 sensor.
  UINT32 Bme280ReadCycles;  // cumulative number of read cycles from BME280.
};
#endif  // BME280_SUPPORT


#ifdef DHT_SUPPORT
struct dht_data
{
  UINT64 TimeStamp;      // time stamp of last reading cycle from DHT sensor.
  float  Temperature;    // temperature reading from DHT.
  float  Humidity;       // relative humidity reading from DHT sensor.
  UINT32 DhtErrors;      // cumulative number of errors while receiving DHT packets (for statistic purposes).
  UINT32 DhtReadCycles;  // cumulative number of DHT read cycles.
};
#endif  // DHT_SUPPORT



/* $TITLE=Global variables declaration / definition. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
         variables whenever possible. This could be a way to improve
         the code in future releases...
\* ------------------------------------------------------------------ */
UINT16 AlarmReachedBitMask        = 0;         // assume no alarms are ringing on entry.
UINT8  AlarmNumber                = 0;         // since Version 6.00, there are now 9 alarms.
UINT8  AlarmTargetDay             = MON;       // blinking day-of-week to be selected or unselected for current alarm setting.
volatile UINT16 AverageLightLevel = 550;       // relative ambient light value (for clock display auto-brightness feature). Assume average light level on entry.

UINT16 BottomKeyPressTime         = 0;         // keep track of the time the Down ("Bottom") key is pressed.

UINT8  ChimeTimeOffDisplay        = CHIME_TIME_OFF;  // variable formatted to display in 12-hours or 24-hours format.
UINT8  ChimeTimeOnDisplay         = CHIME_TIME_ON;   // variable formatted to display in 12-hours or 24-hours format.
UINT8  CommandQueueHead;                       // head of Command circular buffer.
UINT8  CommandQueueTail;                       // tail of Command circular buffer.
UINT8  Core0Queue[MAX_CORE_QUEUE];             // circular buffer for inter-core communication.
UINT8  Core0QueueHead;                         // head of circular buffer for inter-core communication.
UINT8  Core0QueueTail;                         // tail of circular buffer for inter-core communication.
UINT8  Core1Queue[MAX_CORE_QUEUE];             // circular buffer for inter-core communication.
UINT8  Core1QueueHead;                         // head of circular buffer for inter-core communication.
UINT8  Core1QueueTail;                         // tail of circular buffer for inter-core communication.
UINT8  CurrentClockMode = MODE_POWER_UP;       // current clock mode.
UINT8  CurrentDayOfMonth;
UINT8  CurrentDayOfWeek;
UINT16 CurrentDayOfYear;
UINT8  CurrentHour;
UINT8  CurrentHourSetting         = 0;         // hour read from and / or written to the RTC IC.
UINT8  CurrentMinute;
UINT8  CurrentMinuteSetting       = 0;         // minute read from and / or written to real time clock IC.
UINT8  CurrentMonth;
volatile UINT8  CurrentSecond     = 0;         // current value of second (second counter before next minute change).
UINT16 CurrentYear;                            // current year (4 digits).
UINT8  CurrentYearLowPart;                     // lowest two digits of the year (battery backed-up).

UINT64 DebugBitMask;                           // bitmask of code sections to be debugged through UART (see definitions of DEBUG sections above).
UINT16 Dim_AverageLightLevel;                  // Average ADC value across the 20 measurement slots
UINT16 Dim_DutyCycle;                          // Current pwm active cycle
UINT16 Dim_Max_avgadc_value = 0;               // Cumulative adc light level max value, initialise with the minimum number
UINT16 Dim_Min_avgadc_value = 65535;           // Cumulative adc light level min value, initialise with the maximum number
UCHAR  DisplayBuffer[DISPLAY_BUFFER_SIZE];     // framebuffer containing the bitmap of the string to be displayed / scrolled on clock display.
UINT16 DotBlinkCount;                          // cumulate milliseconds to blink the two "middle dots" on clock display.

UINT8  FlagAlarmBeeping           = FLAG_OFF;  // flag indicating an alarm is sounding.
UINT8  FlagBlinking[20] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // bitmap to logically "and" with a character for blinking.
UINT8  FlagDaylightSavingTime;                 // used to evaluate current Daylight Saving Time (DST) / Summer Timer status on clock power-up.
UINT8  FlagIdleCheck              = FLAG_OFF;  // if ON, we keep track of idle time to eventually declare a time-out during setting (clock, alarm or timer).
UINT8  FlagIdleMonitor            = FLAG_OFF;  // monitor system "idle time" to see "how busy" (in fact, "how not busy") the system is. Nothing in common with "idle check" above.
volatile UINT8 FlagIsrContext     = FLAG_OFF;  // flag used to determine if we run in ISR context.
UINT8  FlagScrollData             = FLAG_OFF;  // time has come to scroll data on clock display.
UINT8  FlagScrollStart            = FLAG_OFF;  // flag indicating it is time to start scrolling.
UINT8  FlagSetAlarm               = FLAG_OFF;  // flag indicating we are in alarm setup mode.
UINT8  FlagSetTimer               = FLAG_OFF;  // user pressed the "up" (Middle) button.
UINT8  FlagSetTimerCountDown      = FLAG_OFF;  // flag indicating count down timer is active.
UINT8  FlagSetTimerCountUp        = FLAG_OFF;  // flag indicating count up   timer is active.
UINT8  FlagSetClock               = FLAG_OFF;  // user has pressed the "Set" (top) button to enter clock setup mode.
UINT8  FlagSetupAlarm[SETUP_ALARM_HI_LIMIT];
UINT8  FlagSetupClock[SETUP_CLOCK_HI_LIMIT];
UINT8  FlagSetupTimer[SETUP_TIMER_HI_LIMIT];
UINT8  FlagSetupRTC               = FLAG_OFF;  // flag indicating time must be programmed in the real-time clock IC.
UINT8  FlagTimerCountDownEnd      = FLAG_OFF;  // flag indicating the count down timer reached final count (0m00s).
UINT8  FlagTone                   = FLAG_OFF;  // flag indicating there is a tone sounding.
UINT8  FlagToneOn                 = FLAG_OFF;  // flag indicating it is time to make a tone.
UINT8  FlagUpdateTime             = FLAG_OFF;  // flag indicating it is time to refresh the time on clock display.
UINT8  FlagWebRequestNTPSync      = FLAG_OFF;  // flag indicating that the web page control has requested a date & time sync with the NTP server
UINT8  FlagWebRequestFlashCheck   = FLAG_OFF;  // flag indicating that the web page control has updated things that are stored in the flash and may need updating
struct flash_config FlashConfig;               // Hold the flash data storage in a data structure
UINT8 *FlashData;                                                       // pointer to an allocated RAM memory space used for flash operations.
UINT8 *FlashMemoryAddress = (UINT8 *)(XIP_BASE + FLASH_CONFIG_OFFSET);  // pointer to flash memory used to store clock configuration (flash base address + offset).
UINT8 *FlashMemoryOffset  = (UINT8 *)FLASH_CONFIG_OFFSET;               // offset from Pico's beginning of flash where data will be stored.
UINT16 FlashingCount = 0;                                               // cumulate number of milliseconds before blinking clock display.

UCHAR  GetAddHigh = 0x11;
UCHAR  GetAddLow  = 0x12;
UINT64 GlobalUnixTime;        // system-wide current time based on UTC Unix Time.

UINT16 IdleArchivePacket;     // packet number being archived for debugging purposes.
UINT64 IdleMonitor[14];       // evaluate average number of loops performed per second.
UINT8  IdleMonitorPacket;     // idle monitor packet for current 5-seconds period.
UINT8  LastIdleMonitorPacket; // idle monitor packet number processed in the last pass.
UINT8  IdleNumberOfSeconds;   // keep track of the number of seconds the system has been idle.
UINT16 IdleTimeSamples;
UINT64 IrFinalValue[MAX_IR_READINGS];    // final timer value when receiving edge change from remote control.
UINT64 IrInitialValue[MAX_IR_READINGS];  // initial timer value when receiving edge change from remote control.
UCHAR  IrLevel[MAX_IR_READINGS];         // logic levels of remote control signal: 'L' (low), 'H' (high), or 'X' (undefined).
UINT32 IrResultValue[MAX_IR_READINGS];   // duration of this logic level (Low or High) in the signal received from remote control.
UINT16 IrStepCount;                      // number of "logic level changes" received from IR remote control in current stream.

UINT16 MiddleKeyPressTime = 0;                                                // keep track of the time the Up ("Middle") key is pressed.
UINT8  MonthDays[2][12] = {{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},  // number of days in a month - "leap" year.
                           {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}}; // number of days in a month - "normal" year.

UINT8  NightLightTimeOffDisplay;    // to adapt to 12 or 24-hours time format.
UINT8  NightLightTimeOnDisplay;     // to adapt to 12 or 24-hours time format.
struct ntp_data NTPData;            // Hold the NTP returned data in a data structure

UINT8  PicoType;                    // microcontroller type (Pico or Pico W).
UCHAR  PicoUniqueId[40];            // Pico Unique ID read from flash IC.

#ifdef RELEASE_VERSION
/* Value assigned to CurrentSecond when changing the minutes in clock setup. */
UINT8  ResetSecond = 0;
#else  // RELEASE_VERSION
UINT8  ResetSecond = 50;
#endif // RELEASE_VERSION
UINT8  RowScanNumber;

UINT8  ScrollDotCount = 0;               // keep track of "how many dots" remain to be scrolled to the left on clock display.
UINT8  ScrollQueue[MAX_SCROLL_QUEUE];    // circular buffer containing the tag of the next messages to be scrolled.
UINT8  ScrollQueueHead;                  // head of Scroll circular buffer.
UINT8  ScrollQueueTail;                  // tail of Scroll circular buffer.
UINT8  ScrollSecondCounter = 0;          // keep track of number of seconds to reach time-to-scroll.
UINT8  ScrollStartCount = 0;             // count the number of milliseconds before scrolling one more dot position to the left.
UINT8  SetupSource = SETUP_SOURCE_NONE;  // indicate the source of current setup activities (alarm, clock or timer).
UINT8  SetupStep = 0;                    // indicate the setup step we are through the clock setup, alarm setup, or timer setup.
UINT16 SilencePeriod = 0;                // temporarily turn off most sounds from the clock.
volatile UINT16 SoundActiveHead;         // head of sound circular buzzer for active buzzer.
volatile UINT16 SoundActiveTail;         // tail of sound circular buffer for active buzzer.
volatile UINT16 SoundPassiveHead;        // head of sound circular buffer for passive buzzer.
volatile UINT16 SoundPassiveTail;        // tail of sound circular buffer for passive buzzer.

UINT8  Temp_Disable_Alarm         = FLAG_OFF;  // Control of a temporary alarm disable for 8 hours, flag if active or not
UINT8  Temp_Disable_Hour          = 0;         // Countdown of 5 hours
UINT8  Temp_Disable_Min           = 0;         // Minute value when the alarm disable was activated
UINT8  TimerMinutes               = 0;
UINT8  TimerMode                  = TIMER_OFF; // timer mode (0 = Off / 1 = Count down / 2 = Count up).
UINT8  TimerSeconds               = 0;
UINT8  TimerShowCount             = 0;         // in case we want to refresh timer display less often than every second.
UINT8  TimerShowSecond            = 0;
UINT8  ToneMSecCounter            = 0;         // cumulate number of milliseconds for tone duration.
UINT8  ToneRepeatCount            = 0;         // number of "tones" to sound for different events.
UINT8  ToneType;                               // determine the type of "tone" we are sounding.
UINT16 TopKeyPressTime            = 0;         // keep track of the time the Set ("Top") key is pressed.
UINT8  Two_Keys_Pressed           = FLAG_OFF;  // Flag to saythat two keys have been pressed, so don't activate the one released last

UINT8 UpId                        = 0;

TIME_RTC Time_RTC;

semaphore_t SemSync; // semaphore used to synchronize minutes and seconds display with double dots blinking.

uart_inst_t *Uart;   // Pico's UART used to serially transfer debug data to a VT100-type monitor or to a PC.

UCHAR MonthName[LANGUAGE_HI_LIMIT][13][13] =
{
  {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}},
  {{}, {"January"},   {"February"},  {"March"},     {"April"},     {"May"},       {"June"},      {"July"},      {"August"},    {"September"}, {"October"},   {"November"},  {"December"}},
  {{}, {"Janvier"},   {"Fevrier"},   {"Mars"},      {"Avril"},     {"Mai"},       {"Juin"},      {"Juillet"},   {"Aout"},      {"Septembre"}, {"Octobre"},   {"Novembre"},  {"Decembre"}},
  {{}, {"Januar"},    {"Februar"},   {"Maerz"},     {"April"},     {"Mai"},       {"Juni"},      {"Juli"},      {"August"},    {"September"}, {"Oktober"},   {"November"},  {"Dezember"}},
  {{}, {"leden"},     {"unor"},      {"brezen"},    {"duben"},     {"kveten"},    {"cerven"},    {"cervenec"},  {"srpen"},     {"zari"},      {"rijen"},     {"listopad"},  {"prosinec"}},
  {{}, {"enero"},     {"febrero"},   {"marzo"},     {"abril"},     {"mayo"},      {"junio"},     {"julio"},     {"agosto"},    {"septiembre"},{"octubre"},   {"noviembre"}, {"diciembre"}}
};


UCHAR ShortMonth[LANGUAGE_HI_LIMIT][13][4] =
{
  {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}},
  {{}, {"JAN"},  {"FEB"},  {"MAR"},  {"APR"},  {"MAY"},  {"JUN"},  {"JUL"},  {"AUG"},  {"SEP"},  {"OCT"},  {"NOV"},  {"DEC"}},
  {{}, {"JAN"},  {"FEV"},  {"MAR"},  {"AVR"},  {"MAI"},  {"JUN"},  {"JUL"},  {"AOU"},  {"SEP"},  {"OCT"},  {"NOV"},  {"DEC"}},
  {{}, {"JAN"},  {"FEB"},  {"MAR"},  {"APR"},  {"MAI"},  {"JUN"},  {"JUL"},  {"AUG"},  {"SEP"},  {"OKT"},  {"NOV"},  {"DEZ"}},
  {{}, {"led."}, {"uno."}, {"bre."}, {"dub."}, {"kve."}, {"cvn."}, {"cvc."}, {"srp."}, {"zar."}, {"rij."}, {"lis."}, {"pro."}},
  {{}, {"ene."}, {"feb."}, {"mar."}, {"abr."}, {"may."}, {"jun."}, {"jul."}, {"ago."}, {"sep."}, {"oct."}, {"nov."}, {"dic."}}
};

UCHAR DayName[LANGUAGE_HI_LIMIT][8][13] =
{
  {{}, {}, {}, {}, {}, {}, {}, {}},
  {{}, {"Sunday"},     {"Monday"},     {"Tuesday"},    {"Wednesday"}, {"Thursday"},   {"Friday"},     {"Saturday"}},
  {{}, {"Dimanche"},   {"Lundi"},      {"Mardi"},      {"Mercredi"},  {"Jeudi"},      {"Vendredi"},   {"Samedi"}},
  {{}, {"Sonntag"},    {"Montag"},     {"Dienstag"},   {"Mittwoch"},  {"Donnerstag"}, {"Freitag"},    {"Samstag"}},
  {{}, {"nedele"},     {"pondeli"},    {"utery"},      {"streda"},    {"ctvrtek"},    {"patek"},      {"sobota"}},
  {{}, {"domingo"},    {"lunes"},      {"martes"},     {"miercoles"}, {"jueves"},     {"viernes"},    {"sabado"}}
};

UCHAR ShortDay[LANGUAGE_HI_LIMIT][8][10] =
{
  {{}, {}, {}, {}, {}, {}, {}, {}},
  {{}, {"SUN"}, {"MON"}, {"TUE"}, {"WED"}, {"THU"}, {"FRI"}, {"SAT"}},
  {{}, {"DIM"}, {"LUN"}, {"MAR"}, {"MER"}, {"JEU"}, {"VEN"}, {"SAM"}},
  {{}, {"SON"}, {"MON"}, {"DIE"}, {"MIT"}, {"DON"}, {"FRE"}, {"SAM"}},
  {{}, {"ne"},  {"po"},  {"ut"},  {"st"},  {"ct"},  {"pa"},  {"so"}},
  {{}, {"do."}, {"lu."}, {"ma."}, {"mi."}, {"ju."}, {"vi."}, {"sa."}}
};


struct dst_parameters DstParameters[25] =
{
  { 0,  0,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0, 60},  //  0 - Dummy
  {10,  1,  1,  7,  2,  0,  4,  1,  1,  7,  3,  0, 60},  //  1 - Australia
  {10,  1,  1,  7,  2,  0,  4,  1,  1,  7,  2,  0, 30},  //  2 - Australia-Howe
  { 9,  7,  1,  7, 24,  0,  4,  7,  1,  7, 24,  0, 60},  //  3 - Chile           (StartHour and EndHour changes at 24h00, will change at 00h00 the day after)
  { 3,  1,  8, 14,  0,  0, 11,  1,  1,  7,  1,  0, 60},  //  4 - Cuba
  { 3,  1, 25, 31,  1,  0, 10,  1, 25, 31,  1,  0, 60},  //  5 - European Union  (StartHour and EndHour are based on UTC time)
  { 3,  6, 23, 29,  2,  0, 10,  1, 25, 31,  2,  0, 60},  //  6 - Israel
  { 3,  1, 25, 31,  0,  0, 10,  1, 25, 31,  0,  0, 60},  //  7 - Lebanon
  { 3,  1, 25, 31,  2,  0, 10,  1, 25, 31,  3,  0, 60},  //  8 - Moldova
  { 9,  1, 24, 30,  2,  0,  4,  1,  1,  7,  2,  0, 60},  //  9 - New-Zealand     (StartHour and EndHour are based on UTC time)
  { 3,  1,  8, 14,  2,  0, 11,  1,  1,  7,  2,  0, 60},  // 10 - North America
  { 3,  7, 24, 30,  2,  0, 10,  7, 24, 30,  2,  0, 60},  // 11 - Palestine
  {10,  1,  1,  7,  0,  0,  3,  1, 22, 28,  0,  0, 60},  // 12 - Paraguay
 };
// StartMonth, StartDayOfWeek, StartDayOfMonthLow, StartDayOfMonthHigh, StartHour, StartDayOfYear,
// EndMonth,   EndDayOfWeek,   EndDayOfMonthLow,   EndDayOfMonthHigh,   EndHour,   EndDayOfYear,
// ShiftMinutes
// NOTE: StartDayOfYear and EndDayOfYear will assigned to local DST parameters in "update_dst_ststus()" function.



struct pixel Pixel[7][22]=
{
  {{1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {9, 0},   {9, 1},  {9, 2},  {9, 3},  {9, 4},  {9, 5},  {9, 6},  {9, 7}, {17, 0}, {17, 1}, {17, 2}, {17, 3}, {17, 4}, {17, 5}, {17, 6}, {17, 7}},
  {{2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7}, {10, 0}, {10, 1}, {10, 2}, {10, 3}, {10, 4}, {10, 5}, {10, 6}, {10, 7}, {18, 0}, {18, 1}, {18, 2}, {18, 3}, {18, 4}, {18, 5}, {18, 6}, {18, 7}},
  {{3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {11, 0}, {11, 1}, {11, 2}, {11, 3}, {11, 4}, {11, 5}, {11, 6}, {11, 7}, {19, 0}, {19, 1}, {19, 2}, {19, 3}, {19, 4}, {19, 5}, {19, 6}, {19, 7}},
  {{4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, {4, 7}, {12, 0}, {12, 1}, {12, 2}, {12, 3}, {12, 4}, {12, 5}, {12, 6}, {12, 7}, {20, 0}, {20, 1}, {20, 2}, {20, 3}, {20, 4}, {20, 5}, {20, 6}, {20, 7}},
  {{5, 2}, {5, 3}, {5, 4}, {5, 5}, {5, 6}, {5, 7}, {13, 0}, {13, 1}, {13, 2}, {13, 3}, {13, 4}, {13, 5}, {13, 6}, {13, 7}, {21, 0}, {21, 1}, {21, 2}, {21, 3}, {21, 4}, {21, 5}, {21, 6}, {21, 7}},
  {{6, 2}, {6, 3}, {6, 4}, {6, 5}, {6, 6}, {6, 7}, {14, 0}, {14, 1}, {14, 2}, {14, 3}, {14, 4}, {14, 5}, {14, 6}, {14, 7}, {22, 0}, {22, 1}, {22, 2}, {22, 3}, {22, 4}, {22, 5}, {22, 6}, {22, 7}},
  {{7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 7}, {15, 0}, {15, 1}, {15, 2}, {15, 3}, {15, 4}, {15, 5}, {15, 6}, {15, 7}, {23, 0}, {23, 1}, {23, 2}, {23, 3}, {23, 4}, {23, 5}, {23, 6}, {23, 7}}
};



struct command         CommandQueue[MAX_COMMAND_QUEUE];
struct pwm             Pwm[2];
struct repeating_timer Timer50MSec;  // sound callback.
struct repeating_timer TimerMSec;    // clock buttons handling callback
struct repeating_timer TimerSec;     // time keeping and overall supervision callback
struct sound_active    SoundQueueActive[MAX_ACTIVE_SOUND_QUEUE];
struct sound_passive   SoundQueuePassive[MAX_PASSIVE_SOUND_QUEUE];



#ifdef BME280_SUPPORT
struct bme280_data Bme280Data;
#endif  // BME280_SUPPORT



#ifdef DHT_SUPPORT
struct dht_data DhtData;
#endif  // DHT_SUPPORT



/* Uncomment and modify value below until you get (or you remove) the error message to get an idea of approximate remaining RAM available. */
/* NOTE: Keep in mind that the "malloc()" is not calculated when doing so and must be added to the total RAM required by the firmware. */
// UINT8 RemainingRam[231050];  /// this value was near RAM limit, as of Version 8.00



/* $TITLE=Function prototypes. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
                         Function prototypes
  Provide prototype declaration for functions that use local variables
  with structures for inputs - pass by reference
\* ------------------------------------------------------------------ */

/* Convert "HumanTime" to "TmTime". */
void convert_human_to_tm(struct human_time *HumanTime, struct tm *TmTime);

/* Convert "TmTime" to "HumanTime". */
void convert_tm_to_human(struct tm *TmTime, struct human_time *HumanTime);

/* Display current PWM values for specified PWM. */
void display_pwm(struct pwm *Pwm, UCHAR *TitleString);

/* Read DS3231 real-time clock IC and return current time in "human_time" format. */
void get_current_time(struct human_time *HumanTime);


/* --------------------------------------------------------------------- *\
    Area of Green Clock source code containing many chunks of test code
              that have been used during firmware development.
        Keep in mind that this code has not been updated during code
       evolution and is provided only to help as a basis to help user.
\* --------------------------------------------------------------------- */
#ifdef TEST_CODE
/* Perform different tests on Pico Green Clock (to be used for testing and / or debugging). */
void test_zone(UINT8 TestNumber);
#endif  // TEST_CODE



/* ------------------------------------------------------------------ *\
       Extra code to test different combinations of parameters to
                evaluate correct handling of DST status.
\* ------------------------------------------------------------------ */
#ifdef DST_DEBUG
void test_dst_status(void);
#endif



/* $PAGE */
/* $TITLE=Main program entry point. */
/* ------------------------------------------------------------------ *\
                       Main program entry point.
\* ------------------------------------------------------------------ */
int main(void)
{
  UCHAR Dum1UChar;
  UCHAR String[256];
  UCHAR TempString1[25];
  UCHAR TempString2[25];
  UCHAR TempString3[25];

  UINT8  DstUnit;

  UINT8  BitNumber;
  UINT8  Dum1UInt8;
  UINT8  FlagUpdate;
  UINT8  IrCommand;        // command received from remote control.
  UINT8  Loop1UInt8;
  UINT8  SundayCounter;    // used to find daylight saving time current status.

  int8_t UtcTime;          // used for DST algorithm.

  UINT16 Dum1UInt16;
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;

  int16_t ReturnCode;

  UINT32 Bme280UniqueId;
  UINT32 CounterHiLimit;

  UINT64 ArchiveIdleMonitor[14][120];  // keep an history of idle monitorfor reference purposes (outside monitor is required).
  UINT64 CurrentTimerValue;
  UINT64 CurrentWatchDogReset;
  UINT64 DataBuffer;
  UINT64 Dum1UInt64;
  UINT64 LastWatchDogReset;

  float Duration;
  float Humidity;       // for BME280 or DHT22.
  float Temperature;    // for BME280 or DHT22.

  time_t TimeStamp;

  struct human_time HumanTime;
  struct tm TmTime;



  /* ---------------------------------------------------------------- *\
                   Localization of days and months.
  \* ---------------------------------------------------------------- */

  /* Add accents to some French weekdays and months. */
  MonthName[FRENCH][FEB][1] = (UCHAR)0x90; // e - acute on fevrier.
  MonthName[FRENCH][AUG][2] = (UCHAR)30;   // u - circumflex on aout.
  MonthName[FRENCH][DEC][1] = (UCHAR)0x90; // e - acute on decembre.

  ShortMonth[FRENCH][FEB][1] = (UCHAR)0x90; // e - acute on FEV.
  ShortMonth[FRENCH][AUG][2] = (UCHAR)30;   // u - circumflex on AOU.
  ShortMonth[FRENCH][DEC][1] = (UCHAR)0x90; // e - acute on DEC.


  /* Add accents to some Czech weekdays. */
  DayName[CZECH][MON][4] = (UCHAR)130; // e-caron
  DayName[CZECH][MON][6] = (UCHAR)131; // i-acute
  DayName[CZECH][TUE][0] = (UCHAR)133; // u-acute
  DayName[CZECH][TUE][4] = (UCHAR)132; // y-acute
  DayName[CZECH][WED][2] = (UCHAR)135; // r-caron
  DayName[CZECH][THU][0] = (UCHAR)136; // c-caron
  DayName[CZECH][FRI][1] = (UCHAR)129; // a-acute
  DayName[CZECH][SUN][3] = (UCHAR)130; // e-caron

  ShortDay[CZECH][TUE][0] = (UCHAR)133; // u-acute
  ShortDay[CZECH][THU][0] = (UCHAR)136; // c-caron
  ShortDay[CZECH][FRI][1] = (UCHAR)129; // a-acute
  ShortDay[CZECH][SUN][3] = (UCHAR)130; // e-caron

  /* Add accents to some Czech months. */
  MonthName[CZECH][FEB][0] = (UCHAR)133; // u-acute
  MonthName[CZECH][MAR][1] = (UCHAR)135; // r-caron
  MonthName[CZECH][MAY][2] = (UCHAR)130; // e-caron
  MonthName[CZECH][JUN][0] = (UCHAR)136; // c-caron
  MonthName[CZECH][JUL][0] = (UCHAR)136; // c-caron
  MonthName[CZECH][SEP][1] = (UCHAR)129; // a-acute
  MonthName[CZECH][SEP][2] = (UCHAR)135; // r-caron
  MonthName[CZECH][OCT][0] = (UCHAR)135; // r-caron
  MonthName[CZECH][OCT][1] = (UCHAR)131; // i-acute

  ShortMonth[CZECH][FEB][0] = (UCHAR)133; // u-acute
  ShortMonth[CZECH][MAR][1] = (UCHAR)135; // r-caron
  ShortMonth[CZECH][MAY][2] = (UCHAR)130; // e-caron
  ShortMonth[CZECH][JUN][0] = (UCHAR)136; // c-caron
  ShortMonth[CZECH][JUL][0] = (UCHAR)136; // c-caron
  ShortMonth[CZECH][SEP][1] = (UCHAR)129; // a-acute
  ShortMonth[CZECH][SEP][2] = (UCHAR)135; // r-caron
  ShortMonth[CZECH][OCT][0] = (UCHAR)135; // r-caron
  ShortMonth[CZECH][OCT][1] = (UCHAR)131; // i-acute

  /* Add accents to some Spanish weekdays. */
  DayName[SPANISH][TUE][2] = (UCHAR)130; // e-acute
  DayName[SPANISH][SAT][1] = (UCHAR)129; // a-acute

  ShortDay[SPANISH][SAT][1] = (UCHAR)129; // a-acute


  /* ---------------------------------------------------------------- *\
            Seed random number generator (for dice rolling).
  \* ---------------------------------------------------------------- */
  srand(time_us_64());



  /* ---------------------------------------------------------------- *\
      Initialize all required GPIO ports of the Raspberry Pi Pico.
  \* ---------------------------------------------------------------- */
  init_gpio();



  /* ---------------------------------------------------------------- *\
                 Retrieve Pico's Unique ID from flash IC.
  \* ---------------------------------------------------------------- */
  get_pico_unique_id();



  #ifdef DHT_SUPPORT
  /* ---------------------------------------------------------------- *\
                    Initialize DHT temperature data.
  \* ---------------------------------------------------------------- */
  DhtData.TimeStamp   = 0;
  DhtData.Temperature = 0;
  DhtData.Humidity    = 0;
  #endif  // DHT_SUPPORT



  /* ---------------------------------------------------------------- *\
                      Initialize system idle monitor.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < 14; ++Loop1UInt8)
    IdleMonitor[Loop1UInt8] = 0;

  /* Reset archive values of system idle monitor to get prepared for storage. */
  IdleTimeSamples = 120;
  for (IdleArchivePacket = 0; IdleArchivePacket < IdleTimeSamples; ++IdleArchivePacket)
  {
    for (Loop1UInt8 = 0; Loop1UInt8 < 14; ++Loop1UInt8)
    {
      ArchiveIdleMonitor[Loop1UInt8][IdleArchivePacket] = 0;
    }
  }
  LastIdleMonitorPacket = 0;
  IdleMonitorPacket     = 0;
  IdleArchivePacket     = 0;



  /* ---------------------------------------------------------------- *\
          Determine the type of microcontroller (Pico or PicoW).
     NOTE: There is a #define PICO_W to conditional compile functions
           and libraries required for NTP support (see below).
  \* ---------------------------------------------------------------- */
  PicoType = get_microcontroller_type();

  #ifdef PICO_W
  /* ---------------------------------------------------------------- *\
      If microcontroller is a Pico W, Green Clock real-time clock IC
     will be synchronized periodically through network time protocol.
        User must have encoded credentials to Pico's flash before.
                   (Refer to User Guide for details).
  \* ---------------------------------------------------------------- */
  /* Initialize NTP-related variables on entry. */
  NTPData.NTPErrors      = 0l;        // reset number of NTP errors on entry.
  NTPData.NTPGetTime     = 0ll;
  NTPData.NTPLastUpdate  = 0ll;
  NTPData.NTPReadCycles  = 0l;        // reset number of NTP read cycles on entry.
  NTPData.NTPDelta       = DstParameters[0].StartHour * DstParameters[0].ShiftMinutes * DELTA_TIME;
  NTPData.FlagNTPResync  = FLAG_ON;   // force NTP re-sync on power-up.
  NTPData.FlagNTPSuccess = FLAG_OFF;  // will be turned On after successful NTP answer.

  /* Initialize the CYW43 architecture (CYW43 driver and lwIP stack). */
  init_cyw43(CYW43_COUNTRY_WORLDWIDE);
  #endif  // PICO_W



  /* ---------------------------------------------------------------- *\
                       Wait for CDC USB connection.
  \* ---------------------------------------------------------------- */
  /* Wait for the developer to connect the PC with terminal emulator software. */
  #ifdef USB_CONNECTION
  printf("Waiting for a USB connection... Press the <Set> button on the Green Clock to start.\r");

  /* Wait for CDC USB connection or for user to press the "SET" button. */
  while (!stdio_usb_connected() && gpio_get(SET_BUTTON))
  {
    tone(5);
    sleep_ms(2500);
    printf("USB connection not established yet (press <Set> button to bypass USB connection).\r");
  }

  /* If a press on the "Set" button has been done, make sure it has been released before continuing. */
  if (gpio_get(SET_BUTTON) == 0)
  {
    while (gpio_get(SET_BUTTON) == 0)
    {
      printf("<Set> button has been pressed... waiting for <SET> button to be released.\r");
      tone(10);
      sleep_ms(50);
      tone(10);
      sleep_ms(2000);
    }
  }
  #endif  // USB_CONNECTION



  /* ---------------------------------------------------------------- *\
              Read UCT real-time clock IC for a first time.
  \* ---------------------------------------------------------------- */
  /* Get a first value from the real-time clock IC for debug time-stamping purposes.
     However, callback function to update time will be initialized about 8 seconds later.
     So, we will re-read the RTC IC later to resync. */
  /***/
  get_current_time(&HumanTime);

  CurrentHour        = HumanTime.Hour;
  CurrentMinute      = HumanTime.Minute;
  CurrentSecond      = HumanTime.Second;
  CurrentDayOfMonth  = HumanTime.DayOfMonth;
  CurrentMonth       = HumanTime.Month;
  CurrentYearLowPart = (HumanTime.Year % 100);
  CurrentYear        = HumanTime.Year;
  CurrentDayOfWeek   = HumanTime.DayOfWeek;
  get_current_time(&HumanTime);

  convert_human_to_tm(&HumanTime, &TmTime);
  GlobalUnixTime = convert_tm_to_unix(&TmTime);



  /* ----------------------------------------------------------------------------------------------------- *\
    WARNINGS: Settings below are for debugging purposes only. Be aware that turning On a debug option may
              have a significant impact on normal clock behaviour, namely on timings !!!
              (Important timings could be out-of-sync and even bring the code to a crash in some situation).
              Use them with care and when you face a problem, investigate first if the debug option is not
              in cause.
    1) DEBUG_CORE: Many operations that will be logged are done on core 1. Since core 0 is still running at
       that time, debug infos may be intertwined in the log file and difficult to read.
    2) DEBUG_FLASH: Some flash operations are processed while in a callback function. Trying to log data
       while running in a callback may bring the system to a crash. This debug flag should be used before
       callback functions are started.

              Turn On bits for debug information we want to display on external monitor.
  \* ----------------------------------------------------------------------------------------------------- */
  #ifdef RELEASE_VERSION
  DebugBitMask = DEBUG_NONE;
  #else  // DEVELOPER_VERSION
  /* NOTE: If DEBUG_FLASH is turned On, it allows logging information about flash memory operations.
           However, logging a flash update during a callback procedure takes a relatively long time and will generate a crash.
           So, it is possible to keep all logging information and use the debug before launching the callback, or it is required to trim down
           the logged information if there is a need to consult the logged information during the callback flash configuration update. */
  DebugBitMask  = DEBUG_NONE;
  // DebugBitMask += DEBUG_ALARMS;
  // DebugBitMask += DEBUG_BME280;
  // DebugBitMask += DEBUG_BRIGHTNESS;
  // DebugBitMask += DEBUG_CHIME;
  // DebugBitMask += DEBUG_COMMAND_QUEUE;
  // DebugBitMask += DEBUG_CORE;
  // DebugBitMask += DEBUG_CRC16;
  // DebugBitMask += DEBUG_DHT;
  // DebugBitMask += DEBUG_DST;
  // DebugBitMask += DEBUG_EVENT;
  // DebugBitMask += DEBUG_FLASH;
  // DebugBitMask += DEBUG_IDLE_MONITOR;
  // DebugBitMask += DEBUG_INDICATORS;
  // DebugBitMask += DEBUG_IR_COMMAND;
  // DebugBitMask += DEBUG_NTP;
  // DebugBitMask += DEBUG_PICO_W;
  // DebugBitMask += DEBUG_PWM;
  // DebugBitMask += DEBUG_REMINDER;
  // DebugBitMask += DEBUG_RTC;
  // DebugBitMask += DEBUG_SOUND_QUEUE;
  // DebugBitMask += DEBUG_SCROLL;
  // DebugBitMask += DEBUG_TEMP;
  // DebugBitMask += DEBUG_TEST;
  // DebugBitMask += DEBUG_TIMER;
  // DebugBitMask += DEBUG_TIMING;
  // DebugBitMask += DEBUG_WATCHDOG;
  // DebugBitMask += DEBUG_JINGLE;
  #endif  // DEVELOPER_VERSION

  /* If user requested any section to be debugged through Pico's UART (or USB), send a power-up time stamp to log screen. */
  if (DebugBitMask)
  {
    /* Clear screen. */
    // uart_send(__LINE__, "cls");

    /* "Home" cursor. */
    // uart_send(__LINE__, "home");

    /* Assign temporary values until configuration is read from flash. */
    FlashConfig.CurrentYearCentile = DEFAULT_YEAR_CENTILE;
    FlashConfig.Language           = DEFAULT_LANGUAGE;

    /* Note: Force English for month names in log file since external monitor won't show up accented characters. */
    uart_send(__LINE__, "\r\r\r\r- - - - - = = = = = %2.2u-%s-%4.4u %2.2u:%2.2u GREEN CLOCK LOG INFO = = = = = - - - - -\r\r", CurrentDayOfMonth, MonthName[ENGLISH][CurrentMonth], CurrentYear, CurrentHour, CurrentMinute);
    uart_send(__LINE__, "NOTE: Parts of the time stamps below may be wrong until flash configuration has been\r");
    uart_send(__LINE__, "      read and validated, and callback to read real-time clock has been triggered.\r\r");

    /* Log all debug sections that we follow-up. */
    uart_send(__LINE__, "- Sections below will be logged for debug:\r");
    uart_send(__LINE__, "------------------------------------------\r");
    for (Loop1UInt8 = 0; Loop1UInt8 < 64; ++Loop1UInt8)
    {
      if (DebugBitMask & (1 << Loop1UInt8))
      {
        switch (1 << Loop1UInt8)
        {
          case DEBUG_ALARMS:
            uart_send(__LINE__, "-> Alarms\r");
          break;

          case DEBUG_BME280:
            uart_send(__LINE__, "-> BME280 temperature sensor\r");
          break;

          case DEBUG_BRIGHTNESS:
            uart_send(__LINE__, "-> LED matrix brightness\r");
          break;

          case DEBUG_CHIME:
            uart_send(__LINE__, "-> Hourly chime algorithm\r");
          break;

          case DEBUG_COMMAND_QUEUE:
            uart_send(__LINE__, "-> Command queue\r");
          break;

          case DEBUG_CORE:
            uart_send(__LINE__, "-> Multicore / multithread processing\r");
          break;

          case DEBUG_CRC16:
            uart_send(__LINE__, "-> CRC16 calculation\r");
          break;

          case DEBUG_DHT:
            uart_send(__LINE__, "-> DHT22\r");
          break;

          case DEBUG_DST:
            uart_send(__LINE__, "-> Daylight Saving Time algorithm\r");
          break;

          case DEBUG_EVENT:
            uart_send(__LINE__, "-> Calendar events\r");
          break;

          case DEBUG_FLASH:
            /* NOTE: To be used only before starting the callback functions, otherwise, flash_write() will crash the Firmware.*/
            uart_send(__LINE__, "-> Flash read / write\r");
          break;

          case DEBUG_IDLE_MONITOR:
            uart_send(__LINE__, "-> System idle monitor\r");
          break;

          case DEBUG_INDICATORS:
            uart_send(__LINE__, "-> Clock indicators\r");
          break;

          case DEBUG_IR_COMMAND:
            uart_send(__LINE__, "-> IR commands\r");
          break;

          case DEBUG_NTP:
            uart_send(__LINE__, "-> Network Time Protocol\r");
          break;

          case DEBUG_PICO_W:
            uart_send(__LINE__, "-> Pico W\r");
          break;

          case DEBUG_PWM:
            uart_send(__LINE__, "-> PWM\r");
          break;

          case DEBUG_REMINDER:
            uart_send(__LINE__, "-> Reminders\r");
          break;

          case DEBUG_RTC:
            uart_send(__LINE__, "-> Real-time clock\r");
          break;

          case DEBUG_SCROLL:
            uart_send(__LINE__, "-> Scroll mechanism and timing\r");
          break;

          case DEBUG_SOUND_QUEUE:
            uart_send(__LINE__, "-> Sound queue\r");
          break;

          case DEBUG_TEMP:
            uart_send(__LINE__, "-> Temporary, local debug\r");
          break;

          case DEBUG_TEST:
            uart_send(__LINE__, "-> Test zone\r");
          break;

          case DEBUG_TIMER:
            uart_send(__LINE__, "-> Timer\r");
          break;

          case DEBUG_TIMING:
            uart_send(__LINE__, "-> Timing\r");
          break;

          case DEBUG_JINGLE:
            uart_send(__LINE__, "-> Jingle\r");
          break;

          default:
            sprintf(String, "-> Section #%u\r", Loop1UInt8);
            uart_send(__LINE__, String);
          break;
        }
      }
    }
    uart_send(__LINE__, "------------------------------------------\r\r\r");
  }



  /* ---------------------------------------------------------------- *\
                       Real-time clock IC reporting.
  \* ---------------------------------------------------------------- */
  if (DebugBitMask & DEBUG_RTC)
  {
    uart_send(__LINE__, " Values read from real-time clock IC on entry:\r");
    uart_send(__LINE__, " Before syncing with NTP if using a Pico W\r");
    uart_send(__LINE__, " ---------------------------------------------\r");

    uart_send(__LINE__, "Hour:   %2.2u\r",   HumanTime.Hour);
    uart_send(__LINE__, "Minute: %2.2u\r",   HumanTime.Minute);
    uart_send(__LINE__, "Second: %2.2u\r",   HumanTime.Second);
    uart_send(__LINE__, "DoM:    %2.2u\r",   HumanTime.DayOfMonth);
    uart_send(__LINE__, "Month:  %2.2u\r",   HumanTime.Month);
    uart_send(__LINE__, "Year:   %2.2u\r",   HumanTime.Year);
    uart_send(__LINE__, "DoW:    %2.2u\r",   HumanTime.DayOfWeek);
    uart_send(__LINE__, "DoY:    %2.2u\r\r", HumanTime.DayOfYear);

    uart_send(__LINE__, "CurrentHour:         %2u\r", CurrentHour);
    uart_send(__LINE__, "CurrentMinute:       %2u\r", CurrentMinute);
    uart_send(__LINE__, "CurrentSecond:       %2u\r", CurrentSecond);
    uart_send(__LINE__, "CurrentDayOfWeek:    %2u   (1=Sun  2=Mon  3=Tue  4=Wed  5=Thu  6=Fri  7=Sat)\r", CurrentDayOfWeek);
    uart_send(__LINE__, "CurrentDayOfMonth:   %2u\r", CurrentDayOfMonth);
    uart_send(__LINE__, "CurrentMonth:        %2u\r", CurrentMonth);
    uart_send(__LINE__, "CurrentYear:       %4u\r\r", CurrentYear);

    uart_send(__LINE__, "Timezone must be added to current hour: %d\r\r\r", FlashConfig.Timezone);
  }



  /* ---------------------------------------------------------------- *\
           Infrared GPIO seems to receive spurious interrupts...
                            to be checked.
  \* ---------------------------------------------------------------- */
  if (DebugBitMask & DEBUG_IR_COMMAND)
  {
    uart_send(__LINE__, "GPIO for IR IRQ: %u\r", IR_RX);
    uart_send(__LINE__, "Events to reset / acknowledge: Edge rise event: %8X   Edge fall event: %8X\r", GPIO_IRQ_EDGE_RISE, GPIO_IRQ_EDGE_FALL);
  }



  /* ---------------------------------------------------------------- *\
               Reserve RAM space area for flash operations.
  \* ---------------------------------------------------------------- */
  FlashData = (UINT8 *)malloc(FLASH_SECTOR_SIZE);



  /* ---------------------------------------------------------------- *\
          Retrieve Green Clock configuration from flash memory.
  \* ---------------------------------------------------------------- */
  /* Intentionally left in source code if we want to erase current flash configuration to force generating a new default flash configuration. */
  /*
  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "Erasing configuration section of Pico's flash memory to force generating a new configuration.\r");
  flash_erase(0x1FF000);
  */

  flash_read_config();

  /* Now that "real" year centile has been read back from flash, update current year value and overwrite default value. */
  CurrentYear = (FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart;



  /* ------------------------------------------------------------------------ *\
      Update flash configuration incrementally for each new Firmware version.
      NOTE: As explained in the User Guide, Flash configuration will
            be automatically updated just before the next data
            scrolling, as long as the clock remains in normal
            "show time" mode.
  \* ------------------------------------------------------------------------ */
  /* Proceed step-by-step for each new version. */
  /* -------------------- UPDATE VERSION 6.00 TO VERSION 7.00 -------------------- */
  if (strcmp(FlashConfig.Version, "6.00") == 0)
  {
    /* Convert flash configuration from Version 6.00 to Version 7.00. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Display Flash configuration before conversion from Version 6.00 to Version 7.00.\r");
      flash_display_config();
    }

    sprintf(FlashConfig.Version, "7.00");   // convert to Version 7.00.
    FlashConfig.Timezone       = 0;         // assign default value within valid range.
    FlashConfig.FlagSummerTime = FLAG_OFF;  // FlashConfig.FlagSummerTime will be evaluated and overwritten below.

    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Display Flash configuration after conversion from Version 6.00 to Version 7.00.\r");
      flash_display_config();
    }
  }


  /* -------------------- UPDATE VERSION 7.00 TO VERSION 8.00 -------------------- */
  if (strcmp(FlashConfig.Version, "7.00") == 0)
  {
    sprintf(FlashConfig.Version, "8.00");   // convert to Version 8.00.
    /* No other change required from 7.00 to 8.00. */
  }


  /* -------------------- UPDATE VERSION 8.00 TO VERSION 9.00 -------------------- */
  if (strcmp(FlashConfig.Version, "8.00") == 0)
  {
    /* Convert flash configuration from Version 8.00 to Version 9.00. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Display Flash configuration before conversion from Version 8.00 to Version 9.00.\r");
      flash_display_config();
    }

    sprintf(FlashConfig.Version, "9.01");   // convert to Version 9.01
    FlashConfig.FlagAutoBrightness = FLAG_ON;
    FlashConfig.FlagKeyclick       = FLAG_ON;
    FlashConfig.FlagScrollEnable   = FLAG_ON;
    FlashConfig.FlagSummerTime     = FLAG_OFF;
    FlashConfig.TimeDisplayMode    = H24;
    for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
      FlashConfig.Alarm[Loop1UInt8].FlagStatus = FLAG_OFF;
    sprintf(FlashConfig.SSID,     ".;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.");                                // write specific footprint to flash memory.
    sprintf(FlashConfig.Password, ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.");  // write specific footprint to flash memory.
    sprintf(&FlashConfig.SSID[4],     NETWORK_NAME);
    sprintf(&FlashConfig.Password[4], NETWORK_PASSWORD);


    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Display Flash configuration after conversion from Version 8.00 to Version 9.0x\r");
      flash_display_config();
    }
  }


  /* -------------------- UPDATE VERSION 9.0x TO VERSION 10.00 -------------------- */
  if (strncmp(FlashConfig.Version, "9.", 2) == 0)
  {
    /* Convert flash configuration from Version 9.0x to Version 10.00. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Display Flash configuration before conversion from Version 9.00 to Version 10.00.\r");
      flash_display_config();
    }

    sprintf(FlashConfig.Version, "10.00");   // convert to Version 10.00.
    FlashConfig.FlagAutoBrightness = FLAG_ON;
    FlashConfig.FlagKeyclick       = FLAG_ON;
    FlashConfig.FlagScrollEnable   = FLAG_ON;
    FlashConfig.FlagSummerTime     = FLAG_OFF;
    FlashConfig.TimeDisplayMode    = H24;
    FlashConfig.DimmerMinLightLevel= 225;
    FlashConfig.ShortSetKey        = FLAG_ON;
    for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
      FlashConfig.Alarm[Loop1UInt8].FlagStatus = FLAG_OFF;
    sprintf(FlashConfig.Hostname, ".@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.");                                // write specific footprint to flash memory.
    sprintf(FlashConfig.SSID,     ".;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.");                                // write specific footprint to flash memory.
    sprintf(FlashConfig.Password, ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.");  // write specific footprint to flash memory.
    sprintf(&FlashConfig.Hostname[4], PICO_HOSTNAME);
    sprintf(&FlashConfig.SSID[4],     NETWORK_NAME);
    sprintf(&FlashConfig.Password[4], NETWORK_PASSWORD);

    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Display Flash configuration after conversion from Version 9.00 to Version 10.0x\r");
      flash_display_config();
    }
  }


  /* -------------------- UPDATE VERSION 10.00 TO VERSION 10.01 -------------------- */
  if (strcmp(FlashConfig.Version, "10.00") == 0)
  {
    /* Convert flash configuration from Version 10.0 to Version 10.01. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Display Flash configuration before conversion from Version 10.00 to Version 10.01.\r");
      flash_display_config();
    }
    sprintf(FlashConfig.Version, "10.01");   // convert to Version 10.01.
    // Reset the alarm settings
    /* Default configuration for 9 alarms. Text may be changed for another 40-characters max string. */

    FlashConfig.Alarm[0].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[0].Day    = (1 << MON) + (1 << TUE) + (1 << WED) + (1 << THU) + (1 << FRI);
    FlashConfig.Alarm[0].Hour   = 8;
    FlashConfig.Alarm[0].Minute = 00;
    FlashConfig.Alarm[0].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[0].Jingle = 0;
    #else
    FlashConfig.Alarm[0].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[0].Text, "Alarm 1"); // string to be scrolled when alarm 1 is triggered (ALARM TEXT).

    FlashConfig.Alarm[1].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[1].Day    = (1 << SAT) + (1 << SUN);
    FlashConfig.Alarm[1].Hour   = 14;
    FlashConfig.Alarm[1].Minute = 37;
    FlashConfig.Alarm[1].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[1].Jingle = 0;
    #else
    FlashConfig.Alarm[1].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[1].Text, "Alarm 2"); // string to be scrolled when alarm 2 is triggered (ALARM TEXT).

    FlashConfig.Alarm[2].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[2].Day    = (1 << MON);
    FlashConfig.Alarm[2].Hour   = 14;
    FlashConfig.Alarm[2].Minute = 36;
    FlashConfig.Alarm[2].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[2].Jingle = 0;
    #else
    FlashConfig.Alarm[2].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[2].Text, "Alarm 3"); // string to be scrolled when alarm 3 is triggered (ALARM TEXT).

    FlashConfig.Alarm[3].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[3].Day    = (1 << TUE);
    FlashConfig.Alarm[3].Hour   = 14;
    FlashConfig.Alarm[3].Minute = 35;
    FlashConfig.Alarm[3].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[3].Jingle = 0;
    #else
    FlashConfig.Alarm[3].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[3].Text, "Alarm 4"); // string to be scrolled when alarm 4 is triggered (ALARM TEXT).

    FlashConfig.Alarm[4].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[4].Day    = (1 << WED);
    FlashConfig.Alarm[4].Hour   = 14;
    FlashConfig.Alarm[4].Minute = 34;
    FlashConfig.Alarm[4].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[4].Jingle = 0;
    #else
    FlashConfig.Alarm[4].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[4].Text, "Alarm 5"); // string to be scrolled when alarm 5 is triggered (ALARM TEXT).

    FlashConfig.Alarm[5].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[5].Day    = (1 << THU);
    FlashConfig.Alarm[5].Hour   = 14;
    FlashConfig.Alarm[5].Minute = 33;
    FlashConfig.Alarm[5].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[5].Jingle = 0;
    #else
    FlashConfig.Alarm[5].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[5].Text, "Alarm 6"); // string to be scrolled when alarm 6 is triggered (ALARM TEXT).

    FlashConfig.Alarm[6].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[6].Day    = (1 << FRI);
    FlashConfig.Alarm[6].Hour   = 14;
    FlashConfig.Alarm[6].Minute = 32;
    FlashConfig.Alarm[6].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[6].Jingle = 0;
    #else
    FlashConfig.Alarm[6].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[6].Text, "Alarm 7"); // string to be scrolled when alarm 7 is triggered (ALARM TEXT).

    FlashConfig.Alarm[7].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[7].Day    = (1 << SAT);
    FlashConfig.Alarm[7].Hour   = 14;
    FlashConfig.Alarm[7].Minute = 31;
    FlashConfig.Alarm[7].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[7].Jingle = 0;
    #else
    FlashConfig.Alarm[7].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[7].Text, "Alarm 8"); // string to be scrolled when alarm 8 is triggered (ALARM TEXT).

    FlashConfig.Alarm[8].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
    FlashConfig.Alarm[8].Day    = (1 << SUN);
    FlashConfig.Alarm[8].Hour   = 14;
    FlashConfig.Alarm[8].Minute = 30;
    FlashConfig.Alarm[8].Second = 29;
    #ifdef PASSIVE_PIEZO_SUPPORT
    FlashConfig.Alarm[8].Jingle = 0;
    #else
    FlashConfig.Alarm[8].Jingle = 128;
    #endif
    sprintf(FlashConfig.Alarm[8].Text, "Alarm 9"); // string to be scrolled when alarm 9 is triggered (ALARM TEXT).

      // Reset the WiFi settings too
    sprintf(FlashConfig.Hostname, ".@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.");                                // write specific footprint to flash memory.
    sprintf(FlashConfig.SSID,     ".;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.");                                // write specific footprint to flash memory.
    sprintf(FlashConfig.Password, ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.");  // write specific footprint to flash memory.
    sprintf(&FlashConfig.Hostname[4], PICO_HOSTNAME);
    sprintf(&FlashConfig.SSID[4],     NETWORK_NAME);
    sprintf(&FlashConfig.Password[4], NETWORK_PASSWORD);

    /* Make provision for future parameters. */
    for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved2); ++Loop1UInt16)
    {
      FlashConfig.Reserved2[Loop1UInt16] = 0xFF;
    }

  }

  // /*** One-time FlashConfig writes may be inserted below... ***/
  // // NOTE: If you already ran Firmware Version 10.0x, network hostname, SSID and password will not be updated just by replacing the
  // //       #define HOSTNAME, #define NETWORK_NAME and #define NETWORK_PASSWORD at the beginning of the source code, instead, to set hostname,
  // //       network SSID and password in the code instead of setting them up with the clock, uncomment the six lines below and replace
  // //       MyNetworkName and MyNetworkPassword with the proper strings while keeping the surrounding "double-quotes".
  // sprintf(FlashConfig.Hostname, ".@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.");                                // write specific footprint to flash memory.
  // sprintf(FlashConfig.SSID,         ".;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.");
  // sprintf(FlashConfig.Password,     ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.");
  // sprintf(&FlashConfig.Hostname[4], PICO_HOSTNAME);
  // sprintf(&FlashConfig.SSID[4],     NETWORK_NAME);
  // sprintf(&FlashConfig.Password[4], NETWORK_PASSWORD);
  // Can be included by adding the above within a temporary WiFiCredentials.cpp file that isn't within the git repository
  #if defined __has_include && __has_include ("WiFiCredentials.cpp")
  #include "WiFiCredentials.cpp"
  #endif



  /* ---------------------------------------------------------------- *\
                        Initialize NTP connection.
    Also initialises the WiFi interface, connects and gets an IP address.
  \* ---------------------------------------------------------------- */
  #ifdef PICO_W
  ReturnCode = ntp_init();
  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "main() - ntp_init() return code: %d\r", ReturnCode);
  // Initialise web server daemon
  httpd_init();
  // Initialise web ssi functions
  ssi_init();
  // Initialise web server cgi
  cgi_init();

  #endif // PICO_W



  /* ---------------------------------------------------------------- *\
                    Initialize command queue on entry.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < MAX_COMMAND_QUEUE; ++Loop1UInt8)
  {
    CommandQueue[Loop1UInt8].Command   = 0;
    CommandQueue[Loop1UInt8].Parameter = 0;
  }
  CommandQueueHead = 0;
  CommandQueueTail = 0;



  /* ---------------------------------------------------------------- *\
                    Initialize scroll queue on entry.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < MAX_SCROLL_QUEUE; ++Loop1UInt8)
    ScrollQueue[Loop1UInt8] = 0xAA;  // let's put 0xAA since 0x00 corresponds to Calendar Event number 0.
  ScrollQueueHead = 0;
  ScrollQueueTail = 0;



  /* ---------------------------------------------------------------- *\
            Initialize sound queue for active buzzer on entry.
               (buzzer integrated in the Pico Green Clock)
  \* ---------------------------------------------------------------- */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_ACTIVE_SOUND_QUEUE; ++Loop1UInt16)
  {
    SoundQueueActive[Loop1UInt16].MSec        = 0;
    SoundQueueActive[Loop1UInt16].RepeatCount = 0;
  }
  SoundActiveHead = 0;
  SoundActiveTail = 0;



  /* ---------------------------------------------------------------- *\
            Initialize sound queue for passive buzzer on entry.
                 (must be bought and installed by user)
  \* ---------------------------------------------------------------- */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_PASSIVE_SOUND_QUEUE; ++Loop1UInt16)
  {
    SoundQueuePassive[Loop1UInt16].Freq = 0;
    SoundQueuePassive[Loop1UInt16].MSec = 0;
  }
  SoundPassiveHead = 0;
  SoundPassiveTail = 0;



  /* ---------------------------------------------------------------- *\
       Initialize communication queues for inter-core communication.
              DHT22 communication is done by Pico's core 1
              (DHT22 must be bought and installed by user)
  \* ---------------------------------------------------------------- */
  #ifdef DHT_SUPPORT
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_CORE_QUEUE; ++Loop1UInt16)
  {
    Core0Queue[Loop1UInt16] = 0;  // queue for core 0 commands / responses.
    Core1Queue[Loop1UInt16] = 0;  // queue for core 1 commands / responses.
  }
  Core0QueueHead = 0;
  Core0QueueTail = 0;
  Core1QueueHead = 0;
  Core1QueueTail = 0;
  #endif  // DHT_SUPPORT



  /* ---------------------------------------------------------------- *\
                     Initialize setup flags on entry.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_ALARM_HI_LIMIT; ++Loop1UInt8)
    FlagSetupAlarm[Loop1UInt8] = FLAG_OFF;

  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_CLOCK_HI_LIMIT; ++Loop1UInt8)
    FlagSetupClock[Loop1UInt8] = FLAG_OFF;

  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_TIMER_HI_LIMIT; ++Loop1UInt8)
    FlagSetupTimer[Loop1UInt8] = FLAG_OFF;



  /* ---------------------------------------------------------------- *\
        Clock "Setup order" in most languages is Day-Month(-Year),
                  as opposed to English Month-Day(-Year).
              Also include European English in this
  \* ---------------------------------------------------------------- */
  if ((FlashConfig.Language == FRENCH) || (FlashConfig.Language == SPANISH) || (FlashConfig.Language == CZECH))
  {
    SETUP_DAY_OF_MONTH = 0x03;
    SETUP_MONTH        = 0x04;
  }
  if (FlashConfig.Language == ENGLISH && FlashConfig.DSTCountry == DST_EUROPE)
  {
    SETUP_DAY_OF_MONTH = 0x03;
    SETUP_MONTH        = 0x04;
  }



  /* ---------------------------------------------------------------- *\
                  Initialize semaphore to synchronize
             "seconds" displaying with double dots blinking.
  \* ---------------------------------------------------------------- */
  /* ---------------------------------------------------------------- *\
     NOTE: SDK original documentation about sem_init() is confusing...
           It is written that second parameter is "int16_t initial_permits",
           but this parameter is described as "How many permits are
           initially acquired". We should read "How many permits are
           initially available". This parameter could be set to zero
           in the sem_init() function if the first action taken in
           the code is to release the semaphore.
  \* ---------------------------------------------------------------- */
  sem_init(&SemSync, 1, 1);



  /* ---------------------------------------------------------------- *\
                     Initialize callback functions.
  \* ---------------------------------------------------------------- */
  /* Initialize callback function for 1 millisecond timer (mainly for clock's button press). */
  add_repeating_timer_ms(-1, timer_callback_ms, NULL, &TimerMSec);

  /* Initialize callback function for 1 second timer (for hour change and many actions based on current time). */
  add_repeating_timer_ms(-1000, timer_callback_s, NULL, &TimerSec);

  /* Initialize sound callback function for 50 milliseconds timer (for both active and passive buzzers). */
  add_repeating_timer_ms(-50, sound_callback_ms, NULL, &Timer50MSec);



  /* ---------------------------------------------------------------- *\
      Now that the callback in charge of keeping the real time clock
          updated is started, resync the clock with the RTC IC.
  \* ---------------------------------------------------------------- */
  get_current_time(&HumanTime);

  CurrentHour        = HumanTime.Hour;
  CurrentMinute      = HumanTime.Minute;
  CurrentSecond      = HumanTime.Second;
  CurrentDayOfMonth  = HumanTime.DayOfMonth;
  CurrentMonth       = HumanTime.Month;
  CurrentYearLowPart = (HumanTime.Year % 100);
  CurrentYear        = HumanTime.Year;
  CurrentDayOfWeek   = HumanTime.DayOfWeek;

  convert_human_to_tm(&HumanTime, &TmTime);
  Dum1UInt64 = convert_tm_to_unix(&TmTime);
  if (DebugBitMask & DEBUG_REMINDER)
    uart_send(__LINE__, "Current Unix time: %llu\r", Dum1UInt64);



  /* ---------------------------------------------------------------- *\
      Initialize PWM for clock display brightness and passive buzzer.
  \* ---------------------------------------------------------------- */
  /* Set GPIO for clock display brightness ("Output Enable"). */
  Pwm[PWM_BRIGHTNESS].Gpio = OE;

  /* Set GPIO for passive buzzer. */
  Pwm[PWM_SOUND].Gpio = PPIEZO;

  /* Initialize both PWM. */
  pwm_initialize();




  /* ---------------------------------------------------------------- *\
          Check if user wants to set and save Wi-Fi credentials.
  \* ---------------------------------------------------------------- */
  /* If user pressed on the "Up" (middle) button for at least 5 seconds during power-up, set and save Wi-Fi credentials. */
  if (gpio_get(UP_BUTTON) == 0)
  {
    TimeStamp = time_us_32();  // keep track of timer value when entering the button-press delay.
    while (gpio_get(UP_BUTTON) == 0)
    {
      sleep_ms(1000);
      tone(50);
    }


    Duration = (time_us_32() - (float)TimeStamp);  // evaluate the time the button has been pressed.
    if (Duration > 5.0)
    {
      if (DebugBitMask & DEBUG_NTP)
        uart_send(__LINE__, "<Up> (middle) button pressed for more than 5 seconds (%3.2 sec) during power-up.\r", Duration / 1000000.0);

      for (Loop1UInt8 = 0; Loop1UInt8 < 3; ++Loop1UInt8)
      {
        tone(30);
        sleep_ms(40);
      }
      set_and_save_credentials();
    }
  }


  #ifdef TEST_CODE
  /* ---------------------------------------------------------------- *\
                           Testing area below
  \* ---------------------------------------------------------------- */
  /* By removing some double-slash comments below, we enable debug information to be sent to the external monitor.
     Refer to the User Guide on how to connect and configure an external monitor to an RS232 optional port on the Pico, of to the Pico's USB connector.
     NOTE: Those sections of code are left "as-is". They may have been left in an unstable state after tests. User must use them as "a starting point" for his own tests. */
  // test_zone(1);   // tests on RTC IC registers.
  // test_zone(2);   // tests for flash read / write / erase.
  // test_zone(3);   // tests for CRC16 function.
  // test_zone(4);   // test ambient light intensity.
  // test_zone(5);   // scrolling test.
  // test_zone(6);   // test variable character width and spacing.
  // test_zone(7);   // display 4 X 7 character map.
  // test_zone(8);   // display all characters of the 5 X 7 bitmap.
  // test_zone(9);   // PWM to drive passive buzzer and tests for indicators on the left side of the display.
  // test_zone(10);  // turn on each bit of each byte of the active display matrix.
  // test_zone(11);  // display every 5 X 7 character between a left and right frame.
  // test_zone(12);  // display all 5 X 7 characters, one at a time, using fill_display_buffer_5X7() function.
  // test_zone(13);  // display a few 5 X 7 characters on the display.
  // test_zone(14);  // scroll numbers and capital letters of 4 X 7 characters.
  // test_zone(15);  // display information about system variables for debug purposes.
  // test_zone(16);  // check each bit of each byte of the display matrix.
  // test_zone(17);  // play with clock display indicators.
  // test_zone(18);  // PWM to drive clock display brightness.
  /* ---------------------------------------------------------------- *\
                         End of testing area
  \* ---------------------------------------------------------------- */
  #endif  // TEST_CODE



  #ifndef QUICK_START
  /* ---------------------------------------------------------------- *\
                           Test active buzzer...
              Four short tones to indicate we are entering
                      the main Green Clock firmware.
  \* ---------------------------------------------------------------- */
  sound_queue_active(50, 4);



  /* ---------------------------------------------------------------- *\
                           Test passive buzzer.
                  (must be bought and installed by user).
  \* ---------------------------------------------------------------- */
  #ifdef PASSIVE_PIEZO_SUPPORT
  sound_queue_passive(SILENT, WAIT_4_ACTIVE);  // wait for active sound queue to complete.
  sound_queue_passive(523,   150);
  sound_queue_passive(659,   150);
  sound_queue_passive(784,   150);
  sound_queue_passive(1047,  200);
  sound_queue_passive(784,   150);
  sound_queue_passive(659,   150);
  sound_queue_passive(523,   700);
  sound_queue_passive(SILENT, 50);
  #endif  // PASSIVE_PIEZO_SUPPORT



  /* ---------------------------------------------------------------- *\
                      Test clock LED matrix.
  \* ---------------------------------------------------------------- */
  matrix_test(1);  // rows test.
  matrix_test(3);  // top indicators test.
  matrix_test(4);  // left indicators test.




  /* ---------------------------------------------------------------- *\
            Make some pixel animation on entry (just for fun).
  \* ---------------------------------------------------------------- */
  pixel_twinkling(3);
  #endif // QUICK_START



  /* ---------------------------------------------------------------- *\
              Clear clock LED matrix framebuffer on entry.
  \* ---------------------------------------------------------------- */
  clear_all_leds();



  /* ---------------------------------------------------------------- *\
            Properly adjust most indicators on clock display.
  \* ---------------------------------------------------------------- */
  update_left_indicators();
  update_top_indicators(ALL, FLAG_OFF);              // turn them all Off first.
  update_top_indicators(CurrentDayOfWeek, FLAG_ON);  // then turn On today's indicator.



  /***
  #ifdef DEVELOPER_VERSION
  pwm_on_off(PWM_SOUND, FLAG_ON);
  Loop1UInt16 = 400;
  pwm_set_frequency(Loop1UInt16);
  sleep_ms(100);

  Loop1UInt16 = 800;
  pwm_set_frequency(Loop1UInt16);
  sleep_ms(100);

  Loop1UInt16 = 1200;
  pwm_set_frequency(Loop1UInt16);
  sleep_ms(100);

  Loop1UInt16 = 1500;
  pwm_set_frequency(Loop1UInt16);
  sleep_ms(100);

  Loop1UInt16 = 1800;
  pwm_set_frequency(Loop1UInt16);
  sleep_ms(100);

  Loop1UInt16 = 2400;
  pwm_set_frequency(Loop1UInt16);
  sleep_ms(100);
  pwm_on_off(PWM_SOUND, FLAG_OFF);
  ***/

  /***
  // Sound test.
  pwm_on_off(PWM_SOUND, FLAG_ON);
  for (Loop1UInt8 = 0; Loop1UInt8 < 2; ++Loop1UInt8)
  {
    for (Loop2UInt16 = 500; Loop2UInt16 < 3000; Loop2UInt16 += 25)
    {
      pwm_set_frequency(Loop2UInt16);
      sleep_ms(10);
    }

    for (Loop2UInt16 = 3000; Loop2UInt16 > 500; Loop2UInt16 -= 25)
    {
      pwm_set_frequency(Loop2UInt16);
      sleep_ms(10);
    }

    for (Loop2UInt16 = 400; Loop2UInt16 < 8000; Loop2UInt16 += 4)
      pwm_set_frequency(Loop2UInt16);
      sleep_ms(15);
    }

    for (Loop2UInt16 = 8000; Loop2UInt16 > 400; Loop2UInt16 -= 4)
    {
      pwm_set_frequency(Loop2UInt16);
      sleep_ms(15);
    }

    for (Loop2UInt16 = 400; Loop2UInt16 < 8000; Loop2UInt16 += 4)
    {
      pwm_set_frequency(Loop2UInt16);
      sleep_ms(15);
    }
  }
  pwm_on_off(PWM_SOUND, FLAG_OFF);
  #endif  // DEVELOPER_VERSION
  ***/



  /* ------------------------------------------------------------------------------------------------------------------------ *\
                                                 DAYLIGHT SAVING TIME CHECK
                              ALSO CALLED "SUMMER TIME" OR "SPRING-FORWARD" IN SOME COUNTRIES
               When powering up the clock, determine if we are during the "Daylight Saving Time" active period...
                                Algorithm below allows to cover most countries of the world.
  \* ------------------------------------------------------------------------------------------------------------------------ */
  if (DebugBitMask & DEBUG_DST)
  {
    uart_send(__LINE__, "=========================================================================================================\r");
    uart_send(__LINE__, " Green-Clock power-up phase. This is flash configuration before checking Summer Time status:\r");
    flash_display_config();
    uart_send(__LINE__, "=========================================================================================================\r");
    uart_send(__LINE__, " Current Daylight Saving Time country setting: %2u ", FlashConfig.DSTCountry);

    switch(FlashConfig.DSTCountry)
    {
      case (DST_AUSTRALIA):
        uart_send(__LINE__, "- Australia\r");
      break;

      case (DST_AUSTRALIA_HOWE):
        uart_send(__LINE__, "- Australia - Howe\r");
      break;

      case (DST_CHILE):
        uart_send(__LINE__, "- Chile\r");
      break;

      case (DST_CUBA):
        uart_send(__LINE__, "- Cuba\r");
      break;

      case (DST_EUROPE):
        uart_send(__LINE__, "- Europe\r");
      break;

      case (DST_ISRAEL):
        uart_send(__LINE__, "- Israel\r");
      break;

      case (DST_LEBANON):
        uart_send(__LINE__, "- Lebanon\r");
      break;

      case (DST_MOLDOVA):
        uart_send(__LINE__, "- Moldova\r");
      break;

      case (DST_NEW_ZEALAND):
        uart_send(__LINE__, "- New-Zealand\r");
      break;

      case (DST_NORTH_AMERICA):
        uart_send(__LINE__, "- North America\r");
      break;

      case (DST_PALESTINE):
        uart_send(__LINE__, "- Palestine\r");
      break;

      case (DST_PARAGUAY):
        uart_send(__LINE__, "- Paraguay\r");
      break;
    }
    uart_send(__LINE__, "=========================================================================================================\r");


    /* Indicate if DST parameters being analyzed are for a northern or a southern country. */
    if (DstParameters[FlashConfig.DSTCountry].StartMonth < DstParameters[FlashConfig.DSTCountry].EndMonth)  // northern countries.
      uart_send(__LINE__, " Daylight Saving Time for a northern country\r\r");
    else
      uart_send(__LINE__, " Daylight Saving Time for a southern country\r\r");


    /* Display all DST parameters for current DST setting . */
    uart_send(__LINE__, "     - StartDayOfMonthLow: %2u       EndDayOfMonthLow: %2u\r", DstParameters[FlashConfig.DSTCountry].StartDayOfMonthLow, DstParameters[FlashConfig.DSTCountry].EndDayOfMonthLow);

    uart_send(__LINE__, "    - StartDayOfMonthHigh: %2u      EndDayOfMonthHigh: %2u\r", DstParameters[FlashConfig.DSTCountry].StartDayOfMonthHigh, DstParameters[FlashConfig.DSTCountry].EndDayOfMonthHigh);

    uart_send(__LINE__, "         - StartDayOfWeek: %2u           EndDayOfWeek: %2u      %3.3s (%2u to %2u)    %3.3s (%2u to %2u)\r",
            DstParameters[FlashConfig.DSTCountry].StartDayOfWeek, DstParameters[FlashConfig.DSTCountry].EndDayOfWeek,
            ShortDay[FlashConfig.Language][DstParameters[FlashConfig.DSTCountry].StartDayOfWeek],
            DstParameters[FlashConfig.DSTCountry].StartDayOfMonthLow, DstParameters[FlashConfig.DSTCountry].StartDayOfMonthHigh,
            ShortDay[FlashConfig.Language][DstParameters[FlashConfig.DSTCountry].EndDayOfWeek],
            DstParameters[FlashConfig.DSTCountry].EndDayOfMonthLow,   DstParameters[FlashConfig.DSTCountry].EndDayOfMonthHigh );

    uart_send(__LINE__, "             - StartMonth: %2u               EndMonth: %2u          %9s         %9s\r",
            DstParameters[FlashConfig.DSTCountry].StartMonth, DstParameters[FlashConfig.DSTCountry].EndMonth,
            MonthName[ENGLISH][DstParameters[FlashConfig.DSTCountry].StartMonth], MonthName[ENGLISH][DstParameters[FlashConfig.DSTCountry].EndMonth]);

    uart_send(__LINE__, "              - StartHour: %2u                EndHour: %2u\r\r", DstParameters[FlashConfig.DSTCountry].StartHour, DstParameters[FlashConfig.DSTCountry].EndHour);

    uart_send(__LINE__, "          - Shift minutes: %2u\r\r\r", DstParameters[FlashConfig.DSTCountry].ShiftMinutes);
  }



  /* ---------------------------------------------------------------- *\
              Find DayOfYear for DST start and end dates
           for current country DST setting and current year.
  \* ---------------------------------------------------------------- */
  get_dst_days();



  /* ---------------------------------------------------------------- *\
      Set DST parameters (FlagSummerTime and Timezone) according to
       current DST country code setting and current date and time.
  \* ---------------------------------------------------------------- */
  update_dst_status();



  /* If we want to try different combinations to test correct handling of DST status for different countries, dates, times, etc... */
  #ifdef DST_DEBUG
  test_dst_status();
  #endif  // DST_DEBUG
  /* ------------------------------------------------------------------------------------------------------------------------ *\
                                                End of daylight saving time section
  \* ------------------------------------------------------------------------------------------------------------------------ */


  /* ------------------------------------------------------------------------------------------------------------------------ *\
                                                  Handling of Reminders of type 1.
  \* ------------------------------------------------------------------------------------------------------------------------ */

  /***
  struct reminder1
  {
    struct human_time StartPeriod;
    UINT64 StartPeriodEpoch;
    struct human_time EndPeriod;
    UINT64 EndPeriodEpoch;
    struct human_time FirstRing;
    UINT64 FirstRingEpoch;
    UINT64 RingDuration;
    UINT64 RingRepeatTime;
    UINT64 NextReminderDelay;
    UCHAR  Description[51];
  };
  ***/

  for (Loop1UInt8 = 0; Loop1UInt8 < MAX_REMINDERS1; ++Loop1UInt8)
  {
    if (Reminder1[Loop1UInt8].StartPeriod.Year == 0)
    {
      // if (DebugBitMask & DEBUG_REMINDER)
      //   uart_send(__LINE__, "Reminder1[%2u].Year = 0: This reminder is not defined.\r", Loop1UInt8);

      continue;
    }

    if (DebugBitMask & DEBUG_REMINDER)
      uart_send(__LINE__, "Reminder1[%2u].Year != 0: This reminder is defined.... finding epoch where needed\r", Loop1UInt8);

    Dum1UInt16 = Reminder1[Loop1UInt8].StartPeriod.Year;
    if (Reminder1[Loop1UInt8].StartPeriod.Year == 9999)
    {
      /* Year 9999 is a placeholder to repeat the reminder for every year. */
       Reminder1[Loop1UInt8].StartPeriod.Year == CurrentYear;
    }
    convert_human_to_tm(&Reminder1[Loop1UInt8].StartPeriod, &TmTime);
    Reminder1[Loop1UInt8].StartPeriodEpoch = convert_tm_to_unix(&TmTime);
    if (DebugBitMask & DEBUG_REMINDER)
      uart_send(__LINE__, "Reminder1[%2u].StartPeriodEpoch found: %llu\r", Loop1UInt8, Reminder1[Loop1UInt8].StartPeriodEpoch);

    /* Restore original year in case it was the 9999 placeholder and replaced temporarily. */
    Reminder1[Loop1UInt8].StartPeriod.Year = Dum1UInt16;



    Dum1UInt16 = Reminder1[Loop1UInt8].EndPeriod.Year;
    if (Reminder1[Loop1UInt8].EndPeriod.Year == 9999)
    {
      /* Year 9999 is a placeholder to repeat the reminder for every year. */
       Reminder1[Loop1UInt8].EndPeriod.Year == CurrentYear;
    }
    convert_human_to_tm(&Reminder1[Loop1UInt8].EndPeriod, &TmTime);
    Reminder1[Loop1UInt8].EndPeriodEpoch = convert_tm_to_unix(&TmTime);
    if (DebugBitMask & DEBUG_REMINDER)
      uart_send(__LINE__, "Reminder1[%2u].EndPeriodEpoch found:   %llu\r", Loop1UInt8, Reminder1[Loop1UInt8].EndPeriodEpoch);

    /* Restore original year in case it was the 9999 placeholder and replaced temporarily. */
    Reminder1[Loop1UInt8].EndPeriod.Year = Dum1UInt16;
  }

  /* ------------------------------------------------------------------------------------------------------------------------ *\
                                                End of handling of reminders of type 1
  \* ------------------------------------------------------------------------------------------------------------------------ */


  /* ---------------------------------------------------------------- *\
                        Scroll firmware version.
  \* ---------------------------------------------------------------- */
  switch (FlashConfig.Language)
  {
    case (CZECH):
      sprintf(String, "Pico Green Clock - verze firmwaru %s    ", FIRMWARE_VERSION);
    break;

    case (ENGLISH):
    default:
      sprintf(String, "Pico Green Clock - Firmware Version %s    ", FIRMWARE_VERSION);
    break;

    case (FRENCH):
      sprintf(String, "Pico Green Clock - Microcode Version %s    ", FIRMWARE_VERSION);
    break;

    case (SPANISH):
      sprintf(String, "Pico Green Clock - Version de Firmware %s    ", FIRMWARE_VERSION);
    break;
  }
  scroll_string(24, String);



  /* ---------------------------------------------------------------- *\
        Special message if sound has been cut-off at compile-time.
  \* ---------------------------------------------------------------- */
  #ifdef SOUND_DISABLED
  /* Special warning if sound has been cut-off in source code. */
  switch (FlashConfig.Language)
  {
    case (CZECH):
      scroll_string(24, "POZOR - BEZ ZVUKU!    ");
    break;

    case (FRENCH):
      scroll_string(24, "ATTENTION - PAS DE SON    ");
    break;

    case (SPANISH):
      scroll_string(24, "AVISO - SONIDO APAGADO    ");
    break;

    default:
    case (ENGLISH):
      scroll_string(24, "WARNING - SOUND CUT-OFF    ");
    break;
  }
  #endif  // SOUND_DISABLED


  /* ---------------------------------------------------------------- *\
           Special message if some debug options are selected.
  \* ---------------------------------------------------------------- */
  if (DebugBitMask)
  {
    switch (FlashConfig.Language)
    {
      case (CZECH):
        scroll_string(24, "DEBUG ZAPNUT!    ");
      break;

      case (FRENCH):
        scroll_string(24, "DES DEBUG SONT ACTIFS    ");
      break;

      case (SPANISH):
        scroll_string(24, "DEPURACION ACTIVADA    ");
      break;

      default:
      case (ENGLISH):
        scroll_string(24, "SOME DEBUG ON    ");
      break;
    }
  }



  #ifdef BME280_SUPPORT
  /* ---------------------------------------------------------------- *\
                        Initialize BME280 sensor.
                 (must be bought and installed by user).
  \* ---------------------------------------------------------------- */
  Bme280Data.Bme280Errors     = 0l;  // reset number of read errors on entry.
  Bme280Data.Bme280ReadCycles = 0l;  // reset number of read cycles on entry.
  if (bme280_init())
  {
    switch (FlashConfig.Language)
    {
      case (CZECH):
        sprintf(String, "Chyba nastaveni BME280.    ");
        String[14] = (UINT8)131; // i-acute
        scroll_string(24, String);
      break;

      case (FRENCH):
        scroll_string(24, "BME280 erreur d'initialisation    ");
      break;

      case (SPANISH):
        scroll_string(24, "BME280 error de inicializacion    ");
      break;

      default:
      case (ENGLISH):
        scroll_string(24, "BME280 initialization error    ");
      break;
    }
  }
  else
  {
    /* Read BME280 device ID to make sure it is a "real" BME280, since some companies sell BMP280 while saying they are BME280. */
    /* BME280 device ID is 0x60, while 0x56 and 0x57 are BMP280 samples and 0x58 are BMP280 mass production. */
    Dum1UInt8 = bme280_read_device_id();
    switch (Dum1UInt8)
    {
      case 0x60:
        scroll_string(24, "BME280 device ID: 0x60    ");
      break;

      case 0x56:
      case 0x57:
        sprintf(String, "BMP280 device ID: 0x%2.2X (sample production units)    ", Dum1UInt8);
        scroll_string(24, String);
        scroll_string(24, "Your device seems not to be a true BME280    ");
      break;

      case 0x58:
        scroll_string(24, "BMP280 device ID: 0x58 (mass production units)    ");
        scroll_string(24, "Your device seems not to be a true BME280    ");
      break;

      default:
        sprintf(String, "Unrecognized BME280 device ID: 0x%2.2X    ", Dum1UInt8);
        scroll_string(24, String);
        scroll_string(24, "Your device seems not to be a true BME280    ");
      break;
    }

    /* Read BME280 unique id ("serial number" of the specific device used). */
    Bme280UniqueId = bme280_read_unique_id();
    sprintf(String, "BME280 Unique ID: %4.4X %4.4X    ", ((Bme280UniqueId & 0xFFFF0000) >> 16), Bme280UniqueId & 0xFFFF);
    scroll_string(24, String);

    /* Read calibration data of the specific device used (written in BME280's non volatile memory). */
    bme280_read_calib_data();

    /* Compute calibration parameters from calibration data. */
    bme280_compute_calib_param();
  }
  #endif // BME280_SUPPORT



  #ifndef QUICK_START
  /* ---------------------------------------------------------------- *\
               Scroll microcontroller type (Pico or PicoW)
                        and Pico Unique Number
            NOTE: Scroll queue will be processed when entering
                      the main program loop below.
  \* ---------------------------------------------------------------- */
  scroll_queue(TAG_PICO_TYPE);
  scroll_queue(TAG_PICO_UNIQUE_ID);

  // Debug - play out a jingle to see how it sounds
  #ifdef PASSIVE_PIEZO_SUPPORT
  #ifdef DEBUG_JINGLE
    // play_jingle(JINGLE_BIRTHDAY);
    // play_jingle(JINGLE_ENCOUNTER);
    // play_jingle(JINGLE_FETE);
    // play_jingle(JINGLE_RACING);
    // play_jingle(JINGLE_CHRISTMAS);
    // play_jingle(JINGLE_SUNHASHAT);
    // play_jingle(JINGLE_CANOND);
  #endif  // DEBUG_JINGLE
  #endif  // PASSIVE_PIEZO_SUPPORT



  /* ---------------------------------------------------------------- *\
              Scroll daylight saving time status on power up.
            NOTE: Scroll queue will be processed when entering
                      the main program loop below.
  \* ---------------------------------------------------------------- */
  scroll_queue(TAG_DST);



  /* ---------------------------------------------------------------- *\
              Scroll power supply voltage value on power up.
           NOTE: Scroll queue will be processed when entering
                      the main program loop below.
  \* ---------------------------------------------------------------- */
  scroll_queue(TAG_VOLTAGE);



  /* ---------------------------------------------------------------- *\
                    Scroll Pico internal temperature.
            NOTE: Scroll queue will be processed when entering
                      the main program loop below.
  \* ---------------------------------------------------------------- */
  scroll_queue(TAG_PICO_TEMP);
  #endif  // QUICK_START



  /* Initialize interrupt handler to capture remote control infrared commands data stream. */
  #ifdef IR_SUPPORT
  IrStepCount = 0;
  gpio_set_irq_enabled_with_callback(IR_RX, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, (gpio_irq_callback_t)&isr_signal_trap);
  #endif  // IR_SUPPORT



  /* ---------------------------------------------------------------- *\
                  Validate communication with DHT22.
  \* ---------------------------------------------------------------- */
  #ifdef DHT_SUPPORT
  /* Start the thread to run on core 1 (second Pico's core) to read the data stream from DHT22 without interference from callback functions
     and other potential interrupts on core 0. After trial-and-error, I came to the conclusion that this was the optimal solution. I did try
     disabling interrupts, however the best that I got was a good communication but with glitches on the clock display while disabling / re-enabling interrupts. */
  multicore_launch_core1(core1_main);


  /* Send the command to core 1 to read DHT22 and then wait for the read cycle to complete. */
  core_queue(1, CORE1_READ_DHT);
  sleep_ms(400);

  /* Read back answer (success or failure) from core 1. Data has been saved in a structure global to both cores. */
  if (core_unqueue(0) == CORE0_DHT_ERROR)
  {
    switch (FlashConfig.Language)
    {
      case (CZECH):
         scroll_string(24, "Chyba v komunikaci s DHT22.    ");
      break;

      case (FRENCH):
        scroll_string(24, "DHT22 erreur de communication    ");
      break;

      case (SPANISH):
        scroll_string(24, "DHT22 error de comunicacion    ");
      break;

      default:
      case (ENGLISH):
        scroll_string(24, "DHT22 communication error    ");
      break;
    }
  }
  else
  {
    if (DebugBitMask & DEBUG_CORE)
    {
      TimeStamp = time_us_64();
      uart_send(__LINE__, "DHT22 read successful\r");
      uart_send(__LINE__, "Current TimeStamp: %llu\r", TimeStamp);
      uart_send(__LINE__, "DHT22 TimeStamp:   %llu\r", DhtData.TimeStamp);
      uart_send(__LINE__, "TimeStamp difference: %2.2f milliseconds\r", ((TimeStamp - DhtData.TimeStamp) / 1000.0));
      uart_send(__LINE__, "Temperature: %2.2f\r", DhtData.Temperature);
      uart_send(__LINE__, "Humidity:    %2.2f\r", DhtData.Humidity);
    }
  }
  #endif  // DHT_SUPPORT



  if (DebugBitMask & DEBUG_EVENT)
  {
    uart_send(__LINE__, "Event    Day      Month       Description\r");
    uart_send(__LINE__, "number\r");

    for (Loop1UInt8 = 0; Loop1UInt8 < MAX_EVENTS; ++Loop1UInt8)
      uart_send(__LINE__, "  %2u      %2llu %10s (%2.2u)   [%s]\r", Loop1UInt8, CalendarEvent[Loop1UInt8].Day, MonthName[ENGLISH][CalendarEvent[Loop1UInt8].Month], CalendarEvent[Loop1UInt8].Month, CalendarEvent[Loop1UInt8].Description);
  }


  /*** Enable a watchdog in the main program loop. If the loop gets stuck (and the watchdog timer is not refreshed,
     the watchdog will reset the Pico.
     NOT IMPLEMENTED YET. Watchdog maximum time is 8.3 seconds...***/
  /// watchdog_enable(0x7FFFFF, true);  // about 8.3 seconds watchdog, pause it when debugging the Pico.



  /* ======================================================================================= *\
                              Main program loop... will loop for ever.
  \* ======================================================================================= */
  FlagUpdate = FLAG_OFF;
  while (TRUE)
  {
    /***
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      if ((((CurrentMinute - 1) % 3) == 0) && (FlagUpdate == FLAG_ON)) FlagUpdate = FLAG_OFF;

      if (((CurrentMinute % 3) == 0) && (FlagUpdate == FLAG_OFF))
      {
        play_jingle(JINGLE_RACING);


        if (SoundActiveHead != SoundActiveTail)
        {
          uart_send(__LINE__, "ACTIVE SOUND QUEUE (%u)   Head: %4u   Tail: %4u\r", MAX_ACTIVE_SOUND_QUEUE, SoundActiveHead, SoundActiveTail);
          uart_send(__LINE__, "          MSec    Repeat\r");

          for (Loop1UInt16 = 0; Loop1UInt16 < MAX_ACTIVE_SOUND_QUEUE; ++Loop1UInt16)
            uart_send(__LINE__, "(%4u)   %5u     %5u\r", Loop1UInt16, SoundQueueActive[Loop1UInt16].MSec, SoundQueueActive[Loop1UInt16].RepeatCount);
        }


        if (SoundPassiveHead != SoundPassiveTail)
        {
          uart_send(__LINE__, "PASSIVE SOUND QUEUE (%u)   Head: %4u   Tail: %4u\r", MAX_PASSIVE_SOUND_QUEUE, SoundPassiveHead, SoundPassiveTail);
          uart_send(__LINE__, "        Freq   MSec\r");

          for (Loop1UInt16 = 0; Loop1UInt16 < MAX_PASSIVE_SOUND_QUEUE; ++Loop1UInt16)
            uart_send(__LINE__, "(%4u) %5u  %5u\r", Loop1UInt16, SoundQueuePassive[Loop1UInt16].Freq, SoundQueuePassive[Loop1UInt16].MSec);
        }

        FlagUpdate = FLAG_ON;
      }
    }
    ***/


    /*** Reset watchdog. ***  NOT IMPLEMENTED YET ***
    watchdog_update();
    if (DebugBitMask & DEBUG_CORE)
    {
      CurrentWatchDogReset = time_us_64();
      uart_send(__LINE__, "Reset watchdog: %llu - %llu = %llu", CurrentWatchDogReset, LastWatchDogReset, (CurrentWatchDogReset - LastWatchDogReset));
      LastWatchDogReset = CurrentWatchDogReset;
    }

    sleep_ms(10000);  // force watchdog trigger.
    if (DebugBitMask & DEBUG_WATCHDOG)
      uart_send(__LINE__, String);
    ***/


    #ifdef PICO_W
    /* Manage NTP resync. */
    CurrentTimerValue = time_us_64();
    if ((NTPData.FlagNTPResync) || (CurrentTimerValue >= (NTPData.NTPLastUpdate + DELTA_TIME)))
    {
      if (DebugBitMask & DEBUG_NTP)
        uart_send(__LINE__, "Requesting Green Clock synchronization through NTP  (FlagNTPResync: %2.2u)\r", NTPData.FlagNTPResync);

      /* Retrieve local time from NTP. */
      ntp_get_time();

      /* Wait for NTP result. */
      for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
      {
        if (NTPData.FlagNTPSuccess == FLAG_ON)
        {
          if (DebugBitMask & DEBUG_NTP)
          {
            uart_send(__LINE__, "Green Clock time before resync:\r");
            uart_send(__LINE__, "DoW: %s   Date: %2.2u/%2.2u/%4.4u   Time: %2.2u:%2.2u:%2.2u\r\r", DayName[FlashConfig.Language][CurrentDayOfWeek], CurrentDayOfMonth, CurrentMonth, CurrentYear, CurrentHour, CurrentMinute, CurrentSecond);


            /* Display DS3231 current time (real-time clock IC) to visualize time drift since last NTP re-sync. */
            Time_RTC = Read_RTC();

            CurrentHour        = bcd_to_byte(Time_RTC.hour);
            CurrentMinute      = bcd_to_byte(Time_RTC.minutes);
            CurrentSecond      = bcd_to_byte(Time_RTC.seconds);
            CurrentDayOfMonth  = bcd_to_byte(Time_RTC.dayofmonth);
            CurrentMonth       = bcd_to_byte(Time_RTC.month);
            CurrentYearLowPart = bcd_to_byte(Time_RTC.year);
            CurrentYear        = (FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart;
            CurrentDayOfWeek   = get_day_of_week(((FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart), CurrentMonth, CurrentDayOfMonth);

            uart_send(__LINE__, "DS3231 time before resync:\r");
            uart_send(__LINE__, "DoW: %s   Date: %2.2u/%2.2u/%4.4u   Time: %2.2u:%2.2u:%2.2u\r\r", DayName[FlashConfig.Language][CurrentDayOfWeek], CurrentDayOfMonth, CurrentMonth, CurrentYear, CurrentHour, CurrentMinute, CurrentSecond);
            uart_send(__LINE__, "NTP time (synchronizing clock with those values (delay: %u milliseconds).\r", 50 * Loop1UInt8);
            uart_send(__LINE__, "DoW: %s   Date: %2.2u/%2.2u/%4.4u   Time: %2.2u:%2.2u:%2.2u\r\r", DayName[FlashConfig.Language][NTPData.CurrentDayOfWeek], NTPData.CurrentDayOfMonth, NTPData.CurrentMonth, NTPData.CurrentYear, NTPData.CurrentHour, NTPData.CurrentMinute, NTPData.CurrentSecond);
          }

          NTPData.FlagNTPSuccess = FLAG_OFF;
          sprintf(String, "%4.4u", CurrentYear);
          CurrentYearLowPart = atoi(&String[2]);

          set_time(NTPData.CurrentSecond, NTPData.CurrentMinute, NTPData.CurrentHour, NTPData.CurrentDayOfWeek, NTPData.CurrentDayOfMonth, NTPData.CurrentMonth, CurrentYearLowPart);
          show_time();  // update time display as soon as possible.

          /* Read back the real-time clock IC. */
          Time_RTC = Read_RTC();

          CurrentHour        = bcd_to_byte(Time_RTC.hour);
          CurrentMinute      = bcd_to_byte(Time_RTC.minutes);
          CurrentSecond      = bcd_to_byte(Time_RTC.seconds);
          CurrentDayOfMonth  = bcd_to_byte(Time_RTC.dayofmonth);
          CurrentMonth       = bcd_to_byte(Time_RTC.month);
          CurrentYearLowPart = bcd_to_byte(Time_RTC.year);
          CurrentYear        = (FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart;
          CurrentDayOfWeek   = get_day_of_week(((FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart), CurrentMonth, CurrentDayOfMonth);

          if (DebugBitMask & DEBUG_NTP)
          {
            uart_send(__LINE__, "Real time clock, as read-back from DS3231.\r");
            uart_send(__LINE__, "DoW: %s   Date: %2.2u/%2.2u/%4.4u   Time: %2.2u:%2.2u:%2.2u\r", DayName[FlashConfig.Language][CurrentDayOfWeek], CurrentDayOfMonth, CurrentMonth, CurrentYear, CurrentHour, CurrentMinute, CurrentSecond);
            uart_send(__LINE__, "NTPData.NTPGetTime: 0x%10.10llX\r", NTPData.NTPGetTime);
            if (NTPData.NTPGetTime)
              uart_send(__LINE__, "Time elapsed since last poll: %10lld usec.   %10lld usec.\r\r", (time_us_64() - NTPData.NTPLastUpdate), (time_us_64() - NTPData.NTPGetTime));
          }

          NTPData.FlagNTPResync = FLAG_OFF;
          NTPData.NTPGetTime    = time_us_64();
          NTPData.NTPLastUpdate = time_us_64();

          break;  // get out of "for" loop.
        }


        if (NTPData.FlagNTPSuccess == FLAG_POLL)
        {
          if (DebugBitMask & DEBUG_NTP)
            uart_send(__LINE__, "NTP poll succeeded\r\r");

          NTPData.FlagNTPSuccess = FLAG_OFF;
          NTPData.NTPLastUpdate  = time_us_64();

          break;  // get out of "for" loop.
        }

        sleep_ms(50);
      }


      /* If current NTP update request failed, add one minute to update delay and retry. */
      if (Loop1UInt8 >= 10)
      {
        NTPData.FlagNTPResync = FLAG_OFF;  // NTP resync error... postpone re-sync.
        ++NTPData.NTPErrors;
        if (DebugBitMask & DEBUG_NTP)
          uart_send(__LINE__, "Failed to synchronize clock with NTP.\r\r");

        NTPData.NTPLastUpdate += 60000000;   // simulate that last update occured later than reality to postpone next retry.
      }
    }
    #endif  // PICO_W



    /* If user pressed the "Set" (top) button (detected in the callback msec function), proceed with clock setup. */
    if (FlagSetClock == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON;  // keep track of inactive (idle) time to eventually declare a time-out.
      setup_clock_frame();      // display clock template and current values.
      FlagSetClock = FLAG_OFF;  // reset clock setup mode flag after this pass.
    }


    /* If we just entered the "Alarm" setup mode (detected in the call-back function), proceed with alarm setup. */
    if (FlagSetAlarm == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON;  // keep track of inactive (idle) time to eventually declare a time-out.
      setup_alarm_frame();      // display alarm template and current values.
      FlagSetAlarm = FLAG_OFF;  // reset alarm setup mode flag after this pass.
    }


    /* If we just entered the "Timer" setup mode (detected in the call-back function), proceed with timer setup. */
    if (FlagSetTimer == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON;  // keep track of inactive (idle) time to eventually declare a time-out.
      setup_timer_frame();      // display timer template and current values.
      FlagSetTimer = FLAG_OFF;  // reset timer setup mode after this pass.
    }


    /* Check if it is time to update time display on the clock. */
    if ((FlagUpdateTime == FLAG_ON) && (ScrollDotCount == 0))
    {
      show_time();
      FlagUpdateTime = FLAG_OFF;
    }

    /* Check to see if a web page update asks for a flash stored value to be changed */
    if ((FlagWebRequestFlashCheck == FLAG_ON) && ((CurrentSecond % 5) == 0))
    {
      // Check to see if the flash memory needs updating and update if needed
      flash_check_config();
      // Clear the flag
      FlagWebRequestFlashCheck = FLAG_OFF;
    }

    /* Check to see if a web page asks for a date/time synchronisation with the NTP server */
    if ((FlagWebRequestNTPSync == FLAG_ON) && ((CurrentSecond % 5) == 4))
    {
      /* Request a NTP re-sync if clock setup has been changed (Time, Date, or Timezone may have been changed). */
      NTPData.FlagNTPResync = FLAG_ON;
      // Clear the flag
      FlagWebRequestNTPSync = FLAG_OFF;
    }


    if (CurrentClockMode == MODE_SHOW_TIME) show_time;


    /* If a command has been inserted in the command queue, process it. */
    if (CommandQueueHead != CommandQueueTail) process_command_queue();


    /* If a tag has been inserted in the scroll queue, process it. */
    if (ScrollQueueHead != ScrollQueueTail) process_scroll_queue();


    #ifdef IR_SUPPORT
    /* If infrared remote control support is enable, check if data has been received by infrared sensor. */
    if (IrStepCount != 0)
    {
      if (DebugBitMask & DEBUG_IR_COMMAND)
      {
        printf("\r\r");
        uart_send(__LINE__, "Received IR data - IrStepCount: %u\r", IrStepCount);
      }

      sleep_ms(250);  // make sure all IR burst has been received.

      if (DebugBitMask & DEBUG_IR_COMMAND)
      {
        uart_send(__LINE__, "Received IR data after wait time - IrStepCount: %u\r", IrStepCount);

        /***/
        // Optionally display timing for every logic level change of last received infrared burst.
        for (Loop1UInt16 = 0; Loop1UInt16 < IrStepCount; ++Loop1UInt16)
          uart_send(__LINE__, "IR event number: %3u    IrLevel: %c   IrInitialValue: %10llu    IrFinalValue: %10llu    IrResultValue: %5lu\r", Loop1UInt16, IrLevel[Loop1UInt16], IrInitialValue[Loop1UInt16], IrFinalValue[Loop1UInt16], IrResultValue[Loop1UInt16], IrFinalValue[Loop1UInt16] - IrInitialValue[Loop1UInt16]);

        uart_send(__LINE__, "Total number of logic level changes (IrStepCount): %u (0 to %u)\r", IrStepCount, (IrStepCount - 1));
         printf("\r\r");
        /***/


        /* Display debug header of last infrared burst decoded. */
        uart_send(__LINE__, "Event   Bit   Level  Duration  Level  Duration            Result\r");
        uart_send(__LINE__, "number number\r");

        DataBuffer = 0ll;
        for (Loop1UInt16 = 0; Loop1UInt16 < IrStepCount; Loop1UInt16 += 2)
        {
          BitNumber = (((Loop1UInt16 - 2) / 2) + 1);

          /* Display two <Get ready> levels from IR burst. */
          if (Loop1UInt16 < 2)
            uart_send(__LINE__, " [%2u]    --     %c     %5lu      %c     %5lu           <get ready>\r", Loop1UInt16, IrLevel[Loop1UInt16], IrResultValue[Loop1UInt16], IrLevel[Loop1UInt16 + 1], IrResultValue[Loop1UInt16 + 1]);


          /* Display 32 data bits. */
          if ((BitNumber > 0) && (BitNumber <= 32))
          {
            DataBuffer <<= 1;
            if (IrResultValue[Loop1UInt16 + 1] > 1400) ++DataBuffer;
            uart_send(__LINE__, " [%2u]   %3u     %c     %5lu      %c     %5lu      Data: 0x%8.8X\r", Loop1UInt16, BitNumber, IrLevel[Loop1UInt16], IrResultValue[Loop1UInt16], IrLevel[Loop1UInt16 + 1], IrResultValue[Loop1UInt16 + 1], DataBuffer);
          }


          /* Display extra bits. */
          if (BitNumber > 32)
            uart_send(__LINE__, " [%2u]    --     %c     %5lu      %c     %5lu\r", Loop1UInt16, IrLevel[Loop1UInt16], IrResultValue[Loop1UInt16], IrLevel[Loop1UInt16 + 1], IrResultValue[Loop1UInt16 + 1]);
        }
        printf("\r\r");
      }

      if ((decode_ir_command(FLAG_OFF, &IrCommand) == 0) && (IrCommand != IR_LO_LIMIT) && (IrCommand < IR_HI_LIMIT))
      {
        if (DebugBitMask & DEBUG_IR_COMMAND)
          uart_send(__LINE__, "Going to process_ir_command(%u)\r", IrCommand);

        /* Process IR command only if it has been received / decoded without error. */
        IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.

        process_ir_command(IrCommand);
      }
      else
      {
        if (DebugBitMask & DEBUG_IR_COMMAND)
          uart_send(__LINE__, "Error in decoding IR command\r");
      }
    }
    #endif  // IR_SUPPORT



    /* Note: IdleMonitor[00] = Cumulative loops from 00 to 05 seconds
             IdleMonitor[01] = Cumulative loops from 05 to 10 seconds
             IdleMonitor[02] = Cumulative loops from 10 to 15 seconds
             IdleMonitor[03] = Cumulative loops from 15 to 20 seconds
             IdleMonitor[04] = Cumulative loops from 20 to 25 seconds
             IdleMonitor[05] = Cumulative loops from 25 to 30 seconds
             IdleMonitor[06] = Cumulative loops from 30 to 35 seconds
             IdleMonitor[07] = Cumulative loops from 35 to 40 seconds
             IdleMonitor[08] = Cumulative loops from 40 to 45 seconds
             IdleMonitor[09] = Cumulative loops from 45 to 50 seconds
             IdleMonitor[10] = Cumulative loops from 50 to 55 seconds
             IdleMonitor[11] = Cumulative loops from 55 to 00 seconds

             IdleMonitor[12] = Active count for current 5-seconds period.

             IdleMonitor[13] = Average number of idle loops per second for the last 60-seconds period. */

    /* Find what must be the target packet number given current value of CurrentSecond. */
    IdleMonitorPacket = CurrentSecond / 5;

    /* If we just crossed a "5-seconds" period change, make some housekeeping. */
    if (IdleMonitorPacket != LastIdleMonitorPacket)
    {
      IdleMonitor[LastIdleMonitorPacket] = IdleMonitor[12];
      IdleMonitor[12] = 0;

      LastIdleMonitorPacket = IdleMonitorPacket;

      if (DebugBitMask & DEBUG_IDLE_MONITOR)
        uart_send(__LINE__, "IdleMonitorPacket = %2u\r", IdleMonitorPacket);
    }

    /* One more system loop executed. */
    ++IdleMonitor[12];

    /* Re-evaluate System Idle Monitor every minute, at xxm35s. */
    if ((CurrentSecond == 35) && (FlagIdleMonitor == FLAG_OFF))
    {
      if (DebugBitMask & DEBUG_IDLE_MONITOR)
      {
        for (Loop1UInt8 = 0; Loop1UInt8 < 14; ++Loop1UInt8)
        {
          ArchiveIdleMonitor[Loop1UInt8][IdleArchivePacket] = IdleMonitor[Loop1UInt8];
        }
        ++IdleArchivePacket;
      }

      /* Calculate average System Idle Monitor for last 60-seconds period. */
      IdleMonitor[13] = 0;
      for (Loop1UInt8 = 0; Loop1UInt8 < 12; ++Loop1UInt8)
      {
        IdleMonitor[13] += IdleMonitor[Loop1UInt8];
      }

      IdleMonitor[13] = IdleMonitor[13] / 60;

      if (DebugBitMask & DEBUG_IDLE_MONITOR)
      {
        for (Loop1UInt8 = 0; Loop1UInt8 < 12; ++Loop1UInt8)
          uart_send(__LINE__, "IdleMonitor[%2.2u] = %8llu  (%2.2u to %2.2u))\r", Loop1UInt8, IdleMonitor[Loop1UInt8], (Loop1UInt8 * 5), ((Loop1UInt8 * 5) + 5));

        uart_send(__LINE__, "Current period: IdleMonitor[12] = %llu\r", IdleMonitor[12]);
        uart_send(__LINE__, "Resulting system idle monitor   = %llu\r\r\r", IdleMonitor[13]);
      }

      FlagIdleMonitor = FLAG_ON; // only one calculation per 5-seconds period.
    }

    /* Prepare IdleMonitor to be displayed again in one minute. */
    if ((CurrentSecond == 36) && (FlagIdleMonitor == FLAG_ON))
      FlagIdleMonitor = FLAG_OFF;

    if (DebugBitMask & DEBUG_IDLE_MONITOR)
    {
      if (IdleArchivePacket >= IdleTimeSamples)
      {
        for (IdleArchivePacket = 0; IdleArchivePacket < IdleTimeSamples; ++IdleArchivePacket)
        {
          for (Loop1UInt8 = 0; Loop1UInt8 < 12; ++Loop1UInt8)
            uart_send(__LINE__, "ArchiveIdleMonitor[%2.2u][%3.3u] = %8llu)\r", Loop1UInt8, IdleArchivePacket, ArchiveIdleMonitor[Loop1UInt8][IdleArchivePacket]);

          uart_send(__LINE__, "\r");
          uart_send(__LINE__, "ArchiveIdleMonitor[12][%3.3u] = %8llu (current period)\r\r", IdleArchivePacket, ArchiveIdleMonitor[12][IdleArchivePacket]);
          uart_send(__LINE__, "System idle monitor   [%3.3u] = %8llu\r\r\r", IdleArchivePacket, ArchiveIdleMonitor[13][IdleArchivePacket]);
        }
        IdleArchivePacket = 0;
      }
    }
  }
}





/* $PAGE */
/* $TITLE=adc_read_light() */
/* ------------------------------------------------------------------ *\
      Read ambient light level from Pico's analog-to-digital gpio.
\* ------------------------------------------------------------------ */
UINT16 adc_read_light(void)
{
  static UINT16 Count;
  UINT16 LightValue;


  /* Notes:
     ADC 0 (gpio 26)  is for photo-resistor (ambient light level).
     ADC 1 (gpio 27)  not used
     ADC 2 (gpio 28)  not used
     ADC 3 (gpio 29)  is for power supply voltage.
     ADC 4 (internal) is internally connected to read Pico's temperature. */
  adc_select_input(0);


  /* Read gpio 26. */
  LightValue = adc_read();
  if (DebugBitMask & DEBUG_BRIGHTNESS)
  {
    ++Count;
    if (Count == 2500)
    {
      /* Print an update every 2.5 seconds. */
      uart_send(__LINE__, "Raw reading adc_read_light(): %u   (4095 - %u) = %u\r", LightValue, LightValue, 4095 - LightValue);
      Count = 0;
    }
  }


  /* The way the photoresistor is installed in the circuit: the higher the number, the darker it is, which is counter-intuitive...
     ADC precision on the Pico is 12 bits (0 to 4095). Let's inverse the value so that an higher value means more light... */
  LightValue = 4095 - LightValue;

  return LightValue;
}





/* $PAGE */
/* $TITLE=adc_read_pico_temp() */
/* -------------------------------------------------------------------- *\
   Read Pico's internal temperature from Pico's analog-to-digital gpio.
\* -------------------------------------------------------------------- */
void adc_read_pico_temp(float *DegreeC, float *DegreeF)
{
  UINT8 Dum1UInt8;

  UINT16 AdcRawValue;

  float AdcVolts;


  /* Make Pico's internal temperature readable from ADC. */
  adc_set_temp_sensor_enabled(true);

  /* Notes:
     ADC 0 (gpio 26)  is for photo-resistor (ambient light level).
     ADC 1 (gpio 27)  not used
     ADC 2 (gpio 28)  not used
     ADC 3 (gpio 29)  is for power supply voltage.
     ADC 4 (internal) is internally connected to read Pico's temperature. */
  adc_select_input(4);


  /* Get ADC raw value. */
  AdcRawValue = adc_read();


  /* Convert ADC raw value to volts. Reference voltage (3.28 volts - actual reading on the pin) should be measured if temperature seems erroneous. */
  AdcVolts = (AdcRawValue * 3.28f) / 4096;


  /* Return both Celsius and Fahrenheit values. Evaluate Pico internal temperature from voltage value.
     NOTE: 27 degree C gives a voltage of 0.706 volt and each additional degree C reduces the voltage by 0.001721 volt (1.721 mV). */
  /* Celsius value. */
  if (DegreeC != (float *)0x00)
    *DegreeC = (27 - ((AdcVolts - 0.706) / 0.001721));

  /* Fahrenheit value. */
  if (DegreeF != (float *)0x00)
    *DegreeF = (((27 - ((AdcVolts - 0.706) / 0.001721)) * 9 / 5) + 32);

  return;
}





/* $PAGE */
/* $TITLE=adc_read_voltage() */
/* ------------------------------------------------------------------ *\
      Read power supply voltage from Pico's analog-to-digital gpio.
\* ------------------------------------------------------------------ */
float adc_read_voltage(void)
{
  UINT16 AdcValue;

  float Volts;

  /*** Strangely, voltage reading doesn't seem to work on the Pico W as it does on the Pico... ***/
  /*** I'll have to investigate this later... ***/

  /* Notes:
     ADC 0 (gpio 26)  is for photo-resistor (ambient light level).
     ADC 1 (gpio 27)  not used
     ADC 2 (gpio 28)  not used
     ADC 3 (gpio 29)  is for power supply voltage.
     ADC 4 (internal) is internally connected to read Pico's temperature. */
  adc_select_input(3);

  /* For Pico W, it is important that GPIO 25 be high to read the power supply voltage. */
  if (PicoType == TYPE_PICO_W) gpio_put(PICO_LED, 1);

  /* Read ADC converter raw value. */
  AdcValue = adc_read();

  /* Convert raw value to voltage value. */
  Volts = AdcValue * (3.3 / (1 << 12));

  if (DebugBitMask & DEBUG_PICO_W)
  {
    uart_send(__LINE__, "Reading when level high: %u\r", AdcValue);
    uart_send(__LINE__, "Converted to volts: %2.2f\r", Volts);
  }

  /* Reset output to low level. */
  if (PicoType == TYPE_PICO_W) gpio_put(PICO_LED, 0);

  /* Read ADC converter raw value. */
  AdcValue = adc_read();

  /* Convert raw value to voltage value. */
  Volts = AdcValue * (3.3 / (1 << 12));

  if (DebugBitMask & DEBUG_PICO_W)
  {
    uart_send(__LINE__, "Reading when level low: %u\r", AdcValue);
    uart_send(__LINE__, "Converted to volts: %2.2f\r", Volts);
  }

  return Volts;
}





/* $PAGE */
/* $TITLE=adjust_clock_brightness() */
/* --------------------------------------------------------------------- *\
    Read ambient light level from Pico's analog-to-digital gpio, where
                   a photo resistor has been connected
            (inside the Green Clock, above the USB connector)
         and then, adjust clock display brightness according to
     average ambient light level for the last 60 seconds (hysteresis).
    NOTE: Clock must be setup for auto-brightness. Refer to User Guide
\* --------------------------------------------------------------------- */
void adjust_clock_brightness(void)
{
  UCHAR String[128];

  UINT Loop1UInt;
  static UINT NextCell = 0;  // point to next slot of circular buffer to be read.

  UINT16 current_adc_light;
  UINT16 AverageLevel;
  UINT16 Cycles;

  static UINT16 AmbientLightMSecCounter;
  static UINT16 LightLevel[MAX_LIGHT_SLOTS] = {550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550};  // assume average ambient light level on entry.

  int32_t TempLevel;

  static UINT64 CumulativeLightLevel;


  /* If the clock has been setup for auto-brightness, get ambient light level from Pico's analog-to-digital converter.
     (if clock is not in auto-brightness mode, an "average light level value" of 550 will be kept as default until auto-brightness is selected). */
  if (FlashConfig.FlagAutoBrightness == FLAG_ON)
  {
    /* Cumulate 5000 readings of the ambient light value (5 seconds of readings every msec.). */
    ++AmbientLightMSecCounter;
    current_adc_light = adc_read_light();
    CumulativeLightLevel += current_adc_light;


    /* Check if 5000 milliseconds (5 seconds) have elapsed. */
    if (AmbientLightMSecCounter >= 5000)
    {
      /* Reset the 5000 milliseconds counter. */
      AmbientLightMSecCounter = 0;

      /* Calculate the average ambient light level for the last five seconds (5000 milliseconds). */
      LightLevel[NextCell++] = CumulativeLightLevel / 5000;  // average light level for the last 5 seconds.
      if (NextCell >= MAX_LIGHT_SLOTS) NextCell = 0;         // replace the slot for the last 5-seconds period and when out-of-bound, revert to zero.
      CumulativeLightLevel   = 0;


      /* Calculate average ambient light level for the last minute, based on the "MAX_LIGHT_SLOTS" number of "5-seconds" light level slots. */
      for (Loop1UInt = 0; Loop1UInt < MAX_LIGHT_SLOTS; ++Loop1UInt)
        CumulativeLightLevel += LightLevel[Loop1UInt];


      AverageLightLevel    = (CumulativeLightLevel / MAX_LIGHT_SLOTS);  // average light level for the last "MAX_LIGHT_SLOTS" number of "5-seconds" light level slots.
      CumulativeLightLevel = 0;  // get ready to cumulate the next 5000 milliseconds.


      /* Update current clock display brightness, based on average ambient light level for the last 2 minutes (hysteresis).
         Ambient light level goes from around 200 (very dark) to 1500 (very bright). */
      AverageLevel = AverageLightLevel;

      if (AverageLightLevel < FlashConfig.DimmerMinLightLevel)
        AverageLevel = FlashConfig.DimmerMinLightLevel;
      if (AverageLightLevel > (FlashConfig.DimmerMinLightLevel + BRIGHTNESS_LIGHTLEVELRANGE))
        AverageLevel = (FlashConfig.DimmerMinLightLevel + BRIGHTNESS_LIGHTLEVELRANGE);
      Cycles = (UINT16)((AverageLevel - FlashConfig.DimmerMinLightLevel) / (BRIGHTNESS_LIGHTLEVELRANGE / (BRIGHTNESS_LIGHTLEVELSTEP * 1.0)));
      pwm_set_cycles(Cycles);

      Dim_AverageLightLevel = AverageLightLevel;
      Dim_DutyCycle = (1000 - Pwm[PWM_BRIGHTNESS].Level);
      if (AverageLightLevel < Dim_Min_avgadc_value)
        Dim_Min_avgadc_value = AverageLightLevel;
      if (AverageLightLevel > Dim_Max_avgadc_value)
        Dim_Max_avgadc_value = AverageLightLevel;

      if (DebugBitMask & DEBUG_BRIGHTNESS)
      {
        uart_send(__LINE__, "Instant level: %4u   Av1: %4u   Av2: %4u   (Av2 - 230): %3u\r", adc_read_light(), AverageLightLevel, AverageLevel, (AverageLevel - 230));
        uart_send(__LINE__, "((Av2 - 230) / 500.0): %3.2f   (UINT16)((Av2 - 230) / 500.0): %3u   PWM cycles: %3u\r\r", ((AverageLevel - 230) / 500.0), (UINT16)((AverageLevel - 230) / 500.0), Cycles);
      }
    }
  }
  else
  {
    Dim_DutyCycle = (1000 - Pwm[PWM_BRIGHTNESS].Level);
  }
  return;
}





#ifdef BME280_SUPPORT
/* $PAGE */
/* $TITLE=bme280_compute_calib_param() */
/* ------------------------------------------------------------------ *\
                    Compute calibration parameters
        from calibration data read in device non volatile memory.
\* ------------------------------------------------------------------ */
UINT8 bme280_compute_calib_param()
{
  UCHAR String[256];

  UINT16 Dum1UInt16;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_compute_calib_param(Entering)\r\r");


  /* Compute calibration parameters for temperature. */
  Bme280Data.DigT1 =  (UINT16)(Bme280Data.Bme280CalibData[0] + (Bme280Data.Bme280CalibData[1] << 8));
  Bme280Data.DigT2 = (int16_t)(Bme280Data.Bme280CalibData[2] + (Bme280Data.Bme280CalibData[3] << 8));
  Bme280Data.DigT3 = (int16_t)(Bme280Data.Bme280CalibData[4] + (Bme280Data.Bme280CalibData[5] << 8));

  /* Compute calibration parameters for pressure. */
  Bme280Data.DigP1 =   (UINT16)(Bme280Data.Bme280CalibData[6] + (Bme280Data.Bme280CalibData[7] << 8));
  Bme280Data.DigP2 = (int16_t)((Bme280Data.Bme280CalibData[8] + (Bme280Data.Bme280CalibData[9] << 8)) & 0xFFFF);

  Dum1UInt16 = (((Bme280Data.Bme280CalibData[11] << 8) & 0xFF00) + Bme280Data.Bme280CalibData[10]);
  Bme280Data.DigP3 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280Data.Bme280CalibData[13] << 8) & 0xFF00) + Bme280Data.Bme280CalibData[12]);
  Bme280Data.DigP4 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280Data.Bme280CalibData[15] << 8) & 0xFF00) + Bme280Data.Bme280CalibData[14]);
  Bme280Data.DigP5 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280Data.Bme280CalibData[17] << 8) & 0xFF00) + Bme280Data.Bme280CalibData[16]);
  Bme280Data.DigP6 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280Data.Bme280CalibData[19] << 8) & 0xFF00) + Bme280Data.Bme280CalibData[18]);
  Bme280Data.DigP7 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280Data.Bme280CalibData[21] << 8) & 0xFF00) + Bme280Data.Bme280CalibData[20]);
  Bme280Data.DigP8 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280Data.Bme280CalibData[23] << 8) & 0xFF00) + Bme280Data.Bme280CalibData[22]);
  Bme280Data.DigP9 = (int16_t)Dum1UInt16;

  /* Compute calibration parameters for humidity. */
  Bme280Data.DigH1 =     (UCHAR)Bme280Data.Bme280CalibData[25];
  Bme280Data.DigH2 =  (int16_t)(Bme280Data.Bme280CalibData[26] + (Bme280Data.Bme280CalibData[27] << 8));
  Bme280Data.DigH3 =     (UCHAR)Bme280Data.Bme280CalibData[28];
  Bme280Data.DigH4 = (int16_t)((Bme280Data.Bme280CalibData[29] << 4) +  (Bme280Data.Bme280CalibData[30] & 0x0F));
  Bme280Data.DigH5 = (int16_t)((Bme280Data.Bme280CalibData[31] << 4) + ((Bme280Data.Bme280CalibData[30] & 0xF0) >> 4));
  Bme280Data.DigH6 =      (char)Bme280Data.Bme280CalibData[32];

  if (DebugBitMask & DEBUG_BME280)
  {
    uart_send(__LINE__, "Temperature:\r");
    uart_send(__LINE__, "DigT1: 0x%8.8X   %7u (UINT16)\r",    Bme280Data.DigT1, Bme280Data.DigT1);
    uart_send(__LINE__, "DigT2: 0x%8.8X   %7d (INT16)\r",     Bme280Data.DigT2, Bme280Data.DigT2);
    uart_send(__LINE__, "DigT3: 0x%8.8X   %7d (INT16)\r\r\r", Bme280Data.DigT3, Bme280Data.DigT3);

    uart_send(__LINE__, "Pressure:\r");
    uart_send(__LINE__, "DigP1: 0x%8.8X   %7u (UINT16)\r",    Bme280Data.DigP1,           Bme280Data.DigP1);
    uart_send(__LINE__, "DigP2: 0x%8.8X   %7d (INT16)\r",    (Bme280Data.DigP2 & 0xFFFF), Bme280Data.DigP2);
    uart_send(__LINE__, "DigP3: 0x%8.8X   %7d (INT16)\r",     Bme280Data.DigP3,           Bme280Data.DigP3);
    uart_send(__LINE__, "DigP4: 0x%8.8X   %7d (INT16)\r",     Bme280Data.DigP4,           Bme280Data.DigP4);
    uart_send(__LINE__, "DigP5: 0x%8.8X   %7d (INT16)\r",     Bme280Data.DigP5,           Bme280Data.DigP5);
    uart_send(__LINE__, "DigP6: 0x%8.8X   %7d (INT16)\r",    (Bme280Data.DigP6 & 0xFFFF), Bme280Data.DigP6);
    uart_send(__LINE__, "DigP7: 0x%8.8X   %7d (INT16)\r",     Bme280Data.DigP7,           Bme280Data.DigP7);
    uart_send(__LINE__, "DigP8: 0x%8.8X   %7d (INT16)\r",    (Bme280Data.DigP8 & 0xFFFF), Bme280Data.DigP8);
    uart_send(__LINE__, "DigP9: 0x%8.8X   %7d (INT16)\r\r\r", Bme280Data.DigP9,           Bme280Data.DigP9);

    uart_send(__LINE__, "Humidity:\r");
    uart_send(__LINE__, "DigH1: 0x%8.8X   %7u (UCHAR)\r",      Bme280Data.DigH1, Bme280Data.DigH1);
    uart_send(__LINE__, "DigH2: 0x%8.8X   %7d (INT16)\r",      Bme280Data.DigH2, Bme280Data.DigH2);
    uart_send(__LINE__, "DigH3: 0x%8.8X   %7u (UCHAR)\r",      Bme280Data.DigH3, Bme280Data.DigH3);
    uart_send(__LINE__, "DigH4: 0x%8.8X   %7d (INT16)\r",      Bme280Data.DigH4, Bme280Data.DigH4);
    uart_send(__LINE__, "DigH5: 0x%8.8X   %7d (INT16)\r",      Bme280Data.DigH5, Bme280Data.DigH5);
    uart_send(__LINE__, "DigH6: 0x%8.8X   %7d (CHAR)\r\r\r\r", Bme280Data.DigH6, Bme280Data.DigH6);
  }

  return 0;
}





/* $PAGE */
/* $TITLE=bme280_get_temp() */
/* ------------------------------------------------------------------ *\
     Read temperature, humidity and barometric pressure from BME280.
     NOTE: Altitude and humidex values are not trusted for now.
\* ------------------------------------------------------------------ */
UINT8 bme280_get_temp(void)
{
  UCHAR String[64];

  UINT16 Dum1UInt16;
  int16_t Dum1Int16;
  UINT8 HumParam;
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 Buffer[8];

  UINT32 ReturnCode;
  UINT32 ReturnCode1;
  UINT32 ReturnCode2;

  int32_t Dum1Int32;
  int32_t Dum2Int32;
  int32_t FinalHum;
  int32_t RawTemp;
  int32_t RawHum;
  int32_t RawPress;
  int32_t TempFactor;

  int64_t Dum1Int64;
  int64_t Dum2Int64;
  int64_t FinalPress;

  float Alpha;
  float Dum1Float;
  float DewPoint;


  /* One more temperature reading from BME280. */
  ++Bme280Data.Bme280ReadCycles;

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_get_temp(Entering)\r\r");


  /* -------------------- Force a BME280 reading -------------------- */
  /* --- and oversampling for temperature and barometric pressure --- */
  /* Bits 1 and 0: BME280 sensor mode.  00 = sleep mode; 01, 10 = forced mode; 11 = normal mode.
     Bits 4, 3, 2: oversampling for barometric pressure data.
     Bits 7, 6, 5: oversampling for temperature data.
     Note: 0b101 = oversampling X 16; 0b100 = oversampling X 8; 0b011 = oversampling X 4;
           0b010 = oversampling X 2;  0b001 = oversampling X 1; 0b000 = skip oversampling. */
  Buffer[0] = BME280_REGISTER_CTRL_MEAS;
  Buffer[1] = 0b00100000   // temperature oversampling X 1
            + 0b00000100   // pressure oversampling X 1
            + 0b00000001;  // BME280 in "force" mode

  /* Send address of the BME280's ctrl_meas, while checking for an eventual error code. */
  Buffer[1] = 0x24; // sleep mode
  ReturnCode = i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, true);
  sleep_ms(300);
  Buffer[1] = 0x25; // force mode
  ReturnCode = i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, true);
  sleep_ms(300);
  Buffer[1] = 0x24; // sleep mode
  ReturnCode = i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, true);
  sleep_ms(300);
  Buffer[1] = 0x25; // force mode
  ReturnCode = i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, true);
  sleep_ms(300);
  if (ReturnCode == PICO_ERROR_GENERIC)
  {
    ++Bme280Data.Bme280Errors;
    return 0xFF;
  }

  /* Next, read temperature, humidity and barometric pressure raw data.
     Give some time to the device to complete operation
     (for some reason, the "status" register doesn't seem to indicate a pending operation) . */
  bme280_read_registers(Buffer);

  /* Compute raw values. */
  RawPress = ((Buffer[0] << 12) + (Buffer[1] << 4) + ((Buffer[2] & 0xF0) >> 4));
  RawTemp  = ((Buffer[3] << 12) + (Buffer[4] << 4) + ((Buffer[5] & 0xF0) >> 4));
  RawHum   = ((Buffer[6] <<  8) +  Buffer[7]);

  if (DebugBitMask & DEBUG_BME280)
  {
    uart_send(__LINE__, "RawPress: %5.5X      ", RawPress);
    uart_send(__LINE__, "- RawTemp:  %5.5X      ", RawTemp);
    uart_send(__LINE__, "- RawHum:   %5.4X\r\r\r", RawHum);
  }

  /* Compute actual temperature. */
  Dum1Int32 = ((((RawTemp >> 3)  - ((int32_t)Bme280Data.DigT1 << 1))) * ((int32_t)Bme280Data.DigT2)) >> 11;
  Dum2Int32 = (((((RawTemp >> 4) - ((int32_t)Bme280Data.DigT1)) * ((RawTemp >> 4) - ((int32_t)Bme280Data.DigT1))) >> 12) * ((int32_t)Bme280Data.DigT3)) >> 14;
  TempFactor = Dum1Int32 + Dum2Int32;
  Bme280Data.TemperatureC = ((TempFactor * 5 + 128) >> 8) / 100.0;
  Bme280Data.TemperatureF = ((Bme280Data.TemperatureC * 9) / 5.0) + 32;
  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "Temperature:    %3.3f'C   %3.3f'F\r\r\r", Bme280Data.TemperatureC, Bme280Data.TemperatureF);


  /* Compute actual relative humidity. */
  FinalHum = (TempFactor - ((int32_t)76800));
  FinalHum = (((((RawHum << 14) - (((int32_t)Bme280Data.DigH4) << 20) - (((int32_t)Bme280Data.DigH5) * FinalHum)) +
                ((int32_t)16384)) >> 15) *
              (((((((FinalHum * ((int32_t)Bme280Data.DigH6)) >> 10) *
                  (((FinalHum * ((int32_t)Bme280Data.DigH3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                   ((int32_t)Bme280Data.DigH2) + 8192) >> 14));
  FinalHum = (FinalHum - (((((FinalHum >> 15) * (FinalHum >> 15)) >> 7) * ((int32_t)Bme280Data.DigH1)) >> 4));
  FinalHum = (FinalHum < 0 ? 0 : FinalHum);
  FinalHum = (FinalHum > 419430400 ? 419430400 : FinalHum);
  FinalHum = (int32_t)(FinalHum >> 12);
  Bme280Data.Humidity = FinalHum / 1024.0;
  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "Humidity:       %2.3f%%\r\r\r", Bme280Data.Humidity);

  /* Compute actual barometric pressure. */
  Dum1Int64 = ((int64_t)TempFactor) - 128000;
  Dum2Int64 = Dum1Int64 * Dum1Int64 * (int64_t)Bme280Data.DigP6;
  Dum2Int64 = Dum2Int64 + ((Dum1Int64 * (int64_t)Bme280Data.DigP5) << 17);
  Dum2Int64 = Dum2Int64 + (((int64_t)Bme280Data.DigP4) << 35);
  Dum1Int64 = ((Dum1Int64 * Dum1Int64 * (int64_t)Bme280Data.DigP3) >> 8) + ((Dum1Int64 * (int64_t)Bme280Data.DigP2) << 12);
  Dum1Int64 = (((((int64_t)1) << 47) + Dum1Int64)) * ((int64_t)Bme280Data.DigP1) >> 33;
  if (Dum1Int64 == 0)
    return 0;  // to prevent a division by zero.

  FinalPress = 1048576 - RawPress;
  FinalPress = (((FinalPress << 31) - Dum2Int64) * 3125) / Dum1Int64;
  Dum1Int64 = (((int64_t)Bme280Data.DigP9) * (FinalPress >> 13) * (FinalPress >> 13)) >> 25;
  Dum2Int64 = (((int64_t)Bme280Data.DigP8) * FinalPress) >> 19;
  FinalPress = (UINT32)((FinalPress + Dum1Int64 + Dum2Int64) >> 8) + (((int64_t)Bme280Data.DigP7) << 4);
  Bme280Data.Pressure = FinalPress / 25600.0;
  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "Pressure:       %4.3f hPa\r\r\r", Bme280Data.Pressure);

  /* NOTE: Relative altitude may not be an interesting value to calculate since this value changes with
           normal and usual pressure changes due to atmospheric factors. */
  /*** Compute approximate altitude above sea level, based on atmospheric pressure and current temperature.
     NOTE: 1013.25 hPa is the standard pressure at sea level. ***
  Bme280Data.Altitude = 44330.0 * (1.0 - pow(Bme280Data.Pressure / 1013.25, 0.190294957));

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "Altitude:       %4.3f - seems to vary too much with pressure change over time\r\r\r", Bme280Data.Altitude);
  ***/

  /* NOTE: Equation below to calculate the humidex factor is wrong. References about how to calculate this parameter are hard to
             find and not always accurate. This part will need to be reworked when trustful documentation is found on the subject. */
  /* Compute humidex factor, based on current temperature, relative humidity and atmospheric pressure. */
  /* Humidex is not valid if temperature is below 0'C. */
  /*** Part requiring rework... ***
  if (Bme280Data.TemperatureC < 0)
    Bme280Data.Humidex = 0;

  Alpha = ((17.27 * Bme280Data.TemperatureC) / (237.7 + Bme280Data.TemperatureC)) + log(Bme280Data.Humidity / 100);
  DewPoint = (237.7 * Alpha) / (17.27 - Alpha);

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "Dew point:      %4.3f - need to be confirmed\r\r\r", DewPoint);

  Bme280Data.Humidex = (TempC + (0.5555 * (Bme280Data.Pressure - 10.0)));

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "Humidex:        %4.3f - current equation is incorrect\r\r\r\r\r", Bme280Data.Humidex);
  ***/

  return 0;
}





/* $PAGE */
/* $TITLE=bme280_init() */
/* ------------------------------------------------------------------ *\
                     Initialize the BME280 sensor.
\* ------------------------------------------------------------------ */
UINT8 bme280_init(void)
{
  UCHAR String[64];

  UINT8 Buffer[2];

  UINT32 ReturnCode;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_init(Entering)\r\r\r");


  /* ---------------------- Reset the BME280. ----------------------- */
  Buffer[0] = BME280_REGISTER_RESET;
  Buffer[1] = 0xB6; // reset command.

  /* Send the soft reset command to the BME280's while checking for an eventual error code. */
  if (i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, true) == PICO_ERROR_GENERIC)
    return 0xFF;

  /* --------- Configure oversampling for humidity reading. --------- */
  /* Note: 0b101 = oversampling X 16; 0b100 = oversampling X 8; 0b011 = oversampling X 4;
           0b010 = oversampling X 2;  0b001 = oversampling X 1; 0b000 = skip oversampling. */
  Buffer[0] = BME280_REGISTER_CTRL_HUM;
  Buffer[1] = 0b00000001;

  /* Send the humidity oversampling configuration to the BME280's while checking for an eventual error code. */
  if (i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, true) == PICO_ERROR_GENERIC)
    return 0xFF;

  /* ----------------- Configure BME280 sensor mode ----------------- */
  /* --- and oversampling for temperature and barometric pressure --- */
  /* Bits 1 and 0: BME280 sensor mode.  00 = sleep mode; 01, 10 = forced mode; 11 = normal mode.
     Bits 4, 3, 2: oversampling for barometric pressure data.
     Bits 7, 6, 5: oversampling for temperature data.
     Note: 0b101 = oversampling X 16; 0b100 = oversampling X 8; 0b011 = oversampling X 4;
           0b010 = oversampling X 2;  0b001 = oversampling X 1; 0b000 = skip oversampling. */
  Buffer[0] = BME280_REGISTER_CTRL_MEAS;
  Buffer[1] = 0b00100000 + 0b00000100 + 0x00000000;

  /* Send temperature and pressure oversampling configuration to the BME280's, along with "sleep mode". */
  if (i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, true) == PICO_ERROR_GENERIC)
    return 0xFF;

  /* ---------------------- Set configuration ---------------------- */
  /* Bit 0: 0 for I2C protocol; 1 for SPI protocol.
     Bit 1: not used
     Bits 2, 3, 4: Time constant of IIR filter (010 = filter coefficient: 4).
     Bits 5, 6, 7: Determine inactive time duration in "normal" mode. (100 = 500 msec). */
  Buffer[0] = BME280_REGISTER_CONFIG;
  Buffer[1] = 0b10001000;

  /* Send configuration parameters to the BME280's config register. */
  if (i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, false) == PICO_ERROR_GENERIC)
    return 0xFF;

  return 0;
}





/* $PAGE */
/* $TITLE=bme280_read_all_registers() */
/* ------------------------------------------------------------------ *\
                  Read all BME280 readable registers
                except calibration data and device ID,
            which are supported by other specific functions.
\* ------------------------------------------------------------------ */
UINT8 bme280_read_all_registers()
{
  UCHAR Buffer[13];
  UCHAR String[64];

  UINT8 Loop1UInt8;
  UINT32 ReturnCode;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_read_all_registers(Entering)    ");


  /* Give adress of the first BME280 register to read (lowest address). */
  Buffer[0] = BME280_REGISTER_CTRL_HUM;

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 1, true);
  ReturnCode = i2c_read_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 13, false);
  if (ReturnCode == PICO_ERROR_GENERIC)
    return 0xFF;

  if (DebugBitMask & DEBUG_BME280)
  {
    /* Validate return code, to make sure register read was successful. */
    uart_send(__LINE__, "Read(%u)\r\r", ReturnCode);

    /* Display ctrl_hum register content. */
    uart_send(__LINE__, "ctrl_hum:   %2.2X\r", Buffer[0]);

    /* Display status register content. */
    uart_send(__LINE__, "status:     %2.2X\r", Buffer[1]);

    /* Display ctrl_meas register content. */
    uart_send(__LINE__, "ctrl_meas:  %2.2X\r", Buffer[2]);

    /* Display config register content. */
    uart_send(__LINE__, "config:     %2.2X\r", Buffer[3]);

    /* Display unknown register content. */
    uart_send(__LINE__, "unknown:    %2.2X\r", Buffer[4]);

    /* Display press_msb register content. */
    uart_send(__LINE__, "press_msb:  %2.2X\r", Buffer[5]);

    /* Display press_lsb register content. */
    uart_send(__LINE__, "press_lsb:  %2.2X\r", Buffer[6]);

    /* Display press_xlsb register content. */
    uart_send(__LINE__, "press_xlsb: %2.2X\r", Buffer[7]);

    /* Display temp_msb register content. */
    uart_send(__LINE__, "temp_msb:   %2.2X\r", Buffer[8]);

    /* Display temp_lsb register content. */
    uart_send(__LINE__, "temp_lsb:   %2.2X\r", Buffer[9]);

    /* Display temp_xlsb register content. */
    uart_send(__LINE__, "temp_xlsb:  %2.2X\r", Buffer[10]);

    /* Display hum_msb register content. */
    uart_send(__LINE__, "hum_msb:    %2.2X\r", Buffer[11]);

    /* Display hum_lsb register content. */
    uart_send(__LINE__, "hum_lsb:    %2.2X\r\r\r\r\r", Buffer[12]);
  }

  return 0;
}





/* $PAGE */
/* $TITLE=bme280_read_calib_data() */
/* ------------------------------------------------------------------ *\
       Read BME280 calibration data for the specific device used.
\* ------------------------------------------------------------------ */
UINT8 bme280_read_calib_data()
{
  UCHAR Buffer;
  UCHAR String[64];

  UINT8 Loop1UInt8;
  UINT32 ReturnCode;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_read_calib_data(Entering)\r\r");


  /* Read first chunk of BME280 calibration data. */
  Buffer = BME280_REGISTER_CALIB00;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, Bme280Data.Bme280CalibData, 26, false, 10000000);

  /* Read second chunk of BME280 calibration data. */
  Buffer = BME280_REGISTER_CALIB26;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, &Bme280Data.Bme280CalibData[26], 16, false, 10000000);

  if (DebugBitMask & DEBUG_BME280)
  {
    /* Display calibration data to optional monitor connected via UART. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 26; ++Loop1UInt8)
      uart_send(__LINE__, "Calib[%2.2u]  Address: 0x%2.2X = %2.2X\r", Loop1UInt8, (Loop1UInt8 + 0x88), Bme280Data.Bme280CalibData[Loop1UInt8]);

    for (Loop1UInt8 = 0; Loop1UInt8 < 16; ++Loop1UInt8)
      uart_send(__LINE__, "Calib[%2.2u]  Address: 0x%2.2X = %2.2X\r", (Loop1UInt8 + 26), (0xE1 + Loop1UInt8), Bme280Data.Bme280CalibData[Loop1UInt8 + 26]);

    uart_send(__LINE__, "\r\r");
  }

  return 0;
}





/* $PAGE */
/* $TITLE=bme280_read_config() */
/* ------------------------------------------------------------------ *\
                   Read BME280 configuration register.
\* ------------------------------------------------------------------ */
UINT8 bme280_read_config(void)
{
  UCHAR String[64];

  UINT8 Buffer;
  UINT32 ReturnCode;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_read_config(Entering)           ");


  Buffer = BME280_REGISTER_CONFIG;

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true);
  i2c_read_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, false);

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "Config: 0x%2.2X\r\r", Buffer);

  return Buffer;
}





/* $PAGE */
/* $TITLE=bme280_read_device_id() */
/* ------------------------------------------------------------------ *\
                        Read BME280 device ID.
                Must be 0x60 for a true Bosch BME280,
                 or 0x56 and 0x57 for BMP280 samples
              and 0x58 for BMP280 mass production units.
\* ------------------------------------------------------------------ */
UINT8 bme280_read_device_id(void)
{
  UCHAR String[64];

  UINT8 Buffer;
  UINT32 ReturnCode;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_read_device_id(Entering)         ");


  Buffer = BME280_REGISTER_ID;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, false, 10000000);

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "- Read(%u)    0x%2.2X\r\r\r", ReturnCode, Buffer);

  return Buffer;
}





/* $PAGE */
/* $TITLE=bme280_read_registers() */
/* ------------------------------------------------------------------ *\
            Read BME280 registers containing temperature,
                  humidity and barometric pressure.
\* ------------------------------------------------------------------ */
UINT8 bme280_read_registers(UINT8 *Buffer)
{
  UCHAR String[64];

  UINT8 Loop1UInt8;
  UINT32 ReturnCode;


  /* Give address of the first BME280 register to read (lowest address). */
  Buffer[0] = BME280_REGISTER_PRESS_MSB;

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_read_registers(Entering)  %2.2X\r\r", Buffer[0]);

  Loop1UInt8 = 0;
  while (bme280_read_status() != 0)
    ++Loop1UInt8;

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 1, true);
  i2c_read_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 8, false);

  if (DebugBitMask & DEBUG_BME280)
  {
    /* Display registers read from BME280. */
    uart_send(__LINE__, "Registers:       (wait: %4u)          ", Loop1UInt8);
    for (Loop1UInt8 = 0; Loop1UInt8 < 8; ++Loop1UInt8)
      uart_send(__LINE__, "- %2.2X ", Buffer[Loop1UInt8]);
    uart_send(__LINE__, "\r\r");
  }

  return 0;
}





/* $PAGE */
/* $TITLE=bme280_read_status() */
/* ------------------------------------------------------------------ *\
                      Read BME280 current status.
            (Should be 0x00 when no operation is pending).
\* ------------------------------------------------------------------ */
UINT8 bme280_read_status(void)
{
  UCHAR String[64];

  UINT8 Buffer;
  UINT32 ReturnCode;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_read_status(Entering)           ");


  Buffer = BME280_REGISTER_STATUS;

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true);
  i2c_read_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, false);

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "- Status: 0x%2.2X\r\r", Buffer);

  /* Bits 0 and 3 indicate that bme280 operation is on-going. */
  return (Buffer & 0x9);
}





/* $PAGE */
/* $TITLE=bme280_read_unique_id() */
/* ------------------------------------------------------------------ *\
   Read BME280 unique id ("serial number" of the specific device used.
\* ------------------------------------------------------------------ */
UINT32 bme280_read_unique_id()
{
  UCHAR String[64];

  UINT8 Buffer[4];
  UINT32 ReturnCode;
  UINT32 UniqueId;


  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "bme280_read_unique_id(Entering)         ");


  /* Give adress of the first BME280 register to read (lowest address). */
  Buffer[0] = BME280_REGISTER_UNIQUE_ID;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, Buffer, 4, false, 10000000);
  if (ReturnCode == PICO_ERROR_GENERIC)
    return 0xFF;

  UniqueId = ((((uint32_t)Buffer[3] + ((uint32_t)Buffer[2] << 8)) & 0x7FFFF) << 16) +
             (((uint32_t)Buffer[1]) << 8) + (uint32_t)Buffer[0];

  if (DebugBitMask & DEBUG_BME280)
    uart_send(__LINE__, "- Read(%u)    Hex: %4.4X %4.4X\r\r\r", ReturnCode, ((UniqueId & 0xFFFF0000) >> 16), UniqueId & 0xFFFF);

  return UniqueId;
}
#endif  // BME280_SUPPORT





/* $PAGE */
/* $TITLE=clear_all_leds() */
/* ------------------------------------------------------------------ *\
                  Clear all the leds on clock display.
          This is useful before reading DHT22 and / or before
      programming Pico's flash memory, since we disable interrupts
             when doing so and this would make glitches
                      on clock display otherwise.
\* ------------------------------------------------------------------ */
void clear_all_leds(void)
{
  UINT8 Loop1UInt8;


  for (Loop1UInt8 = 0; Loop1UInt8 < 24; ++Loop1UInt8)
    DisplayBuffer[Loop1UInt8] = 0x00;

  return;
}





/* $PAGE */
/* $TITLE=clear_framebuffer() */
/* ------------------------------------------------------------------ *\
                 Clear the clock virtual framebuffer
                starting at the given column position.
           If the start column is inside the "real" display,
               then the closk display will be cleared.
            NOTE: Top and left indicators are not cleared.
\* ------------------------------------------------------------------ */
void clear_framebuffer(UINT8 StartColumn)
{
  do
  {
    fill_display_buffer_4X7(StartColumn, ' ');
    StartColumn += 8;
  } while (StartColumn < sizeof(DisplayBuffer));

  return;
}





/* $PAGE */
/* $TITLE=command_queue() */
/* ------------------------------------------------------------------ *\
        Queue a command to be processed while in main context.
              This allows triggering an "heavy" process
                while in interrupt (callback) context.
\* ------------------------------------------------------------------ */
UINT8 command_queue(UINT8 Command, UINT16 Parameter)
{
  UCHAR String[256];


  /* Check if the command circular buffer is full. */
  if (((CommandQueueTail > 0) && (CommandQueueHead == CommandQueueTail - 1)) || ((CommandQueueTail == 0) && (CommandQueueHead == MAX_COMMAND_QUEUE - 1)))
  {
    /* Command queue is full, return error code. */
    return MAX_COMMAND_QUEUE;
  }

  /* If there is at least one slot available in the queue, insert the command to be processed. */
  CommandQueue[CommandQueueHead].Command   = Command;
  CommandQueue[CommandQueueHead].Parameter = Parameter;
  ++CommandQueueHead;

  if (DebugBitMask & DEBUG_COMMAND_QUEUE)
    uart_send(__LINE__, "- Command queueing: %3u   %3u\r", Command, Parameter);

  /* Revert to zero when reaching out-of-bound. */
  if (CommandQueueHead >= MAX_COMMAND_QUEUE)
    CommandQueueHead = 0;

  return 0;
}





/* $PAGE */
/* $TITLE=command_unqueue() */
/* ------------------------------------------------------------------ *\
             Unqueue next command from the command queue.
\* ------------------------------------------------------------------ */
UINT8 command_unqueue(UINT8 *Command, UINT16 *Parameter)
{
  UCHAR String[256];


  /* Check if the command queue is empty. */
  if (CommandQueueHead == CommandQueueTail)
  {
    /* In case of empty queue or queue error, return queue head and queue tail instead. */
    *Command   = CommandQueueHead;
    *Parameter = CommandQueueTail;

    return 0xFF;
  }

  /* Extract data for next command to process. */
  *Command   = CommandQueue[CommandQueueTail].Command;
  *Parameter = CommandQueue[CommandQueueTail].Parameter;

  /* And reset this slot in the command queue. */
  CommandQueue[CommandQueueTail].Command   = 0;
  CommandQueue[CommandQueueTail].Parameter = 0;


  if (DebugBitMask & DEBUG_COMMAND_QUEUE)
    uart_send(__LINE__, "- Command unqueuing Command: %u   Parameter: %u\r", *Command, *Parameter);


  /* Adjust queue tail. */
  ++CommandQueueTail;

  /* If reaching out-of-bound, revert to zero. */
  if (CommandQueueTail >= MAX_COMMAND_QUEUE) CommandQueueTail = 0;

  return 0;
}





/* $PAGE */
/* $TITLE=convert_h24_to_h12() */
/* ------------------------------------------------------------------ *\
          Convert a 24-hour format time to 12-hour format.
\* ------------------------------------------------------------------ */
UINT8 convert_h24_to_h12(UINT8 Hour, UINT8 *AmFlag, UINT8 *PmFlag)
{
  UCHAR String[128];

  UINT8 Dum1UInt8;


  if (Hour == 0)
  {
    /* If Hour is between "00h00" and 00h59, change it to "12hxx AM". */
    *AmFlag = FLAG_ON;
    *PmFlag = FLAG_OFF;
    Dum1UInt8 = 12;
  }
  else if (Hour == 12)
  {
    /* If Hour is between "12h00" and 12h59, turn ON the "PM" indicator and turn OFF the "AM" indicator. */
    *AmFlag = FLAG_OFF;
    *PmFlag = FLAG_ON;
    Dum1UInt8 = 12;
  }
  else if (Hour > 12)
  {
    /* If Hour is between 13h00 and 23h59, convert to 1 to 12 and turn ON the "PM" indicator. */
    *AmFlag = FLAG_OFF;
    *PmFlag = FLAG_ON;
    Dum1UInt8 = (Hour - 12);
  }
  else
  {
    /* If Hour is between 01h00 and 11h59, turn on the "AM" indicator and turn OFF the "PM" indicator. */
    *AmFlag = (UINT8)FLAG_ON;
    *PmFlag = (UINT8)FLAG_OFF;
    Dum1UInt8 = Hour;
  }

  return Dum1UInt8;
}





/* $PAGE */
/* $TITLE=convert_human_to_tm() */
/* ------------------------------------------------------------------ *\
                     Convert "HumanTime" to "TmTime".
\* ------------------------------------------------------------------ */
void convert_human_to_tm(struct human_time *HumanTime, struct tm *TmTime)
{
  TmTime->tm_hour  = HumanTime->Hour;
  TmTime->tm_min   = HumanTime->Minute;
  TmTime->tm_sec   = HumanTime->Second;
  TmTime->tm_mday  = HumanTime->DayOfMonth;
  TmTime->tm_mon   = HumanTime->Month - 1;
  TmTime->tm_year  = HumanTime->Year - 1900;
  TmTime->tm_wday  = HumanTime->DayOfWeek - 1;
  TmTime->tm_yday  = HumanTime->DayOfYear - 1;
  if (FlashConfig.FlagSummerTime == FLAG_ON)
    TmTime->tm_isdst = 1;
  else
   TmTime->tm_isdst = -1;

  return;
}





/* $PAGE */
/* $TITLE=convert_tm_to_human() */
/* ------------------------------------------------------------------ *\
                     Convert "TmTime" to "HumanTime".
\* ------------------------------------------------------------------ */
void convert_tm_to_human(struct tm *TmTime, struct human_time *HumanTime)
{
  HumanTime->Hour       = TmTime->tm_hour;
  HumanTime->Minute     = TmTime->tm_min;
  HumanTime->Second     = TmTime->tm_sec;
  HumanTime->DayOfMonth = TmTime->tm_mday;
  HumanTime->Month      = TmTime->tm_mon + 1;
  HumanTime->Year       = TmTime->tm_year + 1900;
  HumanTime->DayOfWeek  = TmTime->tm_wday + 1;
  HumanTime->DayOfYear  = TmTime->tm_yday + 1;
  if (TmTime->tm_isdst > 0)
    HumanTime->FlagDst = FLAG_ON;
  else
    HumanTime->FlagDst = FLAG_OFF;

  if (DebugBitMask & DEBUG_RTC)
  {
    uart_send(__LINE__, "convert_tm_to_human():\r");
    uart_send(__LINE__, "HumanTime->Hour: %2u\r",         HumanTime->Hour);
    uart_send(__LINE__, "HumanTime->Minute:     %2.2u\r", HumanTime->Minute);
    uart_send(__LINE__, "HumanTime->Second:     %2.2u\r", HumanTime->Second);
    uart_send(__LINE__, "HumanTime->DayOfMonth: %2.2u\r", HumanTime->DayOfMonth);
    uart_send(__LINE__, "HumanTime->Month:      %2.2u\r", HumanTime->Month);
    uart_send(__LINE__, "HumanTime->Year:     %4.4u\r",   HumanTime->Year);
    uart_send(__LINE__, "HumanTime->DayOfWeek: %u\r",     HumanTime->DayOfWeek);
    uart_send(__LINE__, "HumanTime->DayOfYear:  %3.3u\r", HumanTime->DayOfYear);
    uart_send(__LINE__, "HumanTime->FlagDst:     %2.2X\r\r\r", HumanTime->FlagDst);
   }

  return;
}





/* $PAGE */
/* $TITLE=convert_tm_to_unix() */
/* ------------------------------------------------------------------ *\
                     Convert "TmTime" to "Unix Time".
         NOTE: Unix Time is based on UCT time, not local time.
\* ------------------------------------------------------------------ */
UINT64 convert_tm_to_unix(struct tm *TmTime)
{
  return mktime(TmTime);
}





/* $PAGE */
/* $TITLE=convert_unix_to_tm() */
/* ------------------------------------------------------------------ *\
             Convert "Unix Time" (Epoch) to "tm Time".
       NOTE: Both Unix Time and TmTime are relative to UCT.
\* ------------------------------------------------------------------ */
void convert_unix_to_tm(time_t UnixTime, struct tm *TmTime, UINT8 FlagLocalTime)
{
  /* If asked for local time, take Timezone into account. */
  if (FlagLocalTime == FLAG_ON)
    UnixTime += (FlashConfig.Timezone * 60 * 60);

  TmTime = localtime(&UnixTime);

  if (DebugBitMask & DEBUG_RTC)
  {
    uart_send(__LINE__, "convert_unix_to_tm(): UnixTime: %llu\r\r", UnixTime);
    uart_send(__LINE__, "TmTime->tm_hour  = %u\r", TmTime->tm_hour);
    uart_send(__LINE__, "TmTime->tm_min   = %u\r", TmTime->tm_min);
    uart_send(__LINE__, "TmTime->tm_sec   = %u\r", TmTime->tm_sec);
    uart_send(__LINE__, "TmTime->tm_mday  = %u\r", TmTime->tm_mday);
    uart_send(__LINE__, "TmTime->tm_mon   = %u\r", TmTime->tm_mon);
    uart_send(__LINE__, "TmTime->tm_year  = %u\r", TmTime->tm_year);
    uart_send(__LINE__, "TmTime->tm_wday  = %u\r", TmTime->tm_wday);
    uart_send(__LINE__, "TmTime->tm_yday  = %u\r", TmTime->tm_yday);
    uart_send(__LINE__, "TmTime->tm_isdst = %u\r\r\r", TmTime->tm_isdst);
  }

  return;
}





#ifdef DHT_SUPPORT
/* $PAGE */
/* $TITLE=core1_main() */
/* ------------------------------------------------------------------ *\
            Thread to be run on Pico's core 1 (second core).
\* ------------------------------------------------------------------ */
void core1_main(void)
{
  UCHAR String[256];

  UINT8 Command;
  UINT8 FlagError;

  float Humidity;
  float Temperature;


  /* Log a message when core 1 starts. */
  if (DebugBitMask & DEBUG_CORE)
    uart_send(__LINE__, "==================== CORE 1 - Core 1 thread started...\r");


  /* Loop forever, waiting for commands from core 0. */
  while (1)
  {
    /* Get command sent from core 0. */
    Command = core_unqueue(1);


    /* If queue is empty, no command received yet... Wait until receiving a valid command. */
    if (Command == MAX_CORE_QUEUE)
    {
      sleep_ms(2);  // command queue is empty, wait a little while before checking again.
    }
    else
    {
      /* There is a command to process. */
      switch (Command)
      {
        case (CORE1_READ_DHT):
          if (DebugBitMask & DEBUG_CORE)
            uart_send(__LINE__, "==================== CORE 1 - Received command READ_DHT\r");

          if (read_dht(&Temperature, &Humidity))
          {
            /* Error while reading DHT22. Retry a maximum of 3 times. */
            if (DebugBitMask & DEBUG_CORE)
              uart_send(__LINE__, "==================== CORE 1 - DHT read error... Return error code to core 0\r");

            /* Return error code to core 0. */
            core_queue(0, CORE0_DHT_ERROR);
          }
          else
          {
            /* No error, send data to core 0. */
            DhtData.TimeStamp   = time_us_64();
            DhtData.Temperature = Temperature;
            DhtData.Humidity    = Humidity;

            if (DebugBitMask & DEBUG_CORE)
            {
              uart_send(__LINE__, "==================== CORE 1 - DHT read successful... Return result to core 0 (%f   %f)\r", Temperature, Humidity);
              uart_send(__LINE__, "==================== CORE 1 - TimeStamp: %llu   Temperature: %f   Humidity: %f\r", DhtData.TimeStamp, DhtData.Temperature, DhtData.Humidity);
            }

            /* Return response to core 0. */
            core_queue(0, CORE0_DHT_READ_COMPLETED);
          }
        break;
      }
    }
  }
}





/* $PAGE */
/* $TITLE=core_queue() */
/* ------------------------------------------------------------------ *\
        Queue the given command in the appropriate core queue.
\* ------------------------------------------------------------------ */
UINT8 core_queue(UINT8 CoreNumber, UINT8 Command)
{
  UCHAR String[256];

  UINT8 Dum1UChar;


  if (DebugBitMask & DEBUG_CORE)
    uart_send(__LINE__, "Entering core_queue():       Target core: %u          Command: %2u\r", CoreNumber, Command);


  switch (CoreNumber)
  {
    case (0):
      if (DebugBitMask & DEBUG_CORE)
        uart_send(__LINE__, "Core 0 queue on entry: Core0QueueHead: %2u   Core0QueueTail: %2u\r", Core0QueueHead, Core0QueueTail);

      /* Check if the circular buffer is full. */
      if (((Core0QueueTail > 0) && (Core0QueueHead == Core0QueueTail - 1)) || ((Core0QueueTail == 0) && (Core0QueueHead == MAX_CORE_QUEUE - 1)))
      {
        if (DebugBitMask & DEBUG_CORE)
          uart_send(__LINE__, "Core0Queue full\r");

        /* Core 0 queue is full, return error code. */
        return MAX_CORE_QUEUE;
      }

      /* If there is at least one slot available in the queue, insert the command for the other core. */
      Core0Queue[Core0QueueHead] = Command;
      ++Core0QueueHead;

      if (Core0QueueHead >= MAX_CORE_QUEUE)
        Core0QueueHead = 0;  // revert to zero when reaching out-of-bound.

      if (DebugBitMask & DEBUG_CORE)
        uart_send(__LINE__, "Core 0 queue on exit:  Core0QueueHead: %2u   Core0QueueTail: %2u\r", Core0QueueHead, Core0QueueTail);

      return 0x00;
    break;


    case (1):
      if (DebugBitMask & DEBUG_CORE)
        uart_send(__LINE__, "Core 1 queue on entry: Core1QueueHead: %u   Core1QueueTail: %u\r", Core1QueueHead, Core1QueueTail);

      /* Check if the circular buffer is full. */
      if (((Core1QueueTail > 0) && (Core1QueueHead == Core1QueueTail - 1)) || ((Core1QueueTail == 0) && (Core1QueueHead == MAX_CORE_QUEUE - 1)))
      {
        if (DebugBitMask & DEBUG_CORE)
          uart_send(__LINE__, "Core1Queue full\r");

        /* Core 1 queue is full, return error code. */
        return MAX_CORE_QUEUE;
      }

      /* If there is at least one slot available in the queue, insert the command to be sent to the other core. */
      Core1Queue[Core1QueueHead] = Command;
      ++Core1QueueHead;

      if (Core1QueueHead >= MAX_CORE_QUEUE)
        Core1QueueHead = 0;  // revert to zero when reaching out-of-bound.

      if (DebugBitMask & DEBUG_CORE)
        uart_send(__LINE__, "Core 1 queue on exit:  Core1QueueHead: %u   Core1QueueTail: %u\r", Core1QueueHead, Core1QueueTail);

      return 0x00;
    break;


    default:
      /* Invalid core number specified until Raspberry Pi decides to manufacture a Pico with 8 cores :-) */
      if (DebugBitMask & DEBUG_CORE)
        uart_send(__LINE__, "Wrong core specified for core_queue() function [%u]\r", CoreNumber);
    break;
  }
}





/* $PAGE */
/* $TITLE=core_unqueue() */
/* ------------------------------------------------------------------ *\
          Unqueue next command for the specified core number.
\* ------------------------------------------------------------------ */
UINT8 core_unqueue(UINT8 CoreNumber)
{
  UINT8 Command;


  switch (CoreNumber)
  {
    case (0):
      /* Check if core queue is empty. */
      if (Core0QueueHead == Core0QueueTail)
        return MAX_CORE_QUEUE;

      Command = Core0Queue[Core0QueueTail];
      Core0Queue[Core0QueueTail] = 0x00;
      ++Core0QueueTail;

      /* If reaching out-of-bound, revert to zero. */
      if (Core0QueueTail >= MAX_CORE_QUEUE)
        Core0QueueTail = 0;

      return Command;
    break;

    case (1):
      /* Check if core queue is empty. */
      if (Core1QueueHead == Core1QueueTail)
        return MAX_CORE_QUEUE;

      Command = Core1Queue[Core1QueueTail];
      Core1Queue[Core1QueueTail] = 0x00;
      ++Core1QueueTail;

      /* If reaching out-of-bound, revert to zero. */
      if (Core1QueueTail >= MAX_CORE_QUEUE)
        Core1QueueTail = 0;

      return Command;
    break;
  }
}
#endif  // DHT_SUPPORT





/* $PAGE */
/* $TITLE=crc16() */
/* ------------------------------------------------------------------ *\
        Find the cyclic redundancy check of the specified data.
\* ------------------------------------------------------------------ */
UINT16 crc16(UINT8 *Data, UINT16 DataSize)
{
  UCHAR String[256];

  UINT16 CrcValue;
  UINT8 Loop1UInt8;


  /* Validate data pointer. */
  if (Data == NULL) return 0;

  if (DebugBitMask & DEBUG_CRC16)
  {
    uart_send(__LINE__, "Calculating CRC16 of this packet (size: %u):\r", DataSize);
    display_data(Data, DataSize);
  }

  CrcValue = 0;

  while (DataSize-- > 0)
  {
    CrcValue = CrcValue ^ (UINT8)*Data++ << 8;

    for (Loop1UInt8 = 0; Loop1UInt8 < 8; ++Loop1UInt8)
    {
      if (CrcValue & 0x8000)
        CrcValue = CrcValue << 1 ^ CRC16_POLYNOM;
      else
        CrcValue = CrcValue << 1;
    }
  }

  if (DebugBitMask & DEBUG_CRC16)
    uart_send(__LINE__, "CRC16 computed: %X\r\r\r", CrcValue & 0xFFFF);

  return (CrcValue & 0xFFFF);
}





#ifdef IR_SUPPORT
#include REMOTE_FILENAME
#endif





/* $PAGE */
/* $TITLE=date_stamp() */
/* ------------------------------------------------------------------ *\
                 Generate a date stamp for debug info.
\* ------------------------------------------------------------------ */
void date_stamp(UCHAR *String)
{
  UCHAR Month[16];

  UINT Language;
  UINT Loop1UInt;
  UINT YearCentile;

  struct human_time HumanTime;
  struct tm         TmTime;


  /* Retrieve current tm local time. */
  /// convert_unix_to_tm(GlobalUnixTime, &TmTime, FLAG_ON);


  /* Convert current tm local time to human local time. */
  /// convert_tm_to_human();


  /* Make sure year centile is valid (should now be retrieved from flash). */
  if ((FlashConfig.CurrentYearCentile > 10) && (FlashConfig.CurrentYearCentile < 30))
    YearCentile = FlashConfig.CurrentYearCentile;
  else
    YearCentile = 20;

  /* NOTE: Use English month names since accents don't show up on external terminal. */
  sprintf(String, "[%2.2u-%s-%4.4u %2.2u:%2.2u:%2.2u] - ", CurrentDayOfMonth, ShortMonth[ENGLISH][CurrentMonth], CurrentYear, CurrentHour, CurrentMinute, CurrentSecond);

  return;
}





/* $PAGE */
/* $TITLE=display_data() */
/* ------------------------------------------------------------------ *\
         Display data sent in argument to an external monitor
                       through RS232 or USB CDC.
\* ------------------------------------------------------------------ */
void display_data(UCHAR *Data, UINT32 Size)
{
  UCHAR String[256];

  UINT32 Loop1UInt32;
  UINT32 Loop2UInt32;


  uart_send(__LINE__, "Entering display_data(size: %u)\r", Size);


  for (Loop1UInt32 = 0; Loop1UInt32 < Size; Loop1UInt32 += 16)
  {
    sprintf(String, "[%4.4X] ", Loop1UInt32);

    for (Loop2UInt32 = 0; Loop2UInt32 < 16; ++Loop2UInt32)
    {
      if ((Loop1UInt32 + Loop2UInt32) >= Size)
        strcat(String, "   ");
      else
        sprintf(&String[strlen(String)], "%2.2X ", Data[Loop1UInt32 + Loop2UInt32]);
    }


    uart_send(__LINE__, String);


    /* Add separator. */
    sprintf(String, "| ");

    for (Loop2UInt32 = 0; Loop2UInt32 < 16; ++Loop2UInt32)
    {
      if ((Loop1UInt32 + Loop2UInt32) >= Size)
        break; // do not count up to 16 if we already reached end of data to display.
      if ((Data[Loop1UInt32 + Loop2UInt32] >= 0x20) && (Data[Loop1UInt32 + Loop2UInt32] <= 0x7E) && (Data[Loop1UInt32 + Loop2UInt32] != 0x25))
      {
        sprintf(&String[strlen(String)], "%c", Data[Loop1UInt32 + Loop2UInt32]);
      }
      else
      {
        strcat(String, ".");
      }
    }
    strcat(String, "\r");
    uart_send(__LINE__, String);

    // sleep_ms(50);  // let some time to flush UART buffer.
  }

  uart_send(__LINE__, "Exiting display_data()\r\r\r");

  return;
}





/* $PAGE */
/* $TITLE=display_pvm() */
/* ------------------------------------------------------------------ *\
             Display current PWM values for specified PWM.
\* ------------------------------------------------------------------ */
void display_pwm(struct pwm *Pwm, UCHAR *TitleString)
{
  UCHAR String[256];

  UINT16 Dum1UInt16;

  UINT32 SystemClock;


  /* Retrieve system clock (Pico is 125 MHz)... */
  SystemClock = clock_get_hz(clk_sys);

  uart_send(__LINE__, TitleString);

  uart_send(__LINE__, "Pwm->Gpio:         %u    (13 = Brightness   19 = Sound)\r", Pwm->Gpio);

  if (Pwm->OnOff == FLAG_OFF)
    uart_send(__LINE__, "Pwm->OnOff:        Off\r");
  else
    uart_send(__LINE__, "Pwm->OnOff:        On\r");


  uart_send(__LINE__, "Pwm->Slice:        %u (%u)\r", Pwm->Slice, pwm_gpio_to_slice_num(Pwm->Gpio));
  uart_send(__LINE__, "Pwm->Channel:      %u (%u)\r", Pwm->Channel, pwm_gpio_to_channel(Pwm->Gpio));
  uart_send(__LINE__, "SystemClock:       %u:\r", SystemClock);
  uart_send(__LINE__, "Pwm->ClockDivider: %f\r", Pwm->ClockDivider);

  Pwm->Clock = (UINT32)(SystemClock / Pwm->ClockDivider);
  uart_send(__LINE__, "Pwm->Clock:        %u\r", Pwm->Clock);
  uart_send(__LINE__, "Pwm->Wrap:         %u\r", Pwm->Wrap);

  Pwm->Frequency = (UINT32)(Pwm->Clock / Pwm->Wrap);
  uart_send(__LINE__, "Pwm->Frequency:    %lu\r", Pwm->Frequency);

  if (Pwm->Level > Pwm->Wrap)
  {
    Dum1UInt16 = Pwm->Level;
    Pwm->Level = Pwm->Frequency;
    uart_send(__LINE__, "Pwm->Level:        %u  === has been reset from %u ===\r", Pwm->Level, Dum1UInt16);
  }
  else
  {
    uart_send(__LINE__, "Pwm->Level:        %u\r", Pwm->Level);
  }

  if (Pwm->Gpio == OE)
    Pwm->Cycles = (UINT8)(100 - (((float)Pwm->Level / Pwm->Wrap) * 100.0));
  else
    Pwm->Cycles = (UINT8)(((float)Pwm->Level / Pwm->Wrap) * 100.0);

  uart_send(__LINE__, "Pwm->Cycles:    %u\r", Pwm->Cycles);
  uart_send(__LINE__, "CurrentClockMode:  %u\r\r\r\r", CurrentClockMode);
}





/* $PAGE */
/* $TITLE=evaluate_blinking_time() */
/* ------------------------------------------------------------------ *\
         Evaluate if it is time to blink data on the display.
\* ------------------------------------------------------------------ */
void evaluate_blinking_time(void)
{
  UCHAR String[256];

  UINT8 Loop1UInt8;


  /* Check if we are in setup mode. */
  if (SetupStep != SETUP_NONE)
  {
    ++FlashingCount;

    if (FlashingCount == 500)
    {
      /* Toggle blink status every 500 milliseconds (half a second). */
      FlashingCount = 0;                                  // reset flashing count for next pass.
      FlagBlinking[SetupStep] = ~FlagBlinking[SetupStep]; // toggle from 0x00 to 0xFF and vice-versa

      switch (SetupSource)
      {
        case SETUP_SOURCE_ALARM:
          FlagSetAlarm = FLAG_ON;
        break;

        case SETUP_SOURCE_CLOCK:
          FlagSetClock = FLAG_ON;
        break;

        case SETUP_SOURCE_TIMER:
          FlagSetTimer = FLAG_ON;
        break;
      }

      /* If we are setting up alarm day-of-week, blink current selection. */
      if (CurrentClockMode == MODE_ALARM_SETUP)
      {
        if (SetupStep == SETUP_ALARM_DAY)
        {
          /* Turn On day-of-week indicators for days already selected... */
          for (Loop1UInt8 = SUN; Loop1UInt8 <= SAT; ++Loop1UInt8)
          {
            if (FlashConfig.Alarm[AlarmNumber].Day & (1 << Loop1UInt8))
              update_top_indicators(Loop1UInt8, FLAG_ON);
            else
              update_top_indicators(Loop1UInt8, FLAG_OFF);
          }

          /* ...and blink day-of-week indicator for the one hilight for selection. */
          if (FlagBlinking[SetupStep])
          {
            /* Turn on day-of-week-indicator ("blink On"). */
            update_top_indicators(AlarmTargetDay, FLAG_ON);
          }
          else
          {
            /* Turn off day-of-week-indicator ("blink Off"). */
            update_top_indicators(AlarmTargetDay, FLAG_OFF);
          }
        }
      }
    }
  }


  /* While the clock is displayed, blink the top "middle dot" during the first 20 seconds, then the bottom
     "middle dot" during 20 to 40 seconds, and finally both "middle dots" between 40 and 59 seconds. */
  if (CurrentClockMode == MODE_SHOW_TIME)
  {
    ++DotBlinkCount;

    /* 500 means "erase the target dot(s)". */
    if (DotBlinkCount == 500)
    {
      /* We change the status of blinking double-dots, allow "display seconds" to be updated
         on display at the same time if it is active (to prevent a glitch in display update). */
      sem_release(&SemSync);

      if (CurrentSecond < 15)
      {
        /* From 0 to 14 seconds, blink the top "middle dot". */
        DisplayBuffer[11] &= 0xEF; // slim ":" - turn Off top dot.

        /* Make sure the other dot (bottom dot) is on. */
        DisplayBuffer[13] |= 0x10; // slim ":" - make sure bottom dot is turned On.
      }
      else if (CurrentSecond < 30)
      {
        /* From 15 to 29 seconds, blink the bottom "middle dot". */
        DisplayBuffer[11] |= 0x10; // slim ":" - make sure top dot is turned On.

        /* Erase bottom dot. */
        DisplayBuffer[13] &= 0xEF; // slim ":" - erase bottom dot.
      }
      else if (CurrentSecond < 45)
      {
        /* From 30 to 44 seconds, alternate between both "middle dots". */
        DisplayBuffer[11] |= 0x10; // slim ":" - turn On top dot.

        /* Turn Off bottom dot. */
        DisplayBuffer[13] &= 0xEF; // slim ":" - turn Off bottom dot.
      }
      else
      {
        /* From 45 to 59 seconds, blink both "middle dots". */
        DisplayBuffer[11] &= 0xEF; // slim ":" - turn Off top dot.
        DisplayBuffer[13] &= 0xEF; // slim ":" - turn Off bottom dot.
      }
    }

    /* 1000 means "redraw the target dot(s)". */
    if (DotBlinkCount == 1000)
    {
      /* We change the status of blinking double-dots, allow "display seconds" to be updated
         on display at the same time if it is active (to prevent a glitch in display update). */
      sem_release(&SemSync);

      if (CurrentSecond < 15)
      {
        /* From 0 to 14 seconds, blink the top "middle dot". */
        DisplayBuffer[11] |= 0x10; // slim ":" - turn On top dot.
      }
      else if (CurrentSecond < 30)
      {
        /* From 15 to 29 seconds, blink the bottom "middle dot". */
        DisplayBuffer[13] |= 0x10; // slim ":" - turn On bottom dot.
      }
      else if (CurrentSecond < 45)
      {
        /* From 30 to 44 seconds, alternate between both "middle dots". */
        DisplayBuffer[11] &= 0xEF; // slim ":" - turn Off top dot.
        DisplayBuffer[13] |= 0x10; // slim ":" - turn On bottom dot.
      }
      else
      {
        /* From 45 to 59 seconds, blink both "middle dot". */
        DisplayBuffer[11] |= 0x10; // slim ":" - turn On top dot.
        DisplayBuffer[13] |= 0x10; // slim ":" - turn On bottom dot.
      }

      DotBlinkCount = 0; // reset DotBlinkCount
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=evaluate_scroll_time() */
/* ------------------------------------------------------------------ *\
              Evaluate if it is time to scroll characters
                 one dot to the left on clock display.
\* ------------------------------------------------------------------ */
void evaluate_scroll_time(void)
{
  /* Check if there is text currently scrolling. */
  if (FlagScrollStart == FLAG_ON)
  {
    /* Scroll one dot to the left every "SCROLL_DOT_TIME" milliseconds (typically from 70 to 100 milliseconds). */
    ++ScrollStartCount;
  }


  /* Check if we reached the delay before scrolling one more dot to the left. */
  if (ScrollStartCount == SCROLL_DOT_TIME)
  {
    /* Check if there are some more columns to scroll in the display buffer. */
    if (ScrollDotCount > 0)
    {
      /* There are more characters to scroll on the display. */
      scroll_one_dot();  // scroll one more dot to the left on clock display.
      --ScrollDotCount;   // one less dot to be scrolled on the display.
    }
    else
    {
      fill_display_buffer_4X7(24, ' ');  // blank the first "invisible column" at the right of the display buffer when done.
      FlagScrollStart = FLAG_OFF;        // reset scroll start flag when done.
      FlagUpdateTime  = FLAG_ON;         // request a time update on the clock.
    }

    ScrollStartCount = 0;  // reset the millisecond counter to wait for next dot scroll to the left.
  }

  return;
}





/* $PAGE */
/* $TITLE=fill_display_buffer_5X7() */
/* ------------------------------------------------------------------ *\
      Set the display framebuffer with the given ASCII character
              beginning at the given column position.
          Character width is also passed as an argument.
      NOTE: left-most column visible on clock display is column 2
            columns 0 and 1 are used by indicators.
      This function returns the position of the next free column.
\* ------------------------------------------------------------------ */
UINT16 fill_display_buffer_5X7(UINT8 Column, UINT8 AsciiCharacter)
{
  UINT8 ColumnInSection;
  UINT8 RowNumber;
  UINT8 SectionNumber;

  UINT16 TableOffset;
  UINT16 Width;


  /* If column number given as an argument is out-of-bound, return DISPLAY_BUFFER_SIZE. */
  if (Column > DISPLAY_BUFFER_SIZE) return DISPLAY_BUFFER_SIZE;


  /* First 2 columns are reserved for indicators. */
  if (Column < 2) Column = 2;


  SectionNumber   = Column / 8;
  ColumnInSection = Column % 8;


  /* Special handling of character 0x00 used to blink characters on the display. */
  if (AsciiCharacter == 0x00) AsciiCharacter = 32; // blank ("space") character.


  /* Determine the digit bitmap corresponding to the character we want to display. */
  if ((AsciiCharacter >= 0x1E) && (AsciiCharacter <= 0x91))
  {
    TableOffset = (UINT16)((AsciiCharacter - 0x1E) * 7);
    Width       = CharWidth[AsciiCharacter - 0x1E];
  }
  else
  {
    /* If ASCII character is out-of-bound, replace it with "?". */
    TableOffset = (UINT16)((0x3F - 0x1E) * 7);
    Width       = CharWidth[AsciiCharacter - 0x3F];
  }


  for (RowNumber = 1; RowNumber < 8; ++RowNumber)
  {
    if (ColumnInSection > 0)
    {
      DisplayBuffer[(SectionNumber * 8) + RowNumber] = (DisplayBuffer[(SectionNumber * 8) + RowNumber] & (0xFF >> (8 - ColumnInSection))) | ((reverse_bits(CharMap[TableOffset + RowNumber - 1])) << ColumnInSection);
      if (SectionNumber < (sizeof(DisplayBuffer) / 8) - 1)
      {
        DisplayBuffer[(SectionNumber * 8) + 8 + RowNumber] = (DisplayBuffer[(SectionNumber * 8) + 8 + RowNumber] & (0xFF << (8 - ColumnInSection))) | ((reverse_bits(CharMap[TableOffset + RowNumber - 1])) >> (8 - ColumnInSection));
      }
    }
    else
    {
      DisplayBuffer[(SectionNumber * 8) + RowNumber] = (reverse_bits(CharMap[TableOffset + RowNumber - 1]));
    }
  }

  return Column + Width;
}





/* $PAGE */
/* $TITLE=fill_display_buffer_4X7() */
/* ------------------------------------------------------------------ *\
         Fill the framebuffer with the 4 X 7 character bitmap
              corresponding to the given ASCII character,
   beginning at the specified column position of the display memory.
   If the selected columns are located in the "physical" section of
      the display memory, the character will show up immediately.
    If the selected columns are located in the "virtual" section of
   the display memory, successive calls to scroll_one_dot() function
   are required for the characters to be scrolled inside the visible
                    part of the clock display.
\* ------------------------------------------------------------------ */
void fill_display_buffer_4X7(UINT8 Column, UINT8 AsciiCharacter)
{
  UINT8 BitmapCharacter;
  UINT8 ColumnInSection;
  UINT8 RowNumber;
  UINT8 SectionNumber;


  Column += DisplayOffset;      // first 2 columns are used by "indicators".
  SectionNumber = Column / 8;   // display is divided in sections of 8 columns.
  ColumnInSection = Column % 8; // determine the relative column number is the specific section.

  /* Determine the character in the bitmap table corresponding to the ASCII character we want to display. */
  if ((AsciiCharacter >= 0x2D) && (AsciiCharacter <= 0x2D + 83))
    BitmapCharacter = AsciiCharacter - 0x2D;
  else
  {
    switch (AsciiCharacter)
    {
      case 0: // ASCII value 0x00
        /* Special blank character when blinking. */
        BitmapCharacter = 46;
      break;

      case 0x16: // ASCII value 0x16
        /* Slim ":" */
        BitmapCharacter = 13;
      break;

      case ' ': // ASCII value 0x20
        /* Blank " " ("space") character. */
        BitmapCharacter = 46;
      break;

      default:
        /* Not a valid character, display symbol "?" */
        BitmapCharacter = 49;
      break;
    }
  }

  /* Then, transfer the bitmap to framebuffer. */
  for (RowNumber = 1; RowNumber < 8; ++RowNumber)
  {
    if (ColumnInSection > 0)
    {
      DisplayBuffer[(SectionNumber * 8) + RowNumber] = (DisplayBuffer[(SectionNumber * 8) + RowNumber] & (0xFF >> (8 - ColumnInSection))) | ((CharacterMap[(BitmapCharacter * 7) + RowNumber - 1]) << ColumnInSection);
      if (SectionNumber < (sizeof(DisplayBuffer) / 8) - 1)
      {
        DisplayBuffer[(SectionNumber * 8) + 8 + RowNumber] = (DisplayBuffer[(SectionNumber * 8) + 8 + RowNumber] & (0xFF << (8 - ColumnInSection))) | ((CharacterMap[(BitmapCharacter * 7) + RowNumber - 1]) >> (8 - ColumnInSection));
      }
    }
    else
    {
      DisplayBuffer[(SectionNumber * 8) + RowNumber] = (CharacterMap[(BitmapCharacter * 7) + RowNumber - 1]);
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=flash_check_crc16() */
/* ------------------------------------------------------------------ *\
            Compare crc16 between flash saved configuration
                   and current active configuration.
\* ------------------------------------------------------------------ */
void flash_check_config(void)
{
  UCHAR String[256];

  UINT16 Crc16;


  /* Calculate CRC16 for current active clock configuration. */
  Crc16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);

  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "CRC16 computed for current active clock configuration: 0x%X (%u)\r", Crc16, Crc16);

  /* We know that FlashConfig.Crc16 corresponds to the crc16 value that has been retrieved the last time the configuration was restored from (or saved to) flash.
     So, if the current active configuration's CRC16 is still the same as the last one retrieved from (or saved to) flash,
     it means that configuration has not changed from what it is in flash). */
  if (FlashConfig.Crc16 == Crc16)
  {
    if (DebugBitMask & DEBUG_FLASH)
      uart_send(__LINE__, "Current active configuration is the same as the last one retrieved from flash.\r\r\r");

    return;
  }
  else
  {
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Current active configuration is different than the one in flash.\r");
      uart_send(__LINE__, "Update flash configuration.\r\r\r");
    }

    /* Update flash configuration so that it matches the current active configuration. */
    flash_save_config();
  }

  return;
}





/* $PAGE */
/* $TITLE=flash_display() */
/* ------------------------------------------------------------------ *\
           Display flash content through external monitor.
\* ------------------------------------------------------------------ */
void flash_display(UINT32 Offset, UINT32 Length)
{
  UCHAR String[256];

  UINT8 *FlashBaseAddress;
  UINT32 Loop1UInt32;
  UINT32 Loop2UInt32;


  /* Compute target flash memory address.
     NOTE: XIP_BASE ("eXecute-In-Place") Is the base address of the flash memory in the Pico's address space (memory map). */
  FlashBaseAddress = (UINT8 *)(XIP_BASE);

  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "Entering flash_display()\r");

  uart_send(__LINE__, "XIP_BASE: 0x%p   Offset: 0x%6.6X   Length: 0x%X (%u)\r", XIP_BASE, Offset, Length, Length);


  for (Loop1UInt32 = Offset; Loop1UInt32 < (Offset + Length); Loop1UInt32 += 16)
  {
    sprintf(String, "[%p] ", XIP_BASE + Loop1UInt32);

    for (Loop2UInt32 = 0; Loop2UInt32 < 16; ++Loop2UInt32)
    {
      sprintf(&String[strlen(String)], "%2.2X ", FlashBaseAddress[Loop1UInt32 + Loop2UInt32]);
    }
    uart_send(__LINE__, String);


    /* Add separator. */
    sprintf(String, "| ");


    for (Loop2UInt32 = 0; Loop2UInt32 < 16; ++Loop2UInt32)
    {
      if ((FlashBaseAddress[Loop1UInt32 + Loop2UInt32] >= 0x20) && (FlashBaseAddress[Loop1UInt32 + Loop2UInt32] <= 0x7E)  && (FlashBaseAddress[Loop1UInt32 + Loop2UInt32] != 0x25))
      {
        sprintf(&String[Loop2UInt32 + 2], "%c", FlashBaseAddress[Loop1UInt32 + Loop2UInt32]);
      }
      else
      {
        sprintf(&String[Loop2UInt32 + 2], ".");
      }
    }
    uart_send(__LINE__, String);
    uart_send(__LINE__, "\r");
  }

  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "Exiting flash_display()\r\r\r");

  return;
}





/* $PAGE */
/* $TITLE=flash_display_config() */
/* ------------------------------------------------------------------ *\
           Display Green Clock current active configuration
                       to an external monitor.
       The parameters displayed are those that are being saved
                       to Pico's flash memory.
\* ------------------------------------------------------------------ */
UINT8 flash_display_config(void)
{
  UCHAR DayMask[16];
  UCHAR String[256];

  UINT8 Dum1UInt8;

  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;


  uart_send(__LINE__, "=========================================================================================================\r");
  uart_send(__LINE__, "                                     Entering flash_display_config()\r");
  uart_send(__LINE__, "=========================================================================================================\r");

  uart_send(__LINE__, "NOTE: FLAG_OFF = 0x%2.2X   FLAG_ON = 0x%2.2X\r", FLAG_OFF, FLAG_ON);
  uart_send(__LINE__, "NOTE: sizeof(FlashConfig):           0x%3.3X (%3u)\r", sizeof(FlashConfig), sizeof(FlashConfig));
  uart_send(__LINE__, "NOTE: sizeof(FlashConfig.Reserved1): 0x%3.3X (%3u)\r", sizeof(FlashConfig.Reserved1), sizeof(FlashConfig.Reserved1));
  uart_send(__LINE__, "NOTE: sizeof(FlashConfig.Reserved2): 0x%3.3X (%3u)\r\r", sizeof(FlashConfig.Reserved2), sizeof(FlashConfig.Reserved2));
  uart_send(__LINE__, "[%X] Firmware version:         %c%c%c%c\r", &FlashConfig.Version, FlashConfig.Version[0], FlashConfig.Version[1], FlashConfig.Version[2], FlashConfig.Version[3]);
  uart_send(__LINE__, "[%X] CurrentYearCentile:        %3u\r", &FlashConfig.CurrentYearCentile, FlashConfig.CurrentYearCentile);
  uart_send(__LINE__, "[%X] Language:                  %3u     (01 = Eng   02 = Fre  03 = Ger  04 - Cze)\r", &FlashConfig.Language, FlashConfig.Language);
  uart_send(__LINE__, "[%X] DSTCountry:                %3u     ( 0 = No DST support   Refer to user guide for all others)\r", &FlashConfig.DSTCountry, FlashConfig.DSTCountry);
  uart_send(__LINE__, "[%X] Timezone:                  %3d\r", &FlashConfig.Timezone, FlashConfig.Timezone);
  uart_send(__LINE__, "[%X] Flag Summer Time status:  0x%2.2X   (0x00 = inactive   0x01 = active)\r", &FlashConfig.FlagSummerTime, FlashConfig.FlagSummerTime);
  uart_send(__LINE__, "[%X] TemperatureUnit:          0x%2.2X   (0x00 = Celsius    0x01 = Fahrenheit)\r", &FlashConfig.TemperatureUnit, FlashConfig.TemperatureUnit);
  uart_send(__LINE__, "[%X] TimeDisplayMode:          0x%2.2X   (0x00 = 12Hours    0x01 = 24Hours)\r", &FlashConfig.TimeDisplayMode, FlashConfig.TimeDisplayMode);
  uart_send(__LINE__, "[%X] ChimeMode:                 %3u     (00 = Off   01 = On   02 = On, Intermittent)\r", &FlashConfig.ChimeMode, FlashConfig.ChimeMode);
  uart_send(__LINE__, "[%X] ChimeTimeOn:               %3u\r", &FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOn);
  uart_send(__LINE__, "[%X] ChimeTimeOff:              %3u\r", &FlashConfig.ChimeTimeOff, FlashConfig.ChimeTimeOff);
  uart_send(__LINE__, "[%X] NightLightMode:            %3u     (00 = Off   01 = On   02 = Night   03 = Auto)\r", &FlashConfig.NightLightMode, FlashConfig.NightLightMode);
  uart_send(__LINE__, "[%X] NightLightTimeOn:          %3u\r", &FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOn);
  uart_send(__LINE__, "[%X] NightLightTimeOff:         %3u\r", &FlashConfig.NightLightTimeOff, FlashConfig.NightLightTimeOff);
  uart_send(__LINE__, "[%X] FlagAutoBrightness:       0x%2.2X     (00 = Off   01 = On)\r", &FlashConfig.FlagAutoBrightness, FlashConfig.FlagAutoBrightness);
  uart_send(__LINE__, "[%X] FlagKeyclick:             0x%2.2X     (00 = Off   01 = On)\r", &FlashConfig.FlagKeyclick, FlashConfig.FlagKeyclick);
  uart_send(__LINE__, "[%X] FlagScrollEnable:         0x%2.2X     (00 = Off   01 = On)\r", &FlashConfig.FlagScrollEnable, FlashConfig.FlagScrollEnable);
  uart_send(__LINE__, "[%X] FlagSummerTime:           0x%2.2X     (00 = Off   01 = On)\r", &FlashConfig.FlagSummerTime, FlashConfig.FlagSummerTime);
  uart_send(__LINE__, "[%X] DimmerMinLightLevel:       %3u\r", &FlashConfig.DimmerMinLightLevel, FlashConfig.DimmerMinLightLevel);
  uart_send(__LINE__, "[%X] ShortSetKey:              0x%2.2X     (00 = Off   01 = On)\r", &FlashConfig.ShortSetKey, FlashConfig.ShortSetKey);
  /* Display Reserved1 data. */
  uart_send(__LINE__, "[%X] Reserved1 - size: 0x%2.2X (%3u):\r  ", &FlashConfig.Reserved1[Loop1UInt16], sizeof(FlashConfig.Reserved1), sizeof(FlashConfig.Reserved1));
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved1); ++Loop1UInt16)
  {
    uart_send(__LINE__, "- 0x%2.2X ", FlashConfig.Reserved1[Loop1UInt16]);
    if (((Loop1UInt16 + 1) % 10) == 0)
      uart_send(__LINE__, "\r  ");
  }
  uart_send(__LINE__, "\r");


  /* Display all 7 days-of-week with its corresponding bitmask. */
  uart_send(__LINE__, "Bit mask used for alarm DayOfWeek selection:\r");
  for (Loop1UInt16 = 1; Loop1UInt16 < 8; ++Loop1UInt16)
  {
    /* Get binary string representing day mask for this particular day-of-week. */
    uint64_to_binary_string(1 << Loop1UInt16, 8, String);

    /* Force English if flash configuration has not already been read. */
    if ((FlashConfig.Language > LANGUAGE_LO_LIMIT) && (FlashConfig.Language < LANGUAGE_HI_LIMIT))
      sprintf(&String[strlen(String)], "   %s\r", DayName[FlashConfig.Language][Loop1UInt16]);
    else
      sprintf(&String[strlen(String)], "   %s\r", DayName[ENGLISH][Loop1UInt16]);
    uart_send(__LINE__, String);
  }
  uart_send(__LINE__, "\r");


  /* Scan and display all alarms (0 to 8) -> (1 to 9 for clock user). */
  for (Loop1UInt16 = 0; Loop1UInt16 < 9; ++Loop1UInt16)
  {
    uart_send(__LINE__, "[%X] Alarm[%2.2u].Status:         0x%2.2X     (00 = Off   01 = On)\r", &FlashConfig.Alarm[Loop1UInt16].FlagStatus, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].FlagStatus);
    uart_send(__LINE__, "[%X] Alarm[%2.2u].Hour:            %3u\r", &FlashConfig.Alarm[Loop1UInt16].Hour, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].Hour);
    uart_send(__LINE__, "[%X] Alarm[%2.2u].Minute:          %3u\r", &FlashConfig.Alarm[Loop1UInt16].Minute, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].Minute);
    uart_send(__LINE__, "[%X] Alarm[%2.2u].Second:          %3u\r", &FlashConfig.Alarm[Loop1UInt16].Second, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].Second);
    uart_send(__LINE__, "[%X] Alarm[%2.2u].Jingle:          %3u\r", &FlashConfig.Alarm[Loop1UInt16].Jingle, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].Jingle);

    uint64_to_binary_string(FlashConfig.Alarm[Loop1UInt16].Day, 8, DayMask);
    sprintf(String, "[%8.8X] Alarm[%2.2u].DayMask:    %s     (0x%2.2X) ", &FlashConfig.Alarm[Loop1UInt16].Day, Loop1UInt16, DayMask, FlashConfig.Alarm[Loop1UInt16].Day);

    for (Loop2UInt16 = 1; Loop2UInt16 < 8; ++Loop2UInt16)
    {
      if (FlashConfig.Alarm[Loop1UInt16].Day & (1 << Loop2UInt16))
      {
        Dum1UInt8 = strlen(String);
        sprintf(&String[strlen(String)], "%s", DayName[FlashConfig.Language][Loop2UInt16]);
        String[Dum1UInt8 + 3] = 0x20;  // space separator.
        String[Dum1UInt8 + 4] = 0x00;  // keep first 3 characters of day name.
      }
    }
    strcat(String, "\r\r");
    uart_send(__LINE__, String);
  }


  uart_send(__LINE__, "Note: Host, SSID and Password begin at 5th character position, superimposed on top of two different footprints.\r");
  uart_send(__LINE__, "      There is also an end-of-string character at the end of each string that we don't see on the screen.\r");
  /* In case hostname is not initialized, display it character by character. */
  sprintf(String, "HOST:     [");
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Hostname); ++Loop1UInt16)
    sprintf(&String[strlen(String)], "%c", FlashConfig.Hostname[Loop1UInt16]);
  strcat(String, "]\r");
  uart_send(__LINE__, String);


  /* In case SSID is not initialized, display it character by character. */
  sprintf(String, "SSID:     [");
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.SSID); ++Loop1UInt16)
    sprintf(&String[strlen(String)], "%c", FlashConfig.SSID[Loop1UInt16]);
  strcat(String, "]\r");
  uart_send(__LINE__, String);


  /* In case password is not initialized, display it character by character. */
  sprintf(String, "Password: [");
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Password); ++Loop1UInt16)
    sprintf(&String[strlen(String)], "%c", FlashConfig.Password[Loop1UInt16]);
  strcat(String, "]\r\r");
  uart_send(__LINE__, String);


  /* Display Reserved2 data. */
  uart_send(__LINE__, "[%X] Reserved2 - size 0x%2.2X (%3u):\r  ", &FlashConfig.Reserved2[Loop1UInt16], sizeof(FlashConfig.Reserved2), sizeof(FlashConfig.Reserved2));
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved2); ++Loop1UInt16)
  {
    sprintf(String, "- 0x%2.2X ", FlashConfig.Reserved2[Loop1UInt16]);
    uart_send(__LINE__, String);

    if (((Loop1UInt16 + 1) % 10) == 0)
      uart_send(__LINE__, "\r  ");
  }
  uart_send(__LINE__, "\r\r");

  uart_send(__LINE__, "[%X] CRC16: 0x%4.4X\r\r\r", &FlashConfig.Crc16, FlashConfig.Crc16);
  uart_send(__LINE__, "Size of data for CRC16: %u - %u = %u\r", &FlashConfig.Crc16, &FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  uart_send(__LINE__, "(in hex: 0x%8.8X - 0x%8.8X = 0x%8.8X)\r", &FlashConfig.Crc16, &FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  uart_send(__LINE__, "=========================================================================================================\r\r\r");

  return 0;
}





/* $PAGE */
/* $TITLE=flash_erase() */
/* ------------------------------------------------------------------ *\
                  Erase data in Pico's flash memory.
     NOTES:
     - The way the electronics is done, one sector of the flash
       (4096 bytes) must be erased at a time.
     - This function has been kept simple and one sector (4096 bytes)
       will be erased, beginning at the specified offset, which must
       be aligned on a sector boundary (4096).
\* ------------------------------------------------------------------ */
void flash_erase(UINT32 FlashMemoryOffset)
{
  UCHAR String[256];

  UINT8 OriginalClockMode;

  UINT16 Loop1UInt16;

  UINT32 InterruptMask;


  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "Entering flash_erase(offset: 0x%8.8X)\r", FlashMemoryOffset);

  /* Erase an area of the Pico's flash memory.
     NOTES:
     FlashMemoryOffset is the offset in flash memory (relative to XIP_BASE) and must be aligned on a sector boundary (a multiple of 4096).
     The second parameter must be a multiple of a sector size (a multiple of 4096). */
  if (FlashMemoryOffset % FLASH_SECTOR_SIZE)
  {
    if (DebugBitMask & DEBUG_FLASH)
      uart_send(__LINE__, "Offset specified for flash_erase(0x%8.8X) is not aligned on a sector boundary (multiple of 4096)\r", FlashMemoryOffset);

    while (FlashMemoryOffset % FLASH_SECTOR_SIZE)
    {
      ++FlashMemoryOffset;
    }

    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Offset has been shifted to 0x%8.8X\r", FlashMemoryOffset);
      sleep_ms(100);  // wait for UART before disabling interrupts.
    }
  }


  /* NOTE: Caller is responsible for dimming clock display while interrupts will be disabled. */
  /* Keep track of interrupt mask on entry. */
  InterruptMask = save_and_disable_interrupts();

  /* Erase flash area to reprogram. */
  flash_range_erase(FlashMemoryOffset, FLASH_SECTOR_SIZE);

  /* Restore original interrupt mask when done. */
  restore_interrupts(InterruptMask);


  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "Exiting  flash_erase()\r\r\r");

  return;
}





/* $PAGE */
/* $TITLE=flash_read_config() */
/* ------------------------------------------------------------------ *\
                   Read Green Clock from flash memory.
\* ------------------------------------------------------------------ */
UINT8 flash_read_config(void)
{
  UCHAR String[256];

  UINT16 Dum1UInt16;
  UINT8 *FlashBaseAddress;
  UINT16 Loop1UInt16;


  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Entering flash_read_config()\r");
    uart_send(__LINE__, "sizeof(FlashConfig): 0x%X (%u)\r", sizeof(FlashConfig), sizeof(FlashConfig));
  }

  /* Read Green Clock configuration from Pico's flash memory (as an array of UINT8). */
  FlashBaseAddress = (UINT8 *)(XIP_BASE);
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig); ++Loop1UInt16)
  {
    ((UINT8 *)(FlashConfig.Version))[Loop1UInt16] = FlashBaseAddress[FLASH_CONFIG_OFFSET + Loop1UInt16];
  }

  /* Display configuration values retrieved from flash memory. */
  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Display Green Clock configuration retrieved from flash memory:\r");
    flash_display_config();
  }

  /* Validate CRC16 read from flash. */
  Dum1UInt16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);

  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "CRC16 saved in flash configuration: 0x%X (%u)\r", FlashConfig.Crc16, FlashConfig.Crc16);
    uart_send(__LINE__, "CRC16 computed from data read:      0x%X (%u)\r", Dum1UInt16, Dum1UInt16);
  }

  /* If CRC16 read from flash does not correspond to the data read, assign default values and save new configuration. */
  if (FlashConfig.Crc16 == Dum1UInt16)
  {
    if (DebugBitMask & DEBUG_FLASH)
      uart_send(__LINE__, "Flash configuration is valid.\r\r\r");

    return 0;
  }

  if (FlashConfig.Crc16 == 0xFFFF)
  {
    /* CRC16 is not valid. If CRC16 read from flash is 0xFFFF, we assume that no configuration has ever been saved to flash memory.
       If so, assign default values to configuration parameters and save it to flash. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Flash configuration has never been initialized...\r");
      uart_send(__LINE__, "Save a default configuration to flash.\r\r\r");
    }
  }
  else
  {
    /* Configuration read from flash seems to be corrupted. Save a valid default configuration. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Flash configuration seems to be corrupted...\r");
      uart_send(__LINE__, "Save a default configuration to flash.\r\r\r");
    }
  }

  /* Assign default values and save a new configuration to flash. */
  sprintf(FlashConfig.Version, "%s", FIRMWARE_VERSION);   // firmware version number.
  FlashConfig.CurrentYearCentile = 20;                    // assume we are in years 20xx. Green Clock was always reverting to 20.
  FlashConfig.Language           = DEFAULT_LANGUAGE;      // hourly chime will begin at this hour.
  FlashConfig.DSTCountry         = DST_COUNTRY;           // specifies how to handle the daylight saving time depending of country (see User Guide).
  FlashConfig.Timezone           = 0;                     // time difference between local time and Universal Coordinated Time.
  FlashConfig.FlagSummerTime     = FLAG_OFF;              // system will evaluate and overwrite this value on next power-up sequence.
  FlashConfig.TemperatureUnit    = TEMPERATURE_DEFAULT;   // CELSIUS or FAHRENHEIT default value (see clock options above).
  FlashConfig.TimeDisplayMode    = TIME_DISPLAY_DEFAULT;  // H24 or H12 default value (see clock options above).
  FlashConfig.ChimeMode          = CHIME_DEFAULT;         // chime mode (Off / On / Day).
  FlashConfig.ChimeTimeOn        = CHIME_TIME_ON;         // hourly chime will begin at this hour.
  FlashConfig.ChimeTimeOff       = CHIME_TIME_OFF;        // hourly chime will begin at this hour.
  FlashConfig.NightLightMode     = NIGHT_LIGHT_DEFAULT;   // night light mode (On / Off / Auto / Night).
  FlashConfig.NightLightTimeOn   = NIGHT_LIGHT_TIME_ON;   // default night light time on.
  FlashConfig.NightLightTimeOff  = NIGHT_LIGHT_TIME_OFF;  // default night light time off.
  FlashConfig.FlagAutoBrightness = FLAG_ON;               // flag indicating we are in "Auto Brightness" mode.
  FlashConfig.FlagKeyclick       = FLAG_ON;               // flag for keyclick ("button-press" tone)
  FlashConfig.FlagScrollEnable   = SCROLL_DEFAULT;        // flag indicating the clock will scroll the date and temperature at regular intervals on the display.
  FlashConfig.DimmerMinLightLevel= 225;                   // lowest average light level for dimmest display level
  FlashConfig.ShortSetKey        = FLAG_ON;               // flag to say if the set key short press should set the time or the alarm

  /* Make provision for future parameters. */
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved1); ++Loop1UInt16)
  {
    FlashConfig.Reserved1[Loop1UInt16] = 0xFF;
  }

  /* Default configuration for 9 alarms. Text may be changed for another 40-characters max string. */

  FlashConfig.Alarm[0].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[0].Day    = (1 << MON) + (1 << TUE) + (1 << WED) + (1 << THU) + (1 << FRI);
  FlashConfig.Alarm[0].Hour   = 8;
  FlashConfig.Alarm[0].Minute = 00;
  FlashConfig.Alarm[0].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[0].Jingle = 0;
  sprintf(FlashConfig.Alarm[0].Text, "Alarm 1"); // string to be scrolled when alarm 1 is triggered (ALARM TEXT).

  FlashConfig.Alarm[1].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[1].Day    = (1 << SAT) + (1 << SUN);
  FlashConfig.Alarm[1].Hour   = 14;
  FlashConfig.Alarm[1].Minute = 37;
  FlashConfig.Alarm[1].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[1].Jingle = 0;
  sprintf(FlashConfig.Alarm[1].Text, "Alarm 2"); // string to be scrolled when alarm 2 is triggered (ALARM TEXT).

  FlashConfig.Alarm[2].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[2].Day    = (1 << MON);
  FlashConfig.Alarm[2].Hour   = 14;
  FlashConfig.Alarm[2].Minute = 36;
  FlashConfig.Alarm[2].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[2].Jingle = 0;
  sprintf(FlashConfig.Alarm[2].Text, "Alarm 3"); // string to be scrolled when alarm 3 is triggered (ALARM TEXT).

  FlashConfig.Alarm[3].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[3].Day    = (1 << TUE);
  FlashConfig.Alarm[3].Hour   = 14;
  FlashConfig.Alarm[3].Minute = 35;
  FlashConfig.Alarm[3].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[3].Jingle = 0;
  sprintf(FlashConfig.Alarm[3].Text, "Alarm 4"); // string to be scrolled when alarm 4 is triggered (ALARM TEXT).

  FlashConfig.Alarm[4].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[4].Day    = (1 << WED);
  FlashConfig.Alarm[4].Hour   = 14;
  FlashConfig.Alarm[4].Minute = 34;
  FlashConfig.Alarm[4].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[4].Jingle = 0;
  sprintf(FlashConfig.Alarm[4].Text, "Alarm 5"); // string to be scrolled when alarm 5 is triggered (ALARM TEXT).

  FlashConfig.Alarm[5].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[5].Day    = (1 << THU);
  FlashConfig.Alarm[5].Hour   = 14;
  FlashConfig.Alarm[5].Minute = 33;
  FlashConfig.Alarm[5].Second = 29;
  FlashConfig.Alarm[5].Jingle = 0;
  sprintf(FlashConfig.Alarm[5].Text, "Alarm 6"); // string to be scrolled when alarm 6 is triggered (ALARM TEXT).

  FlashConfig.Alarm[6].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[6].Day    = (1 << FRI);
  FlashConfig.Alarm[6].Hour   = 14;
  FlashConfig.Alarm[6].Minute = 32;
  FlashConfig.Alarm[6].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[6].Jingle = 0;
  sprintf(FlashConfig.Alarm[6].Text, "Alarm 7"); // string to be scrolled when alarm 7 is triggered (ALARM TEXT).

  FlashConfig.Alarm[7].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[7].Day    = (1 << SAT);
  FlashConfig.Alarm[7].Hour   = 14;
  FlashConfig.Alarm[7].Minute = 31;
  FlashConfig.Alarm[7].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[7].Jingle = 0;
  sprintf(FlashConfig.Alarm[7].Text, "Alarm 8"); // string to be scrolled when alarm 8 is triggered (ALARM TEXT).

  FlashConfig.Alarm[8].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[8].Day    = (1 << SUN);
  FlashConfig.Alarm[8].Hour   = 14;
  FlashConfig.Alarm[8].Minute = 30;
  FlashConfig.Alarm[8].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  FlashConfig.Alarm[8].Jingle = 0;
  sprintf(FlashConfig.Alarm[8].Text, "Alarm 9"); // string to be scrolled when alarm 9 is triggered (ALARM TEXT).

  sprintf(FlashConfig.Hostname, ".@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.@.");                                // write specific footprint to flash memory.
  sprintf(FlashConfig.SSID,     ".;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.;.");                                // write specific footprint to flash memory.
  sprintf(FlashConfig.Password, ".:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.");  // write specific footprint to flash memory.
  sprintf(&FlashConfig.Hostname[4], PICO_HOSTNAME);
  sprintf(&FlashConfig.SSID[4],     NETWORK_NAME);
  sprintf(&FlashConfig.Password[4], NETWORK_PASSWORD);

  /* Make provision for future parameters. */
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved2); ++Loop1UInt16)
  {
    FlashConfig.Reserved2[Loop1UInt16] = 0xFF;
  }

  /* Save the default configuration assigned above. */
  flash_save_config();

  return 1;
}





/* $PAGE */
/* $TITLE=flash_save_config() */
/* ------------------------------------------------------------------ *\
            Save current Green Clock configuration to flash.
\* ------------------------------------------------------------------ */
UINT8 flash_save_config(void)
{
  UCHAR String[256];

  UINT16 Loop1UInt16;


  /* Calculate CRC16 to include it in the packet being flashed. */
  FlashConfig.Crc16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);

  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "=========================================================================================================\r");
    uart_send(__LINE__, "                             SAVING current Green Clock configuration to flash.\r");
    uart_send(__LINE__, "                                Pointer to FlashConfig.Version: 0x%8.8X\r", FlashConfig.Version);
    uart_send(__LINE__, "                                Pointer to FlashConfig.Crc16:   0x%8.8X\r", &FlashConfig.Crc16);
    uart_send(__LINE__, "                           &FlashConfig.Crc16 - &FlashConfig.Version: 0x%X (%u)\r", (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
    uart_send(__LINE__, "=========================================================================================================\r");

    /* Display configuration being saved. */
    flash_display_config();
  }

  flash_write(FLASH_CONFIG_OFFSET, (UINT8 *)FlashConfig.Version, sizeof(FlashConfig));

  /* Display flash configuration as saved. Will crash the clock if done inside a callback. *
  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Display flash configuration as saved (including 64 bytes guard before and after target area):\r");
    flash_display((FLASH_CONFIG_OFFSET - 64), (4096 + 128));
  }
  */

  return 0;
}





/* $PAGE */
/* $TITLE=flash_write() */
/* ------------------------------------------------------------------ *\
                    Write data to Pico's flash memory.
     To keep things simple in the code, always one sector is updated
                            in the flash.
\* ------------------------------------------------------------------ */
UINT flash_write(UINT32 NewDataOffset, UINT8 NewData[], UINT16 NewDataSize)
{
  UCHAR String[256];

  UINT16 CurrentCycles;
  UINT8 *FlashBaseAddress;
  UINT8 OriginalClockMode;

  UINT16 Loop1UInt16;

  UINT32 SectorOffset;
  UINT32 InterruptMask;


  /* NOTE: DEBUG_FLASH will work fine while writing to flash before starting the callback_s function.
           If flash_write() is called by callback_s while DEBUG_FLASH is On, garbage will show up on the screen and
           firmware will crash. Logging must be kept to a minimum if logging is to be used inside flash_write() while
           called by callback_s. */
  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Entering flash_write()   Data offset: 0x%X   Data size: 0x%X (%u)\r", NewDataOffset, NewDataSize, NewDataSize);
    uart_send(__LINE__, "Display the data to write to flash.\r");
    display_data(NewData, NewDataSize);
  }

  SectorOffset  = NewDataOffset;  // assume offset specified is on a sector boundary.
  NewDataOffset = 0;              // assume that new data will be at the beginning of sector.
  if (SectorOffset % FLASH_SECTOR_SIZE)
  {
    /* Offset specified is not aligned on a sector boundary. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "New data offset specified (0x%6.6X) is not aligned on a sector boundary (multiple of 4096)\r", SectorOffset);
      uart_send(__LINE__, "Phased out by 0x%X (%u) bytes.\r", SectorOffset % FLASH_SECTOR_SIZE, SectorOffset % FLASH_SECTOR_SIZE);
    }

    while (SectorOffset % FLASH_SECTOR_SIZE)
    {
      --SectorOffset;
      ++NewDataOffset;
    }

    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Sector offset has been shifted down to 0x%6.6X\r", SectorOffset);
      uart_send(__LINE__, "And new data offset has been shifted up to 0x%X (%u)\r", NewDataOffset, NewDataOffset);
    }
  }

  if (NewDataOffset + NewDataSize > 4096)
  {
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "The arguments given cross a sector boundary which is not allowed...\r");
      uart_send(__LINE__, "Sector offset: %X   New data offset: %X  New data size: %X\r\r\r", SectorOffset, NewDataOffset, NewDataSize);
    }

    return 1;
  }

  /* A wear levelling algorithm has not been implemented since the flash usage for configuration parameters doesn't require it.
     However, flash write should not be use for intensive data logging without adding a wear levelling algorithm. */
  FlashBaseAddress = (UINT8 *)(XIP_BASE);

  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "FlashBaseAddress: %p   Sector offset: %X   Target offset in sector: %X\r", FlashBaseAddress, SectorOffset, NewDataOffset);

  /* Take a copy of current flash content. */
  for (Loop1UInt16 = 0; Loop1UInt16 < FLASH_SECTOR_SIZE; ++Loop1UInt16)
    FlashData[Loop1UInt16] = FlashBaseAddress[SectorOffset + Loop1UInt16];

  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Display original data retrieved from flash\r");
    display_data(FlashData, FLASH_SECTOR_SIZE);
  }

  /* Overwrite the memory area that we want to save. */
  memcpy(&FlashData[NewDataOffset], NewData, NewDataSize);

  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Data to be written to flash offset %X:\r", SectorOffset);
    display_data(FlashData, FLASH_SECTOR_SIZE);
  }


  /* Blank display so that we don't see frozen display while interrupts are disabled. */
  CurrentCycles = Pwm[PWM_BRIGHTNESS].Cycles;
  pwm_set_cycles(0);

  /* Erase flash before reprogramming. */
  flash_erase(SectorOffset);

  /* Keep track of interrupt mask and disable interrupts during flash writing. */
  InterruptMask = save_and_disable_interrupts();

  /* Save data to flash memory. */
  flash_range_program(SectorOffset, FlashData, FLASH_SECTOR_SIZE);

  /* Restore original interrupt mask when done. */
  restore_interrupts(InterruptMask);

  /* Restore display when done. */
  pwm_set_cycles(CurrentCycles);

  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "Exiting flash_write()\r\r\r");

  return 0;
}





/* $PAGE */
/* $TITLE=format_temp() */
/* ------------------------------------------------------------------ *\
         Format string to display temperature on clock display.
\* ------------------------------------------------------------------ */
void format_temp(UCHAR *TempString, UCHAR *PreString, float Temperature, float Humidity, float Pressure)
{
  /* Write temperature to the string to be displayed... */
  if (FlashConfig.TemperatureUnit == CELSIUS)
  {
    /* ...in Celsius. */
    sprintf(TempString, "%s%2.2f%cC", PreString, Temperature, 0x80);
  }
  else
  {
    /* ...or in Fahrenheit. */
    sprintf(TempString, "%s%2.2f%cF", PreString, Temperature, 0x80);
  }

  if (Humidity != 0)
  {
    switch (FlashConfig.Language)
    {
      case (CZECH):
        sprintf(&TempString[strlen(TempString)], "  vlh: %2.2f%%", Humidity);
      break;

      case (ENGLISH):
      case (FRENCH):
      default:
        sprintf(&TempString[strlen(TempString)], "  Hum: %2.2f%%", Humidity);
      break;
    }
  }

  if (Pressure != 0)
  {
    switch (FlashConfig.Language)
    {
      case (CZECH):
        sprintf(&TempString[strlen(TempString)], "  tlak: %2.2f%% hPa.", Pressure);
      break;

      case (FRENCH):
        sprintf(&TempString[strlen(TempString)], "  Pression: %2.2f%% hPa", Pressure);
      break;

      case (SPANISH):
        sprintf(&TempString[strlen(TempString)], "  Presion: %2.2f%% hPa", Pressure);
      break;

      default:
      case (ENGLISH):
        sprintf(&TempString[strlen(TempString)], "  Pressure: %2.2f%% hPa", Pressure);
      break;
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=get_ambient_temperature() */
/* ------------------------------------------------------------------ *\
      Get temperature from DS3231 RTC IC using I2C communication.
\* ------------------------------------------------------------------ */
float get_ambient_temperature(UINT8 TemperatureUnit)
{
  UCHAR StartTran[2] = {0x0E, 0x20};
  UCHAR String[256];
  UCHAR TempPart1;
  UCHAR TempPart2;
  UCHAR TempString[16];

  float Temperature;


  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, StartTran,   2, FALSE);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &GetAddHigh, 1, TRUE);
  i2c_read_blocking (I2C_PORT, DS3231_ADDRESS, &TempPart1,  1, FALSE);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &GetAddLow,  1, TRUE);
  i2c_read_blocking (I2C_PORT, DS3231_ADDRESS, &TempPart2,  1, FALSE);
  TempPart2 = (TempPart2 >> 6) * 25;

  sprintf(TempString, "%u.%u", TempPart1, TempPart2);
  Temperature = strtof(TempString, NULL);

  if (TemperatureUnit == FAHRENHEIT) Temperature = ((Temperature * 9 / 5) + 32);

  /* Use DEBUG_DHT since it is temperature related. */
  if (DebugBitMask & DEBUG_DHT)
    uart_send(__LINE__, "Temperature from DS3231: TempString: %s   Temperature: %2.2f ", TempString, Temperature);

  return Temperature;
}





/* $PAGE */
/* $TITLE=get_date_string() */
/* ------------------------------------------------------------------ *\
                   Read DS3231 real-time clock IC
           and return current time in "human_time" format.
\* ------------------------------------------------------------------ */
void get_current_time(struct human_time *HumanTime) {
  TIME_RTC RealTimeCLock;

  RealTimeCLock = Read_RTC();

  HumanTime->Hour        = bcd_to_byte(RealTimeCLock.hour);
  HumanTime->Minute      = bcd_to_byte(RealTimeCLock.minutes);
  HumanTime->Second      = bcd_to_byte(RealTimeCLock.seconds);
  HumanTime->DayOfMonth  = bcd_to_byte(RealTimeCLock.dayofmonth);
  HumanTime->Month       = bcd_to_byte(RealTimeCLock.month);
  if (FlashConfig.CurrentYearCentile != 0)
    HumanTime->Year      = (FlashConfig.CurrentYearCentile * 100) + bcd_to_byte(RealTimeCLock.year);
  else
    HumanTime->Year      = 2000 + bcd_to_byte(RealTimeCLock.year);
  HumanTime->DayOfWeek   = get_day_of_week(HumanTime->Year, HumanTime->Month, HumanTime->DayOfMonth);
  HumanTime->DayOfYear   = get_day_of_year(HumanTime->Year, HumanTime->Month, HumanTime->DayOfMonth);

  /*** Add FlagDst support. ***/

  /* Make basic validation in case real-clock IC contains invalid values. */
  if (HumanTime->Hour   > 23)     HumanTime->Hour   = 0;
  if (HumanTime->Minute > 59)     HumanTime->Minute = 0;
  if (HumanTime->Second > 59)     HumanTime->Second = 0;
  if (HumanTime->DayOfMonth > 31) HumanTime->DayOfMonth = 1;
  if (HumanTime->Month      > 12) HumanTime->Month      = 1;
  if ((HumanTime->DayOfWeek < 1) || (HumanTime->DayOfWeek > 7)) HumanTime->DayOfWeek = 1;
  if ((HumanTime->DayOfYear < 1) || (HumanTime->DayOfYear > 7)) HumanTime->DayOfYear = 1;

  if (DebugBitMask & DEBUG_RTC)
  {
    printf("[%u]get_current_time()\r\r", __LINE__);
    printf("HumanTime->Hour   %2.2u\r",     HumanTime->Hour);
    printf("HumanTime->Minute %2.2u\r",     HumanTime->Minute);
    printf("HumanTime->Second %2.2u\r\r",   HumanTime->Second);
    printf("HumanTime->DoM    %2.2u\r",     HumanTime->DayOfMonth);
    printf("HumanTime->Month  %2.2u\r",     HumanTime->Month);
    printf("HumanTime->Year   %2.2u\r",     HumanTime->Year);
    printf("HumanTime->DoW    %2.2u\r",     HumanTime->DayOfWeek);
    printf("HumanTime->DoY    %2.2u\r\r\r", HumanTime->DayOfYear);
  }

  return;
}





/* $PAGE */
/* $TITLE=get_date_string() */
/* ------------------------------------------------------------------ *\
                Build the string containing the date
                  to be scrolled on clock display.
\* ------------------------------------------------------------------ */
void get_date_string(UCHAR *String) {
  UCHAR Suffix[3];

  UINT8 DumDayOfMonth;
  UINT8 DumMonth;
  UINT8 DumYearLowPart;
  UINT8 TemperatureF;

  UINT16 Loop1UInt16;


  /* Wipe string on entry. */
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(String); ++Loop1UInt16)
    String[Loop1UInt16] = 0x00;

  /* Read the real-time clock IC DS3231. */
  Time_RTC         = Read_RTC();
  DumMonth         = bcd_to_byte(Time_RTC.month);
  DumDayOfMonth    = bcd_to_byte(Time_RTC.dayofmonth);
  DumYearLowPart   = bcd_to_byte(Time_RTC.year);
  CurrentDayOfWeek = get_day_of_week(((FlashConfig.CurrentYearCentile * 100) + DumYearLowPart), DumMonth, DumDayOfMonth);


  if (FlashConfig.Language == CZECH)
  {
    /* Day-of-week name. */
    sprintf(String, "%s ", DayName[FlashConfig.Language][CurrentDayOfWeek]);

    /* Add Day-of-month. */
    sprintf(&String[strlen(String)], "%X.", Time_RTC.dayofmonth);

    /* Add month number. */
    sprintf(&String[strlen(String)], "%u.", DumMonth);

    /* Add 4-digits year. */
    sprintf(&String[strlen(String)], "%2.2u%2.2u  ", FlashConfig.CurrentYearCentile, CurrentYearLowPart);
  }


  if (FlashConfig.Language == ENGLISH)
  {
    /* DayOfWeek and month name. */
    sprintf(String, "%s %s", DayName[FlashConfig.Language][CurrentDayOfWeek], MonthName[FlashConfig.Language][DumMonth]);

    /* Find suffix to add to day-of-month. */
    switch (DumDayOfMonth)
    {
      case (1):
      case (21):
      case (31):
        sprintf(Suffix, "st");
      break;

      case (2):
      case (22):
        sprintf(Suffix, "nd");
      break;

      case (3):
      case (23):
        sprintf(Suffix, "rd");
      break;

      default:
        sprintf(Suffix, "th");
      break;
    }

    /* DayOfMonth and its suffix, then the 4-digits year. */
    sprintf(&String[strlen(String)], " %X%s %2.2u%2.2u ", Time_RTC.dayofmonth, Suffix, FlashConfig.CurrentYearCentile, CurrentYearLowPart);
  }


  if ((FlashConfig.Language == FRENCH) || (FlashConfig.Language == SPANISH))
  {
    /* Day-of-week name. */
    sprintf(String, "%s ", DayName[FlashConfig.Language][CurrentDayOfWeek]);

    /* Add Day-of-month. */
    if (Time_RTC.dayofmonth == 1)
      sprintf(&String[strlen(String)], "%Xer ", Time_RTC.dayofmonth);  // first of month.
    else
      sprintf(&String[strlen(String)], "%X ", Time_RTC.dayofmonth);    // other dates.

    /* Add month name. */
    sprintf(&String[strlen(String)], "%s ", MonthName[FlashConfig.Language][DumMonth]);

    /* Add 4-digits year. */
    sprintf(&String[strlen(String)], " %2.2u%2.2u  ", FlashConfig.CurrentYearCentile, CurrentYearLowPart);
  }


  if (FlashConfig.Language == GERMAN)
  {
    /* DayOfWeek and month name. */
    sprintf(String, "%s %X. %s", DayName[FlashConfig.Language][CurrentDayOfWeek], Time_RTC.dayofmonth, MonthName[FlashConfig.Language][DumMonth]);

    /* Add 4-digits year. */
    sprintf(&String[strlen(String)], " %2.2u%2.2u  ", FlashConfig.CurrentYearCentile, CurrentYearLowPart);
  }

  return;
}





/* $PAGE */
/* $TITLE=get_day_of_week() */
/* ------------------------------------------------------------------ *\
                       Return the DayOfWeek,
               given the day-of-month, month and year
      SUN: 1   MON: 2   TUE: 3   WED: 4   THU: 5   FRI: 6   SAT: 7

   https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Implementation-dependent_methods
\* ------------------------------------------------------------------ */
UINT8 get_day_of_week(UINT16 Year, UINT8 Month, UINT8 DayOfMonth)
{
  UINT8 DayOfWeek;


  if (Month == 1 || Month == 2)
  {
    Month += 12;
    --Year;
  }

  DayOfWeek = (DayOfMonth + 1 + 2 * Month + 3 * (Month + 1) / 5 + Year + Year / 4 - Year / 100 + Year / 400) % 7;

  return DayOfWeek + 1;
}





/* $PAGE */
/* $TITLE=get_day_of_year() */
/* ------------------------------------------------------------------ *\
         Determine the day-of-year of date given in argument.
\* ------------------------------------------------------------------ */
UINT16 get_day_of_year(UINT16 YearNumber, UINT8 MonthNumber, UINT8 DayNumber)
{
  UCHAR String[256];

  UINT8 Loop1UInt8;

  UINT16 TargetDayNumber;


  /* Initializations. */
  TargetDayNumber = 0;


  /* Add up all complete months. */
  for (Loop1UInt8 = 1; Loop1UInt8 < MonthNumber; ++Loop1UInt8)
  {
    TargetDayNumber += get_month_days(YearNumber, Loop1UInt8);

    if (DebugBitMask & DEBUG_DST)
    {
      // uart_send(__LINE__, "Adding month %u [%3s] TargetDayNumber += %u (cumulative: %u)\r", Loop1UInt8, ShortMonth[ENGLISH][Loop1UInt8], get_month_days(YearNumber, Loop1UInt8), TargetDayNumber);
    }
  }

  /* Then add days of the last, partial month. */
  if (DebugBitMask & DEBUG_DST)
  {
    // uart_send(__LINE__, "Final DayNumber after adding final partial month: (%u + %u)\r\r\r", TargetDayNumber, DayNumber, TargetDayNumber + DayNumber);
  }

  TargetDayNumber += DayNumber;

  return TargetDayNumber;
}




/* $PAGE */
/* $TITLE=get_dst_days() */
/* ------------------------------------------------------------------ *\
      Determine the day-of-year for DST start day and DST end day.
\* ------------------------------------------------------------------ */
void get_dst_days(void)
{
  UCHAR String[256];

  UINT8 DaysInMonth;
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 TargetDayOfWeek;


  /* ------------------------------------------------------------------------ *\
                    Calculate DayOfYear for DST start date.
  \* ------------------------------------------------------------------------ */
  /* Total number of days in target month. */
  DaysInMonth = get_month_days(CurrentYear, DstParameters[FlashConfig.DSTCountry].StartMonth);

  if (DebugBitMask & DEBUG_DST)
  {
    uart_send(__LINE__, "Calculating DST start day number for %s (%u to %u) %s-%u \r",
            DayName[ENGLISH][DstParameters[FlashConfig.DSTCountry].StartDayOfWeek],
            DstParameters[FlashConfig.DSTCountry].StartDayOfMonthLow,
            DstParameters[FlashConfig.DSTCountry].StartDayOfMonthHigh,
            ShortMonth[ENGLISH][DstParameters[FlashConfig.DSTCountry].StartMonth],
            CurrentYear);

    uart_send(__LINE__, "DST start month is %s and contains %u days\r", MonthName[ENGLISH][DstParameters[FlashConfig.DSTCountry].StartMonth], DaysInMonth);
  }


  /* Find the DayOfMonth when DST starts. */
  for (Loop1UInt8 = 1; Loop1UInt8 <= DaysInMonth; ++Loop1UInt8)
  {
    TargetDayOfWeek = get_day_of_week(CurrentYear, DstParameters[FlashConfig.DSTCountry].StartMonth, Loop1UInt8);

    if (DebugBitMask & DEBUG_DST)
      uart_send(__LINE__, "Checking day of month: %2u   day of week: %u [%8s]\r", Loop1UInt8, TargetDayOfWeek, DayName[ENGLISH][TargetDayOfWeek]);


    if ((Loop1UInt8 >= DstParameters[FlashConfig.DSTCountry].StartDayOfMonthLow) && (Loop1UInt8 <= DstParameters[FlashConfig.DSTCountry].StartDayOfMonthHigh) && (TargetDayOfWeek == DstParameters[FlashConfig.DSTCountry].StartDayOfWeek))
    {
      if (DebugBitMask & DEBUG_DST)
        uart_send(__LINE__, "Found the starting DayOfMonth: %2u\r", Loop1UInt8);

      break;  // get out of "for" loop when we found DST start day.
    }
  }


  /* Evaluate the DayOfYear for DST starting date. */
  DstParameters[FlashConfig.DSTCountry].StartDayOfYear = get_day_of_year(CurrentYear, DstParameters[FlashConfig.DSTCountry].StartMonth, Loop1UInt8);

  if (DebugBitMask & DEBUG_DST)
  {
    sprintf(String, "Total number of days:  ");
    for (Loop2UInt8 = 1; Loop2UInt8 < DstParameters[FlashConfig.DSTCountry].StartMonth; ++Loop2UInt8)
    {
      sprintf(&String[strlen(String)], "%u + ", get_month_days(CurrentYear, Loop2UInt8));
    }
    sprintf(&String[strlen(String)], "%u\r", Loop1UInt8);
    uart_send(__LINE__, String);

    uart_send(__LINE__, " ----------> StartDayOfYear for DST: %u   %s %2u-%s-%4.4u\r\r\r", DstParameters[FlashConfig.DSTCountry].StartDayOfYear, ShortDay[FlashConfig.Language][DstParameters[FlashConfig.DSTCountry].StartDayOfWeek], Loop1UInt8, ShortMonth[ENGLISH][DstParameters[FlashConfig.DSTCountry].StartMonth], CurrentYear);
  }





  /* ------------------------------------------------------------------------ *\
                    Calculate DayOfYear for DST end date.
  \* ------------------------------------------------------------------------ */
  /* Total number of days in target month. */
  DaysInMonth = get_month_days(CurrentYear, DstParameters[FlashConfig.DSTCountry].EndMonth);

  if (DebugBitMask & DEBUG_DST)
  {
    uart_send(__LINE__, "Calculating DST end day number for %s (%u to %u) %s-%4.4u \r",
            DayName[FlashConfig.Language][DstParameters[FlashConfig.DSTCountry].EndDayOfWeek],
            DstParameters[FlashConfig.DSTCountry].EndDayOfMonthLow,
            DstParameters[FlashConfig.DSTCountry].EndDayOfMonthHigh,
            ShortMonth[ENGLISH][DstParameters[FlashConfig.DSTCountry].EndMonth],
            CurrentYear);

    uart_send(__LINE__, "DST end month is %s and contains %u days\r", MonthName[FlashConfig.Language][DstParameters[FlashConfig.DSTCountry].EndMonth], DaysInMonth);
  }


  /* Find the DayOfMonth when DST ends. */
  for (Loop1UInt8 = 1; Loop1UInt8 <= DaysInMonth; ++Loop1UInt8)
  {
    TargetDayOfWeek = get_day_of_week(CurrentYear, DstParameters[FlashConfig.DSTCountry].EndMonth, Loop1UInt8);

    if (DebugBitMask & DEBUG_DST)
      uart_send(__LINE__, "Checking day of month: %2u  day of week: %u [%8s]\r", Loop1UInt8, TargetDayOfWeek, DayName[FlashConfig.Language][TargetDayOfWeek]);

    if ((Loop1UInt8 >= DstParameters[FlashConfig.DSTCountry].EndDayOfMonthLow) && (Loop1UInt8 <= DstParameters[FlashConfig.DSTCountry].EndDayOfMonthHigh) && (TargetDayOfWeek == DstParameters[FlashConfig.DSTCountry].EndDayOfWeek))
    {
      if (DebugBitMask & DEBUG_DST)
        uart_send(__LINE__, "Found the ending DayOfMonth: %2u\r", Loop1UInt8);

      break;  // get out of "for" loop when we found DST end day.
    }
  }


  /* Evaluate the DayOfYear for DST ending date. */
  DstParameters[FlashConfig.DSTCountry].EndDayOfYear = get_day_of_year(CurrentYear, DstParameters[FlashConfig.DSTCountry].EndMonth, Loop1UInt8);

  if (DebugBitMask & DEBUG_DST)
  {
    sprintf(String, "Total number of days:  ");
    for (Loop2UInt8 = 1; Loop2UInt8 < DstParameters[FlashConfig.DSTCountry].EndMonth; ++Loop2UInt8)
    {
      sprintf(&String[strlen(String)], "%u + ", get_month_days(CurrentYear, Loop2UInt8));
    }
    sprintf(&String[strlen(String)], "%u\r", Loop1UInt8);
    uart_send(__LINE__, String);

    uart_send(__LINE__, " ----------> EndDayOfYear for DST: %u   %s %2u-%s-%4.4u\r", DstParameters[FlashConfig.DSTCountry].EndDayOfYear, ShortDay[FlashConfig.Language][DstParameters[FlashConfig.DSTCountry].EndDayOfWeek], Loop1UInt8, ShortMonth[ENGLISH][DstParameters[FlashConfig.DSTCountry].EndMonth], CurrentYear);
    uart_send(__LINE__, "=========================================================================================================\r\r\r");
  }

  return;
}





/* $PAGE */
/* $TITLE=get_microcontroller_type() */
/* ------------------------------------------------------------------ *\
         Determine if the microcontroller is a Pico or a Pico W
\* ------------------------------------------------------------------ */
UINT8 get_microcontroller_type(void)
{
  UCHAR String[128];

  UINT16 AdcValue1;
  UINT16 AdcValue2;

  float Volts1;
  float Volts2;


  /* Notes - On the Green Clock:
     ADC 0 (gpio 26)  is for photo-resistor (ambient light level).
     ADC 1 (gpio 27)  not used
     ADC 2 (gpio 28)  not used
     ADC 3 (gpio 29)  is for power supply voltage.
     ADC 4 (internal) is internally connected to read Pico's temperature. */

  /* Select power supply input. */
  adc_select_input(3);

  gpio_put(PICO_LED, 1);

  /* Read ADC converter raw value. */
  AdcValue1 = adc_read();

  /* Convert raw value to voltage value. */
  Volts1 = AdcValue1 * (3.3 / (1 << 12));



  /* The important power supply value to consider is when GPIO25 is Low. */
  gpio_put(PICO_LED, 0);

  /* Read ADC converter raw value. */
  AdcValue2 = adc_read();

  /* Convert raw value to voltage value. */
  Volts2 = AdcValue2 * (3.3 / (1 << 12));

  if (DebugBitMask & DEBUG_PICO_W)
    uart_send(__LINE__, "AdcValue1: %4u   Volts1: %4.2f   AdcValue2: %4u   Volts2: %4.2f\r", AdcValue1, Volts1, AdcValue2, Volts2);

  if (Volts2 > 3.0)
    return TYPE_PICO;
  else
    return TYPE_PICO_W;
}





/* $PAGE */
/* $TITLE=get_month_days() */
/* ------------------------------------------------------------------ *\
            Return the number of days of a specific month,
    given the specified year (to know if it is a leap year or not).
\* ------------------------------------------------------------------ */
UINT8 get_month_days(UINT16 CurrentYear, UINT8 MonthNumber)
{
  if (((CurrentYear % 4 == 0) && (CurrentYear % 100 != 0)) || (CurrentYear % 400 == 0))
  {
    switch (MonthNumber)
    {
      case 1:
        return MonthDays[0][0];
      break;

      case 2:
        return MonthDays[0][1];
      break;

      case 3:
        return MonthDays[0][2];
      break;

      case 4:
        return MonthDays[0][3];
      break;

      case 5:
        return MonthDays[0][4];
      break;

      case 6:
        return MonthDays[0][5];
      break;

      case 7:
        return MonthDays[0][6];
      break;

      case 8:
        return MonthDays[0][7];
      break;

      case 9:
        return MonthDays[0][8];
      break;

      case 10:
        return MonthDays[0][9];
      break;

      case 11:
        return MonthDays[0][10];
      break;

      case 12:
        return MonthDays[0][11];
      break;
    }
  }
  else
  {
    switch (MonthNumber)
    {
      case 1:
        return MonthDays[1][0];
      break;

      case 2:
        return MonthDays[1][1];
      break;

      case 3:
        return MonthDays[1][2];
      break;

      case 4:
        return MonthDays[1][3];
      break;

      case 5:
        return MonthDays[1][4];
      break;

      case 6:
        return MonthDays[1][5];
      break;

      case 7:
        return MonthDays[1][6];
      break;

      case 8:
        return MonthDays[1][7];
      break;

      case 9:
        return MonthDays[1][8];
      break;

      case 10:
        return MonthDays[1][9];
      break;

      case 11:
        return MonthDays[1][10];
      break;

      case 12:
        return MonthDays[1][11];
      break;
    }
  }
}





/* $PAGE */
/* $TITLE=get_pico_unique_id() */
/* ------------------------------------------------------------------ *\
              Retrieve Pico's Unique ID from the flash IC.
        It's better to call this function during initialization
                phase, before core 1 begins to run.
\* ------------------------------------------------------------------ */
void get_pico_unique_id(void)
{
  UINT8 Loop1UInt8;

  pico_unique_board_id_t board_id;


  pico_get_unique_board_id(&board_id);

  PicoUniqueId[0] = 0x00;  // initialize as null string.

  /* Build the Unique ID string in hex. */
  for (Loop1UInt8 = 0; Loop1UInt8 < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++Loop1UInt8)
  {
    sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "%2.2X", board_id.id[Loop1UInt8]);
    if ((Loop1UInt8 % 2) && (Loop1UInt8 != 7)) sprintf(&PicoUniqueId[strlen(PicoUniqueId)], "-");
  }

  return;
}





/* $PAGE */
/* $TITLE=init_gpio() */
/* ------------------------------------------------------------------ *\
              GPIO ports initialization / configuration

              GPIO  0 - UART0 - TX.
              GPIO  1 - UART0 - RX.
              GPIO  2 - (In) Top button.
              GPIO  3 - (In) SQW (DS3231 RTC IC).
              GPIO  4 - Not used.
              GPIO  5 - Not used.
              GPIO  6 - (I2C) SDA (Temperature reading).
              GPIO  7 - (I2C) SCL (Temperature reading).
              GPIO  8 - (In) DHT22 (must be added by user... not on the original Green Clock).
              GPIO  9 - (In) Infrared phototransistor to receive remote control commands (must be added by user).
              GPIO 10 - (Out) CLK (LED matrix controller).
              GPIO 11 - (Out) SDI (LED matrix controller).
              GPIO 12 - (Out) LE.
              GPIO 13 - (PWM-Out) OE (Output Enable - Clock display brightness control).
              GPIO 14 - (Out) Active piezo / buzzer (included in Green clock).
              GPIO 15 - (In) Down button.
              GPIO 16 - (Out) A0 (LED matrix brightness control).
              GPIO 17 - (In) Up button.
              GPIO 18 - (Out) A1 (LED matrix brightness control).
              GPIO 19 - (PWM-Out) Passive piezo / buzzer (must be added by user).
              GPIO 20 - Not used.
              GPIO 21 - Not used.
              GPIO 22 - (Out) A2 (LED matrix brightness control).
              GPIO 23 - Used internally for voltage regulation.
              GPIO 24 - Used internally for voltage detection.
              GPIO 25 - On-board Pico LED (different usage on Pico W).
              GPIO 26 - ADC0 (Ambient light reading).
              GPIO 27 - Not used.
              GPIO 28 - Not used.
              GPIO 29 - ADC-Vref (Power supply reading).

\* ------------------------------------------------------------------ */
int init_gpio(void)
{
  /* NOTE: OE (for clock display brightness) and PPIEZO (for passive buzzer) GPIOs are initialized as PWM output on their own in function pwm_initialize(). */

  stdio_init_all();

  /* Refer to the GPIO table above to know which GPIOs are used and for what purpose. */
  gpio_init(A0);
  gpio_init(A1);
  gpio_init(A2);

  gpio_init(CLK);
  gpio_init(LE);
  gpio_init(SDI);
  gpio_init(SQW);

  gpio_set_dir(A0,  GPIO_OUT);
  gpio_set_dir(A1,  GPIO_OUT);
  gpio_set_dir(A2,  GPIO_OUT);

  gpio_set_dir(CLK, GPIO_OUT);
  gpio_set_dir(LE,  GPIO_OUT);
  gpio_set_dir(SDI, GPIO_OUT);
  gpio_set_dir(SQW, GPIO_IN);



  /* GPIOs for clock buttons. */
  gpio_init(SET_BUTTON);
  gpio_init(UP_BUTTON);
  gpio_init(DOWN_BUTTON);

  /* Configure the GPIO associated to the three push-buttons of the clock in "input" mode. */
  gpio_set_dir(SET_BUTTON,  GPIO_IN);
  gpio_set_dir(UP_BUTTON,   GPIO_IN);
  gpio_set_dir(DOWN_BUTTON, GPIO_IN);

  /* Setup a pull-up on those three GPIOs. */
  gpio_pull_up(SET_BUTTON);
  gpio_pull_up(UP_BUTTON);
  gpio_pull_up(DOWN_BUTTON);



  /* Initialize UART0 used to send information to a VT-101 type monitor for debugging purposes. */
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  uart_init(uart0, 921600);
  uart_set_format(uart0, 8, 1, UART_PARITY_NONE);



  /* If "SOUND_DISABLED" is defined, absolutely no sound will be made by the clock. */
  #ifndef SOUND_DISABLED
  /* Original Green-Clock active piezo. */
  gpio_init(BUZZ);
  gpio_set_dir(BUZZ, GPIO_OUT);
  #endif  // SOUND_DISABLED



  /* If user installed an infrared sensor (VS1838B) and support for a remote control. */
  #ifdef IR_SUPPORT
  /* Initialize infrared sensor gpio. */
  gpio_init(IR_RX);

  /* IR sensor will receive IR command from remote control. */
  gpio_set_dir(IR_RX, GPIO_IN);

  /* Line will remain at High level until a signal is received. */
  gpio_pull_up(IR_RX);
  #endif



  /* If user installed a DHT22 external temperature and humidity sensor. */
  #ifdef DHT_SUPPORT
  gpio_init(DHT22);
  gpio_set_dir(DHT22, GPIO_IN); // While idle, keep the line as output.
  gpio_pull_up(DHT22);            // Keep signal high while idle.
  #endif  // DHT_SUPPORT



  /* Inter integrated circuit (I2C) protocol configuration (for DS3231 and optionally for BME280). Set baud rate to 100 kHz.
     Pico will default to "master". */
  i2c_init(I2C_PORT, 100000);
  gpio_set_function(SDA, GPIO_FUNC_I2C); // data line.
  gpio_set_function(SCL, GPIO_FUNC_I2C); // clock line.

  gpio_pull_up(SDA);
  gpio_pull_up(SCL);



  /* Initialize Pico's analog-to-digital (ADC) converter. */
  adc_init();

  /* Disable other functions for these gpio's since they will be used as ADC converter (make sure there is no pull-up either). */
  adc_gpio_init(ADC_LIGHT);  // ambient light.
  adc_gpio_init(ADC_VCC);    // power supply voltage.

  return 0;
}





/* $PAGE */
/* $TITLE=input_string() */
/* ------------------------------------------------------------------ *\
                       Read a string from stdin.
\* ------------------------------------------------------------------ */
void input_string(UCHAR *String)
{
  int8_t DataInput;

  UINT8 Loop1UInt8;


  Loop1UInt8 = 0;
  do
  {
    DataInput = getchar_timeout_us(50000);

    switch (DataInput)
    {
      case (PICO_ERROR_TIMEOUT):
      case (0):
        continue;
      break;

      case (8):
        /* <Backspace> */
        if (Loop1UInt8 > 0)
        {
          --Loop1UInt8;
          String[Loop1UInt8] = 0x00;
          printf("%c %c", 0x08, 0x08);  // erase character under the cursor.
        }
      break;

      case (0x0D):
        /* <Enter> */
        if (Loop1UInt8 == 0)
        {
          String[Loop1UInt8++] = (UCHAR)DataInput;
          String[Loop1UInt8++] = 0x00;
        }
        printf("\r");
      break;

      default:
        printf("%c", (UCHAR)DataInput);
        String[Loop1UInt8] = (UCHAR)DataInput;
        // printf("Loop1UInt8: %3u   %2.2X - %c\r", Loop1UInt8, DataInput, DataInput);  /// for debugging purposes.
        ++Loop1UInt8;
      break;
    }
  } while((Loop1UInt8 < 128) && (DataInput != 0x0D));

  String[Loop1UInt8] = '\0';  // end-of-string
  printf("\r\r\r");

  /* Optionally display each character entered. */
  /***
  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
    printf("%2u:[%2.2X]   ", Loop1UInt8, String[Loop1UInt8]);
  printf("\r");
  ***/

  return;
}





/* $PAGE */
/* $TITLE=isr_signal_trap() */
/* ----------------------------------------------------------------- *\
         Interrupt handler for signal received from IR sensor
                           (type VS1838B).
\* ----------------------------------------------------------------- */
gpio_irq_callback_t isr_signal_trap(UINT8 gpio, UINT32 Events)
{
  if (gpio == IR_RX)
  {
    /* IR line goes from Low to High. */
    if (Events & GPIO_IRQ_EDGE_RISE)
    {
      IrFinalValue[IrStepCount]   = time_us_64();                                                       // this is the final timer value for current Low level.
      IrResultValue[IrStepCount]  = (UINT32)(IrFinalValue[IrStepCount] - IrInitialValue[IrStepCount]);  // calculate duration of current Low level.
      IrLevel[IrStepCount]        = 'L';                                                                // identify  as Low level.
      ++IrStepCount;                                                                                    // start next logic level change.
      IrInitialValue[IrStepCount] = time_us_64();                                                       // this is also start timer of next High level.

      gpio_acknowledge_irq(IR_RX, GPIO_IRQ_EDGE_RISE);
   }


    /* IR line goes from High to Low. */
    if (Events & GPIO_IRQ_EDGE_FALL)
    {
      if (IrStepCount > 0)
      {
        IrFinalValue[IrStepCount]  =  time_us_64();                                                      // this is the final timer value for current High level.
        IrResultValue[IrStepCount] = (UINT32)(IrFinalValue[IrStepCount] - IrInitialValue[IrStepCount]);  // calculate duration of current High level.
        IrLevel[IrStepCount]       = 'H';                                                                // identify as High level.
        ++IrStepCount;                                                                                   // start next logic level change.
      }
      IrInitialValue[IrStepCount]  = time_us_64();                                                       // this is also start timer of next Low level.

      gpio_acknowledge_irq(IR_RX, GPIO_IRQ_EDGE_FALL);
    }
  }
}





/* $PAGE */
/* $TITLE=matrix_test() */
/* ------------------------------------------------------------------ *\
          Test Green Clock LED matrix, column-by-column,
                 and also all display indicators.
\* ------------------------------------------------------------------ */
void matrix_test(UINT8 TestNumber)
{
  UCHAR String[128];

  UINT8 Column;
  UINT8 ColumnUp[23];
  UINT8 ColumnDown[23];
  UINT8 DurationSeconds;
  UINT8 Loop1UInt8;
  UINT8 Row;
  UINT8 Section;

  UINT16 Delay;

  UINT64 CurrentTimer;
  UINT64 StartTimer;


  /* Wait till eventual current scrolling has completed. */
  while (ScrollDotCount);
  CurrentClockMode = MODE_TEST;


  /* Rows test. */
  if (TestNumber == 1)
  {
    /* Wipe framebuffer on entry. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;

    /* Turn On all pixels, one at a time, row per row. */
    Delay = 25;
    for (Row = 1; Row < 8; ++Row)
    {
      for (Column = 1; Column < 23; ++Column)
      {
        // tone(5);
        set_pixel(Row, Column, FLAG_ON);
        sleep_ms(Delay);
      }
    }


    sleep_ms(500);


    /* Turn Off all pixels, one at a time, row per row. */
    for (Row = 1; Row < 8; ++Row)
    {
      for (Column = 1; Column < 23; ++Column)
      {
        // tone(5);
        set_pixel(Row, Column, FLAG_OFF);
        sleep_ms(Delay);
      }
    }


    /* Clear framebuffer when done. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;
  }





  /* Columns test. */
  if (TestNumber == 2)
  {
    /* Clear framebuffer on entry. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;

    /* ---------------------- LED matrix test ------------------------- */
    /* Scan each section of clock matrix... */
    for (Section = 0; Section < 4; ++Section)
    {
      /* ...and scan each column of each section... */
      for (Column = 0; Column < 8; ++Column)
      {
        /* First two columns are used by indicators. */
        if ((Section == 0) && (Column < 2))
          continue;

        /* ...and turn on each row of each column of each section. */
        if (Section < 3)
        {
          for (Row = 1; Row < 8; ++Row)
          {
            DisplayBuffer[(Section * 8) + Row] |= 1 << Column;
          }
        }

        /* Turn Off previous section while turning On next one. */
        if (Section > 0)
        {
          for (Row = 1; Row < 8; ++Row)
          {
            DisplayBuffer[((Section - 1) * 8) + Row] &= ~(1 << Column);
          }
        }
        sleep_ms(200);
      }
    }

    /* Clear framebuffer when done. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;
  }



  /* --------------------- Top indicators test ---------------------- */
  if (TestNumber == 3)
  {
    /* Clear framebuffer on entry. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;

    Delay = 300;

    DisplayBuffer[16] |= 1 << 6;
    sleep_ms(1000); // first delay longer so that user can watch indicators.
    DisplayBuffer[16] |= 1 << 5;
    sleep_ms(Delay);
    DisplayBuffer[16] |= 1 << 3;
    sleep_ms(Delay);
    DisplayBuffer[16] |= 1 << 2;
    sleep_ms(Delay);
    DisplayBuffer[16] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[8] |= 1 << 7;
    sleep_ms(Delay);
    DisplayBuffer[8] |= 1 << 5;
    sleep_ms(Delay);
    DisplayBuffer[8] |= 1 << 4;
    sleep_ms(Delay);
    DisplayBuffer[8] |= 1 << 2;
    sleep_ms(Delay);
    DisplayBuffer[8] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[0] |= 1 << 7;
    sleep_ms(Delay);
    DisplayBuffer[0] |= 1 << 6;
    sleep_ms(Delay);
    DisplayBuffer[0] |= 1 << 4;
    sleep_ms(Delay);
    DisplayBuffer[0] |= 1 << 3;
    sleep_ms(1500);

    /* Clear framebuffer when done. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;
  }



  /* ------------------- Left indicators test -------------------- */
  if (TestNumber == 4)
  {
    /* Clear framebuffer on entry. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;

    Delay = 300;

    DisplayBuffer[0] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[0] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[1] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[1] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[2] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[2] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[3] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[3] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[4] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[4] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[5] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[5] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[6] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[6] |= 1 << 1;
    sleep_ms(Delay);
    DisplayBuffer[7] |= 1 << 0;
    sleep_ms(Delay);
    DisplayBuffer[7] |= 1 << 1;
    sleep_ms(1200);


    /* ---------------------- White LEDs test ------------------------- */
    for (Loop1UInt8 = 0; Loop1UInt8 < 5; ++Loop1UInt8)
    {
      IndicatorButtonLightsOn;
      sleep_ms(200);

      IndicatorButtonLightsOff;
      sleep_ms(200);
    }

    /* Wipe framebuffer when done. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0x00;
  }



  /* --------------------------- Snowflakes --------------------------- */
  if (TestNumber == 5)
  {
    /* Wipe framebuffer on entry. */
    clear_framebuffer(0);

    /* Initialize Up and Down limits. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 23; ++Loop1UInt8)
    {
      ColumnUp[Loop1UInt8]   = 0;
      ColumnDown[Loop1UInt8] = 0;
    }


    /* Perform animation for the specified number of seconds. */
    DurationSeconds = 15;
    StartTimer = (UINT64)time_us_64();
    while ((CurrentTimer = (UINT64)time_us_64()) < (StartTimer + (DurationSeconds * 1000000)))
    {
      do
      {
        Column = ((rand() % 22) + 1);
      } while (Column % 2);

      if (DebugBitMask & DEBUG_TEMP)
        uart_send(__LINE__, "Random Column: %2u   ColumnUp[%2u]: %u   ColumnDown[%2u]: %u ", Column, Column, ColumnUp[Column], Column, ColumnDown[Column]);


      if ((ColumnUp[Column] == 0) && (CurrentTimer < (StartTimer + ((DurationSeconds - 2) * 1000000))))
      {
        /* Empty column, set the up and down limits for it. */
        Row = ((rand() % 3) + 1);
        ColumnUp[Column]   = Row;
        ColumnDown[Column] = Row;

        set_pixel(ColumnDown[Column], Column, FLAG_ON);

        if (DebugBitMask & DEBUG_TEMP)
          uart_send(__LINE__, "- Empty column - Random Row: %u   Turn  On pixel %u, %2u\r", Row, Row, Column);
      }
      else
      {
        /* Column already occupied or five seconds before end of animation... check current down position. */
        if (DebugBitMask & DEBUG_TEMP)
          uart_send(__LINE__, "- Occupied - Up[%2u]: %u   Down[%2u]: %u ", Column, ColumnUp[Column], Column, ColumnDown[Column]);

        if ((ColumnDown[Column] < 7) && (ColumnDown[Column] != 0))
        {
          ++ColumnDown[Column];
          set_pixel(ColumnDown[Column], Column, FLAG_ON);

          if (DebugBitMask & DEBUG_TEMP)
            uart_send(__LINE__, "- Turn  On pixel %u, %2u\r", ColumnDown[Column], Column);
        }
        else
        {
          /* We already reached the lowest down mark, erase falling flake. */
          if (DebugBitMask & DEBUG_TEMP)
            uart_send(__LINE__, "- Turn Off pixel %u, %2u\r", ColumnUp[Column], Column);

          set_pixel(ColumnUp[Column], Column, FLAG_OFF);
          ++ColumnUp[Column];

          /* If this column is totally erased, re-initialize it. */
          if (ColumnUp[Column] == 8)
          {
            ColumnUp[Column]   = 0;
            ColumnDown[Column] = 0;
          }
        }
      }
      sleep_ms(10);
    }
  }

  sleep_ms(400);

  return;
}





/* $PAGE */
/* $TITLE=pixel_twinkling() */
/* ------------------------------------------------------------------ *\
                       Make some pixel animation.
\* ------------------------------------------------------------------ */
void pixel_twinkling(UINT16 Seconds)
{
  UINT8 DumFrame;
  UINT8 DumBit;
  UINT8 Status[24][8];

  UINT16 Frequency;
  UINT16 Loop1UInt16;

  UINT64 StartTime;


  /* If there is something scrolling on clock display, wait for the scrolling to complete.  */
  while (ScrollDotCount);

  /* Make sure double dots don't blink on clock display. */
  CurrentClockMode = MODE_TEST;

  /* All pixels are Off on entry. Set their status accordingly. */
  for (DumFrame = 0; DumFrame < 24; ++DumFrame)
    for (DumBit = 0; DumBit < 8; ++DumBit)
      Status[DumFrame][DumBit] = FLAG_OFF;



  /* Make sure sounds do not begin before pixel twinkling. */
  #ifdef PASSIVE_PIEZO_SUPPORT
  sound_queue_passive(SILENT, 50);

  /* Make random sounds while playing with pixels (50 msec per sound - one callback duration). */
  for (Loop1UInt16 = 1; Loop1UInt16 < (Seconds * 1000 / 95); ++Loop1UInt16)
  {
    Frequency = (rand() % 7000) + 300;

    /* Add a new sound in sound queue. If sound queue is full, stop adding sounds. */
    if (sound_queue_passive(Frequency, 50) == MAX_PASSIVE_SOUND_QUEUE)
      break;
  }
  #endif  // PASSIVE_PIEZO_SUPPORT



  StartTime = time_us_64();
  while (time_us_64() < (StartTime + (1000000 * Seconds)))
  {
    /* Generate a Framebuffer, excluding 8 and 16 that are reserved. */
    do
    {
      DumFrame = rand() % 24;
    } while ((DumFrame == 0) || (DumFrame == 8) || (DumFrame == 16));

    /* Generate a bitnumber. */
    DumBit = rand() % 8;

    /* Exclude bit numbers that are reserved. */
    if ((DumFrame < 9) && ((DumBit == 0) || (DumBit == 1)))
      continue;

    /* Check status of generated bit and toggle it. */
    if (Status[DumFrame][DumBit] == FLAG_OFF)
    {
      /* This bit is turned Off, turn it On. */
      DisplayBuffer[DumFrame] |= (1 << DumBit);
      Status[DumFrame][DumBit] = FLAG_ON;
    }
    else
    {
      /* This bit is turned On, turn it Off. */
      DisplayBuffer[DumFrame] &= ~(1 << DumBit);
      Status[DumFrame][DumBit] = FLAG_OFF;
    }

    /* Short pause before next pixel action. */
    sleep_ms(2);
  }

  return;
}





/* $PAGE */
/* $TITLE=play_jingle() */
/* ------------------------------------------------------------------ *\
                      Play the specified jingle.
    NOTE:
    Music timing:
    double croche: 100 msec
    croche         200 msec
    noire          400 msec
    blanche        800 msec
    ronde         1600 msec
    un point apres la note lui ajoute la moitie de sa valeur.
\* ------------------------------------------------------------------ */
void play_jingle(UINT16 JingleNumber)
{
  #ifndef PASSIVE_PIEZO_SUPPORT
  return;
  #else

  if (JingleNumber == JINGLE_BIRTHDAY)
  {
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(LA_b,  400);
    sound_queue_passive(SOL_b, 400);
    sound_queue_passive(DO_c,  400);
    sound_queue_passive(SI_b,  800);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(LA_b,  400);
    sound_queue_passive(SOL_b, 400);
    sound_queue_passive(RE_c,  400);
    sound_queue_passive(DO_c,  800);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_c, 400);
    sound_queue_passive(MI_c,  400);
    sound_queue_passive(DO_c,  400);
    sound_queue_passive(SI_b,  400);
    sound_queue_passive(LA_b,  800);
    sound_queue_passive(FA_c,  200);
    sound_queue_passive(FA_c,  200);
    sound_queue_passive(MI_c,  400);
    sound_queue_passive(DO_c,  400);
    sound_queue_passive(RE_c,  400);
    sound_queue_passive(DO_c, 1000);

    return;
  }

  if (JingleNumber == JINGLE_ENCOUNTER)
  {
    /* Close encounter of the third kind. */
    sound_queue_passive(SOL_b, 600);
    sound_queue_passive(LA_b,  600);
    sound_queue_passive(FA_b,  600);
    sound_queue_passive(FA_a,  600);
    sound_queue_passive(DO_b,  800);

    return;
  }

  if (JingleNumber == JINGLE_FETE)
  {
    /* "Gens du pays". */
    sound_queue_passive(LA_b,       200);
    sound_queue_passive(SOL_b,      200);
    sound_queue_passive(FA_b,       200);
    sound_queue_passive(DO_c,       800);
    sound_queue_passive(LA_b,       200);
    sound_queue_passive(SOL_b,      200);
    sound_queue_passive(FA_b,       200);
    sound_queue_passive(RE_c,       800);
    sound_queue_passive(SOL_b,      200);
    sound_queue_passive(LA_DIESE_b, 200);
    sound_queue_passive(RE_c,       200);
    sound_queue_passive(DO_c,       600);
    sound_queue_passive(FA_b,       200);
    sound_queue_passive(LA_b,       600);
    sound_queue_passive(SOL_b,      200);
    sound_queue_passive(FA_b,      1600);

    return;
  }

  if (JingleNumber == JINGLE_RACING)
  {
    /* "Horse racing" / "Call to post". */
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(DO_c,  200);
    sound_queue_passive(MI_c,  200);
    sound_queue_passive(SOL_c, 300);
    sound_queue_passive(SOL_c, 100);
    sound_queue_passive(SOL_c, 200);
    sound_queue_passive(MI_c,  300);
    sound_queue_passive(MI_c,  100);
    sound_queue_passive(MI_c,  200);
    sound_queue_passive(DO_c,  200);
    sound_queue_passive(MI_c,  200);
    sound_queue_passive(DO_c,  200);
    sound_queue_passive(SOL_b, 800);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(DO_c,  200);
    sound_queue_passive(MI_c,  200);
    sound_queue_passive(SOL_c, 300);
    sound_queue_passive(SOL_c, 100);
    sound_queue_passive(SOL_c, 200);
    sound_queue_passive(MI_c,  300);
    sound_queue_passive(MI_c,  100);
    sound_queue_passive(MI_c,  200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(DO_c, 1400);

    return;
  }

  if (JingleNumber == JINGLE_CHRISTMAS)
  {
    /* We wish you a Merry Christmas */
    sound_queue_passive(RE_b,       400);
    sound_queue_passive(SOL_b,      400);
    sound_queue_passive(SOL_b,      200);
    sound_queue_passive(LA_b,       200);
    sound_queue_passive(SOL_b,      200);
    sound_queue_passive(FA_DIESE_b, 200);
    sound_queue_passive(MI_b,       400);
    sound_queue_passive(MI_b,       450);
    sound_queue_passive(MI_b,       400);
    sound_queue_passive(LA_b,       400);
    sound_queue_passive(LA_b,       200);
    sound_queue_passive(SI_b,       200);
    sound_queue_passive(LA_b,       200);
    sound_queue_passive(SOL_b,      200);
    sound_queue_passive(FA_DIESE_b, 400);
    sound_queue_passive(RE_b,       450);
    sound_queue_passive(RE_b,       400);
    sound_queue_passive(SI_b,       400);
    sound_queue_passive(SI_b,       200);
    sound_queue_passive(DO_c,       200);
    sound_queue_passive(SI_b,       200);
    sound_queue_passive(LA_b,       200);
    sound_queue_passive(SOL_b,      400);
    sound_queue_passive(MI_b,       450);
    sound_queue_passive(RE_b,       200);
    sound_queue_passive(RE_b,       200);
    sound_queue_passive(MI_b,       400);
    sound_queue_passive(LA_b,       400);
    sound_queue_passive(FA_DIESE_b, 400);
    sound_queue_passive(SOL_b,      800);

    return;
  }

  if (JingleNumber == JINGLE_SUNHASHAT)
  {
    /* The Sun Has Got His Hat On */
    sound_queue_passive(SOL_a,      300);
    sound_queue_passive(DO_a,       300);
    sound_queue_passive(MI_a,       300);
    sound_queue_passive(SOL_a,      300);
    sound_queue_passive(DO_b,       300);
    sound_queue_passive(LA_a,       300);
    sound_queue_passive(SOL_a,      300);
    sound_queue_passive(SILENT,     600);
    sound_queue_passive(DO_a,       300);
    sound_queue_passive(MI_a,       300);
    sound_queue_passive(SOL_a,      300);
    sound_queue_passive(MI_b,       300);
    sound_queue_passive(RE_b,       900);
    sound_queue_passive(DO_b,       300);
    sound_queue_passive(SI_a,       300);
    sound_queue_passive(DO_b,       300);
    sound_queue_passive(LA_a,       300);
    sound_queue_passive(SI_a,       300);
    sound_queue_passive(SOL_a,      300);
    sound_queue_passive(LA_a,       300);
    sound_queue_passive(FA_a,       300);
    sound_queue_passive(SOL_a,      300);
    sound_queue_passive(MI_a,       300);
    sound_queue_passive(FA_a,       300);
    sound_queue_passive(RE_a,       300);
    sound_queue_passive(SOL_a,      300);
    sound_queue_passive(DO_a,       900);

    return;
  }

  if (JingleNumber == JINGLE_CANOND)
  {
    /* Pachelbel Canon a D
    4/4 = 300, C5~ F5#        */
    sound_queue_passive(F_SHARP_5,  150);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(E_5,         75);
    sound_queue_passive(F_SHARP_5,  150);
    sound_queue_passive(E_5,         75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(E_5,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(E_5,         75);
    sound_queue_passive(F_SHARP_5,   75);
    sound_queue_passive(E_5,         75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,        150);
    sound_queue_passive(B_4,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,        150);
    sound_queue_passive(D_4,         75);
    sound_queue_passive(E_4,         75);
    sound_queue_passive(F_4,         75);
    sound_queue_passive(G_4,         75);
    sound_queue_passive(F_4,         75);
    sound_queue_passive(E_4,         75);
    sound_queue_passive(F_4,         75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(B_4,        150);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(B_4,        150);
    sound_queue_passive(A_4,         75);
    sound_queue_passive(G_4,         75);
    sound_queue_passive(A_4,         75);
    sound_queue_passive(G_4,         75);
    sound_queue_passive(F_4,         75);
    sound_queue_passive(G_4,         75);
    sound_queue_passive(A_4,         75);
    sound_queue_passive(B_4,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(B_4,        150);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,        150);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(B_4,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(E_5,         75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(C_SHARP_5,   75);
    sound_queue_passive(D_5,         75);
    sound_queue_passive(B_4,         75);
    sound_queue_passive(C_SHARP_5,   75);
  }

  #endif // PASSIVE_PIEZO_SUPPORT
}





/* $PAGE */
/* $TITLE=process_command_queue() */
/* ------------------------------------------------------------------ *\
         Handle the command that has been put in command queue.
\* ------------------------------------------------------------------ */
void process_command_queue(void)
{
  UCHAR String[128];

  UINT8 Command;

  UINT16 Parameter;


  if (DebugBitMask & DEBUG_COMMAND_QUEUE)
    uart_send(__LINE__, "Entering process_command_queue()\r");


  /* Check if for any reason, the function has been called while the command queue is empty. */
  if (CommandQueueHead == CommandQueueTail) return;


  if (DebugBitMask & DEBUG_COMMAND_QUEUE)
    uart_send(__LINE__, "CommandQueueHead: %u   CommandQueueTail: %u\r\r", CommandQueueHead, CommandQueueTail);


  while (CommandQueueTail != CommandQueueHead)
  {
    command_unqueue(&Command, &Parameter);

    if (Command != MAX_COMMAND_QUEUE)
    {
      switch (Command)
      {
        case (COMMAND_PLAY_JINGLE):
          if (DebugBitMask & DEBUG_COMMAND_QUEUE)
            uart_send(__LINE__, "Processing play_jingle(%u)\r\r", Parameter);

          play_jingle(Parameter);
        break;
      }
    }
  }

  return;
}





#ifdef IR_SUPPORT
/* $PAGE */
/* $TITLE=process_ir_command() */
/* ------------------------------------------------------------------ *\
     Function to execute a command received from remte control.
     NOTE: This function is independant from "decode_ir_command()",
           so that it can be used, modified and improved with any
           brand of remote control. Only the "decode_ir_command()"
           needs to be changed (by adding another "#include" module
           and adapted to the brand of remote control used.
\* ------------------------------------------------------------------ */
void process_ir_command(UINT8 IrCommand)
{
  UCHAR Minute;
  UCHAR Second;
  UCHAR String[128];

  UINT8 AmFlag;
  UINT8 PixelStatus[2][7][4];
  UINT8 PixelId[2][7][4][2];
  UINT8 Dum1UInt8;
  UINT8 Dum2UInt8;
  UINT8 Dum3UInt8;
  UINT8 DumBit;
  UINT8 DumColumn;
  UINT8 DumDayOfMonth;
  UINT8 DumFrame;
  UINT8 DumMonth;
  UINT8 DumRow;
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 PmFlag;
  UINT8 Status[24][8];

  UINT16 DumYear;
  UINT16 Frequency;
  UINT16 Loop1UInt16;

  UINT64 RandomUnit[6];
  UINT64 StartTime;
  UINT64 TotalCount;

  float Humidity;
  float Temperature;
  float TemperatureF;


  if (DebugBitMask & DEBUG_IR_COMMAND)
    uart_send(__LINE__, "process_ir_command(%2.2u)   Limits: %2.2u - %2.2u\r", IrCommand, IR_LO_LIMIT, IR_HI_LIMIT);


  /* Check if IrCommand is out-of-bound or "do nothing" (On-Off). */
  if ((IrCommand == IR_LO_LIMIT) || (IrCommand >= IR_HI_LIMIT))
  {
    // scroll_string(24, "Out-of-bound");
    return;
  }



  if (IrCommand == IR_POWER_ON_OFF)
  {
    // play_jingle(JINGLE_FETE);  ///
    scroll_string(24, "Button Power On / Off");
  }



  if (IrCommand == IR_BUTTON_TOP_QUICK)
  {
    // scroll_string(24, "Button 'Set': Top quick");


    /* If one or more alarms are ringing, the "Set" button is used to shut them Off. */
    if (AlarmReachedBitMask || FlagTimerCountDownEnd)
    {
      if (AlarmReachedBitMask) AlarmReachedBitMask = 0;


      /* If "Set" button (or IR equivalent) has been pressed while count-down timer alarm is ringing, reset count-down timer alarm so that it stops ringing. */
      if (FlagTimerCountDownEnd == FLAG_ON)
      {
        FlagTimerCountDownEnd = FLAG_OFF;

        if (DebugBitMask & DEBUG_TIMER)
          uart_send(__LINE__, "FlagTimerCountDownEnd has been reset.\r");
      }
    }
    else
    {
      IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.

      /* If we are already in a setup mode, perform housekeeping for current SetupStep and jump to next SetupStep. */
      switch (SetupSource)
      {
        case SETUP_SOURCE_ALARM:
          set_mode_out();
          FlagSetAlarm = FLAG_ON;
          ++SetupStep;
          /* Cleanup will be done in setup_alarm_frame() when done. */
        break;

        case SETUP_SOURCE_CLOCK:
          set_mode_out();
          FlagSetClock = FLAG_ON;
          ++SetupStep;
          /* Cleanup will be done in setup_clock_frame() when done. */
        break;

        case SETUP_SOURCE_TIMER:
          set_mode_out();
          FlagSetTimer = FLAG_ON;
          ++SetupStep;
          /* Cleanup will be done in setup_timer_frame() when done. */
        break;

        default:
          /* If not already in a setup mode, "Set" button triggers entering in clock setup mode. */
          FlagSetClock        = FLAG_ON;
          SetupSource         = SETUP_SOURCE_CLOCK;
          CurrentClockMode    = MODE_CLOCK_SETUP;
          IdleNumberOfSeconds = 0;  // reset the number of seconds the system has been idle.
          ++SetupStep;
        break;
      }
    }
  }



  if (IrCommand == IR_BUTTON_TOP_LONG)
  {
    // scroll_string(24, "Button 'Random / Down': Top long");

    IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
    set_mode_out();          // housekeeping for previous data

    /* If we are not already in setup mode, enter alarm setup mode. */
    FlagSetAlarm = FLAG_ON; // enter alarm setup mode.
    SetupSource  = SETUP_SOURCE_ALARM;
    ++SetupStep;  // process through all alarm setup steps.
  }



  if (IrCommand == IR_BUTTON_MIDDLE_QUICK)
  {
    // scroll_string(24, "Button 'Volume Up': Middle quick");

    IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.

    /* If we are already in a setup mode, perform housekeeping for current SetupStep and jump on to next SetupStep. */
    switch (SetupSource)
    {
      case SETUP_SOURCE_ALARM:
        setup_alarm_variables(FLAG_UP); // perform alarm settings.
        set_mode_out();                 // program RTC IC if required.
        FlagSetAlarm = FLAG_ON;         // make sure main program loop displays new value.
      break;

      case SETUP_SOURCE_CLOCK:
        setup_clock_variables(FLAG_UP); // perform clock settings.
        set_mode_out();                 // program RTC IC if required.
        FlagSetClock = FLAG_ON;         // make sure main program loop displays new value.
      break;

      case SETUP_SOURCE_TIMER:
        setup_timer_variables(FLAG_UP); // perform timer settings.
        set_mode_out();                 // housekeeping.
        FlagSetTimer = FLAG_ON;         // make sure main program loop displays new value.
      break;

      default:
        /* NOTE: Temperature unit toggling has been transferred to the list of clock setup parameters. */
        /* Temporarily toggle Night light On / Off. However, night light setting will return to its normal setting in the next seconds. */
        Dum1UInt8 = (1 << 2) | (1 << 5);
        if (DisplayBuffer[0] & Dum1UInt8)
          IndicatorButtonLightsOff;
        else
          IndicatorButtonLightsOn;

        /* If a DHT22 or BME280 has been installed, display outside parameters. */
        scroll_queue(TAG_BME280_TEMP);
        scroll_queue(TAG_DHT22_TEMP);
        scroll_queue(TAG_DS3231_TEMP);
      break;
    }
  }



  if (IrCommand == IR_BUTTON_MIDDLE_LONG)
  {
    // scroll_string(24, "Button 'Repeat / Up': Middle long");

    IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
    set_mode_out();          // housekeeping for previous data

    /* If we are already in a setup mode, perform housekeeping for current SetupStep and jump on to next SetupStep. */
    switch (SetupSource)
    {
      case SETUP_SOURCE_ALARM:
      case SETUP_SOURCE_CLOCK:
      case SETUP_SOURCE_TIMER:
        /* If already is some setup mode, then no action. */
      break;

      default:
        /* If we're not already in a setup mode, long press on the "Up" button triggers timer setup mode. */
        if (SetupStep == SETUP_NONE)
        {
          FlagSetTimer = FLAG_ON;
          SetupSource  = SETUP_SOURCE_TIMER;
          ++SetupStep;
        }
      break;
    }
  }



  if (IrCommand == IR_BUTTON_BOTTOM_QUICK)
  {
    // scroll_string(24, "Button 'Volume Down': Bottom quick");

    IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.

    /* If we are already in a setup mode, perform housekeeping for current SetupStep and jump on to next SetupStep. */
    switch (SetupSource)
    {
      case SETUP_SOURCE_ALARM:
        setup_alarm_variables(FLAG_DOWN); // perform alarm settings.
        set_mode_out();
        FlagSetAlarm = FLAG_ON;
      break;

      case SETUP_SOURCE_CLOCK:
        setup_clock_variables(FLAG_DOWN); // perform clock settings.
        set_mode_out();
        FlagSetClock = FLAG_ON;
      break;

      case SETUP_SOURCE_TIMER:
        setup_timer_variables(FLAG_DOWN); // perform timer settings.
        set_mode_out();
        FlagSetTimer = FLAG_ON;
      break;

      default:
        /* NOTE: Clock display auto-brightness toggling has been added to the list of clock setup parameters.
                 Leave a copy of this function here for quicker access if required. */
        if (FlashConfig.FlagAutoBrightness == FLAG_ON)
        {
          FlashConfig.FlagAutoBrightness = FLAG_OFF;
          IndicatorAutoLightOff;
        }
        else
        {
          FlashConfig.FlagAutoBrightness = FLAG_ON;
          IndicatorAutoLightOn;
        }
      break;
    }
  }



  if (IrCommand == IR_BUTTON_BOTTOM_LONG)
  {
    // scroll_string(24, "Button 'Stop': Bottom long");

    /* If we are in a setup mode, "bottom button long" is used to get out of setup mode (emulate clock bottom button long press). */
    if (SetupSource != SETUP_SOURCE_NONE)
    {
      IdleNumberOfSeconds = 0;         // reset the number of seconds the system has been idle.
      set_mode_timeout();              // exit setup mode.
      set_mode_out();                  // make required housekeeping.
      SetupSource = SETUP_SOURCE_NONE; // no more in setup mode.
      SetupStep = SETUP_NONE;          // reset SetupStep.
    }
    else
    {
      scroll_queue(TAG_AMBIENT_LIGHT);
    }
  }



  if (IrCommand == IR_DICE_ROLLING)
  {
    // scroll_string(24, "Button 'Band': Dice rolling");

    /* Prevent time display. */
    CurrentClockMode = MODE_TEST;

    IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.

    /* Clear all PixelStatus on entry. */
    clear_framebuffer(0);

    /* Initialize the status of each pixel of the two dices (4 X 7 digits) to "blank" on entry. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 7; ++Loop1UInt8) // scan each row
    {
      for (Loop2UInt8 = 0; Loop2UInt8 < 4; ++Loop2UInt8) // scan each column
      {
        PixelStatus[0][Loop1UInt8][Loop2UInt8] = 0; // each pixel of first digit is blank on entry.
        PixelStatus[1][Loop1UInt8][Loop2UInt8] = 0; // each pixel of second digit is blank on entry.
      }
    }

    /* For each dice, assign the DisplayBuffer number and bit number corresponding to each pixel. */
    /* First dice (first digit). */
    PixelId[0][0][0][0] = 1; // DisplayBuffer number
    PixelId[0][0][0][1] = 6; // bit number

    PixelId[0][0][1][0] = 1; // DisplayBuffer number
    PixelId[0][0][1][1] = 7; // bit number

    PixelId[0][0][2][0] = 9; // DisplayBuffer number
    PixelId[0][0][2][1] = 0; // bit number

    PixelId[0][0][3][0] = 9; // DisplayBuffer number
    PixelId[0][0][3][1] = 1; // bit number

    PixelId[0][1][0][0] = 2; // DisplayBuffer number
    PixelId[0][1][0][1] = 6; // bit number

    PixelId[0][1][1][0] = 2; // DisplayBuffer number
    PixelId[0][1][1][1] = 7; // bit number

    PixelId[0][1][2][0] = 10; // DisplayBuffer number
    PixelId[0][1][2][1] = 0;  // bit number

    PixelId[0][1][3][0] = 10; // DisplayBuffer number
    PixelId[0][1][3][1] = 1;  // bit number

    PixelId[0][2][0][0] = 3; // DisplayBuffer number
    PixelId[0][2][0][1] = 6; // bit number

    PixelId[0][2][1][0] = 3; // DisplayBuffer number
    PixelId[0][2][1][1] = 7; // bit number

    PixelId[0][2][2][0] = 11; // DisplayBuffer number
    PixelId[0][2][2][1] = 0;  // bit number

    PixelId[0][2][3][0] = 11; // DisplayBuffer number
    PixelId[0][2][3][1] = 1;  // bit number

    PixelId[0][3][0][0] = 4; // DisplayBuffer number
    PixelId[0][3][0][1] = 6; // bit number

    PixelId[0][3][1][0] = 4; // DisplayBuffer number
    PixelId[0][3][1][1] = 7; // bit number

    PixelId[0][3][2][0] = 12; // DisplayBuffer number
    PixelId[0][3][2][1] = 0;  // bit number

    PixelId[0][3][3][0] = 12; // DisplayBuffer number
    PixelId[0][3][3][1] = 1;  // bit number

    PixelId[0][4][0][0] = 5; // DisplayBuffer number
    PixelId[0][4][0][1] = 6; // bit number

    PixelId[0][4][1][0] = 5; // DisplayBuffer number
    PixelId[0][4][1][1] = 7; // bit number

    PixelId[0][4][2][0] = 13; // DisplayBuffer number
    PixelId[0][4][2][1] = 0;  // bit number

    PixelId[0][4][3][0] = 13; // DisplayBuffer number
    PixelId[0][4][3][1] = 1;  // bit number

    PixelId[0][5][0][0] = 6; // DisplayBuffer number
    PixelId[0][5][0][1] = 6; // bit number

    PixelId[0][5][1][0] = 6; // DisplayBuffer number
    PixelId[0][5][1][1] = 7; // bit number

    PixelId[0][5][2][0] = 14; // DisplayBuffer number
    PixelId[0][5][2][1] = 0;  // bit number

    PixelId[0][5][3][0] = 14; // DisplayBuffer number
    PixelId[0][5][3][1] = 1;  // bit number

    PixelId[0][6][0][0] = 7; // DisplayBuffer number
    PixelId[0][6][0][1] = 6; // bit number

    PixelId[0][6][1][0] = 7; // DisplayBuffer number
    PixelId[0][6][1][1] = 7; // bit number

    PixelId[0][6][2][0] = 15; // DisplayBuffer number
    PixelId[0][6][2][1] = 0;  // bit number

    PixelId[0][6][3][0] = 15; // DisplayBuffer number
    PixelId[0][6][3][1] = 1;  // bit number

    /* Second dice (second digit). */
    PixelId[1][0][0][0] = 9; // DisplayBuffer number
    PixelId[1][0][0][1] = 7; // bit number

    PixelId[1][0][1][0] = 17; // DisplayBuffer number
    PixelId[1][0][1][1] = 0;  // bit number

    PixelId[1][0][2][0] = 17; // DisplayBuffer number
    PixelId[1][0][2][1] = 1;  // bit number

    PixelId[1][0][3][0] = 17; // DisplayBuffer number
    PixelId[1][0][3][1] = 2;  // bit number

    PixelId[1][1][0][0] = 10; // DisplayBuffer number
    PixelId[1][1][0][1] = 7;  // bit number

    PixelId[1][1][1][0] = 18; // DisplayBuffer number
    PixelId[1][1][1][1] = 0;  // bit number

    PixelId[1][1][2][0] = 18; // DisplayBuffer number
    PixelId[1][1][2][1] = 1;  // bit number

    PixelId[1][1][3][0] = 18; // DisplayBuffer number
    PixelId[1][1][3][1] = 2;  // bit number

    PixelId[1][2][0][0] = 11; // DisplayBuffer number
    PixelId[1][2][0][1] = 7;  // bit number

    PixelId[1][2][1][0] = 19; // DisplayBuffer number
    PixelId[1][2][1][1] = 0;  // bit number

    PixelId[1][2][2][0] = 19; // DisplayBuffer number
    PixelId[1][2][2][1] = 1;  // bit number

    PixelId[1][2][3][0] = 19; // DisplayBuffer number
    PixelId[1][2][3][1] = 2;  // bit number

    PixelId[1][3][0][0] = 12; // DisplayBuffer number
    PixelId[1][3][0][1] = 7;  // bit number

    PixelId[1][3][1][0] = 20; // DisplayBuffer number
    PixelId[1][3][1][1] = 0;  // bit number

    PixelId[1][3][2][0] = 20; // DisplayBuffer number
    PixelId[1][3][2][1] = 1;  // bit number

    PixelId[1][3][3][0] = 20; // DisplayBuffer number
    PixelId[1][3][3][1] = 2;  // bit number

    PixelId[1][4][0][0] = 13; // DisplayBuffer number
    PixelId[1][4][0][1] = 7;  // bit number

    PixelId[1][4][1][0] = 21; // DisplayBuffer number
    PixelId[1][4][1][1] = 0;  // bit number

    PixelId[1][4][2][0] = 21; // DisplayBuffer number
    PixelId[1][4][2][1] = 1;  // bit number

    PixelId[1][4][3][0] = 21; // DisplayBuffer number
    PixelId[1][4][3][1] = 2;  // bit number

    PixelId[1][5][0][0] = 14; // DisplayBuffer number
    PixelId[1][5][0][1] = 7;  // bit number

    PixelId[1][5][1][0] = 22; // DisplayBuffer number
    PixelId[1][5][1][1] = 0;  // bit number

    PixelId[1][5][2][0] = 22; // DisplayBuffer number
    PixelId[1][5][2][1] = 1;  // bit number

    PixelId[1][5][3][0] = 22; // DisplayBuffer number
    PixelId[1][5][3][1] = 2;  // bit number

    PixelId[1][6][0][0] = 15; // DisplayBuffer number
    PixelId[1][6][0][1] = 7;  // bit number

    PixelId[1][6][1][0] = 23; // DisplayBuffer number
    PixelId[1][6][1][1] = 0;  // bit number

    PixelId[1][6][2][0] = 23; // DisplayBuffer number
    PixelId[1][6][2][1] = 1;  // bit number

    PixelId[1][6][3][0] = 23; // DisplayBuffer number
    PixelId[1][6][3][1] = 2;  // bit number

    /* Turn on all pixels of both dices. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 2; ++Loop1UInt8) // 2 dices (2 digits)
    {
      for (DumRow = 0; DumRow < 7; ++DumRow) // 7 rows
      {
        for (DumColumn = 0; DumColumn < 4; ++DumColumn) // 4 columns
        {
          DisplayBuffer[PixelId[Loop1UInt8][DumRow][DumColumn][0]] |= (1 << PixelId[Loop1UInt8][DumRow][DumColumn][1]);
        }
      }
    }


    /* -------------------------------------------------------------- *\
                Check the quality of the random generator.
     * -------------------------------------------------------------- *
         I coded this section to check the quality of the random
         generator. On one of the pass that I analyzed,
         I got the following results:

         TotalCount = 50,663,127

         [1] = 8,447,462
         [2] = 8,440,344
         [3] = 8,442,616
         [4] = 8,443.091
         [5] = 8,441,867
         [6] = 8.447.747

         The gap between the lowest count and the highest count is
         less than 0.1 %, so I think the result is relatively good...
     * -------------------------------------------------------------- *

    TotalCount = 0;

    for (Loop1UInt8 = 0; Loop1UInt8 < 6; ++Loop1UInt8)
      RandomUnit[Loop1UInt8] = 0;


    StartTime = time_us_64();
    while (time_us_64() < (StartTime + (1000000 * 60)))
    {
      Dum1UInt8 = rand() % 6 + 1;
      ++RandomUnit[Dum1UInt8 - 1];
      ++TotalCount;
    }


    sprintf(String, "TotalCount: %llu", TotalCount);
    scroll_string(24, String);

    sprintf(String, "[1]: %llu [2]: %llu [3]: %llu [4]: %llu [5]: %llu [6]: %llu",
                    RandomUnit[0], RandomUnit[1], RandomUnit[2], RandomUnit[3], RandomUnit[4], RandomUnit[5]);
    scroll_string(24, String);
    sleep_ms(20000);
    \* -------------------------------------------------------------- */

    /* Leave both digit matrix all On for a short while. */
    sleep_ms(200);

    #ifdef PASSIVE_PIEZO_SUPPORT
    /* Make sure random sounds do not begin before pixel twinkling. */
    sound_queue_passive(SILENT, 100);

    /* Make random sounds while playing with pixels (50 msec per sound). */
    for (Loop1UInt16 = 0; Loop1UInt16 < (2 * 1000 / 95); ++Loop1UInt16)
    {
      Frequency = (rand() % 7000) + 200;

      /* If sound queue is full, do not try to put more sounds. */
      if (sound_queue_passive(Frequency, 50) == MAX_PASSIVE_SOUND_QUEUE)
        break;
    }
    #endif

    /* Make random pixel animation on display. */
    StartTime = time_us_64();
    while (time_us_64() < (StartTime + (1000000 * 2)))
    {
      for (Loop1UInt8 = 0; Loop1UInt8 < 2; ++Loop1UInt8)
      {
        /* Generate a random row number for each dice. */
        DumRow = rand() % 7;

        /* Generate a random column number for each dice. */
        DumColumn = rand() % 4;

        /* Check current status of generated pixel and toggle it. */
        if (PixelStatus[Loop1UInt8][DumRow][DumColumn] == 0)
        {
          // This pixel is turned Off, turn it On.
          DisplayBuffer[PixelId[Loop1UInt8][DumRow][DumColumn][0]] |= (1 << PixelId[Loop1UInt8][DumRow][DumColumn][1]);
          PixelStatus[Loop1UInt8][DumRow][DumColumn] = 1;
        }
        else
        {
          // This pixel is turned On, turn it Off.
          DisplayBuffer[PixelId[Loop1UInt8][DumRow][DumColumn][0]] &= ~(1 << PixelId[Loop1UInt8][DumRow][DumColumn][1]);
          PixelStatus[Loop1UInt8][DumRow][DumColumn] = 0;
        }
      }

      /* Short pause before next pixel action. */
      sleep_ms(2);
    }

    /* Clear display before displaying dice values. */
    clear_framebuffer(0);

    /* Roll dice number 1. */
    Dum1UInt8 = rand() % 6 + 1;
    fill_display_buffer_4X7(4, 0x30 + Dum1UInt8);

    sleep_ms(300);  // artificially delay dice 2 result.

    /* Roll dice number 2. */
    Dum1UInt8 = rand() % 6 + 1;
    fill_display_buffer_4X7(13, 0x30 + Dum1UInt8);

    /* Note: time display will be back on next minute change. */
  }



  if (IrCommand == IR_DISPLAY_OUTSIDE_TEMP)
  {
    // scroll_string(24, "Button 'CD door': Outside temperature");

    if (DebugBitMask & DEBUG_IR_COMMAND)
      uart_send(__LINE__, "Processing IR_DISPLAY_OUTSIDE_TEMP\r\r");

    scroll_queue(TAG_BME280_TEMP);
    scroll_queue(TAG_DHT22_TEMP);
    scroll_queue(TAG_DS3231_TEMP);
    scroll_queue(TAG_PICO_TEMP);
  }



  if (IrCommand == IR_FULL_TEST)
  {
    // scroll_string(24, "Button 'Play / Pause': Full Pico Green Clock tests & demos");

    CurrentClockMode = MODE_TEST; // prevent double dots blinking.

    /* If user requested "Full test", assume he wants to get rid of an eventual running SilencePeriod. */
    SilencePeriod = 0;

    #ifdef PASSIVE_PIEZO_SUPPORT
    /* Test passive buzzer. */
    sound_queue_passive(SILENT, 1000); // silence period before beginning jingles.

    play_jingle(JINGLE_ENCOUNTER);
    sound_queue_passive(SILENT, 1500); // silence period between jingles.

    play_jingle(JINGLE_BIRTHDAY);
    sound_queue_passive(SILENT, 1500); // silence period between jingles.

    play_jingle(JINGLE_FETE);
    sound_queue_passive(SILENT, 1500); // silence period between jingles.

    play_jingle(JINGLE_RACING);
    #endif

    /* LED matrix tests. */
    matrix_test(1);
    matrix_test(2);
    matrix_test(3);
    matrix_test(4);
    matrix_test(5);

    /* Pixel twinkling on clock display. */
    pixel_twinkling(10);

    update_top_indicators(ALL, FLAG_OFF);              // first, turn Off all days' indicators.
    update_top_indicators(CurrentDayOfWeek, FLAG_ON);  // then, turn On today's indicator.

    update_left_indicators();

    #ifdef BME280_SUPPORT
    /* Give a way to bring back BME280 back on track in case it has just been connected,
       or if there has been memory corruption with calibration parameters, or for some other reason...
       This allows using remote control to reset the BME280 instead of having to cycle the power supply. */
    bme280_init();

    /* Read calibration data of the specific device used (written in BME280's non volatile memory). */
    bme280_read_calib_data();

    /* Compute calibration parameters from calibration data. */
    bme280_compute_calib_param();
    #endif

    /* Scroll many items. */
    scroll_queue(TAG_FIRMWARE_VERSION); // firmware version number.
    scroll_queue(TAG_PICO_TYPE);        // Pico microcontroller type (Pico or PicoW").
    scroll_queue(TAG_PICO_UNIQUE_ID);   // Pico microcontroller unique ID ("serial number").
    scroll_queue(TAG_DHT22_TEMP);       // display ambient temperature.
    scroll_queue(TAG_BME280_TEMP);      // outside temperature parameters (if a BME280 has been installed by user).
    scroll_queue(TAG_PICO_TEMP);        // Pico microcontroller internal temperature.
    scroll_queue(TAG_DST);              // daylight saving time mode / status.
    scroll_queue(TAG_AMBIENT_LIGHT);    // current ambient light.
    scroll_queue(TAG_VOLTAGE);          // power supply voltage.
    scroll_queue(TAG_BME280_DEVICE_ID); // BME280 device ID if one has been installed by user.
    scroll_queue(TAG_IDLE_MONITOR);     // system idle time monitor.
    #ifdef PICO_W
    scroll_queue(TAG_NTP_ERRORS);       // scroll number of error in NTP requests.
    #endif  // PICO_W

    /* Test active buzzer. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 4; ++Loop1UInt8)
    {
      sound_queue_active(50, 5);
      sound_queue_active(100, SILENT);
    }
    sound_queue_active(500, SILENT);
  }



  if (IrCommand == IR_IDLE_TIME)
  {
    // scroll_string(24, "Button rewind");

    scroll_queue(TAG_IDLE_MONITOR);
  }



  if (IrCommand == IR_SILENCE_PERIOD)
  {
    // scroll_string(24, "Button 'CD': Count-down silence period");

    SilencePeriod += (SILENCE_PERIOD_UNIT * 60);

    switch (FlashConfig.Language)
    {
      case (CZECH):
        if ((int)(SilencePeriod / 60) == 1)
        {
          sprintf(String, "Obdobi klidu: %u minuta.", (int)(SilencePeriod / 60));
          String[5] = (UINT8)131; // i-acute
        }
        else
        {
          sprintf(String, "Obdobi klidu: %u minut.", (int)(SilencePeriod / 60));
          String[5] = (UINT8)131; // i-acute
        }
      break;

      case (FRENCH):
        sprintf(String, "Periode de silence: %u minutes", (int)(SilencePeriod / 60));
        String[1] = (UINT8)31; // e - acute.
      break;

      case (SPANISH):
        sprintf(String, "Periodo de silencio: %u minutos", (int)(SilencePeriod / 60));
        String[3] = (UINT8)131; // i - acute.
      break;

      case (ENGLISH):
      case (GERMAN):
      default:
        sprintf(String, "Silence period: %u minutes", (int)(SilencePeriod / 60));
      break;
    }
    scroll_string(24, String);
  }



  if (IrCommand == IR_DISPLAY_GENERIC)
  {
    // scroll_string(24, "Button 'Display': Generic display");

    FlagIdleCheck = FLAG_ON;
    if (CurrentClockMode != MODE_DISPLAY)
    {
      /* Entering "MODE_DISPLAY" for the first time. */
      CurrentClockMode = MODE_DISPLAY;

      /* While in generic "MODE_DISPLAY", stop blinking both dots on the display and make sure they are steady On. */
      DisplayBuffer[11] |= 0x10; // slim ":" - top dot.
      DisplayBuffer[13] |= 0x10; // slim ":" - bottom dot.
    }
    else
    {
      /* Direct access to a specific clock parameter.
         NOTE: For convenience, we use the same settings for display as for settings. */
      // SETUP_CLOCK_LO_LIMIT       0x00
      // SETUP_NONE                 0x00
      // SETUP_HOUR                 0x01
      // SETUP_MINUTE               0x02
      // SETUP_MONTH                0x03
      // SETUP_DAY_OF_MONTH         0x04
      // SETUP_YEAR                 0x05
      // SETUP_DST                  0x06
      // SETUP_UTC                  0x07
      // SETUP_KEYCLICK             0x08
      // SETUP_SCROLLING            0x09
      // SETUP_TEMP_UNIT            0x0A
      // SETUP_LANGUAGE             0x0B
      // SETUP_TIME_FORMAT          0x0C
      // SETUP_HOURLY_CHIME         0x0D
      // SETUP_CHIME_TIME_ON        0x0E
      // SETUP_CHIME_TIME_OFF       0x0F
      // SETUP_NIGHT_LIGHT          0x10
      // SETUP_NIGHT_LIGHT_TIME_ON  0x11
      // SETUP_NIGHT_LIGHT_TIME_OFF 0x12
      // SETUP_AUTO_BRIGHT          0x13
      // SETUP_CLOCK_HI_LIMIT       0x14
      switch (SetupStep)
      {
        case (0x01):  // SETUP_HOUR
          /* Hour, minute, seconds, day-of-month, month and year display. */
          if (FlashConfig.TimeDisplayMode == H12)
          {
            Dum1UInt8 = convert_h24_to_h12(CurrentHour, &AmFlag, &PmFlag);
            sprintf(String, "%u:%2.2u:%2.2u %s", Dum1UInt8, CurrentMinute, CurrentSecond, AmFlag ? "AM" : "PM");
            scroll_string(24, String);
            get_date_string(String);
            scroll_string(24, String);
          }
          else
          {
            sprintf(String, "%2.2u:%2.2u:%2.2u", CurrentHour, CurrentMinute, CurrentSecond);
            scroll_string(24, String);
            get_date_string(String);
            scroll_string(24, String);
          }
          scroll_queue(TAG_NTP_STATUS);
          scroll_queue(TAG_WIFI_CREDENTIALS);
        break;

        case (0x06):  // SETUP_DST
          /* Daylight saving time status. */
          scroll_queue(TAG_DST);
          scroll_queue(TAG_TIMEZONE);
        break;

        case (0x07):  // SETUP_KEYCLICK
          /* Beep (keyclick). */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              if (FlashConfig.FlagKeyclick == FLAG_ON)
              {
                sprintf(String, "opakovani");
                String[6] = (UINT8)129; // a-acute
                String[8] = (UINT8)131; // i-acute
                sprintf(String, "Zvuk klaves zapnut - doba %u ms, %s 1: %u %s 2: %u.", TONE_KEYCLICK_DURATION, String, TONE_KEYCLICK_REPEAT1, String, TONE_KEYCLICK_REPEAT2);
                String[7] = (UINT8)129; // a-acute
              }
              else
              {
                sprintf(String, "Zvuk klaves vypnut.");
                String[7] = (UINT8)129; // a-acute
              }
            break;

            case (FRENCH):
              if (FlashConfig.FlagKeyclick == FLAG_ON)
              {
                sprintf(String, "Keyclick On - Duree %u ms Repeat1: %u Repeat2: %u", TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1, TONE_KEYCLICK_REPEAT2);
                String[17] = (UINT8)0x90; // e - acute.
              }
              else
              {
                sprintf(String, "Keyclick Off");
              }
            break;

            case (SPANISH):
              if (FlashConfig.FlagKeyclick == FLAG_ON)
              {
                sprintf(String, "Clic en clave activado - Duracion %u ms Repeat1: %u Repeat2: %u", TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1, TONE_KEYCLICK_REPEAT2);
                String[31] = (UINT8)139; // o - acute.
              }
              else
              {
                sprintf(String, "Clic en clave desactivado");
              }
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              if (FlashConfig.FlagKeyclick == FLAG_ON)
              {
                sprintf(String, "Keyclick On - Duration: %u ms Repeat1: %u Repeat2: %u", TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1, TONE_KEYCLICK_REPEAT2);
              }
              else
              {
                sprintf(String, "Keyclick Off");
              }
            break;

            scroll_string(24, String);
          }
        break;

        case (0x08):  // SETUP_SCROLLING
          /* Display scroll. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              if (FlashConfig.FlagScrollEnable == FLAG_ON)
              {
                sprintf(String, "tecek: %u ms.", SCROLL_DOT_TIME);
                String[2] = (UINT8)136; // c-caron
                sprintf(String, "Posuv zapnut - opakovani: %u minut   rychlost %s", SCROLL_PERIOD_MINUTE, String);
                String[17] = (UINT8)129; // a-acute
                String[24] = (UINT8)131; // i-acute
              }
              else
              {
                 sprintf(String, "Posuv vypnut.");
              }
            break;

            case (FRENCH):
              if (FlashConfig.FlagScrollEnable == FLAG_ON)
              {
                sprintf(String, "Scrolling On - Periode: %u minutes   Vitesse des dots: %u msec.", SCROLL_PERIOD_MINUTE, SCROLL_DOT_TIME);
                String[16] = (UINT8)31;  // e accent aigu.
              }
              else
              {
                sprintf(String, "Scrolling OFF.");
              }
            break;

            case (SPANISH):
              if (FlashConfig.FlagScrollEnable == FLAG_ON)
              {
                sprintf(String, "Desplazamiento activado - Duracion: %u minutos   Velodicad de los dots: %u msec.", SCROLL_PERIOD_MINUTE, SCROLL_DOT_TIME);
                String[32] = (UINT8)139;  // o-acute.
              }
              else
              {
                sprintf(String, "Desplazamiento desactivado.");
              }
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              if (FlashConfig.FlagScrollEnable == FLAG_ON)
              {
                sprintf(String, "Scrolling On - Frequency: %u minutes   Dot speed: %u msec.", SCROLL_PERIOD_MINUTE, SCROLL_DOT_TIME);
              }
              else
              {
                sprintf(String, "Scrolling Off.");
              }
            break;
          }
          scroll_string(24, String);
        break;

        case (0x09):  // SETUP_TEMP_UNIT
          /* Temperature unit. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              if (FlashConfig.TemperatureUnit == CELSIUS)
              {
                sprintf(String, "Jedn. teploty: Celsius.   ");
                String[4]  = (UINT8)31;  // e accent aigu.
                String[13] = (UINT8)31;  // e accent aigu.
              }
              else
              {
                sprintf(String, "Jedn. teploty: Fahrenheit.   ");
                String[4]  = (UINT8)31;  // e accent aigu.
                String[13] = (UINT8)31;  // e accent aigu.
              }
            break;

            case (FRENCH):
              if (FlashConfig.TemperatureUnit == CELSIUS)
              {
                sprintf(String, "Unite de temperature: Celsius   ");
                String[4]  = (UINT8)31;  // e accent aigu.
                String[13] = (UINT8)31;  // e accent aigu.
              }
              else
              {
                sprintf(String, "Unite de temperature: Fahrenheit   ");
                String[4]  = (UINT8)31;  // e accent aigu.
                String[13] = (UINT8)31;  // e accent aigu.
              }
            break;

            case (SPANISH):
              if (FlashConfig.TemperatureUnit == CELSIUS)
              {
                sprintf(String, "Unidad de temperatura: Celsius   ");
                String[4]  = (UINT8)31;  // e accent aigu.
                String[13] = (UINT8)31;  // e accent aigu.
              }
              else
              {
                sprintf(String, "Unidad de temperatura: Fahrenheit   ");
                String[4]  = (UINT8)31;  // e accent aigu.
                String[13] = (UINT8)31;  // e accent aigu.
              }
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              if (FlashConfig.TemperatureUnit == CELSIUS)
              {
                sprintf(String, "Temperature unit is Celsius   ");
              }
              else
              {
                sprintf(String, "Temperature unit is Fahrenheit   ");
              }
            break;
          }
          scroll_string(24, String);

          scroll_queue(TAG_DS3231_TEMP);
          scroll_queue(TAG_BME280_TEMP);
          scroll_queue(TAG_DHT22_TEMP);
        break;

        case (0x0A): // SETUP_LANGUAGE
          /* Language. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              sprintf(String, "Jazyk je cestina   ");
              String[9] = (UINT8)136;
            break;

            case (ENGLISH):
              sprintf(String, "Language is English   ");
            break;

            case (FRENCH):
              sprintf(String, "La langue est le francais   ");
            break;

            case (SPANISH):
              sprintf(String, "La lengua es el espanol   ");
              String[20] = (UINT8)140; // n-tilde
            break;

            case (GERMAN):
              sprintf(String, "Language is German   ");
            break;

            default:
              sprintf(String, "Undefined language   ");
            break;
          }
          scroll_string(24, String);

        break;

        case (0x0B): // SETUP_TIME_FORMAT
          /* Time display format. */
          switch (FlashConfig.Language)
          {
           case (CZECH):
              if (FlashConfig.TimeDisplayMode == H12)
              {
                sprintf(String, "12hodinove zobrazeni casu.");
                String[9] = (UINT8)138; // e-acute
                String[19] = (UINT8)131; // i-acute
                String[21] = (UINT8)136; // c-caron
              }
              else
              {
                sprintf(String, "24hodinove zobrazeni casu.");
                String[9] = (UINT8)138; // e-acute
                String[19] = (UINT8)131; // i-acute
                String[21] = (UINT8)136; // c-caron
              }
            break;

            case (FRENCH):
              if (FlashConfig.TimeDisplayMode == H12)
              {
                sprintf(String, "Format d'affichage de l'heure: 12 heures");
              }
              else
              {
                sprintf(String, "Format d'affichage de l'heure: 24 heures");
              }
            break;

            case (SPANISH):
              if (FlashConfig.TimeDisplayMode == H12)
              {
                sprintf(String, "Formato de visualizacion de la hora: 12 horas");
                String[22] = (UINT8)139; // o-acute
              }
              else
              {
                sprintf(String, "Formato de visualizacion de la hora: 24 horas");
                String[22] = (UINT8)139; // o-acute
              }
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              if (FlashConfig.TimeDisplayMode == H12)
              {
                sprintf(String, "Time display format: 12-hours");
              }
              else
              {
                sprintf(String, "Time display format: 24-hours");
              }
            break;
          }
          scroll_string(24, String);
        break;

        case (0x0C): // SETUP_HOURLY_CHIME
          /* Hourly chime. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              if (FlashConfig.ChimeMode == CHIME_OFF)
              {
                sprintf(String, "Hodinovy zvuk je vypnut.");
                String[7] = (UINT8)132; // y-acute
              }
              else if (FlashConfig.ChimeMode == CHIME_ON)
              {
                sprintf(String, "Hodinovy zvuk je zapnut.");
                String[7] = (UINT8)132; // y-acute
              }
              else if (FlashConfig.ChimeMode == CHIME_DAY)
              {
                sprintf(String, "Hodinovy zvuk zapnut jen od %u:00 do %u:00.", FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff);
                String[7] = (UINT8)132; // y-acute
              }
            break;

            case (FRENCH):
              if (FlashConfig.ChimeMode == CHIME_OFF)
              {
                sprintf(String, "Le signal horaire est a Off");
              }
              else if (FlashConfig.ChimeMode == CHIME_ON)
              {
                sprintf(String, "Le signal horaire est a On");
              }
              else if (FlashConfig.ChimeMode == CHIME_DAY)
              {
                sprintf(String, "Le signal horaire est intermittent, de %u:00 a %u:00", FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff);
              }
            break;

            case (SPANISH):
              if (FlashConfig.ChimeMode == CHIME_OFF)
              {
                sprintf(String, "El timbre cada hora esta desactivado");
                String[23] = (UINT8)129; // a-acute
              }
              else if (FlashConfig.ChimeMode == CHIME_ON)
              {
                sprintf(String, "El timbre cada hora esta activado");
                String[23] = (UINT8)129; // a-acute
              }
              else if (FlashConfig.ChimeMode == CHIME_DAY)
              {
                sprintf(String, "El timbre cada hora esta en intermitente, de %u:00 a %u:00", FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff);
                String[23] = (UINT8)129; // a-acute
              }
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              if (FlashConfig.ChimeMode == CHIME_OFF)
              {
                sprintf(String, "Hourly chime is Off");
              }
              else if (FlashConfig.ChimeMode == CHIME_ON)
              {
                sprintf(String, "Hourly chime is On");
              }
              else if (FlashConfig.ChimeMode == CHIME_DAY)
              {
                sprintf(String, "Hourly chime is intermittent, from %u:00 to %u:00", FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff);
              }
            break;
          }
          scroll_string(24, String);
        break;

        case (0x0F): // SETUP_NIGHT_LIGHT
          /* Night light. */
          switch (FlashConfig.Language)
          {
           case (CZECH):
              if (FlashConfig.NightLightMode == NIGHT_LIGHT_OFF)
              {
                sprintf(String, "Nocni svetlo je vypnuto.");
                String[3] = (UINT8)136; // c-caron
                String[4] = (UINT8)131; // i-acute
                String[8] = (UINT8)130; // e-caron
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_ON)
              {
                sprintf(String, "Nocni svetlo je zapnuto.");
                String[3] = (UINT8)136; // c-caron
                String[4] = (UINT8)131; // i-acute
                String[8] = (UINT8)130; // e-caron
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT)
              {
                sprintf(String, "Nocni svetlo zapnuto jen od %u:00 do %u:00.", FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOff);
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
              {
                sprintf(String, "Nocni svetlo je zapnuto automaticky.");
              }
            break;

           case (FRENCH):
              if (FlashConfig.NightLightMode == NIGHT_LIGHT_OFF)
              {
                sprintf(String, "La veilleuse de nuit est a Off");
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_ON)
              {
                sprintf(String, "La veilleuse de nuit est a On");
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT)
              {
                sprintf(String, "La veilleuse de nuit est intermittente, de %u:00 a %u:00", FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOff);
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
              {
                sprintf(String, "La veilleuse de nuit est automatique");
              }
            break;

           case (SPANISH):
              if (FlashConfig.NightLightMode == NIGHT_LIGHT_OFF)
              {
                sprintf(String, "La luz nocturna esta desactivado");
                String[19] = (UINT8)129; // a-acute
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_ON)
              {
                sprintf(String, "La luz nocturna esta activado");
                String[19] = (UINT8)129; // a-acute
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT)
              {
                sprintf(String, "La luz nocturna esta en intermitente, de %u:00 a %u:00", FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOff);
                String[19] = (UINT8)129; // a-acute
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
              {
                sprintf(String, "La luz nocturna esta en automatica");
                String[19] = (UINT8)129; // a-acute
                String[29] = (UINT8)129; // a-acute
              }
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              if (FlashConfig.NightLightMode == NIGHT_LIGHT_OFF)
              {
                sprintf(String, "Night light is Off");
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_ON)
              {
                sprintf(String, "Night light is On");
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT)
              {
                sprintf(String, "Night light is intermittent, from %u:00 to %u:00", FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOff);
              }
              else if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
              {
                sprintf(String, "Night light is automatic");
              }
            break;
          }
          scroll_string(24, String);
        break;

        case (0x12): // SETUP_AUTO_BRIGHT
          /* Light level. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              if (FlashConfig.FlagAutoBrightness == FLAG_ON)
              {
                sprintf(String, "zobrazeni");
                String[9] = (UINT8)131;  // i-acute
                sprintf(String, "Automaticke nastaveni jasu je zapnuto. Jas: %u   hystereze: %u   %s: %u%c.", adc_read_light(), AverageLightLevel, String, Pwm[PWM_BRIGHTNESS].Cycles, '%');
                String[10] = (UINT8)138;  // e-acute
                String[20] = (UINT8)131;  // i-acute
              }
              else
              {
                sprintf(String, "Automaticke nastaveni jasu je vypnuto.");
                String[10] = (UINT8)138;  // e-acute
                String[20] = (UINT8)131;  // i-acute
              }
            break;

            case (FRENCH):
              if (FlashConfig.FlagAutoBrightness == FLAG_ON)
              {
                sprintf(String, "Intensite automatique est a On   Luminosite: %u   Hysteresis: %u   Affichage: %u%c", adc_read_light(), AverageLightLevel, Pwm[PWM_BRIGHTNESS].Cycles, '%');
                String[8]  = (UINT8)31;  // e accent aigu.
                String[41] = (UINT8)31;  // e accent aigu.
              }
              else
              {
                sprintf(String, "Intensite automatique est a Off");
                String[8] = (UINT8)31; // e accent aigu.
              }
            break;

            case (SPANISH):
              if (FlashConfig.FlagAutoBrightness == FLAG_ON)
              {
                sprintf(String, "Intensidad automatica esta activada   Luminosidad: %u   Hysteresis: %u   Monitor: %u%c", adc_read_light(), AverageLightLevel, Pwm[PWM_BRIGHTNESS].Cycles, '%');
                String[16] = (UINT8)129; // a-acute
                String[25] = (UINT8)129; // a-acute
              }
              else
              {
                sprintf(String, "Intensidad automatica esta desactivada");
                String[16] = (UINT8)129; // a-acute
                String[25] = (UINT8)129; // a-acute
              }
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              if (FlashConfig.FlagAutoBrightness == FLAG_ON)
              {
                sprintf(String, "Auto brightness is On   Light level: %u   Hysteresis: %u   Display: %u%c", adc_read_light(), AverageLightLevel, Pwm[PWM_BRIGHTNESS].Cycles, '%');
              }
              else
              {
                sprintf(String, "Auto brightness is Off");
              }
            break;
         }
         scroll_string(24, String);

        break;
      }
    }
  }


  if (IrCommand == IR_DISPLAY_EVENTS_TODAY)
  {
    // scroll_string(24, "Button 'Over': Events today");
    Dum1UInt8 = 0;
    for (Loop1UInt8 = 0; Loop1UInt8 < MAX_EVENTS; ++Loop1UInt8)
    {
      if ((CalendarEvent[Loop1UInt8].Month == CurrentMonth) && (CalendarEvent[Loop1UInt8].Day == CurrentDayOfMonth))
      {
        ++Dum1UInt8;

        switch (FlashConfig.Language)
        {
          case (ENGLISH):
          case (GERMAN):
          default:
            sprintf(String, "%s %u: %s   /   ", MonthName[FlashConfig.Language][CurrentMonth], CurrentDayOfMonth, CalendarEvent[Loop1UInt8].Description);
          break;

          case (CZECH):
          case (FRENCH):
          case (SPANISH):
            sprintf(String, "%u %s: %s   /   ", CurrentDayOfMonth, MonthName[FlashConfig.Language][CurrentMonth], CalendarEvent[Loop1UInt8].Description);
          break;
        }
        scroll_string(24, String);
      }
    }

    switch (FlashConfig.Language)
    {
      case (CZECH):
        if (Dum1UInt8 == 0)
        {
          sprintf(String, "Dnes zadna udalost.");
          String[5]  = (UINT8)137;  // z-caron
          String[6]  = (UINT8)129;  // a-acute
          String[9]  = (UINT8)129;  // a-acute
          String[13] = (UINT8)129;  // a-acute
        }
        else
        {
          if (Dum1UInt8 == 1)
          {
            sprintf(String, "Dnes 1 udalost.");
            String[9] = (UINT8)129; // a-acute
          }
          else
          {
            sprintf(String, "udalosti");
            String[2] = (UINT8)129; // a-acute
            sprintf(String, "Dnes %u %s.", Dum1UInt8, String);
          }
        }
      break;

      case (FRENCH):
        if (Dum1UInt8 == 0)
        {
          sprintf(String, "Aucun evenement aujourd'hui");
          String[6] = (UCHAR)31; // e accent aigu.
          String[8] = (UCHAR)31; // e accent aigu
        }
        else
        {
          if (Dum1UInt8 == 1)
          {
            sprintf(String, "1 evenement aujourd'hui");
            String[2] = (UCHAR)31; // e accent aigu.
            String[4] = (UCHAR)31; // e accent aigu
          }
          else
          {
            sprintf(String, "%u evenements aujourd'hui", Dum1UInt8);
            String[strlen(String) - 20] = (UCHAR)31; // e accent aigu.
            String[strlen(String) - 22] = (UCHAR)31; // e accent aigu
          }
        }
      break;

      case (SPANISH):
        if (Dum1UInt8 == 0)
        {
          sprintf(String, "No hay eventos hoy");
        }
        else
        {
          if (Dum1UInt8 == 1)
          {
            sprintf(String, "Un evento hoy");
          }
          else
          {
            sprintf(String, "%u eventos hoy", Dum1UInt8);
          }
        }
      break;

      case (ENGLISH):
      case (GERMAN):
      default:
        if (Dum1UInt8 == 0)
          sprintf(String, "No event today");
        else if (Dum1UInt8 == 1)
          sprintf(String, "1 event today");
        else
        {
          sprintf(String, "%u events today", Dum1UInt8);
        }
      break;
    }
    scroll_string(24, String);
  }


  if (IrCommand == IR_DISPLAY_EVENTS_THIS_WEEK)
  {
    // scroll_string(24, "Button 'Mute': Events this week");

    /* Retrieve beginning of week (previous Sunday). */
    Dum1UInt8 = get_day_of_week(CurrentYear, CurrentMonth, CurrentDayOfMonth);

    if (DebugBitMask & DEBUG_EVENT)
      uart_send(__LINE__, "Today is %s [DayOfWeek %u] %2u-%s-%4.4u\r", DayName[FlashConfig.Language][Dum1UInt8], Dum1UInt8, CurrentDayOfMonth, MonthName[FlashConfig.Language][CurrentMonth], CurrentYear);

    DumDayOfMonth = CurrentDayOfMonth;
    DumMonth      = CurrentMonth;
    DumYear       = CurrentYear;

    if (Dum1UInt8 != SUN)
    {
      do
      {
        --DumDayOfMonth;
        if (DumDayOfMonth == 0)
        {
          --DumMonth;
          if (DumMonth == 0)
          {
            --DumYear;
            DumMonth = 12;
          }
          DumDayOfMonth = get_month_days(DumYear, DumMonth);
        }

        Dum1UInt8 = get_day_of_week(DumYear, DumMonth, DumDayOfMonth);

        if (DebugBitMask & DEBUG_EVENT)
          uart_send(__LINE__, "Back one day, to %8s [%u] %2u-%s-%4.4u\r", DayName[FlashConfig.Language][Dum1UInt8], Dum1UInt8, DumDayOfMonth, MonthName[FlashConfig.Language][DumMonth], DumYear);
      } while (Dum1UInt8 != SUN);
    }

    /* Display date of the first day of the week (Sunday) that has been found. */
    if (DebugBitMask & DEBUG_EVENT)
      uart_send(__LINE__, "Week beginning: %2u-%s-%4u\r", DumDayOfMonth, MonthName[FlashConfig.Language][DumMonth], DumYear);

    /* And display all events for each of the next seven days, starting with this Sunday. */
    Dum1UInt8 = 0;

    /* Scan each day of current week. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 7; ++Loop1UInt8)
    {
      if (DebugBitMask & DEBUG_EVENT)
        uart_send(__LINE__, "Checking date:  %2u-%s-%4.4u\r", DumDayOfMonth, MonthName[ENGLISH][DumMonth], DumYear);

      /* Check every calendar event to find a match with date under evaluation. */
      for (Loop2UInt8 = 0; Loop2UInt8 < MAX_EVENTS; ++Loop2UInt8)
      {
        if (DebugBitMask & DEBUG_EVENT)
          uart_send(__LINE__, "Checking event number %2u  %2llu %10s [%s]\r", Loop2UInt8, CalendarEvent[Loop2UInt8].Day, MonthName[ENGLISH][CalendarEvent[Loop2UInt8].Month], CalendarEvent[Loop2UInt8].Description);

        if ((CalendarEvent[Loop2UInt8].Month == DumMonth) && (CalendarEvent[Loop2UInt8].Day == DumDayOfMonth))
        {
          if (DebugBitMask & DEBUG_EVENT)
            uart_send(__LINE__, "Match found !\r");

          switch (FlashConfig.Language)
          {
            case (CZECH):
            case (FRENCH):
            case (SPANISH):
              sprintf(String, "%u %s: %s   /   ", DumDayOfMonth, MonthName[FlashConfig.Language][DumMonth], CalendarEvent[Loop2UInt8].Description);
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              sprintf(String, "%s %u: %s   /   ", MonthName[FlashConfig.Language][DumMonth], DumDayOfMonth, CalendarEvent[Loop2UInt8].Description);
            break;
          }
          scroll_string(24, String);

          ++Dum1UInt8;
        }
      }

      /* Check next date. */
      ++DumDayOfMonth;
      if (DumDayOfMonth > get_month_days(DumYear, DumMonth))
      {
        DumDayOfMonth = 1;
        ++DumMonth;
        if (DumMonth > 12)
        {
          DumMonth = 1;
          ++DumYear;
        }
      }
    }

    switch (FlashConfig.Language)
    {
      case (CZECH):
        if (Dum1UInt8 == 0)
        {
          sprintf(String, "   Tento tyden zadna udalost.");
          String[10] = (UINT8)132; // y-acute
          String[15] = (UINT8)137; // z-caron
          String[16] = (UINT8)129; // a-acute
          String[19] = (UINT8)129; // a-acute
          String[23] = (UINT8)129; // a-acute
        }
        else
        {
          if (Dum1UInt8 == 1)
          {
            sprintf(String, "   Tento tyden 1 udalost.");
            String[10] = (UINT8)132; // y-acute
            String[15] = (UINT8)137; // z-caron
            String[19] = (UINT8)129; // a-acute
          }
          else
          {
            sprintf(String, "udalosti");
            String[2] = (UINT8)129; // a-acute
            String[8] = (UINT8)131; // i-acute
            sprintf(String, "   Tento tyden %u %s.", Dum1UInt8, String);
            String[10] = (UINT8)132; // y-acute
         }
        }
      break;

      case (FRENCH):
        if (Dum1UInt8 == 0)
        {
          sprintf(String, "   Aucun evenement cette semaine");
          String[9]  = (UCHAR)31; // e accent aigu.
          String[11] = (UCHAR)31; // e accent aigu
        }
        else
        {
          if (Dum1UInt8 == 1)
          {
            sprintf(String, "   1 evenement cette semaine");
            String[5] = (UCHAR)31; // e accent aigu.
            String[7] = (UCHAR)31; // e accent aigu
          }
          else
          {
            sprintf(String, "   %u evenements cette semaine", Dum1UInt8);
            String[strlen(String) - 22] = (UCHAR)31; // e accent aigu.
            String[strlen(String) - 24] = (UCHAR)31; // e accent aigu
          }
        }
      break;

      case (SPANISH):
        if (Dum1UInt8 == 0)
        {
          sprintf(String, "   No hay eventos esta semana");
        }
        else
        {
          if (Dum1UInt8 == 1)
          {
            sprintf(String, "   1 evento esta semana");
          }
          else
          {
            sprintf(String, "   %u eventos esta semana", Dum1UInt8);
          }
        }
      break;

      case (ENGLISH):
      case (GERMAN):
      default:
        if (Dum1UInt8 == 0)
          sprintf(String, "   No event this week");
        else
        {
          if (Dum1UInt8 == 1)
          {
            sprintf(String, "   1 event this week");
          }
          else
          {
            sprintf(String, "   %u events this week", Dum1UInt8);
          }
        }
      break;
    }
    scroll_string(24, String);
  }


  if (IrCommand == IR_DISPLAY_SECOND)
  {
    // scroll_string(24, "Button 'Time': Display second");
    /* Wait till eventual current scrolling has completed. */
    while (ScrollDotCount);

    /* Allow two middle dots blinking during minutes and seconds display. */
    CurrentClockMode = MODE_SHOW_TIME;

    /* Reset the number of seconds the system has been idle. */
    IdleNumberOfSeconds = 0;

    /* Display seconds during: 10 times 500 msec (5 seconds). */
    for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
    {
      /* Synchronize seconds update on clock display with middle dots blinking to prevent a glitch while updating. */
      if (sem_acquire_timeout_ms(&SemSync, 1000) == true)
      {
        Time_RTC = Read_RTC();

        fill_display_buffer_4X7(0, 0x30 + Time_RTC.minutes / 16);
        fill_display_buffer_4X7(5, 0x30 + Time_RTC.minutes % 16);
        fill_display_buffer_4X7(10, 0x3A); // slim ":"
        fill_display_buffer_4X7(12, 0x30 + Time_RTC.seconds / 16);
        fill_display_buffer_4X7(17, 0x30 + Time_RTC.seconds % 16);
      }
    }

    FlagUpdateTime = FLAG_ON; // request a time update when done with seconds display.
  }

  return;
}
#endif





/* $PAGE */
/* $TITLE=process_scroll_queue() */
/* ------------------------------------------------------------------ *\
          Handle the tag that has been put in scroll queue.
\* ------------------------------------------------------------------ */
void process_scroll_queue(void)
{
  UCHAR DayMask[16];
  UCHAR TempString[DISPLAY_BUFFER_SIZE];
  UCHAR String[256];
  UCHAR Voltage[12];

  UINT8 Command;
  UINT8 DhtDisplay[4];
  UINT8 Dum1UInt8;
  UINT8 Head;
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 Tag;
  UINT8 Tail;

  UINT16 Loop1UInt16;

  UINT64 CurrentTimeStamp;

  int Dum1Int;

  float DegreeC;
  float DegreeF;
  float Humidity;
  float Pressure;
  float Temperature;
  float TemperatureF;
  float Volts;


  /* Check if for any reason, the function has been called while the scroll queue is empty. */
  if (ScrollQueueHead == ScrollQueueTail) return;



  while (ScrollQueueTail != ScrollQueueHead)
  {
    /* For debugging purposes - Keep track of scroll queue head and scroll queue tail before dequeuing. */
    // Head = ScrollQueueHead;
    // Tail = ScrollQueueTail;

    Tag = scroll_unqueue();

    /* NOTE: A special Calendar Event (with Description = "Info") may be inserted in the bundle of calendar events.
             This specific case may be handled (see below) to display whatever information we want. If no Calendar Event
             exists with a description "Info", this code will not interfere with normal clock behavior. This tag may be
             triggered while running in ISR context and execution will executed in main() context, giving more time to
             execute whatever process we want to perform. The same apply to TAG_DEBUG who may be called from ISR's. */
    if (strcmp(CalendarEvent[Tag].Description, "Info") == 0) Tag = TAG_INFO;


    /* When Tag number is lower than MAX_EVENTS, it means to scroll a Calendar Event. */
    if (Tag < MAX_EVENTS)
    {
      /* We must scroll a Calendar Event. */
      scroll_string(24, CalendarEvent[Tag].Description);

      while (ScrollDotCount)
        sleep_ms(100);  // wait until scrolling is over.
    }
    else
    {
      /* Special cases when tag number is at the end of UINT8 (beginning at 0xFF and counting down). */
      switch (Tag)
      {
          case (TAG_AMBIENT_LIGHT):
          /* If we are not currently in setup mode, display ambient light. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              sprintf(String, "Jas: %u   hystereze: %u   displej: %u%c.    ", adc_read_light(), AverageLightLevel, Pwm[PWM_BRIGHTNESS].Cycles, '%');
            break;

            case (FRENCH):
              sprintf(String, "Luminosite: %u   Hysteresis: %u   Affichage: %u%c    ", adc_read_light(), AverageLightLevel, Pwm[PWM_BRIGHTNESS].Cycles, '%');
              String[9] = (UINT8)31; // e accent aigu.
            break;

            case (SPANISH):
              sprintf(String, "Luminosidad: %u   Hysteresis: %u   Monitor: %u%c    ", adc_read_light(), AverageLightLevel, Pwm[PWM_BRIGHTNESS].Cycles, '%');
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              sprintf(String, "Ambient light: %u   Hysteresis: %u   Display: %u%c    ", adc_read_light(), AverageLightLevel, Pwm[PWM_BRIGHTNESS].Cycles, '%');
            break;
          }
          scroll_string(24, String);
        break;



        case (TAG_BME280_DEVICE_ID):
          #ifdef BME280_SUPPORT  // if a BME280 outside temperature, humidity and atmospheric pressure sensor has been installed by user.
          Dum1UInt8 = bme280_read_device_id();
          sprintf(String, "BME280 device ID: 0x%2.2X    ", Dum1UInt8);
          scroll_string(24, String);
          #endif  // BME280_SUPPORT
        break;



        case (TAG_BME280_TEMP):
          #ifdef BME280_SUPPORT // if a BME280 outdoor temperature, humidity and barometric pressure sensor has been installed by user.
          /* This block will read and display temperature, relative humidity and atmospheric pressure from an optional BME280 sensor
             that must be bought and installed by user (refer to User Guide for details). Since it will usually be used for outside
             temperature reading, the strings "Out" ("Ext: " in French) is added to the data being scrolled (see below). You must changes this data if
             you plan to use the BME280 to read something else than outside temperature. */

          /* -------------------------------------------------------------------- *\
                  Read outdoor temperature, humidity and barometric pressure
                               from BME280 installed by user
               and scroll them on clock display only if no error while reading.
          \* -------------------------------------------------------------------- */
          if (bme280_get_temp() == 0)
          {
            // Build-up the string to be scrolled, first with temperature...
            if (FlashConfig.TemperatureUnit == CELSIUS)
            {
              // ...in Celsius.
              switch (FlashConfig.Language)
              {
                case (CZECH):
                  sprintf(String, "Venku: %2.2f %cC, %2.2f %%, %4.2f hPa.", Bme280Data.TemperatureC, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                 break;

                case (FRENCH):
                  sprintf(String, "Ext: %2.2f%cC  Hum: %2.2f%%  Pression: %4.2f hPa ", Bme280Data.TemperatureC, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                break;

                case (SPANISH):
                  sprintf(String, "Ext: %2.2f%cC  Hum: %2.2f%%  Presion: %4.2f hPa ", Bme280Data.TemperatureC, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                  String[34] = (UINT8)139; // o-acute
                break;

                case (ENGLISH):
                default:
                  sprintf(String, "Out: %2.2f%cC  Hum: %2.2f%%  Pressure: %4.2f hPa ", Bme280Data.TemperatureC, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                break;

              }
            }
            else
            {
              // ...or in Fahrenheit.
              switch (FlashConfig.Language)
              {
                case (CZECH):
                  sprintf(String, "Venku: %2.2f %cF, %2.2f %%, %4.2f hPa.", Bme280Data.TemperatureF, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                break;

                case (FRENCH):
                  sprintf(String, "Ext: %2.2f%cF  Hum: %2.2f%%  Pression: %4.2f hPa ", Bme280Data.TemperatureF, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                break;

                case (SPANISH):
                  sprintf(String, "Ext: %2.2f%cF  Hum: %2.2f%%  Presion: %4.2f hPa ", Bme280Data.TemperatureF, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                  String[34] = (UINT8)139; // o-acute
                break;

                case (ENGLISH):
                default:
                  sprintf(String, "Out: %2.2f%cF  Hum: %2.2f%%  Pressure: %4.2f hPa ", Bme280Data.TemperatureF, 0x80, Bme280Data.Humidity, Bme280Data.Pressure);
                break;
              }
            }


            if (DebugBitMask & DEBUG_BME280)
            {
              uart_send(__LINE__, String);
              uart_send(__LINE__, "\r");
            }
            scroll_string(24, String);
          }

          // For statistic purposes, display the cumulative number of errors while reading BME280 data.
          sprintf(String, " [%lu/%lu]    ", Bme280Data.Bme280Errors, Bme280Data.Bme280ReadCycles);
          if (DebugBitMask & DEBUG_BME280)
          {
            uart_send(__LINE__, String);
            uart_send(__LINE__, "\r\r");
          }
          scroll_string(24, String);
          /* ------------------------- END OF BLOCK ---------------------------- */
          #endif  // BME280_SUPPORT
        break;



        case (TAG_WIFI_CREDENTIALS):
          /* Scroll Wi-Fi credentials saved in flash memory. */
          sprintf(String, "HOST: [%s]   SSID: [%s]   Pwd: [%s]", FlashConfig.Hostname, FlashConfig.SSID, FlashConfig.Password);
          scroll_string(24, String);
        break;



        case (TAG_DATE):
          /* Scroll current date. */
          get_date_string(String);
          scroll_string(24, String);
        break;



        case (TAG_DEBUG):
          /* For debug purposes. Display a few variables while implementing DST support. */
          sprintf(String, "L: %2.2X  FSTDoM: %2.2X  FSTM: %2.2X    ", FlashConfig.Language, FlagSetupClock[SETUP_DAY_OF_MONTH], FlagSetupClock[SETUP_MONTH]);
          scroll_string(15, String);
        break;



        case (TAG_DHT22_TEMP):
          /* This block will read and display temperature and relative humidity from an optional DHT22 sensor that must be bought and installed by user
             (refer to User Guide for details). If it is used for outside temperature reading, the string "Out" ("Ext: " in French) may be added between
             the quotes in the call to function "format_temp()" below. */
          #ifdef DHT_SUPPORT
          /* Send command to core 1 to read temperature data from DHT22. */
          core_queue(1, CORE1_READ_DHT);

          /* Initializations. */
          ++DhtData.DhtReadCycles;
          Loop1UInt16 = 0;
          TempString[0] = 0x00;  // init as null string.

          /* While the date is scrolling, wait for core 1 to complete read cycle with DHT22. */
          while (1)
          {
            /* Read response from core 1. */
            Command = core_unqueue(0);

            if (Command == MAX_CORE_QUEUE)
            {
              /* If core 1 has not responded yet, wait some more time and then declare a time-out. */
              sleep_ms(5);

              if (DebugBitMask & DEBUG_CORE)
                uart_send(__LINE__, "-------------------- CORE 0 - Still waiting for core 1 response [%2u msec] Head: %u Tail: %u (Command: %u)\r", (Loop1UInt16 * 10), Core0QueueHead, Core0QueueTail, Command);

              ++Loop1UInt16;

              if (Loop1UInt16 > 50)
              {
                if (DebugBitMask & DEBUG_CORE)
                  uart_send(__LINE__, "-------------------- CORE 0 - Time-out while waiting for core 1 [%u msec] %u - %u (%u)\r", (Loop1UInt16 * 50), Core0QueueHead, Core0QueueTail, Command);

                ++DhtData.DhtErrors;
                break;
              }
            }


            /* If core 1 responded with an error code while trying to read DHT22 temperature data. */
            if (Command == CORE0_DHT_ERROR)
            {
              if (DebugBitMask & DEBUG_CORE)
                uart_send(__LINE__, "-------------------- CORE 0 - Returned an error while trying to read DHT22\r");

              ++DhtData.DhtErrors;
              break;
            }


            /* If core 1 read DHT temperature data successfully. */
            if (Command == CORE0_DHT_READ_COMPLETED)
            {
              if (DebugBitMask & DEBUG_CORE)
              {
                uart_send(__LINE__, "-------------------- CORE 0 - Returned DHT read successful - Temp: %f   Hum: %f\r", DhtData.Temperature, DhtData.Humidity);

                CurrentTimeStamp = time_us_64();
                uart_send(__LINE__, "Current TimeStamp: %llu\r", CurrentTimeStamp);
                uart_send(__LINE__, "DHT22 TimeStamp: %llu\r", DhtData.TimeStamp);
                uart_send(__LINE__, "TimeStamp difference: : %2.2f milliseconds\r", ((CurrentTimeStamp - DhtData.TimeStamp) / 1000.0));
                uart_send(__LINE__, "Temperature: %2.2f\r", DhtData.Temperature);
                uart_send(__LINE__, "Humidity:    %2.2f\r", DhtData.Humidity);

                /* Get the formatted string to be scrolled on clock display. */
                uart_send(__LINE__, "-------------------- CORE 0 - TempString before: [%s]\r", TempString);
              }

              #ifdef RELEASE_VERSION
              format_temp(TempString, "", DhtData.Temperature, DhtData.Humidity, 0.0);
              #else
              format_temp(TempString, "Int: ", DhtData.Temperature, DhtData.Humidity, 0.0);
              #endif

              if (DebugBitMask & DEBUG_CORE)
                uart_send(__LINE__, "-------------------- CORE 0 - TempString after: [%s]\r", TempString);

              break;
            }
          }

          /* No matter if we scroll temperature data or if DHT22 read cycle failed (in which case we don't scroll temperature data),
             in all cases, let's display the total number or DHT22 read errors on the total number of DHT22 read cycles. */
          sprintf(&TempString[strlen(TempString)], " (%lu/%lu)    ", DhtData.DhtErrors, DhtData.DhtReadCycles);
          scroll_string(24, TempString);
          #endif  // DHT_SUPPORT
        break;



        case (TAG_DS3231_TEMP):
          /* This block will read and display temperature from the DS3231 real-time clock IC in the Green-Clock that also provide a temperature sensor. */
          Temperature = get_ambient_temperature(FlashConfig.TemperatureUnit); // read ambient temperature from DS3231.

          /* Write temperature to the string to be displayed... */
          if (FlashConfig.TemperatureUnit == CELSIUS)
          {
            /* ...in Celsius. */
            sprintf(String, "RTC IC: %2.2f%cC    ", Temperature, 0x80);
          }
          else
          {
            /* ...or in Fahrenheit. */
            sprintf(String, "RTC IC:  %2.2f%cF    ", Temperature, 0x80);
          }
          scroll_string(24, String);
        break;



        case (TAG_DST):
          /* Display current daylight saving time setting.
             No support = No DST support at all.
             Inactive   = DST is supported, but we are not during DST period of the year.
             Active     = DST is supported and we are during DST period of the year. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              if (FlashConfig.DSTCountry == DST_NONE)
              {
                sprintf(String, "Funkce letniho casu je vypnuta.");
                String[11] = (UINT8)131; // i-acute
                String[15] = (UINT8)136; // c-caron
              }
              else
              {
                switch(FlashConfig.DSTCountry)
                {
                  case (DST_AUSTRALIA):
                    sprintf(String, "Letni cas: Australie");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                    String[16] = (UINT8)129; // a-acute
                  break;

                  case (DST_AUSTRALIA_HOWE):
                    sprintf(String, "Letni cas: Australie Howe");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                    String[16] = (UINT8)129; // a-acute
                  break;

                  case (DST_CHILE):
                    sprintf(String, "Letni cas: Cile");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                    String[12] = (UINT8)136; // c-caron
                  break;

                  case (DST_CUBA):
                    sprintf(String, "Letni cas: Kuba");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;

                  case (DST_EUROPE):
                    sprintf(String, "Letni cas: Evropa");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;

                  case (DST_ISRAEL):
                    sprintf(String, "Letni cas: Izrael");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;

                  case (DST_LEBANON):
                    sprintf(String, "Letni cas: Libanon");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;

                  case (DST_MOLDOVA):
                    sprintf(String, "Letni cas: Moldavie");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;

                  case (DST_NEW_ZEALAND):
                    sprintf(String, "Letni cas: Novy Zeland");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;

                  case (DST_NORTH_AMERICA):
                    sprintf(String, "Letni cas: Severni Amerika");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;

                  case (DST_PALESTINE):
                    sprintf(String, "Letni cas: Palestina");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                 break;

                  case (DST_PARAGUAY):
                    sprintf(String, "Letni cas: Paraguay");
                    String[4] = (UINT8)131; // i-acute
                    String[6] = (UINT8)136; // c-caron
                  break;
                }

                /* Announce if Daylight Saving Time ("DST") is currently active or inactive, depending of current date. */
                if (FlashConfig.FlagSummerTime == FLAG_ON)
                {
                  strcat(String, " - je letni cas    ");
                  String[10] = (UINT8)131; // i-acute
                  String[12] = (UINT8)136; // c-caron
                }
                else
                {
                  strcat(String, " - neni letni cas    ");
                  String[6] = (UINT8)131; // i-acute
                  String[12] = (UINT8)131; // i-acute
                  String[14] = (UINT8)136; // c-caron
                }
              }
            break;

            case (FRENCH):
              if (FlashConfig.DSTCountry == DST_NONE)
              {
                sprintf(String, "Heure avancee non supportee    ");
                String[11] = (UINT8)31;  // e accent aigu.
                String[25] = (UINT8)31;  // e accent aigu.
              }
              else
              {
                switch(FlashConfig.DSTCountry)
                {
                  case (DST_AUSTRALIA):
                    sprintf(String, "Heure avancee: Australie");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_AUSTRALIA_HOWE):
                    sprintf(String, "Heure avancee: Australie Howe");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_CHILE):
                    sprintf(String, "Heure avancee: Chili");
                    String[11] = (UINT8)31;  // e accent aigu.
                   break;

                  case (DST_CUBA):
                    sprintf(String, "Heure avancee: Cuba");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_EUROPE):
                    sprintf(String, "Heure avancee: Europe");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_ISRAEL):
                    sprintf(String, "Heure avancee: Israel");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_LEBANON):
                    sprintf(String, "Heure avancee: Liban");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_MOLDOVA):
                    sprintf(String, "Heure avancee: Moldavie");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_NEW_ZEALAND):
                    sprintf(String, "Heure avancee: Nouvelle-Zelande");
                    String[11] = (UINT8)31;  // e accent aigu.
                    String[25] = (UINT8)31;  // e accent aigu.
                 break;

                  case (DST_NORTH_AMERICA):
                    sprintf(String, "Heure avancee: Amerique du Nord");
                    String[11] = (UINT8)31;  // e accent aigu.
                    String[17] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_PALESTINE):
                    sprintf(String, "Heure avancee: Palestine");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;

                  case (DST_PARAGUAY):
                    sprintf(String, "Heure avancee: Paraguay");
                    String[11] = (UINT8)31;  // e accent aigu.
                  break;
                }

                /* Announce if Daylight Saving Time ("DST") is currently active or inactive, depending of current date. */
                if (FlashConfig.FlagSummerTime == FLAG_ON)
                {
                  strcat(String, "- active    ");
                }
                else
                {
                  strcat(String, "- inactive    ");
                }
              }
            break;

            case (SPANISH):
              if (FlashConfig.DSTCountry == DST_NONE)
              {
                sprintf(String, "Horario de verano no compatible    ");
              }
              else
              {
                switch(FlashConfig.DSTCountry)
                {
                  case (DST_AUSTRALIA):
                    sprintf(String, "Horario de verano: Australia");
                  break;

                  case (DST_AUSTRALIA_HOWE):
                    sprintf(String, "Horario de verano: Australia Howe");
                  break;

                  case (DST_CHILE):
                    sprintf(String, "Horario de verano: Chile");
                   break;

                  case (DST_CUBA):
                    sprintf(String, "Horario de verano: Cuba");
                  break;

                  case (DST_EUROPE):
                    sprintf(String, "Horario de verano: Europa");
                  break;

                  case (DST_ISRAEL):
                    sprintf(String, "Horario de verano: Israel");
                  break;

                  case (DST_LEBANON):
                    sprintf(String, "Horario de verano: Libano");
                    String[20] = (UINT8)131;  // i-acute.
                  break;

                  case (DST_MOLDOVA):
                    sprintf(String, "Horario de verano: Moldova");
                  break;

                  case (DST_NEW_ZEALAND):
                    sprintf(String, "Horario de verano: Nueva Zelandia");
                 break;

                  case (DST_NORTH_AMERICA):
                    sprintf(String, "Horario de verano: America del Norte");
                    String[21] = (UINT8)31;  // e-acute
                  break;

                  case (DST_PALESTINE):
                    sprintf(String, "Horario de verano: Palestina");
                  break;

                  case (DST_PARAGUAY):
                    sprintf(String, "Horario de verano: Paraguay");
                  break;
                }

                /* Announce if Daylight Saving Time ("DST") is currently active or inactive, depending of current date. */
                if (FlashConfig.FlagSummerTime == FLAG_ON)
                {
                  strcat(String, "- activado    ");
                }
                else
                {
                  strcat(String, "- desactivado    ");
                }
              }
            break;

            case (ENGLISH):
            default:
              if (FlashConfig.DSTCountry == DST_NONE)
              {
                sprintf(String, "No support for daylight saving time    ");
              }
              else
              {
                switch(FlashConfig.DSTCountry)
                {
                  case (DST_AUSTRALIA):
                    sprintf(String, "DST setting: Australia");
                  break;

                  case (DST_AUSTRALIA_HOWE):
                    sprintf(String, "DST setting: Australia Howe");
                  break;

                  case (DST_CHILE):
                    sprintf(String, "DST setting: Chile");
                  break;

                  case (DST_CUBA):
                    sprintf(String, "DST setting: Cuba");
                  break;

                  case (DST_EUROPE):
                    sprintf(String, "DST setting: Europe");
                  break;

                  case (DST_ISRAEL):
                    sprintf(String, "DST setting: Israel");
                  break;

                  case (DST_LEBANON):
                    sprintf(String, "DST setting: Lebanon");
                  break;

                  case (DST_MOLDOVA):
                    sprintf(String, "DST setting: Moldova");
                  break;

                  case (DST_NEW_ZEALAND):
                    sprintf(String, "DST setting: New Zealand");
                  break;

                  case (DST_NORTH_AMERICA):
                    sprintf(String, "DST setting: North America");
                  break;

                  case (DST_PALESTINE):
                    sprintf(String, "DST setting: Palestine");
                  break;

                  case (DST_PARAGUAY):
                    sprintf(String, "DST setting: Paraguay");
                  break;
                }

                /* Announce if Daily Saving Time ("DST") is currently active or inactive, depending of current date. */
                if (FlashConfig.FlagSummerTime == FLAG_ON)
                  strcat(String, " - DST active    ");
                else
                  strcat(String, " - DST inactive    ");
              }
            break;
          }
          scroll_string(24, String);
        break;



        case (TAG_FIRMWARE_VERSION):
          switch (FlashConfig.Language)
          {
            case (CZECH):
              sprintf(String, "UPico Green Clock - verze firmwaru %s    ", FIRMWARE_VERSION);
            break;

            case (ENGLISH):
            case (GERMAN):
            default:
              sprintf(String, "Pico Green Clock - Firmware Version %s    ", FIRMWARE_VERSION);
            break;

            case (FRENCH):
              sprintf(String, "Pico Green Clock - Microcode Version %s    ", FIRMWARE_VERSION);
            break;

            case (SPANISH):
              sprintf(String, "Pico Green Clock - Version del firmware %s    ", FIRMWARE_VERSION);
              String[24] = (UINT8)139; // o-acute
            break;
          }
          scroll_string(24, String);
        break;



        case (TAG_IDLE_MONITOR):
          switch (FlashConfig.Language)
          {
            case (CZECH):
              sprintf(String, "Vytizeni: %llu    ", IdleMonitor[13]);
              String[3] = (UINT8)131; // i-acute
              String[4] = (UINT8)137; // z-caron
              String[8] = (UINT8)131; // i-acute
            break;
            case (SPANISH):
              sprintf(String, "Monitor del tiempo de inactividad del sistema: %llu    ", IdleMonitor[13]);
            break;
            case (ENGLISH):
            case (FRENCH):
            case (GERMAN):
            default:
              sprintf(String, "System idle monitor: %llu    ", IdleMonitor[13]);
            break;
          }
          scroll_string(24, String);
        break;



        case (TAG_INFO):
          if (DebugBitMask & DEBUG_ALARMS)
          {
            uart_send(__LINE__, "Bit mask used for weekday selection:\r");

            /* Display all 7 day-of-week. */
            for (Loop1UInt8 = 1; Loop1UInt8 < 8; ++Loop1UInt8)
            {
              sprintf(String, "                                  ");
              /* Display binary bitmap. */
              for (Loop2UInt8 = 7; Loop2UInt8 > 0; --Loop2UInt8)
              {
                if (Loop1UInt8 == Loop2UInt8)
                  strcat(String, "1");
                else
                  strcat(String, "0");
              }
              sprintf(&String[strlen(String)], "   %s\r", DayName[FlashConfig.Language][Loop1UInt8]);
              uart_send(__LINE__, String);
            }


            /* Scan all alarms (0 to 8). */
            for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
            {
              uart_send(__LINE__, "[%X] Alarm[%2.2u].Status:         0x%2.2X     (00 = Off   FF = On)\r", &FlashConfig.Alarm[Loop1UInt8].FlagStatus, Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].FlagStatus);
              uart_send(__LINE__, "[%X] Alarm[%2.2u].Hour:            %3u\r", &FlashConfig.Alarm[Loop1UInt8].Hour, Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Hour);
              uart_send(__LINE__, "[%X] Alarm[%2.2u].Minute:          %3u\r", &FlashConfig.Alarm[Loop1UInt8].Minute, Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Minute);
              uart_send(__LINE__, "[%X] Alarm[%2.2u].Second:          %3u\r", &FlashConfig.Alarm[Loop1UInt8].Second, Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Second);
              uart_send(__LINE__, "[%X] Alarm[%2.2u].Jingle:          %3u\r", &FlashConfig.Alarm[Loop1UInt8].Jingle, Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Jingle);

              /* Identify all day-of-week targets for each alarm. */
              DayMask[0] = 0x00;  // initialize string as null.
              for (Loop2UInt8 = 7; Loop2UInt8 > 0; --Loop2UInt8)
              {
                if (FlashConfig.Alarm[Loop1UInt8].Day & (1 << Loop2UInt8))
                  strcat(DayMask, "1");
                else
                  strcat(DayMask, "0");
              }
              strcat(DayMask, "0");  // add bit 0 since days-of-week go from 1 to 7 (1 being SUN and 7 being SAT)
              sprintf(String, "[%X] Alarm[%2.2u].DayMask:     %s     (%X) ", &FlashConfig.Alarm[Loop1UInt8].Day, Loop1UInt8, DayMask, FlashConfig.Alarm[Loop1UInt8].Day);


              for (Loop2UInt8 = 1; Loop2UInt8 < 8; ++Loop2UInt8)
              {
                if (FlashConfig.Alarm[Loop1UInt8].Day & (1 << Loop2UInt8))
                {
                  Dum1UInt8 = strlen(String);
                  sprintf(&String[strlen(String)], "%s", DayName[FlashConfig.Language][Loop2UInt8]);
                  String[Dum1UInt8 + 3] = 0x20;  // space separator.
                  String[Dum1UInt8 + 4] = 0x00;  // keep first 3 characters of day name.
                }
              }

              strcat(String, "\r");
              uart_send(__LINE__, String);
            }
          }
        break;



        #ifdef PICO_W
        case (TAG_NTP_ERRORS):
          /* Scroll total number of NTP errors so far. */
          if (NTPData.NTPErrors)
          {
            switch (FlashConfig.Language)
            {
              case (CZECH):
                sprintf(String, "Chyba NTP: %lu/%lu.   ", NTPData.NTPErrors);
              break;

              case (FRENCH):
                sprintf(String, "Erreurs NTP: %lu/%lu   ", NTPData.NTPErrors);
              break;

              case (SPANISH):
                sprintf(String, "Errores NTP: %lu/%lu   ", NTPData.NTPErrors);
              break;

              case (ENGLISH):
              default:
                sprintf(String, "NTP errors: %lu   ", NTPData.NTPErrors);
              break;
            }
            scroll_string(24, String);
          }
        break;
        #endif  // PICO_W



        #ifdef PICO_W
        #ifdef DEVELOPER_VERSION
        case (TAG_NTP_STATUS):
          /* Scroll total number of NTP request failures so far. */
          switch (FlashConfig.Language)
          {
            case (CZECH):
              sprintf(String, "Stav NTP: %lu/%lu.   ", NTPData.NTPErrors, NTPData.NTPReadCycles);
            break;

            case (FRENCH):
              sprintf(String, "Statut NTP: %lu/%lu   ", NTPData.NTPErrors, NTPData.NTPReadCycles);
            break;

            case (SPANISH):
              sprintf(String, "Status NTP: %lu/%lu   ", NTPData.NTPErrors, NTPData.NTPReadCycles);
            break;

            case (ENGLISH):
            default:
              sprintf(String, "NTP status: %lu/%lu   ", NTPData.NTPErrors, NTPData.NTPReadCycles);
            break;
          }
          scroll_string(24, String);
        break;
        #endif  // DEVELOPER_VERSION
        #endif  // PICO_W



        case (TAG_PICO_TEMP):
          adc_read_pico_temp(&DegreeC, &DegreeF);
          switch (FlashConfig.Language)
          {
            case (CZECH):
              if (FlashConfig.TemperatureUnit == CELSIUS)
              {
                /* ...in Celsius. */
                sprintf(String, "Pico tep.: %2.2f %cC    ", DegreeC, 0x80);
              }
              else
              {
                /* ...or in Fahrenheit. */
                sprintf(String, "Pico tep: %2.2f%cF    ", DegreeF, 0x80);
              }
            break;

            case (SPANISH):
              if (FlashConfig.TemperatureUnit == CELSIUS)
              {
                /* ...in Celsius. */
                sprintf(String, "Temp. de la Pico: %2.2f%cC    ", DegreeC, 0x80);
              }
              else
              {
                /* ...or in Fahrenheit. */
                sprintf(String, "Temp de la Pico: %2.2f%cF    ", DegreeF, 0x80);
              }
            break;

            case (ENGLISH):
            case (FRENCH):
            case (GERMAN):
            default:
              if (FlashConfig.TemperatureUnit == CELSIUS)
              {
                /* ...in Celsius. */
                sprintf(String, "Pico temp: %2.2f%cC    ", DegreeC, 0x80);
              }
              else
              {
                /* ...or in Fahrenheit. */
                sprintf(String, "Pico temp: %2.2f%cF    ", DegreeF, 0x80);
              }
            break;
          }
         scroll_string(24, String);
        break;



        case (TAG_PICO_TYPE):
          if (PicoType == TYPE_PICO)
          {
            switch (FlashConfig.Language)
            {
              case (CZECH):
                sprintf(String, "mikrokontroler: Pico.    ");
                String[12] = (UINT8)138; // e-acute
              break;

              case (FRENCH):
                sprintf(String, "Microcontroleur: Pico    ");
              break;

              case (SPANISH):
                sprintf(String, "Microcontrolador: Pico    ");
              break;

              case (ENGLISH):
              default:
                sprintf(String, "Microcontroller: Pico    ");
              break;
            }
          }
          else
          {
            switch (FlashConfig.Language)
            {
              case (CZECH):
                sprintf(String, "mikrokontroler: Pico W    ");
                String[12] = (UINT8)138; // e-acute
              break;

              case (FRENCH):
                sprintf(String, "Microcontroleur: Pico W    ");
              break;

              case (SPANISH):
                sprintf(String, "Microcontrolador: Pico W   ");
              break;

              case (ENGLISH):
              default:
                sprintf(String, "Microcontroller: Pico W    ");
              break;
            }
          }
          scroll_string(24, String);
        break;



        case (TAG_PICO_UNIQUE_ID):
          sprintf(String, "Pico ID: %s   ", PicoUniqueId);
          scroll_string(24, String);
        break;



        case (TAG_QUEUE):
          /* For debug purposes. Display all allocated scroll queue slots. */
          if (ScrollQueueHead == ScrollQueueTail)
          {
            sprintf(String, "H%2.2u = T%2.2u - No element allocated    ", ScrollQueueHead, ScrollQueueTail);
            scroll_string(24, String);
          }
          else
          {
            Dum1UInt8 = ScrollQueueTail;
            while (TRUE)
            {
              sprintf(String, "(H%2.2u) T%2.2u E%2.2u    ", ScrollQueueHead, Dum1UInt8, ScrollQueue[Dum1UInt8]);
              scroll_string(24, String);
              ++Dum1UInt8;

              /* If we reach out-of-bound, revert to 0. */
              if (Dum1UInt8 > (MAX_SCROLL_QUEUE - 1))
                Dum1UInt8 = 0;

              /* If we scrolled all allocated slots. */
              if (Dum1UInt8 == ScrollQueueHead)
                break;
            }
          }
        break;



       case (TAG_TIMEZONE):
          switch (FlashConfig.Language)
          {
            case (CZECH):
              sprintf(String, "casova zona: %d    ", FlashConfig.Timezone);
              String[0] = (UINT8)136; // c-caron
              String[5] = (UINT8)129; // a-acute
              String[8] = (UINT8)139; // o-acute
            break;

            case (SPANISH):
              sprintf(String, "Zona horaria: %d    ", FlashConfig.Timezone);
            break;

            case (ENGLISH):
            case (FRENCH):
            default:
              sprintf(String, "Timezone: %d    ", FlashConfig.Timezone);
            break;
          }
          scroll_string(24, String);
        break;



        /*** Voltaqe reading function from Pico's ADC to be reviewed / debugged. */
        case (TAG_VOLTAGE):
          Volts = adc_read_voltage();
          switch (FlashConfig.Language)
          {
            case (CZECH):
              sprintf(String, "%2.2f V    ", Volts);
            break;

            case (SPANISH):
              sprintf(String, "%2.2f voltios  ", Volts);
            break;

            case (ENGLISH):
            case (FRENCH):
            default:
             sprintf(String, "%2.2f Volts    ", Volts);
            break;
          }
          scroll_string(24, String);
         break;



        default:
          /* Out-of-bound, do nothing. */
          return;
        break;
      }
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=pwm_initialize() */
/* ---------------------------------------------------------------------------- *\
                                Initialize PWM:

               - For clock display variable brightness.
               - For passive buzzer sound.

   NOTES:
   When working with PWM, source program file must include:
   -> #include "hardware/pwm.h"
   and CMakeLists.txt must include:
   -> target_link_libraries(myprogram pico_stdlib hardware_pwm)
\* ---------------------------------------------------------------------------- */
void pwm_initialize(void)
{
  UCHAR String[128];

  UINT8 Loop1UInt8;

  UINT32 SystemClock;


  /* Retrieve system clock (Pico is 125 MHz)... */
  SystemClock = clock_get_hz(clk_sys);


  /* Initialize all GPIOs requiring PWM. */
  for (Loop1UInt8 = PWM_LO_LIMIT; Loop1UInt8 < PWM_HI_LIMIT; ++Loop1UInt8)
  {
    /* Initialize the specified GPIO as a PWM output. */
    gpio_init(Pwm[Loop1UInt8].Gpio);
    gpio_set_function(Pwm[Loop1UInt8].Gpio, GPIO_FUNC_PWM);


    /* Get PWM slice number for this PWM, depending on which GPIO it is connected to. */
    Pwm[Loop1UInt8].Slice = pwm_gpio_to_slice_num(Pwm[Loop1UInt8].Gpio);


    /* Get PWM channel number for this PWM, depending on which GPIO it is connected to. */
    Pwm[Loop1UInt8].Channel = pwm_gpio_to_channel(Pwm[Loop1UInt8].Gpio);


    /* Slow down the clock to get more flexibility with the counter and to reach lower PWM frequencies. */
    Pwm[Loop1UInt8].ClockDivider = SystemClock / 1000000;
    Pwm[Loop1UInt8].Clock        = (UINT32)(SystemClock / Pwm[Loop1UInt8].ClockDivider);
    pwm_set_clkdiv(Pwm[Loop1UInt8].Slice, Pwm[Loop1UInt8].ClockDivider);  // slow down the clock to 1 MHz for this PWM slice.


    /* Complete initialization specific with each one. */
    switch (Loop1UInt8)
    {
      case (PWM_BRIGHTNESS):
        /* Set current values in PWM structure. */
        // Set the base frequency for the PWM duration. The minimal duty cycle of 0 gives an OE driven for 1 period, so the smaller the number the less time OE is active
        // THe lower the frequency the less time the display is active and the lower the mininum light level is
        // 10000 (10KHz) value is 0.1ms active in 10ms period (0.01 of time), 1000 (1KHz) value is 1ms in 1000ms period (0.001 of time)
        // Anything slower than 1KHz is visible on the LED display
        Pwm[Loop1UInt8].Frequency = BRIGHTNESS_PWM_FREQUENCY;
        Pwm[Loop1UInt8].Wrap      = (UINT16)(Pwm[Loop1UInt8].Clock / Pwm[Loop1UInt8].Frequency);
        Pwm[Loop1UInt8].Cycles    = BRIGHTNESS_LIGHTLEVELSTEP;
        // OE is active low, so reverse the duty cycle.
        Pwm[Loop1UInt8].Level     = (UINT16)(Pwm[Loop1UInt8].Wrap * ((BRIGHTNESS_LIGHTLEVELSTEP - Pwm[Loop1UInt8].Cycles) / (BRIGHTNESS_LIGHTLEVELSTEP * 1.0)));

        /* Set PWM frequency by setting a counter wrap value. */
        pwm_set_wrap(Pwm[Loop1UInt8].Slice, Pwm[Loop1UInt8].Wrap);

        /* Set PWM duty cycle given the Divider, Counter and Frequency current values. */
        pwm_set_chan_level(Pwm[Loop1UInt8].Slice, Pwm[Loop1UInt8].Channel, Pwm[Loop1UInt8].Level);

        /* Start PWM. */
        pwm_set_enabled(Pwm[Loop1UInt8].Slice, TRUE);
      break;

      #ifndef SOUND_DISABLED
      case (PWM_SOUND):
        /* Set current values in PWM structure. */
        Pwm[Loop1UInt8].Frequency = 250;
        Pwm[Loop1UInt8].Wrap      = (UINT16)(Pwm[Loop1UInt8].Clock / Pwm[Loop1UInt8].Frequency);
        Pwm[Loop1UInt8].Cycles    = 50;
        Pwm[Loop1UInt8].Level     = (UINT16)(Pwm[Loop1UInt8].Wrap * (Pwm[Loop1UInt8].Cycles / 100.0));

        /* Set PWM frequency by setting a counter wrap value. */
        pwm_set_wrap(Pwm[Loop1UInt8].Slice, Pwm[Loop1UInt8].Wrap);

        /* Set PWM duty cycle given the Divider, Counter and Frequency current values. */
        pwm_set_chan_level(Pwm[Loop1UInt8].Slice, Pwm[Loop1UInt8].Channel, Pwm[Loop1UInt8].Level);

        /* Make sure PWM is turned Off. */
        pwm_set_enabled(Pwm[Loop1UInt8].Slice, FALSE);
      break;
      #endif  // SOUND_DISABLED
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=pwm_on_off() */
/* ---------------------------------------------------------------------------- *\
               Turn On or Off the PWM signal specified in argument.
\* ---------------------------------------------------------------------------- */
void pwm_on_off(UINT8 PwmNumber, UINT8 FlagSwitch)
{
  UCHAR String[256];


  Pwm[PwmNumber].OnOff = FlagSwitch;

  /* Turn On PWM signal. */
  if (Pwm[PwmNumber].OnOff == FLAG_ON)
    pwm_set_enabled(Pwm[PwmNumber].Slice, TRUE);

  /* Turn Off PWM signal. */
  if (Pwm[PwmNumber].OnOff == FLAG_OFF)
  {
    pwm_set_enabled(Pwm[PwmNumber].Slice, FALSE);
  }

  return;
}





/* $PAGE */
/* $TITLE=pwm_set_cycles() */
/* ---------------------------------------------------------------------------- *\
     Set the duty cycle for the PWM controlling the clock display brightness.
     A value of 0 matches one cycle, giving a minimum level of 1 BRIGHTNESS_LIGHTLEVELSTEP
     Note: active cycles is given as a ratio of Cycles to BRIGHTNESS_LIGHTLEVELSTEP.
           For example, a value of (BRIGHTNESS_LIGHTLEVELSTEP/2) -1 means 50% of active cycles
   NOTE: Since "Output Enable" (OE) is active low, we reverse the active cycle
        so that we still have larger numbers for brighter LED levels.
\* ---------------------------------------------------------------------------- */
void pwm_set_cycles(UINT16 Cycles)
{
  UCHAR String[256];


  /* Validate value specified for duty cycle. */
  if (Cycles > BRIGHTNESS_LIGHTLEVELSTEP)
    Cycles = BRIGHTNESS_LIGHTLEVELSTEP;

  /* Set current values in PWM structure. */
  Pwm[PWM_BRIGHTNESS].Cycles = Cycles;
  // Pwm[PWM_BRIGHTNESS].Cycles = 0;
  Pwm[PWM_BRIGHTNESS].Level = (UINT16)(Pwm[PWM_BRIGHTNESS].Wrap * ((BRIGHTNESS_LIGHTLEVELSTEP - Pwm[PWM_BRIGHTNESS].Cycles) / (BRIGHTNESS_LIGHTLEVELSTEP * 1.0)));


  /* Set PWM duty cycle given the Divider, Counter and Frequency current values. */
  pwm_set_chan_level(Pwm[PWM_BRIGHTNESS].Slice, Pwm[PWM_BRIGHTNESS].Channel, Pwm[PWM_BRIGHTNESS].Level);

  return;
}





/* $PAGE */
/* $TITLE=pwm_set_frequency() */
/* ---------------------------------------------------------------------------- *\
          Set the frequency for the PWM controlling the passive buzzer.
\* ---------------------------------------------------------------------------- */
void pwm_set_frequency(UINT16 Frequency)
{
  #ifndef SOUND_DISABLED
  UCHAR String[256];


  /* Indicate current PWM values. */
  Pwm[PWM_SOUND].Frequency = Frequency;
  Pwm[PWM_SOUND].Wrap = (UINT16)(Pwm[PWM_SOUND].Clock / Pwm[PWM_SOUND].Frequency);

  /* Set PWM frequency by setting a counter wrap value. */
  pwm_set_wrap(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].Wrap);

  /* Since we change the frequency, adjust duty cycle accordingly. */
  Pwm[PWM_SOUND].Level = (UINT16)(Pwm[PWM_SOUND].Wrap * (Pwm[PWM_SOUND].Cycles / 100.0));

  /* Set PWM duty cycle given the Divider, Counter and Frequency current values. */
  pwm_set_chan_level(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].Channel, Pwm[PWM_SOUND].Level);
  #endif  // SOUND_DISABLED

  return;
}




#ifdef DHT_SUPPORT
/* $PAGE */
/* $TITLE=read_dht() */
/* ------------------------------------------------------------------ *\
               Read the temperature from external DHT22.
      This temperature and humidity sensor is not part of the Pico
        Green Clock and it must be bought and installed by user.
         This function is called from Pico's 2nd core (core 1).
\* ------------------------------------------------------------------ */
UINT8 read_dht(float *Temperature, float *Humidity)
{
  UCHAR Dum1UChar[41];
  UCHAR Dum2UChar[41];
  UCHAR DhtLevel[MAX_DHT_READINGS];
  UCHAR String[256];

  UINT8 Byte[4];
  UINT8 Checksum;
  UINT8 CurrentBit;
  UINT8 MaxTry;  // maximum number or DHT22 read trials before giving up and declaring an error.
  UINT8 OriginalClockMode;
  UINT8 TempLevel[MAX_DHT_READINGS];
  UINT8 TryNumber;

  UINT16 ComputedChecksum;
  UINT16 DhtStepCount;
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;
  UINT16 Value;

  UINT32 DataBuffer;
  UINT32 InterruptMask;

  UINT64 DhtStartValue[MAX_DHT_READINGS];
  UINT64 DhtFinalValue[MAX_DHT_READINGS];
  UINT64 DhtResultValue[MAX_DHT_READINGS];
  UINT64 InitialTime;
  UINT64 FinalTime;


  /* Allow three chances to read DHT22 without error. Since DHT reading was transfered to core 1, error rate was about 0.5% (very low !). */
  MaxTry    = 3;  // number of read retries before declaring an error.
  TryNumber = 0;


  while (TryNumber < MaxTry)
  {
    if (DebugBitMask & DEBUG_DHT)
      uart_send(__LINE__, "Read DHT22, retry %u\r", TryNumber);

    /* Initializations. */
    DhtStepCount = 0;  // total number of "logic level changes" received from DHT22 (must be 83).

    /* Reset variables before receiving a new data stream. */
    for (Loop1UInt16 = 0; Loop1UInt16 < MAX_DHT_READINGS; ++Loop1UInt16)
      DhtLevel[Loop1UInt16] = 'X';  // logic level: 'X' = Unknown   'L' = Low   'H' = High


    /* Set the line in "output" mode to trigger a "read cycle request" to DHT22. */
    gpio_set_dir(DHT22, GPIO_OUT);

    /* Set the line to a Low logic level to trigger the "read cycle request" to DHT22. */
    gpio_put(DHT22, 0);

    /* Keep "read cycle request" signal Low for a while. */
    sleep_ms(20);

    /* Then, wait for the response from DHT22 (should be 20 to 40 microseconds delay after the request signal, according to DHT22 specifications). */
    gpio_set_dir(DHT22, GPIO_IN);

    /* Set a pull-up resistor on the line. */
    gpio_pull_up(DHT22);



    /* ----------------------------------------------------------------------- *\
       NOTE: Inside every "while" loop below, there is a time-out algorithm.
             As long as everything goes fine, this time-out is useless.
             However, if there is a problem with the DHT22, those time-outs
             will allow the Green Clock to perform with no impact for the user,
             except that the temperature will not be scrolled on the clock.
             Also, the total number of read errors from DHT22 will grow at
             the same rate as the total number of DHT22 readings.

                  ----------------------------------------------

                  Wait for a while until DHT22 begins to respond.
               The line is idle (High level) and we expect the DHT22
                 to begin responding within 20 to 40 microseconds.
    \* ----------------------------------------------------------------------- */
    /* Keep track of the initial and final timer values for this High logic level. Resulting length will be evaluated later. */
    DhtStartValue[DhtStepCount] = time_us_64();
    while (gpio_get(DHT22) == 1)
    {
      DhtFinalValue[DhtStepCount] = time_us_64();
      if ((DhtFinalValue[DhtStepCount] - DhtStartValue[DhtStepCount]) > (UINT64)50) break;  // time-out.
    }
    DhtLevel[DhtStepCount] = 'H';



    /* If we got out of the while loop above with a time-out (after 50 usec), the DHT22 haven't begun responding in the laps time it was supposed to...
       There must be a problem with the DHT22. If this is the case, declare an error and give-up for this read cycle. */
    if ((DhtFinalValue[DhtStepCount] - DhtStartValue[DhtStepCount]) > (UINT64)50)
    {
      if (DebugBitMask & DEBUG_DHT)
        uart_send(__LINE__, "Time-out while waiting for DHT22 to respond, retry number %u\r", TryNumber + 1);

      /*** Try brute force method to see if there is a DHT22 responding on the GPIO line. ***
      sleep_ms(500);                  // wait to make sure previous read cycle is over.
      gpio_set_dir(DHT22, GPIO_OUT);  // initialize DHT22 in output mode.
      gpio_put(DHT22, 0);             // Low level to request a read cycle from DHT22.
      sleep_ms(20);                   // Low level long enough to trigger the DHT22.
      gpio_set_dir(DHT22, GPIO_IN);   // initialize DHT22 in input mode to received the data stream.
      gpio_pull_up(DHT22);            // set a pull-up resistor on the line.

      // Initializations.
      DhtStepCount = 0;
      for (Loop1UInt16 = 0; Loop1UInt16 < MAX_DHT_READINGS; ++Loop1UInt16)
        TempLevel[Loop1UInt16] = 0;

      // Fill-up all slots available to see if logic levels change during this period of time.
      InitialTime  = time_us_64();
      while (DhtStepCount < MAX_DHT_READINGS)  // may be useful to increase the value of MAX_DHT_READINGS to cover a longer period of time.
      {
        TempLevel[DhtStepCount] = gpio_get(DHT22);
        ++DhtStepCount;
      }
      FinalTime = time_us_64();


      if (DebugBitMask & DEBUG_DHT)
      {
        uart_send(__LINE__, "Time-out while trying to read DHT22 (%llu uSec)\r", (DhtFinalValue[DhtStepCount] - DhtStartValue[DhtStepCount]));
        uart_send(__LINE__, "CurrentLevel: %u\r", gpio_get(DHT22));
        uart_send(__LINE__, "Total duration: %llu uSec total   (average %llu)\r", (FinalTime - InitialTime), ((FinalTime - InitialTime) / MAX_DHT_READINGS));

        for (Loop1UInt16 = 0; Loop1UInt16 < MAX_DHT_READINGS; ++Loop1UInt16)
        {
          uart_send(__LINE__, "-[%3u] %u ", Loop1UInt16, TempLevel[Loop1UInt16]);

          if (((Loop1UInt16 + 1) % 15) == 0) uart_send(__LINE__, "\r");
        }
        uart_send(__LINE__, "\r");
      }
      *** End of brute force method test. ***/

      /* One more try... */
      ++TryNumber;

      continue;
    }



    /* One more "logic level change" occured. */
    ++DhtStepCount;



    /* ---------------------------------------------------------------- *\
             First, read the two "get ready" signals from DHT22
                   (one Low level then, one High level).
                 Then, read the 40 bits (5 bytes X 8 bits):
                     (4 data bytes + 1 checksum byte).

          One logic level while waiting for DHT22 to answer (above),
                   then 2 "get ready" signals (or "levels"),
        followed by 40 bits (2 logic levels for each) = 83 steps total.
    \* ---------------------------------------------------------------- */
    for (Loop1UInt16 = 0; Loop1UInt16 < 41; ++Loop1UInt16)
    {
      /* --------------------------------------------------------- *\
                      Receiving the first half-bit.
             Wait as long as the line is Low, which is the first
          half-bit. We keep track of the duration, but only for
          information since the first half-bit is always the same
             length (50 microseconds) and it is meaningless
                          for data decoding.
      \* --------------------------------------------------------- */
      /* Keep track of the initial and final timer values for this logic level. Resulting length will be evaluated later*/
      DhtStartValue[DhtStepCount] = (UINT64)time_us_64();
      while (gpio_get(DHT22) == 0)
      {
        DhtFinalValue[DhtStepCount] = time_us_64();
        if ((DhtFinalValue[DhtStepCount] - DhtStartValue[DhtStepCount]) > 90) break;  // time out.
      }
      DhtLevel[DhtStepCount] = 'L';

      /* One more "logic level change" occured. */
      ++DhtStepCount;



      /* --------------------------------------------------------- *\
                     Receiving the second half-bit.
         wait as long as the line is High, which is the last part
                     of the bit ("second half-bit").
               We keep track of the duration since this is
                   what determines if this is a "0" bit
                  (around 26 microseconds), or a "1" bit
                        (around 70 microseconds).
      \* --------------------------------------------------------- */
      /* Keep track of the initial and final timer values for this logic level. Resulting length will be evaluated later*/
      DhtStartValue[DhtStepCount] = time_us_64();
      while (gpio_get(DHT22) == 1)
      {
        DhtFinalValue[DhtStepCount] = time_us_64();
        if ((DhtFinalValue[DhtStepCount] - DhtStartValue[DhtStepCount]) > 150) break;  // time-out.
      }
      DhtLevel[DhtStepCount] = 'H';

      /* One more "logic level change" occured. */
      ++DhtStepCount;
    }



    /* Optionally display number of steps (logic levels) received. */
    if (DebugBitMask & DEBUG_DHT)
      uart_send(__LINE__, "DHT step count: %u\r", DhtStepCount);



    /* Analyze and process data stream received from DHT22. */
    Checksum   = 0;
    CurrentBit = 0;
    DataBuffer = 0;
    sprintf(Dum2UChar, "00000000");                          // Checksum in binary (8 bits).
    sprintf(Dum1UChar, "00000000000000000000000000000000");  // DataBuffer (result) in binary (32 bits);


    if (DebugBitMask & DEBUG_DHT)
      uart_send(__LINE__, "Before processing data stream\r\r");

    /* Now that all bits have been received, decode the data stream sent by DHT22. */
    for (Loop1UInt16 = 0; Loop1UInt16 < DhtStepCount; ++Loop1UInt16)
    {
      /* Compute the duration of this logic level. */
      DhtResultValue[Loop1UInt16] = (DhtFinalValue[Loop1UInt16] - DhtStartValue[Loop1UInt16]);

      if (Loop1UInt16 == 0)
      {
        if (DebugBitMask & DEBUG_DHT)
        {
          uart_send(__LINE__, "Wait for response from DHT22 - 20 to 40 usec\r");
          uart_send(__LINE__, "Initial: %llu   Final: %llu\r", DhtStartValue[Loop1UInt16], DhtFinalValue[Loop1UInt16]);
          uart_send(__LINE__, "Step:          %2.2u   Level:        %c                 - - - - - ResultValue: %3llu\r\r", Loop1UInt16, DhtLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        }
        continue;
      }


      /* Skip the 1st "Get ready" level received from DHT22 (first: Low level, then High level, both 80 microseconds). */
      if (Loop1UInt16 == 1)
      {
        if (DebugBitMask & DEBUG_DHT)
        {

          uart_send(__LINE__, "1st <get ready> received from DHT22 - around 70 usec\r");
          uart_send(__LINE__, "Initial: %llu   Final: %llu\r", DhtStartValue[Loop1UInt16], DhtFinalValue[Loop1UInt16]);
          uart_send(__LINE__, "Step:          %2.2u   Level:       %c                  - - - - - ResultValue: %3llu\r\r", Loop1UInt16, DhtLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        }
        continue;
      }


      /* Skip the 2nd "Get ready" level received from DHT22 (first: Low level, then High level, both 80 microseconds). */
      if (Loop1UInt16 == 2)
      {
        if (DebugBitMask & DEBUG_DHT)
        {
          uart_send(__LINE__, "2nd <get ready> received from DHT22 - around 70 usec\r");
          uart_send(__LINE__, "Initial: %llu    Final: %llu   Result: %llu\r", DhtStartValue[Loop1UInt16], DhtFinalValue[Loop1UInt16], (DhtFinalValue[Loop1UInt16] - DhtStartValue[Loop1UInt16]));
          uart_send(__LINE__, "Step:          %2.2u    Level:        %c                - - - - - ResultValue: %3llu\r\r", Loop1UInt16, DhtLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        }
        continue;
      }



      /* --------------------------------------------------------- *\
                        Evaluate each bit received.
      \* --------------------------------------------------------- */
      if (DhtLevel[Loop1UInt16] == 'L')
      {
        /* Low level, it is a first half-bit. */
        if (DebugBitMask & DEBUG_DHT)
        {
          uart_send(__LINE__, "Current bit: %u\r", CurrentBit);
          uart_send(__LINE__, "Initial: %llu     Final: %llu\r", DhtStartValue[Loop1UInt16], DhtFinalValue[Loop1UInt16]);
          uart_send(__LINE__, "Step: %2u   Level: %c   (should be around 50 usec)    - - - - - ResultValue: %3llu\r", Loop1UInt16, DhtLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        }
      }
      else
      {
        /* Let's assume on entry that we received a 0-bit. Left-shift all bits already received.
           A new 0-bit is automatically added to the right by default. */
        if (CurrentBit < 32)
          DataBuffer <<= 1;  // We're still receiving the first 32 data bits.
        else
          Checksum <<= 1;    // We already received all 32 data bits, we're receiving the checksum bits.


        /* A 0-bit is around 26 usec long whereas a 1-bit is around 70 usec long.
           if the new bit is a 1-bit), replace the 0-bit just added above by a 1-bit. */
        if (DhtResultValue[Loop1UInt16] > 32)
        {
          /* The length is greater than the 26usec of a typical 0-bit, it must be a 1-bit. */
          if (CurrentBit < 32)
            ++DataBuffer;  // We're still receiving the first 32 data bits, set this 1-bit.
          else
            ++Checksum;    // We already received all 32 data bits, we're receiving the checksum bits, set this 1-bit.
        }



        if (DebugBitMask & DEBUG_DHT)
        {
           /* Generate DataBuffer in binary (in string "Dum1UChar"). */
          for (Loop2UInt16 = 0; Loop2UInt16 < 32; ++Loop2UInt16)
          {
            if (DataBuffer & (1 << Loop2UInt16))
              Dum1UChar[31 - Loop2UInt16] = '1'; // if this is a one bit.
            else
              Dum1UChar[31 - Loop2UInt16] = '0'; // if this is a zero bit.
          }


          /* Generate Checksum in binary (in string "Dum2UChar"). */
          for (Loop2UInt16 = 0; Loop2UInt16 < 8; ++Loop2UInt16)
          {
            if (Checksum & (1 << Loop2UInt16))
              Dum2UChar[7 - Loop2UInt16] = '1'; // if this is a one bit.
            else
              Dum2UChar[7 - Loop2UInt16] = '0'; // if this is a zero bit.
          }

          uart_send(__LINE__, "Initial: %llu     Final: %llu\r", DhtStartValue[Loop1UInt16], DhtFinalValue[Loop1UInt16]);
          uart_send(__LINE__, "Step: %2u   Level: %c   (26 usec = 0  70 usec = 1)    - - - - - ResultValue: %3llu  \r", Loop1UInt16, DhtLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
          uart_send(__LINE__, "DataBuffer in binary: %s\r", Dum1UChar);
          uart_send(__LINE__, "DataBuffer:           0x%8.8X\r", DataBuffer);
          uart_send(__LINE__, "Checksum   in binary: %s\r", Dum2UChar);
          uart_send(__LINE__, "Checksum:             0x%2.2X\r\r", Checksum);
        }

        ++CurrentBit;
      }
    }



    /* Checksum validation. */
    ComputedChecksum = 0;



    /* Calculate the checksum of data bytes received. */
    for (Loop1UInt16 = 0; Loop1UInt16 < 4; ++Loop1UInt16)
    {
      Value = ((DataBuffer >> (24 - (8 * Loop1UInt16))) & 0xFF);

      if (DebugBitMask & DEBUG_DHT)
      {
        /* Generate each part of the checksum in binary. */
        sprintf(Dum2UChar, "  0  0  0  0  0  0  0  0");
        for (Loop2UInt16 = 0; Loop2UInt16 < 8; ++Loop2UInt16)
        {
          if ((Value & (1 << Loop2UInt16)))
          {
            /* If this is a one bit. */
            Dum2UChar[21 - (3 * Loop2UInt16)] = ' ';
            Dum2UChar[22 - (3 * Loop2UInt16)] = ' ';
            Dum2UChar[23 - (3 * Loop2UInt16)] = '1';
          }
          else
          {
            /* If this is a zero bit. */
            Dum2UChar[21 - (3 * Loop2UInt16)] = ' ';
            Dum2UChar[22 - (3 * Loop2UInt16)] = ' ';
            Dum2UChar[23 - (3 * Loop2UInt16)] = '0';
          }
        }

        uart_send(__LINE__, "Calculate Checksum (part %u): 0x%2.2X     %s\r", Loop1UInt16 + 1, Value, Dum2UChar);
      }

      ComputedChecksum += Value;
      ComputedChecksum &= 0xFF; // when adding, may become more than 8 bits.
    }



    /* Display separator. */
    if (DebugBitMask & DEBUG_DHT)
    {
      uart_send(__LINE__, "                                       ------------------------\r");


      /* Display received checksum in binary. */
      sprintf(Dum2UChar, "  0  0  0  0  0  0  0  0");
      for (Loop1UInt16 = 0; Loop1UInt16 < 8; ++Loop1UInt16)
      {
        if (Checksum & (1 << Loop1UInt16))
        {
          /* If this is a one bit. */
          Dum2UChar[21 - (3 * Loop1UInt16)] = ' ';
          Dum2UChar[22 - (3 * Loop1UInt16)] = ' ';
          Dum2UChar[23 - (3 * Loop1UInt16)] = '1';
        }
        else
        {
          /* If this is a zero bit. */
          Dum2UChar[21 - (3 * Loop1UInt16)] = ' ';
          Dum2UChar[22 - (3 * Loop1UInt16)] = ' ';
          Dum2UChar[23 - (3 * Loop1UInt16)] = '0';
        }
      }
      uart_send(__LINE__, "Checksum received:                    %s\r", Dum2UChar);
      uart_send(__LINE__, "Before comparing checksums\r");
    }



    /* Compare the computed checksum with the one received from DHT22 and return an error code in case of error. */
    if (ComputedChecksum == Checksum)
    {
      break;  // get out of while loop if read cycle succeeded.
    }
    else
    {
      if (DebugBitMask & DEBUG_DHT)
        uart_send(__LINE__, "===##### ERROR #####===   Retry (%u/%u) reading DHT22   Checksum calculated: 0x%2.2X   Checksum received: 0x%2.2X\r", (TryNumber + 1), MaxTry, ComputedChecksum, Checksum);

      ++TryNumber;  // one more try to read DHT22 failed.
    }
  }



  if (TryNumber == MaxTry)
  {
    /* We got out of while loop after three failure tryying to read DHT22. */
    if (DebugBitMask & DEBUG_DHT)
    {
      uart_send(__LINE__, "===##### ERROR #####===   Failed to read DHT22 after %u retries (MAX: %u).\r\r\r\r", TryNumber, MaxTry);
      uart_send(__LINE__, "                       Cumulative DhtErrors: %6lu   DhtReadCycles: %6lu\r\r\r", DhtData.DhtErrors, DhtData.DhtReadCycles);
    }

    return 0xFF;
  }


  if (DebugBitMask & DEBUG_DHT)
    uart_send(__LINE__, "===##### SUCCESS #####===   (%lu/%lu)   ComputedChecksum: 0x%2.2X   Checksum: 0x%2.2X\r", DhtData.DhtErrors, DhtData.DhtReadCycles, ComputedChecksum, Checksum);


  /* If no error, compute the temperature and relative humidity from received data. */
  Byte[3] =  (DataBuffer        & 0xFF);
  Byte[2] = ((DataBuffer >> 8)  & 0xFF);
  Byte[1] = ((DataBuffer >> 16) & 0xFF);
  Byte[0] = ((DataBuffer >> 24) & 0xFF);


  /* Decode temperature. */
  *Temperature = (float)((((Byte[2] & 0x7F) << 8) | Byte[3]) / 10.0);
  if (Byte[2] & 0x80) *Temperature *= -1;

  /* Decode relative humidity. */
  *Humidity = (float)((((Byte[0] & 0x7F) << 8) | Byte[1]) / 10.0);

  if (FlashConfig.TemperatureUnit == FAHRENHEIT)
    *Temperature = ((*Temperature * 9 / 5) + 32);


  if ((DebugBitMask & DEBUG_DHT) || (DebugBitMask & DEBUG_CORE))
    uart_send(__LINE__, "===##### SUCCESS #####===   Temperature: %2.3f C   Humidity:    %2.3f %c\r\r\r", *Temperature, *Humidity, 0x25);

  return 0x00;
}
#endif  // DHT_SUPPORT





/* $PAGE */
/* $TITLE=reverse_bits() */
/* ------------------------------------------------------------------ *\
         Reverse the bit order of the byte given in argument.
\* ------------------------------------------------------------------ */
UINT8 reverse_bits(UINT8 InputByte)
{
  UINT8 BitMask;
  UINT8 OutputByte = 0;


  for (BitMask = 1; BitMask > 0; BitMask <<= 1)
  {
    OutputByte <<= 1;

    if (InputByte & BitMask)
      OutputByte |= 1;
  }

  /* For usage in the Waveshare clock, the 5 bits used for the display must be in lowest 5 bits. */
  return (OutputByte >> 3);
}





/* $PAGE */
/* $TITLE=scroll_one_dot() */
/* ------------------------------------------------------------------ *\
        Scroll the data in the framebuffer one dot to the left.
\* ------------------------------------------------------------------ */
void scroll_one_dot(void)
{
  UINT8 CharacterBuffer;  // buffer to temporarily hold a character (to keep track of "indicators" status).
  UINT8 RowNumber;        // row number in the framebuffer.
  UINT8 SectionNumber;    // section number in the framebuffer.


  /* ---------------------------------------------------------------- *\
               Scroll the framebuffer one dot to the left.
  \* ---------------------------------------------------------------- */
  /* Scan the 7 rows of data. Row 0 is reserved for "indicators". */
  for (RowNumber = 1; RowNumber < 8; ++RowNumber)
  {
    CharacterBuffer = DisplayBuffer[RowNumber] & 0x03; // keep track of "indicators" status (first 2 bits).

    /* Scan all "sections" of the framebuffer. One section is 8 bits width. */
    for (SectionNumber = 0; SectionNumber < sizeof(DisplayBuffer) / 8; ++SectionNumber)
    {
      /* If we're not at the last section, move one bit to the left and add the upmost bit of the next section. */
      if (SectionNumber < sizeof(DisplayBuffer) / 8 - 1)
        DisplayBuffer[(SectionNumber * 8) + RowNumber] = DisplayBuffer[(SectionNumber * 8) + RowNumber] >> 1 | DisplayBuffer[(SectionNumber * 8) + RowNumber + 8] << 7;
      else
        DisplayBuffer[(SectionNumber * 8) + RowNumber] = DisplayBuffer[(SectionNumber * 8) + RowNumber] >> 1;
    }
    DisplayBuffer[RowNumber] = (DisplayBuffer[RowNumber] & (~0x03)) | CharacterBuffer;  // restore the "indicators" status.
  }

  return;
}





/* $PAGE */
/* $TITLE=scroll_queue() */
/* ------------------------------------------------------------------ *\
               Queue the given tag in the scroll queue.
            Use the queue algorithm where one slot is lost.
             This prevents the use of a locking mechanism.
\* ------------------------------------------------------------------ */
UINT8 scroll_queue(UINT8 Tag)
{
  UINT8 Dum1UChar;


  if (DebugBitMask & DEBUG_SCROLL)
    uart_send(__LINE__, "Entering scroll_queue()\r");

  /* Check if the scroll circular buffer is full. */
  if (((ScrollQueueTail > 0) && (ScrollQueueHead == ScrollQueueTail - 1)) || ((ScrollQueueTail == 0) && (ScrollQueueHead == MAX_SCROLL_QUEUE - 1)))
  {
    if (DebugBitMask & DEBUG_SCROLL)
      uart_send(__LINE__, "Scroll queue full\r");

    /* Scroll queue is full, return error code. */
    return MAX_SCROLL_QUEUE;
  }

  /* If there is at least one slot available in the queue, insert the tag for the string to be scrolled. */
  ScrollQueue[ScrollQueueHead] = Tag;
  Dum1UChar = ScrollQueueHead;
  ++ScrollQueueHead;

  if (ScrollQueueHead >= MAX_SCROLL_QUEUE)
    ScrollQueueHead = 0;  // revert to zero when reaching out-of-bound.

  if (DebugBitMask & DEBUG_SCROLL)
    uart_send(__LINE__, "Exiting  scroll_queue()\r");

  return 0x00;
}





/* $PAGE */
/* $TITLE=scroll_queue_value() */
/* ------------------------------------------------------------------ *\
           Put a specific value in the queue so that we can
            check for a specific event even inside an ISR.
   MUST BE USED ONLY FOR DEBUG PURPOSES SINCE IT INTERFERES WITH
                        CALENDAR EVENTS.
\* ------------------------------------------------------------------ */
UINT8 scroll_queue_value(UINT8 Tag, UCHAR *String)
{
  static UINT8 EventNumber;


  /* NOTE: We overwrite a calendar event ("Description") with data of
           interest (variable, setting, marker, etc...) and then,
           we write this event number as a tag in the scroll queue.
           Since the tag is between 0 and MAX_EVENTS, it will be handled
           by process_scroll() as a Calendar event and will be scrolled
           on the display (altough not in real time. The function begins
           by overwriting Calendar event 0 and goes on overwriting all
           Calendar events as we need more of them to display info. It
           returns the next event number in case we have more data to be
           displayed). */

  sprintf(CalendarEvent[EventNumber].Description, "%s", String);
  scroll_queue(EventNumber);
  ++EventNumber;
  if (EventNumber >= MAX_EVENTS)
    EventNumber = 0;

  return EventNumber;
}





/* $PAGE */
/* $TITLE=scroll_string() */
/* ------------------------------------------------------------------ *\
            Scroll the specified string on clock display.
\* ------------------------------------------------------------------ */
void scroll_string(UINT8 StartColumn, UCHAR *StringToScroll)
{
  UCHAR String[256];

  UINT16 Loop1UInt16;


  if (DebugBitMask & DEBUG_SCROLL)
    uart_send(__LINE__, "Entering scroll_string()\r");


  if (CurrentClockMode != MODE_SCROLLING)
  {
    if (DebugBitMask & DEBUG_SCROLL)
      uart_send(__LINE__, "Current clock mode: %u\r", CurrentClockMode);

    /* Nothing scrolling on LED display... Clear framebuffer on entry. */
    clear_framebuffer(0);

    /* If no string is scrolling, use the start column specified. (The first two columns are used for LED indicators). */
    if (StartColumn == 0)
      ScrollDotCount = 1; // will be auto-incremented in the fill_display_buffer_5X7() function call.
    else
      ScrollDotCount = StartColumn;
  }
  else
  {
    if (DebugBitMask & DEBUG_SCROLL)
      uart_send(__LINE__, "Current clock mode: MODE_SCROLLING\r");


    if (ScrollDotCount >= (DISPLAY_BUFFER_SIZE - 9))  // make sure we have enough space to add spaces.
    {
      if (DebugBitMask & DEBUG_SCROLL)
        uart_send(__LINE__, "Waiting to add 2 spaces: %u\r", ScrollDotCount);


      /* We're approaching the end of the virtual framebuffer... wait some time for the display buffer to be partly cleared. */
      sleep_ms(SCROLL_DOT_TIME * 20);  // this function should not be called from a callback.
    }

    if (DebugBitMask & DEBUG_SCROLL)
      uart_send(__LINE__, "Filling-up display_buffer [ ]\r");

    /* Add two "space separators" before concatenating next string. */
    ScrollDotCount = fill_display_buffer_5X7(ScrollDotCount, ' ');
    ScrollDotCount = fill_display_buffer_5X7(ScrollDotCount, ' ');
  }


  /* Insert the new string in the framebuffer. */
  for (Loop1UInt16 = 0; Loop1UInt16 < strlen(StringToScroll); ++Loop1UInt16)
  {
    while (ScrollDotCount >= (DISPLAY_BUFFER_SIZE - 6))
    {
      /* We are approaching the end of the framebuffer... wait a some time for the buffer to be partly cleared. */
      if (DebugBitMask & DEBUG_SCROLL)
        uart_send(__LINE__, "Waiting to add text to scroll: [%s] (%u)\r", &StringToScroll[Loop1UInt16], ScrollDotCount);

      sleep_ms(SCROLL_DOT_TIME * 6 * 18);  // wait for about 18 characters to be scrolled.
    }

    if (DebugBitMask & DEBUG_SCROLL)
      uart_send(__LINE__, "Filling-up display_buffer [%c]\r", StringToScroll[Loop1UInt16]);

    ScrollDotCount   = fill_display_buffer_5X7(++ScrollDotCount, StringToScroll[Loop1UInt16]);
    CurrentClockMode = MODE_SCROLLING;
    FlagScrollStart  = FLAG_ON;
  }

  if (DebugBitMask & DEBUG_SCROLL)
    uart_send(__LINE__, "Exiting  scroll_string()\r");

  return;
}




/* $PAGE */
/* $TITLE=scroll_unqueue() */
/* ------------------------------------------------------------------ *\
                 Unqueue next tag from scroll queue.
\* ------------------------------------------------------------------ */
UINT8 scroll_unqueue(void)
{
  UINT8 Tag;


  /* Check if scroll queue is empty. */
  if (ScrollQueueHead == ScrollQueueTail)
    return MAX_EVENTS;

  Tag = ScrollQueue[ScrollQueueTail];
  ScrollQueue[ScrollQueueTail] = 0xAA;
  ++ScrollQueueTail;

  /* If reaching out-of-bound, revert to zero. */
  if (ScrollQueueTail >= MAX_SCROLL_QUEUE)
    ScrollQueueTail = 0;

  return Tag;
}





/* $PAGE */
/* $TITLE=send_data() */
/* ------------------------------------------------------------------ *\
                Send data to the matrix controller IC.
\* ------------------------------------------------------------------ */
void send_data(UINT8 Byte)
{
  UINT8 Loop1UChar;


  for (Loop1UChar = 0; Loop1UChar < 8; ++Loop1UChar)
  {
    CLK_LOW;
    SDI_LOW;

    if (Byte & 0x01)
      SDI_HIGH;

    Byte >>= 1;

    CLK_HIGH;
  }

  return;
}





/* $PAGE */
/* $TITLE=set_and_save_credentials() */
/* ------------------------------------------------------------------ *\
               Request Wi-Fi hostname, SSID and password from user
                    and save them to Pico's flash.
\* ------------------------------------------------------------------ */
void set_and_save_credentials(void)
{
  UCHAR NewHostname[33];
  UCHAR NewSsid[33];
  UCHAR NewPassword[65];
  UCHAR String[256];

  UINT8 CharacterNumber;    // character being entered for Hostname, SSID or password.
  UINT8 Column;
  UINT8 CurrentStepNumber;  // current procedure step (Hostname, SSID, Password).
  UINT8 FlagNextStep;       // flag indicating it's time to continue with next step (Hostname, SSID, Password).
  UINT8 Loop1Number;        // to evaluate the time the "Up" and "Down" buttons have been pressed.
  UINT8 Loop2Number;        // to evaluate the time the "Set" button has been pressed.
  UINT8 OriginalClockMode;  // keep track of clock mode before entering this function.
  UINT8 Row;
  UINT8 TargetCharacter;

  UINT16 Delay;

  UINT32 Duration;
  UINT32 TimeStamp;


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering set_and_save_credentials()\r\r");

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering the function. */
  tone(10);
  sleep_ms(100);


  /* Scroll current credentials on entry. */
  sprintf(String, "Host: [%s]   SSID: [%s]   Pwd: [%s]", FlashConfig.Hostname, FlashConfig.SSID, FlashConfig.Password);
  scroll_string(24, String);

  /* Wait until scrolling is done. */
  while(ScrollDotCount)
    sleep_ms(100);


  /* Prevent dots blinking and other automations on the clock. */
  OriginalClockMode = CurrentClockMode;
  CurrentClockMode = MODE_TEST;


  /* Enter Hostname, SSID and Password. */
  for (CurrentStepNumber = 0; CurrentStepNumber < 3; ++CurrentStepNumber)
  {
    CharacterNumber = 0;
    FlagNextStep    = FLAG_OFF;  // will be turned On when it's time to iterate next "for" loop.
    TargetCharacter = 'A';

    switch (CurrentStepNumber)
    {
      case (0):
        scroll_string(24, "Enter Host:");
      break;

      case (1):
        scroll_string(24, "Enter SSID:");
      break;

      case (2):
        scroll_string(24, "Enter Password:");
      break;
    }


    /* Wait until scrolling is done. */
    while (ScrollDotCount)
      sleep_ms(100);


    fill_display_buffer_5X7(2, ' ');    // last character selected.
    fill_display_buffer_5X7(11, '[');   // open frame for target character.
    fill_display_buffer_5X7(15, TargetCharacter);
    fill_display_buffer_5X7(21, ']');   // close frame for target character.


    CurrentClockMode = MODE_TEST;

    while (FlagNextStep == FLAG_OFF)
    {
      /* Wait until user presses a button. */
      while ((gpio_get(SET_BUTTON)) && (gpio_get(UP_BUTTON)) && (gpio_get(DOWN_BUTTON)));


      /* Initialize data for this specific button press. */
      Delay      = 300;  // initial delay to scan characters.
      Loop1Number = 0;    // reset character scan acceleration on entry.
      TimeStamp  = time_us_32();  // time stamp at beginning of button press.


      if (DebugBitMask & DEBUG_NTP)
        uart_send(__LINE__, "CurrentClockMode: %u  (MODE_TEST: %u)\r", CurrentClockMode, MODE_TEST);


      /* If user pressed the "Set" button. */
      if (gpio_get(SET_BUTTON) == 0)
      {
        Loop2Number = 0;
        if (DebugBitMask & DEBUG_NTP)
          uart_send(__LINE__, "User pressed the <Set> button.\r");

        while (gpio_get(SET_BUTTON) == 0)
        {
          if ((Loop2Number % 10) == 0)
          {
            tone(50);  // audible feedback to user for each second passed.
          }
          ++Loop2Number;
          sleep_ms(100);
        }

        Duration = time_us_32() - TimeStamp;
        sleep_ms(100);  // debounce.

        /* User pressed "Set" (top) button for more than 3 seconds (3,000,000 microseconds). */
        if (Duration < 3000000)
        {
          if (DebugBitMask & DEBUG_NTP)
            uart_send(__LINE__, "<Set> button press shorter than 3 seconds.\r");

          /* "Set" (top) button press is less than 5 seconds, add target character to current string (Hostname, SSID or password). */
          if (CurrentStepNumber == 0) NewHostname[CharacterNumber] = TargetCharacter;
          if (CurrentStepNumber == 1) NewSsid[CharacterNumber] = TargetCharacter;
          if (CurrentStepNumber == 2) NewPassword[CharacterNumber] = TargetCharacter;
          fill_display_buffer_5X7(2, TargetCharacter);   // last character entered.

          /* Refresh open frame and target character. */
          fill_display_buffer_5X7(11, '[');   // open frame for target character.
          fill_display_buffer_5X7(15, TargetCharacter);   //  current target character.
          ++CharacterNumber;
        }
        else
        {
          if (DebugBitMask & DEBUG_NTP)
            uart_send(__LINE__, "<Set> button press longer than 3 seconds.\r");

          /* "Set" (top) button pressed for more than 3 seconds, assign current string to Hostname, SSID or Password to be flashed. */
          if (CurrentStepNumber == 0)
          {
            NewHostname[CharacterNumber] = 0;  // add end-of-string.

            /* Assign Hostname only if it is a non-null string. */
            if (NewHostname[0] != 0)
            {
              sprintf(&FlashConfig.Hostname[4], NewHostname);
              clear_framebuffer(0);
              sprintf(String, "HOST saved: %s      ", NewHostname);
              scroll_string(24, String);
            }
          }
          if (CurrentStepNumber == 1)
          {
            NewSsid[CharacterNumber] = 0;  // add end-of-string.

            /* Assign SSID only if it is a non-null string. */
            if (NewSsid[0] != 0)
            {
              sprintf(&FlashConfig.SSID[4], NewSsid);
              clear_framebuffer(0);
              sprintf(String, "SSID saved: %s      ", NewSsid);
              scroll_string(24, String);
            }
          }

          if (CurrentStepNumber == 2)
          {
            NewPassword[CharacterNumber] = 0;  // add end-of-string.

            /* Assign password only if it is a non-null string. */
            if (NewPassword[0] != 0)
            {
              sprintf(&FlashConfig.Password[4], NewPassword);
              clear_framebuffer(0);
              sprintf(String, "Password saved: %s   ", NewPassword);
              scroll_string(24, String);
            }

            /* Wait until scrolling is done before falling in Green Clock power-up sequence. */
            while (ScrollDotCount)
              sleep_ms(100);
          }
          FlagNextStep = FLAG_ON;
          continue;
        }
      }



      /* If user pressed the "Up" button. */
      if (gpio_get(UP_BUTTON) == 0)
      {
        if (DebugBitMask & DEBUG_NTP)
          uart_send(__LINE__, "User pressed the <Up> button.\r");

        while (gpio_get(UP_BUTTON) == 0)
        {
          sleep_ms(100);  // debounce.
          /* User pressed "Up" (middle) button, scan target characters. */
          ++TargetCharacter;
          if (TargetCharacter == 127) TargetCharacter = 32;
          ++Loop1Number;

          /* Erase previous character. */
          for (Row = 1; Row < 8; ++Row)
            for (Column = 15; Column < 20; ++Column)
              set_pixel(Row, Column, FLAG_OFF);

          fill_display_buffer_5X7(15, TargetCharacter);   //  current target character.
          /// fill_display_buffer_5X7(21, ']');               // close frame for target character.
          if (Loop1Number == 5)  Delay = 250;
          if (Loop1Number == 10) Delay = 120;
          sleep_ms(Delay);
        }
      }



      /* If user pressed the "Down" button. */
      if (gpio_get(DOWN_BUTTON) == 0)
      {
        if (DebugBitMask & DEBUG_NTP)
          uart_send(__LINE__, "User pressed the <Down> button.\r");

        while (gpio_get(DOWN_BUTTON) == 0)
        {
          sleep_ms(100);  // debounce.
          /* User pressed "Down" (bottom) button, scan target characters. */
          --TargetCharacter;
          if (TargetCharacter == 31) TargetCharacter = 126;
          ++Loop1Number;

          /* Erase previous character. */
          for (Row = 1; Row < 8; ++Row)
            for (Column = 15; Column < 20; ++Column)
              set_pixel(Row, Column, FLAG_OFF);

          fill_display_buffer_5X7(15, TargetCharacter);   //  current target character.
          /// fill_display_buffer_5X7(21, ']');  // close frame for target character.
          if (Loop1Number == 5)  Delay = 250;
          if (Loop1Number == 10) Delay = 120;
          sleep_ms(Delay);
        }
      }
    }
  }

  CurrentClockMode = OriginalClockMode;

  return;
}





/* $PAGE */
/* $TITLE=set_mode_out() */
/* ------------------------------------------------------------------ *\
        Program potentially modified clock parameters to RTC IC.
\* ------------------------------------------------------------------ */
void set_mode_out(void)
{
  UINT8 Loop1UInt8;
  UINT8 TargetDayOfWeek;


  if (DebugBitMask & DEBUG_DST)
    uart_send(__LINE__, "Set mode out\r");

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_HOUR]   == FLAG_ON))
    set_hour(CurrentHourSetting);

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_MINUTE] == FLAG_ON))
    set_minute(CurrentMinuteSetting);

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_MONTH]  == FLAG_ON))
  {
    set_month(CurrentMonth);
    TargetDayOfWeek = get_day_of_week(CurrentYear, CurrentMonth, CurrentDayOfMonth);
    set_day_of_week(TargetDayOfWeek);
    update_top_indicators(ALL, FLAG_OFF);  // first, turn Off all days' indicators.
    update_top_indicators(TargetDayOfWeek, FLAG_ON);
  }

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_DAY_OF_MONTH] == FLAG_ON))
  {
    set_day_of_month(CurrentDayOfMonth);
    TargetDayOfWeek = get_day_of_week(CurrentYear, CurrentMonth, CurrentDayOfMonth);
    set_day_of_week(TargetDayOfWeek);
    update_top_indicators(ALL, FLAG_OFF);  // first, turn Off all days' indicators.
    update_top_indicators(TargetDayOfWeek, FLAG_ON);
  }

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_YEAR] == FLAG_ON))
  {
    set_year(CurrentYearLowPart);
    TargetDayOfWeek = get_day_of_week(CurrentYear, CurrentMonth, CurrentDayOfMonth);
    set_day_of_week(TargetDayOfWeek);
    update_top_indicators(ALL, FLAG_OFF);  // first, turn Off all days' indicators.
    update_top_indicators(TargetDayOfWeek, FLAG_ON);
  }

  FlagBlinking[SetupStep] = 0xFF;

  if (FlagSetupAlarm[SETUP_ALARM_MINUTE] == FLAG_ON)
  {
    fill_display_buffer_4X7(13, (FlashConfig.Alarm[AlarmNumber].Minute / 10 + '0') & FlagBlinking[4]);
    fill_display_buffer_4X7(18, (FlashConfig.Alarm[AlarmNumber].Minute % 10 + '0') & FlagBlinking[4]);
  }

  /***
  if ((FlagSetupTimer[SETUP_TIMER_UP_DOWN] == FLAG_ON) && (TimerMode == TIMER_OFF))
  {
    // Timer is currently OFF.
    fill_display_buffer_4X7(13, '0' & FlagBlinking[1]);
    fill_display_buffer_4X7(18, 'F' & FlagBlinking[1]);
  }
  ***/

  IdleNumberOfSeconds = 0; // reset number of seconds the system has been idle.
  FlagTone = FLAG_OFF;     // reset flag tone.
  FlagSetupRTC = FLAG_OFF; // reset flag indicating time settings have changed.

  /* Reset all alarm setup member flags. */
  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_ALARM_HI_LIMIT; ++Loop1UInt8)
    FlagSetupAlarm[Loop1UInt8] = FLAG_OFF;

  /* Reset all clock setup members flags. */
  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_CLOCK_HI_LIMIT; ++Loop1UInt8)
    FlagSetupClock[Loop1UInt8] = FLAG_OFF;

  /* Reset all timer setup member flags. */
  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_TIMER_HI_LIMIT; ++Loop1UInt8)
    FlagSetupTimer[Loop1UInt8] = FLAG_OFF;

  return;
}





/* $PAGE */
/* $TITLE=set_mode_timeout() */
/* ------------------------------------------------------------------ *\
          Exit setup mode because of inactivity (time-out)
                  or when setup has been completed.
\* ------------------------------------------------------------------ */
void set_mode_timeout(void)
{
  if (DebugBitMask & DEBUG_DST)
    uart_send(__LINE__, "Set mode timeout\r");

  ScrollSecondCounter = 0;          // reset number of seconds to reach time-to-scroll.
  SetupSource         = SETUP_SOURCE_NONE;
  FlagIdleCheck       = FLAG_OFF;
  FlagSetAlarm        = FLAG_OFF;  // reset alarm setup mode flag when timed-out.
  FlagSetClock        = FLAG_OFF;  // reset clock setup mode flag when timed-out.
  FlagSetTimer        = FLAG_OFF;  // reset timer setup mode flag when timed-out.
  FlagUpdateTime      = FLAG_ON;   // let's display current time on the clock, now.

  /* Determine the day-of-year for DST start day and DST end day (in case we just change CurrentYear). */
  get_dst_days();

  /* Check for an eventual change in Daylight Saving Time status. */
  update_dst_status();

  /* Request a NTP re-sync if clock setup has been changed (Time, Date, or Timezone may have been changed). */
  NTPData.FlagNTPResync = FLAG_ON;

  return;
}





/* $PAGE */
/* $TITLE=set_pixel() */
/* ------------------------------------------------------------------ *\
                  Turn On or Off the specified pixel.
\* ------------------------------------------------------------------ */
void set_pixel(UINT8 PixelRow, UINT8 PixelColumn, UINT8 Flag)
{
  UINT8 Column;
  UINT8 Row;


  /* Validation of Row number. */
  --PixelRow;  // make it 0-biased
  if (PixelRow > 6) return;

  /* Validation of Column number. */
  --PixelColumn;  // make it 0-biased
  if (PixelColumn > 21) return;

  switch (Flag)
  {
    case (FLAG_ON):
      DisplayBuffer[Pixel[PixelRow][PixelColumn].DisplayBuffer] |= (0x01 << Pixel[PixelRow][PixelColumn].BitNumber);
    break;

    case (FLAG_OFF):
      DisplayBuffer[Pixel[PixelRow][PixelColumn].DisplayBuffer] &= ~(0x01 << Pixel[PixelRow][PixelColumn].BitNumber);
    break;
  }

  return;
}





/* $PAGE */
/* $TITLE=setup_alarm_frame() */
/* ------------------------------------------------------------------ *\
               Display current alarm frame / parameters.
\* ------------------------------------------------------------------ */
void setup_alarm_frame(void)
{
  UCHAR String[256];

  UINT8 AmFlag;
  UINT8 PmFlag;
  UINT8 AlarmHourDisplay; // to display alarm hour in 12 or 24-hours format.
  UINT8 Loop1UInt8;


  CurrentClockMode = MODE_ALARM_SETUP;


  /* Check if current alarm setup step is either alarm number or alarm ON / OFF toggle. */
  if ((SetupStep == SETUP_ALARM_NUMBER) || (SetupStep == SETUP_ALARM_ON_OFF))
  {
    /* Check if current alarm setup step is alarm number select. */
    if (SetupStep == SETUP_ALARM_NUMBER)
    {
      FlagSetupAlarm[SETUP_ALARM_NUMBER] = FLAG_ON;
    }



    /* Check if current alarm setup step is alarm On / Off toggling. */
    if (SetupStep == SETUP_ALARM_ON_OFF)
    {
      FlagSetupAlarm[SETUP_ALARM_ON_OFF] = FLAG_ON;
    }



    /* Display first digit on clock display ("A" for "Alarm").
       NOTE: Alarm number if 1 to 9 for user, but 0 to 8 for firmware. */
    fill_display_buffer_4X7(0, 'A');
    fill_display_buffer_4X7(5, ('1' + AlarmNumber) & FlagBlinking[1]);

    /* Display double dots as display separator. */
    fill_display_buffer_4X7(10, ':');

    fill_display_buffer_4X7(13, '0' & FlagBlinking[2]);
    if (FlashConfig.Alarm[AlarmNumber].FlagStatus == FLAG_ON)
    {
      /* If this alarm is enabled, display "ON" on clock display. */
      fill_display_buffer_4X7(18, 'N' & FlagBlinking[2]);
    }
    else
    {
      /* Otherwise, display "OF" (for "OFF") on display. */
      fill_display_buffer_4X7(18, 'F' & FlagBlinking[2]);
    }

    /* Clean the non visible part of the display when done. */
    clear_framebuffer(26);
  }



  /* Check if current alarm setup step is either alarm hour setup or alarm minute setup. */
  if (SetupStep == SETUP_ALARM_HOUR || SetupStep == SETUP_ALARM_MINUTE)
  {
    if (SetupStep == SETUP_ALARM_HOUR)
    {
      FlagSetupAlarm[SETUP_ALARM_HOUR] = FLAG_ON;
    }



    if (SetupStep == SETUP_ALARM_MINUTE)
    {
      FlagSetupAlarm[SETUP_ALARM_MINUTE] = FLAG_ON;
    }



    /* Display current alarm hour and minute on clock display. */
    if (FlashConfig.TimeDisplayMode == H12)
    {
      AlarmHourDisplay = convert_h24_to_h12(FlashConfig.Alarm[AlarmNumber].Hour, &AmFlag, &PmFlag);
      (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
      (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
    }
    else
    {
      /* We are in "24-hours" display mode. */
      AlarmHourDisplay = FlashConfig.Alarm[AlarmNumber].Hour;
    }


    /* When in 12-hour time display format, first digit is not displayed if it is zero. */
    if ((FlashConfig.TimeDisplayMode == H12) && (AlarmHourDisplay < 10))
    {
      fill_display_buffer_4X7(0, ' ' & FlagBlinking[SETUP_HOUR]);
    }
    else
      fill_display_buffer_4X7(0, (AlarmHourDisplay / 10 + '0') & FlagBlinking[3]);

    fill_display_buffer_4X7(5, (AlarmHourDisplay % 10 + '0') & FlagBlinking[3]);
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(13, (FlashConfig.Alarm[AlarmNumber].Minute / 10 + '0') & FlagBlinking[4]);
    fill_display_buffer_4X7(18, (FlashConfig.Alarm[AlarmNumber].Minute % 10 + '0') & FlagBlinking[4]);

    /* Clean the non visible part of the display when done. */
    clear_framebuffer(26);
  }



  /* Check if current alarm setup step is alarm day-of-week setup. */
  if (SetupStep == SETUP_ALARM_DAY)
  {
    FlagSetupAlarm[SETUP_ALARM_DAY] = FLAG_ON; // flag indicating we are in "Alarm Day-of-week" setup step.
    /* Note: day-of-week indicator for alarm setting will blink in the function "evaluate_blinking_time()". */
  }



  /* Check if we are done with all alarm setting. */
  if (SetupStep >= SETUP_ALARM_HI_LIMIT)
  {
    /* OBSOLETE PIECE OF CODE. Nine (9) alarms are now kept in Pico's flash.
    // If alarm #1 is turned on, write it to the RTC IC.
    if ((AlarmNumber == 0) && (FlashConfig.Alarm[0].FlagStatus == FLAG_ON))
    {
      set_alarm1_clock(ALARM_MODE_HOUR_MIN_SEC_MATCHED, 00, FlashConfig.Alarm[0].Minute, FlashConfig.Alarm[0].Hour, FlashConfig.Alarm[0].Day);
    }

    // If alarm #2 is turned on, write it to the RTC IC.
    if ((AlarmNumber == 1) && (FlashConfig.Alarm[1].FlagStatus == FLAG_ON))
    {
      set_alarm2_clock(FlashConfig.Alarm[1].Minute, FlashConfig.Alarm[1].Hour, FlashConfig.Alarm[1].Day);
    }
    */

    /* Reset all related flags and variables when alarm setup is done. */
    FlagSetAlarm        = FLAG_OFF;  // reset alarm setup flag when done.
    FlagIdleCheck       = FLAG_OFF;  // no more checking for a timeout.
    FlagTone            = FLAG_OFF;  // reset flag tone.
    FlagUpdateTime      = FLAG_ON;   // it's now time to display time on clock display.
    IdleNumberOfSeconds = 0;         // reset the number of seconds the system has been idle.
    ScrollSecondCounter = 0;         // reset number of seconds to reach time-to-scroll (obsolete).
    SetupSource         = SETUP_SOURCE_NONE;
    SetupStep           = SETUP_NONE;

    /* Reset all alarm setup member flags. */
    for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_ALARM_HI_LIMIT; ++Loop1UInt8)
      FlagSetupAlarm[Loop1UInt8] = FLAG_OFF;
  }

  return;
}





/* $PAGE */
/* $TITLE=setup_alarm_variables() */
/* ------------------------------------------------------------------ *\
                   Change current alarm parameters.
\* ------------------------------------------------------------------ */
void setup_alarm_variables(UINT8 FlagButtonSelect)
{
  UCHAR String[256];
  UCHAR DayMask[16];

  UINT8 Dum1UInt8;
  UINT8 Loop1UInt8;


  if (DebugBitMask & DEBUG_ALARMS)
    uart_send(__LINE__, "Entering setup_alarm_variables\r");

  /* Check if we are in "Alarm Number" setup step. */
  if (FlagSetupAlarm[SETUP_ALARM_NUMBER] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* Increment alarm number. */
      ++AlarmNumber;
      if (AlarmNumber > 8) AlarmNumber = 0;
    }
    else
    {
      --AlarmNumber;
      if (AlarmNumber > 8) AlarmNumber = 8;
    }
  }

  /* Toggle alarm On / Off. */
  if (FlagSetupAlarm[SETUP_ALARM_ON_OFF] == FLAG_ON)
  {
    if (FlashConfig.Alarm[AlarmNumber].FlagStatus == FLAG_ON)
      FlashConfig.Alarm[AlarmNumber].FlagStatus = FLAG_OFF;
    else
      FlashConfig.Alarm[AlarmNumber].FlagStatus = FLAG_ON;

    /* We now have 9 alarms available... Check if at least one alarm is On and if ever the case, turn On both alarm indicators. */
    Dum1UInt8 = 0;
    for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
      if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_ON)
        Dum1UInt8 = 1;

    if (Dum1UInt8)
    {
      IndicatorAlarm0On;
      IndicatorAlarm1On;
    }
    else
    {
      IndicatorAlarm0Off;
      IndicatorAlarm1Off;
    }
  }

  /* Check if we are setting up "Alarm Hour". */
  if (FlagSetupAlarm[SETUP_ALARM_HOUR] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Alarm Hour" set mode. */
      ++FlashConfig.Alarm[AlarmNumber].Hour;
      if (FlashConfig.Alarm[AlarmNumber].Hour == 24)
        FlashConfig.Alarm[AlarmNumber].Hour = 0;  // reset to zero when reaching out-of-bound.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Alarm Hour" set mode. */
      --FlashConfig.Alarm[AlarmNumber].Hour;
      if (FlashConfig.Alarm[AlarmNumber].Hour == 255)
        FlashConfig.Alarm[AlarmNumber].Hour = 23;  // reset to 23 when reaching out-of-bound.
    }
  }

  /* Check if we are setting up "Alarm Minute". */
  if (FlagSetupAlarm[SETUP_ALARM_MINUTE] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Alarm Minute" set mode. */
      ++FlashConfig.Alarm[AlarmNumber].Minute;
      if (FlashConfig.Alarm[AlarmNumber].Minute == 60)
        FlashConfig.Alarm[AlarmNumber].Minute = 0;  // reset to zero when reaching out-of-bound.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Alarm Minute" set mode. */
      --FlashConfig.Alarm[AlarmNumber].Minute;
      if (FlashConfig.Alarm[AlarmNumber].Minute == 255)
        FlashConfig.Alarm[AlarmNumber].Minute = 59;  // reset to 59 when reaching out-of-bound.
    }
  }

  /* Check if we are setting up "Alarm Day". */
  if (FlagSetupAlarm[SETUP_ALARM_DAY] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      if (DebugBitMask & DEBUG_ALARMS)
        uart_send(__LINE__, "Received FLAG_UP\r");

      /* User pressed the "Up" (middle) button, increment potential alarm target day-of-week. */
      ++AlarmTargetDay;
      if (AlarmTargetDay >= 8) AlarmTargetDay = 1;  // when reaching out-of-bound, revert to 1 (SUN).
    }

    if (FlagButtonSelect == FLAG_DOWN)
    {
      if (DebugBitMask & DEBUG_ALARMS)
        uart_send(__LINE__, "Received FLAG_DOWN\r");

      /* User presser the "Down" (bottom) button, proceed through all day-of-week. */
      --AlarmTargetDay;
      if ((AlarmTargetDay == 0) || (AlarmTargetDay >= 8)) AlarmTargetDay = 7;  // when reaching out-of-bound, revert to 7 (SAT).
    }

    if (FlagButtonSelect == FLAG_LONG_UP)
    {
      FlashConfig.Alarm[AlarmNumber].Day |= (1 << AlarmTargetDay);

      if (DebugBitMask & DEBUG_ALARMS)
        uart_send(__LINE__, "Received FLAG_LONG_UP\r");
    }

    if (FlagButtonSelect == FLAG_LONG_DOWN)
    {
      FlashConfig.Alarm[AlarmNumber].Day &= (~(1 << AlarmTargetDay));

      if (DebugBitMask & DEBUG_ALARMS)
        uart_send(__LINE__, "Received FLAG_LONG_DOWN\r");
    }
  }

  if (DebugBitMask & DEBUG_ALARMS)
  {
    uart_send(__LINE__, "AlarmTargetDay = %u   (%s)\r", AlarmTargetDay, DayName[FlashConfig.Language][AlarmTargetDay]);

    /* Identify all days-of-week targets for current alarm. */
    DayMask[0] = 0x00;  // initialize bitmask.
    for (Loop1UInt8 = 7; Loop1UInt8 > 0; --Loop1UInt8)
    {
      if (FlashConfig.Alarm[AlarmNumber].Day & (1 << Loop1UInt8))
        strcat(DayMask, "1");
      else
        strcat(DayMask, "0");
    }
    strcat(DayMask, "0");  // add bit 0 since days-of-week go from 1 to 7 (1 being SUN and 7 being SAT)
    sprintf(String, "[%X] Alarm[%2.2u].DayMask:     %s     (%X) ", &FlashConfig.Alarm[AlarmNumber].Day, AlarmNumber, DayMask, FlashConfig.Alarm[AlarmNumber].Day);

    for (Loop1UInt8 = 1; Loop1UInt8 < 8; ++Loop1UInt8)
    {
      if (FlashConfig.Alarm[AlarmNumber].Day & (1 << Loop1UInt8))
      {
        Dum1UInt8 = strlen(String);
        sprintf(&String[strlen(String)], "%s", DayName[FlashConfig.Language][Loop1UInt8]);
        String[Dum1UInt8 + 3] = 0x20;  // space separator.
        String[Dum1UInt8 + 4] = 0x00;  // keep first 3 characters of day name.
      }
    }

    strcat(String, "\r");
    uart_send(__LINE__, String);
    uart_send(__LINE__, "Exiting setup_alarm_variables\r");
  }

  return;
}





/* $PAGE */
/* $TITLE=setup_clock_frame() */
/* ------------------------------------------------------------------ *\
                  Display current clock parameters.
\* ------------------------------------------------------------------ */
void setup_clock_frame(void)
{
  UCHAR String[256];

  UINT8 AmFlag;
  UINT8 PmFlag;
  UINT8 Dum1UChar;
  UINT8 DumDayOfMonth;
  UINT8 DumMonth;
  UINT8 DumYearLowPart;
  UINT8 Loop1UInt8;

  // clock works in 24 hr values, but convert to 12 hr or 24 hr for display
  UINT8 CurrentHourDisplay;

  /* Make sure the function has not been called by mistake. */
  if (SetupStep == SETUP_NONE)
    return;


  CurrentClockMode = MODE_CLOCK_SETUP;


  /* ---------------- Read the real-time clock IC. ------------------ */
  Time_RTC         = Read_RTC();
  DumMonth         = bcd_to_byte(Time_RTC.month);
  DumDayOfMonth    = bcd_to_byte(Time_RTC.dayofmonth);
  DumYearLowPart   = bcd_to_byte(Time_RTC.year);
  CurrentDayOfWeek = get_day_of_week(((FlashConfig.CurrentYearCentile * 100) + DumYearLowPart), DumMonth, DumDayOfMonth);



  /* ------------------- Setup Hour and Minute ---------------------- */
  /* Setup "Hours" and "Minutes". */
  if ((SetupStep == SETUP_HOUR) || (SetupStep == SETUP_MINUTE))
  {
    if (SetupStep == SETUP_HOUR)
    {
      /* Setup "hours". */
      if (FlagSetupClock[SETUP_HOUR] == FLAG_OFF)
      {
        /* Use CurrentHour value as default for hour setting. */
        CurrentHourSetting = bcd_to_byte(Time_RTC.hour);
      }
      FlagSetupClock[SETUP_HOUR] = FLAG_ON;
    }

    if (SetupStep == SETUP_MINUTE)
    {
      /* Setup "minutes". */
      if (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF)
      {
        /* Use CurrentMinute value as default for minute setting. */
        CurrentMinuteSetting = bcd_to_byte(Time_RTC.minutes);
      }
      FlagSetupClock[SETUP_MINUTE] = FLAG_ON;
    }

    if (FlashConfig.TimeDisplayMode == H12)
    {
      CurrentHourDisplay = convert_h24_to_h12(CurrentHourSetting, &AmFlag, &PmFlag);
      (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
      (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
    }
    else
    {
      /* We are in "24-hours" display mode. */
      CurrentHourDisplay = CurrentHour;
    }

    /* When in 12-hour time display format, first digit is not displayed if it is zero. */
    if ((FlashConfig.TimeDisplayMode == H12) && (CurrentHourDisplay < 10))
    {
      fill_display_buffer_4X7(0, ' ' & FlagBlinking[SETUP_HOUR]);
    }
    else
      fill_display_buffer_4X7(0, (CurrentHourDisplay / 10 + '0') & FlagBlinking[SETUP_HOUR]);

    fill_display_buffer_4X7(5, (CurrentHourDisplay % 10 + '0') & FlagBlinking[SETUP_HOUR]);
    fill_display_buffer_4X7(10, 0x3A); // slim ":"
    fill_display_buffer_4X7(12, (CurrentMinute / 10 + '0') & FlagBlinking[SETUP_MINUTE]);
    fill_display_buffer_4X7(17, (CurrentMinute % 10 + '0') & FlagBlinking[SETUP_MINUTE]);
  }



  /* --------------- Setup Month and Day-of-month ------------------- */
  if ((SetupStep == SETUP_DAY_OF_MONTH) || (SetupStep == SETUP_MONTH))
  {
    /* There are conditions where Day-of-month can be higher than allowed. Re-adjust if it is the case. */
    /* For example, we set March 31st, then we change March to April while leaving 31 as the day. */
    CurrentYear = FlashConfig.CurrentYearCentile * 100 + CurrentYearLowPart;
    Dum1UChar = get_month_days(CurrentYear, CurrentMonth);
    if (CurrentDayOfMonth > Dum1UChar)
    {
      CurrentDayOfMonth = Dum1UChar;
    }

    if (SetupStep == SETUP_DAY_OF_MONTH)
    {
      FlagSetupClock[SETUP_DAY_OF_MONTH] = FLAG_ON;
      FlagSetupClock[SETUP_MONTH] = FLAG_OFF;
    }
    else
    {
      FlagSetupClock[SETUP_MONTH] = FLAG_ON;
      FlagSetupClock[SETUP_DAY_OF_MONTH] = FLAG_OFF;
    }

    if ((FlashConfig.Language == CZECH) || (FlashConfig.Language == FRENCH) || (FlashConfig.Language == GERMAN) || (FlashConfig.Language == SPANISH))
    {
      /* In some languages, display and set day-of-month before month.
         Make a trick in the flashing below. */
      fill_display_buffer_4X7(0, ((CurrentDayOfMonth / 10 + 0x30) & FlagBlinking[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(5, ((CurrentDayOfMonth % 10 + 0x30) & FlagBlinking[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(10, '-');
      fill_display_buffer_4X7(13, ((CurrentMonth / 10 + 0x30) & FlagBlinking[SETUP_MONTH]));
      fill_display_buffer_4X7(18, ((CurrentMonth % 10 + 0x30) & FlagBlinking[SETUP_MONTH]));
    }
    else
    {
      /* For English languages. */
      fill_display_buffer_4X7(0, ((CurrentMonth / 10 + 0x30) & FlagBlinking[SETUP_MONTH]));
      fill_display_buffer_4X7(5, ((CurrentMonth % 10 + 0x30) & FlagBlinking[SETUP_MONTH]));
      fill_display_buffer_4X7(10, '-');
      fill_display_buffer_4X7(13, ((CurrentDayOfMonth / 10 + 0x30) & FlagBlinking[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(18, ((CurrentDayOfMonth % 10 + 0x30) & FlagBlinking[SETUP_DAY_OF_MONTH]));
    }
  }



  /* ------------------------- Setup Year --------------------------- */
  if (SetupStep == SETUP_YEAR)
  {
    FlagSetupClock[SETUP_YEAR] = FLAG_ON;

    /* Setup Year (centiles and units). */
    CurrentYear = FlashConfig.CurrentYearCentile * 100 + CurrentYearLowPart;

    fill_display_buffer_4X7(0, FlashConfig.CurrentYearCentile / 10 + 0x30);
    fill_display_buffer_4X7(5, FlashConfig.CurrentYearCentile % 10 + 0x30);
    fill_display_buffer_4X7(10, ((CurrentYearLowPart / 10 + 0x30) & FlagBlinking[SETUP_YEAR]));
    fill_display_buffer_4X7(15, ((CurrentYearLowPart % 10 + 0x30) & FlagBlinking[SETUP_YEAR]));
    fill_display_buffer_4X7(23, ' ');
  }



  /* ------------------ Setup Daylight Saving Time ------------------ */
  if (SetupStep == SETUP_DST)
  {
    /* Setup Daylight Saving Time. */
    FlagSetupClock[SETUP_DST] = FLAG_ON;

    clear_framebuffer(0);
    fill_display_buffer_5X7(2, 'S');
    fill_display_buffer_5X7(8, 'T');

    switch (FlashConfig.DSTCountry)
    {
      default:
      case (DST_NONE):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('0' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_AUSTRALIA):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('1' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_AUSTRALIA_HOWE):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('2' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_CHILE):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('3' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_CUBA):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('4' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_EUROPE):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('5' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_ISRAEL):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('6' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_LEBANON):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('7' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_MOLDOVA):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('8' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_NEW_ZEALAND):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_4X7(16, ('9' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_NORTH_AMERICA):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_5X7(16, ('1' & FlagBlinking[SETUP_DST]));
        fill_display_buffer_4X7(18, ('0' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_PALESTINE):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_5X7(16, ('1' & FlagBlinking[SETUP_DST]));
        fill_display_buffer_4X7(17, ('1' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_PARAGUAY):
        fill_display_buffer_5X7(14, ' ');
        fill_display_buffer_5X7(19, ' ');
        fill_display_buffer_5X7(16, ('1' & FlagBlinking[SETUP_DST]));
        fill_display_buffer_4X7(18, ('2' & FlagBlinking[SETUP_DST]));
      break;
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ----------------------- Setup Timezone ----------------------- */
  if (SetupStep == SETUP_UTC)
  {
    /* Setup Universal Coordinate Time. */
    FlagSetupClock[SETUP_UTC] = FLAG_ON;

    clear_framebuffer(0);
    fill_display_buffer_5X7(2, 'T');
    fill_display_buffer_5X7(8, 'Z');


    /* First clear right section of clock display. */
    fill_display_buffer_5X7(14, ' ');
    fill_display_buffer_5X7(19, ' ');


    /* Then display current Timezone value. */
    if ((FlashConfig.Timezone >= 0) && (FlashConfig.Timezone < 10))
    {
      fill_display_buffer_4X7(16, ((FlashConfig.Timezone + '0') & FlagBlinking[SETUP_UTC]));
    }

    if (FlashConfig.Timezone >= 10)
    {
      fill_display_buffer_4X7(13, (((FlashConfig.Timezone / 10) + '0') & FlagBlinking[SETUP_UTC]));
      fill_display_buffer_4X7(18, (((FlashConfig.Timezone % 10) + '0') & FlagBlinking[SETUP_UTC]));
    }

    if ((FlashConfig.Timezone < 0) && (FlashConfig.Timezone > -10))
    {
      fill_display_buffer_4X7(14, '-' & FlagBlinking[SETUP_UTC]);
      fill_display_buffer_4X7(17, ((-FlashConfig.Timezone + '0') & FlagBlinking[SETUP_UTC]));
    }

    if (FlashConfig.Timezone <= -10)
    {
      fill_display_buffer_4X7(11, '-' & FlagBlinking[SETUP_UTC]);
      fill_display_buffer_4X7(13, (((-FlashConfig.Timezone / 10) + '0') & FlagBlinking[SETUP_UTC]));
      fill_display_buffer_4X7(18, (((-FlashConfig.Timezone % 10) + '0') & FlagBlinking[SETUP_UTC]));
    }
  }




  /* ----------------------- Setup Keyclick ------------------------- */
  if (SetupStep == SETUP_KEYCLICK)
  {
    /* Setup "Keyclick sound". */
    FlagSetupClock[SETUP_KEYCLICK] = FLAG_ON;

    /* Setup "keyclick" ON / OFF". */
    FlagTone = FLAG_ON;

    fill_display_buffer_4X7(0, 'B');
    fill_display_buffer_4X7(5, 'P');
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(12, ('0' & FlagBlinking[SETUP_KEYCLICK]));

    if (FlashConfig.FlagKeyclick == FLAG_ON)
    {
      fill_display_buffer_4X7(17, ('N' & FlagBlinking[SETUP_KEYCLICK]));
    }
    else
    {
      fill_display_buffer_4X7(17, ('F' & FlagBlinking[SETUP_KEYCLICK]));
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ----------------------- Setup Scrolling ------------------------ */
  if (SetupStep == SETUP_SCROLLING)
  {
    /* Setup "Display Scrolling mode ON / OFF". */
    FlagSetupClock[SETUP_SCROLLING] = FLAG_ON; // indicate we are setting up display scroll.

    fill_display_buffer_4X7(0, 'D');
    fill_display_buffer_4X7(5, 'S');
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(12, ('0' & FlagBlinking[SETUP_SCROLLING]));

    if (FlashConfig.FlagScrollEnable == FLAG_ON)
    {
      fill_display_buffer_4X7(17, ('N' & FlagBlinking[SETUP_SCROLLING]));
    }
    else
    {
      fill_display_buffer_4X7(17, ('F' & FlagBlinking[SETUP_SCROLLING]));
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ----------------------- Temperature unit ----------------------- */
  if (SetupStep == SETUP_TEMP_UNIT)
  {
    /* Setup Temperature unit (Celsius or Fahrenheit. */
    FlagSetupClock[SETUP_TEMP_UNIT] = FLAG_ON; // indicate we are setting up temperature unit.

    clear_framebuffer(0);

    fill_display_buffer_5X7(2, 'T');
    fill_display_buffer_5X7(8, 'U');
    fill_display_buffer_4X7(12, 0x3A); // slim ":"

    if (FlashConfig.TemperatureUnit == CELSIUS)
    {
      fill_display_buffer_5X7(17, ('C' & FlagBlinking[SETUP_TEMP_UNIT]));
    }
    else
    {
      fill_display_buffer_5X7(17, ('F' & FlagBlinking[SETUP_TEMP_UNIT]));
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ------------------------ Setup Language ------------------------ */
  if (SetupStep == SETUP_LANGUAGE)
  {
    /* Setup Language (for some clock functions). */
    FlagSetupClock[SETUP_LANGUAGE] = FLAG_ON;

    clear_framebuffer(0);
    fill_display_buffer_5X7(2, 'L');
    fill_display_buffer_5X7(7, 'G');
    /// fill_display_buffer_4X7(11, ':');

    switch (FlashConfig.Language)
    {
      default:
      case (CZECH):
        /// fill_display_buffer_5X7(15, ('C' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(13, ('C' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(19, ('Z' & FlagBlinking[SETUP_LANGUAGE]));
       break;

      case (ENGLISH):
        /// fill_display_buffer_5X7(15, ('E' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(13, ('E' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(19, ('N' & FlagBlinking[SETUP_LANGUAGE]));
       break;

      case (FRENCH):
        /// fill_display_buffer_5X7(15, ('F' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(13, ('F' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(19, ('R' & FlagBlinking[SETUP_LANGUAGE]));
      break;

      case (GERMAN):
        /// fill_display_buffer_5X7(15, ('G' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(13, ('G' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(19, ('E' & FlagBlinking[SETUP_LANGUAGE]));
      break;

      case (SPANISH):
        fill_display_buffer_5X7(13, ('S' & FlagBlinking[SETUP_LANGUAGE]));
        fill_display_buffer_5X7(19, ('P' & FlagBlinking[SETUP_LANGUAGE]));
      break;
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ------------------- Setup Time display mode -------------------- */
  if (SetupStep == SETUP_TIME_FORMAT)
  {
    /* Setup "Hour Display Mode" 24-hours / 12-hours toggling. */
    if (FlagSetupClock[SETUP_TIME_FORMAT] == FLAG_OFF)
    {
      CurrentHour   = bcd_to_byte(Time_RTC.hour);
      CurrentMinute = bcd_to_byte(Time_RTC.minutes);
    }

    FlagSetupClock[SETUP_TIME_FORMAT] = FLAG_ON;

    fill_display_buffer_4X7(0, 'H');
    fill_display_buffer_4X7(5, 'D');
    fill_display_buffer_4X7(10, ':');
    if (FlashConfig.TimeDisplayMode == H24)
    {
      /* "24" for 24-hours display mode. */
      fill_display_buffer_4X7(12, '2' & FlagBlinking[SETUP_TIME_FORMAT]);
      fill_display_buffer_4X7(17, '4' & FlagBlinking[SETUP_TIME_FORMAT]);
    }
    else
    {
      /* "12" for 12-hours display mode. */
      fill_display_buffer_4X7(12, '1' & FlagBlinking[SETUP_TIME_FORMAT]);
      fill_display_buffer_4X7(17, '2' & FlagBlinking[SETUP_TIME_FORMAT]);
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(23);
  }



  /* -------------------- Setup Hourly chime ------------------------ */
  if (SetupStep == SETUP_HOURLY_CHIME)
  {
    /* Setup "Hourly chime" (OFF / ON / Day). */
    FlagSetupClock[SETUP_HOURLY_CHIME] = FLAG_ON;

    fill_display_buffer_4X7(0, 'H');
    fill_display_buffer_4X7(5, 'C');
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(12, '0' & FlagBlinking[SETUP_HOURLY_CHIME]);

    if (FlashConfig.ChimeMode == CHIME_OFF)
    {
      /* 0F for "OFF". */
      fill_display_buffer_4X7(17, 'F' & FlagBlinking[SETUP_HOURLY_CHIME]);
      IndicatorHourlyChimeOff;
    }
    else if (FlashConfig.ChimeMode == CHIME_ON)
    {
      /* 0N for "ON". */
      fill_display_buffer_4X7(17, 'N' & FlagBlinking[SETUP_HOURLY_CHIME]);
      IndicatorHourlyChimeOn;
    }
    else if (FlashConfig.ChimeMode == CHIME_DAY)
    {
      /* 0I for "On, Intermittent" (during daytime only). */
      fill_display_buffer_4X7(18, 'I' & FlagBlinking[SETUP_HOURLY_CHIME]);
      IndicatorHourlyChimeDay;
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* --------------------- Setup Chime time on ---------------------- */
  if (SetupStep == SETUP_CHIME_TIME_ON)
  {
    /* Setup Hourly chime time on (it is 9h00 by default). */
    FlagSetupClock[SETUP_CHIME_TIME_ON] = FLAG_ON;

    fill_display_buffer_4X7(0, 'O');
    fill_display_buffer_4X7(5, 'N');
    fill_display_buffer_4X7(10, ':');

    /* Check if we are in "24-hours" display mode or "12-hours" display mode.
       NOTE: Real-time clock IC keeps track of the clock in 24-hours format. */
    if (FlashConfig.TimeDisplayMode == H12)
    {
      ChimeTimeOnDisplay = convert_h24_to_h12(FlashConfig.ChimeTimeOn, &AmFlag, &PmFlag);
      (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
      (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
    }
    else
    {
      /* We are in "24-hours" display mode. */
      ChimeTimeOnDisplay = FlashConfig.ChimeTimeOn;
    }
    fill_display_buffer_4X7(12, (0x30 + (ChimeTimeOnDisplay / 10)) & FlagBlinking[SETUP_CHIME_TIME_ON]);
    fill_display_buffer_4X7(17, (0x30 + (ChimeTimeOnDisplay % 10)) & FlagBlinking[SETUP_CHIME_TIME_ON]);

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* -------------------- Setup Chime time off ---------------------- */
  if (SetupStep == SETUP_CHIME_TIME_OFF)
  {
    /* Setup Hourly chime time off (it is 22h00 by default). */
    FlagSetupClock[SETUP_CHIME_TIME_OFF] = FLAG_ON;

    fill_display_buffer_4X7(0, 'O');
    fill_display_buffer_4X7(5, 'F');
    fill_display_buffer_4X7(10, ':');

    /* Check if we are in "24-hours" display mode or "12-hours" display mode. */
    if (FlashConfig.TimeDisplayMode == H12)
    {
      ChimeTimeOffDisplay = convert_h24_to_h12(FlashConfig.ChimeTimeOff, &AmFlag, &PmFlag);
      (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
      (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
    }
    else
    {
      /* We are in "24-hours" display mode. */
      ChimeTimeOffDisplay = FlashConfig.ChimeTimeOff;
    }
    fill_display_buffer_4X7(12, (0x30 + (ChimeTimeOffDisplay / 10)) & FlagBlinking[SETUP_CHIME_TIME_OFF]);
    fill_display_buffer_4X7(17, (0x30 + (ChimeTimeOffDisplay % 10)) & FlagBlinking[SETUP_CHIME_TIME_OFF]);

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ---------------------- Setup Night light ----------------------- */
  if (SetupStep == SETUP_NIGHT_LIGHT)
  {
    /* Setup "Night light" (OFF / ON / Night). */
    FlagSetupClock[SETUP_NIGHT_LIGHT] = FLAG_ON;

    clear_framebuffer(0);

    fill_display_buffer_5X7(2, 'N');
    fill_display_buffer_5X7(8, 'L');
    fill_display_buffer_4X7(11, 0x3A); // slim ":"

    if (FlashConfig.NightLightMode == NIGHT_LIGHT_OFF)
    {
      /* OF for "OFF". */
      fill_display_buffer_4X7(13, 'O' & FlagBlinking[SETUP_NIGHT_LIGHT]);
      fill_display_buffer_4X7(18, 'F' & FlagBlinking[SETUP_NIGHT_LIGHT]);
      IndicatorButtonLightsOff;
    }
    else if (FlashConfig.NightLightMode == NIGHT_LIGHT_ON)
    {
      /* ON for "ON". */
      fill_display_buffer_4X7(13, 'O' & FlagBlinking[SETUP_NIGHT_LIGHT]);
      fill_display_buffer_4X7(18, 'N' & FlagBlinking[SETUP_NIGHT_LIGHT]);
      IndicatorButtonLightsOn;
    }
    else if (FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT)
    {
      /* OI for "On, Intermittent" (that is: during nighttime only). */
      fill_display_buffer_4X7(13, 'O' & FlagBlinking[SETUP_NIGHT_LIGHT]);
      fill_display_buffer_4X7(18, 'I' & FlagBlinking[SETUP_NIGHT_LIGHT]);
    }
    else if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
    {
      /* AU for "automatic" (that is: when ambient light is low enough). */
      fill_display_buffer_4X7(13, 'A' & FlagBlinking[SETUP_NIGHT_LIGHT]);
      fill_display_buffer_4X7(18, 'U' & FlagBlinking[SETUP_NIGHT_LIGHT]);
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ------------------ Setup Night light time on ------------------- */
  if (SetupStep == SETUP_NIGHT_LIGHT_TIME_ON)
  {
    /* Setup Night light time on (it is 21h00 by default). */
    FlagSetupClock[SETUP_NIGHT_LIGHT_TIME_ON] = FLAG_ON;

    fill_display_buffer_4X7(0, 'O');
    fill_display_buffer_4X7(5, 'N');
    fill_display_buffer_4X7(10, ':');

    /* Check if we are in "24-hours" display mode or "12-hours" display mode. */
    if (FlashConfig.TimeDisplayMode == H12)
    {
      NightLightTimeOnDisplay = convert_h24_to_h12(FlashConfig.NightLightTimeOn, &AmFlag, &PmFlag);
      (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
      (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
    }
    else
    {
      /* We are in "24-hours" display mode. */
      NightLightTimeOnDisplay = FlashConfig.NightLightTimeOn;
    }
    fill_display_buffer_4X7(12, (0x30 + (NightLightTimeOnDisplay / 10)) & FlagBlinking[SETUP_NIGHT_LIGHT_TIME_ON]);
    fill_display_buffer_4X7(17, (0x30 + (NightLightTimeOnDisplay % 10)) & FlagBlinking[SETUP_NIGHT_LIGHT_TIME_ON]);

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ----------------- Setup Night Light time off ------------------- */
  if (SetupStep == SETUP_NIGHT_LIGHT_TIME_OFF)
  {
    /* Setup Night light time off (it is 8h00 by default). */
    FlagSetupClock[SETUP_NIGHT_LIGHT_TIME_OFF] = FLAG_ON;

    fill_display_buffer_4X7(0, 'O');
    fill_display_buffer_4X7(5, 'F');
    fill_display_buffer_4X7(10, ':');

    /* Check if we are in "24-hours" display mode or "12-hours" display mode. */
    if (FlashConfig.TimeDisplayMode == H12)
    {
      NightLightTimeOffDisplay = convert_h24_to_h12(FlashConfig.NightLightTimeOff, &AmFlag, &PmFlag);
      (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
      (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
    }
    else
    {
      /* We are in "24-hours" display mode. */
      NightLightTimeOffDisplay = FlashConfig.NightLightTimeOff;
    }
    fill_display_buffer_4X7(12, (0x30 + (NightLightTimeOffDisplay / 10)) & FlagBlinking[SETUP_NIGHT_LIGHT_TIME_OFF]);
    fill_display_buffer_4X7(17, (0x30 + (NightLightTimeOffDisplay % 10)) & FlagBlinking[SETUP_NIGHT_LIGHT_TIME_OFF]);

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* -------------------- Setup Auto brightness --------------------- */
  if (SetupStep == SETUP_AUTO_BRIGHT)
  {
    /* Setup clock display "Auto brightness" On / Off. */
    FlagSetupClock[SETUP_AUTO_BRIGHT] = FLAG_ON;

    clear_framebuffer(0);

    fill_display_buffer_5X7(2, 'D');
    fill_display_buffer_5X7(8, 'I');
    fill_display_buffer_5X7(12, 'M');

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }

  if (SetupStep >= SETUP_CLOCK_HI_LIMIT)
  {
    IdleNumberOfSeconds = 0;                  // reset the number of seconds the system has been idle.
    SetupSource         = SETUP_SOURCE_NONE;  // reset setup source when done.
    SetupStep           = SETUP_NONE;         // reset SetupStep when done.
    FlagIdleCheck       = FLAG_OFF;           // getting out of setup mode, no more watching for potential time-out.
    FlagSetClock        = FLAG_OFF;           // getting out of clock setup mode.
    FlagTone            = FLAG_OFF;           // reset flag tone.
    FlagUpdateTime      = FLAG_ON;            // it's now time to update time display on the clock.

    /* Reset all clock setup members before exiting. */
    for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_CLOCK_HI_LIMIT; ++Loop1UInt8)
      FlagSetupClock[Loop1UInt8] = FLAG_OFF;
  }

  return;
}





/* $PAGE */
/* $TITLE=setup_clock_variables() */
/* ------------------------------------------------------------------ *\
                   Change current clock parameters.
\* ------------------------------------------------------------------ */
void setup_clock_variables(UINT8 FlagButtonSelect)
{
  UCHAR String[256];

  UINT8 AmFlag;
  UINT8 PmFlag;
  UINT8 Dum1UInt8; // dummy variable.
  UINT8 Loop1UInt8;
  // clock works in 24 hr values, but convert to 12 hr or 24 hr for display
  UINT8 CurrentHourDisplay;


  /* ---------------------- Hour setting ---------------------------- */
  /* Check if we are setting up the "Hour". */
  if (FlagSetupClock[SETUP_HOUR] == FLAG_ON)
  {
    /* We are setting up the "Hour". */
    FlagSetupRTC = FLAG_ON; // indicate that we must program the RTC IC.

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Hour" setup mode. */
      ++CurrentHourSetting;

      /* If we pass 23 hours (out-of-bound), roll-back to 0. */
      if (CurrentHourSetting == 24) CurrentHourSetting = 0; // if out-of-bound, roll-back to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Hour" setup mode. */
      --CurrentHourSetting;

      /* If we go below zero (out-of-bound), roll-back to 23. */
      if (CurrentHourSetting == 255) CurrentHourSetting = 23;
    }
    CurrentHour = CurrentHourSetting;
  }



  /* ---------------------- Minute setting -------------------------- */
  /* Check if we are setting up the "Minute". */
  if (FlagSetupClock[SETUP_MINUTE] == FLAG_ON)
  {
    /* We are setting up the "Minute". */
    FlagSetupRTC = FLAG_ON; // indicate we must program the RTC IC.

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Minute" setup mode. */
      ++CurrentMinuteSetting;

      /* If we pass 59 minutes (out-of-bound), roll-back to 0. */
      if (CurrentMinuteSetting == 60) CurrentMinuteSetting = 0;
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Minute" setup mode. */
      --CurrentMinuteSetting;

      /* If we go below zero (out-of-bound), set minutes to 59. */
      if (CurrentMinuteSetting == 255) CurrentMinuteSetting = 59;
    }
    CurrentMinute = CurrentMinuteSetting;
  }



  /* ------------------------- Month setting ------------------------ */
  if (FlagSetupClock[SETUP_MONTH] == FLAG_ON)
  {
    /* We are setting up the "Month". */
    FlagSetupRTC = FLAG_ON; // indicate we must program the RTC IC.

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Month" setup mode. */
      ++CurrentMonth;

      /* If we pass 12 (out-of-bound), roll-back to 1. */
      if (CurrentMonth == 13)
        CurrentMonth = 1;
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Month" setup mode. */
      --CurrentMonth;

      /* If we go below 1 (out-of-bound), set month to 12. */
      if (CurrentMonth == 0)
        CurrentMonth = 12;
    }

    /* If we changed month, make sure day-of-month is compatible. First, make
       sure CurrentYear is correct before determining DayOfMonth (for leap years). */
    CurrentYear = (FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart;

    if (CurrentDayOfMonth > get_month_days(CurrentYear, CurrentMonth))
    {
      /* If we are over this month's maximum day, revert to last day of this month. */
      CurrentDayOfMonth = get_month_days(CurrentYear, CurrentMonth);
    }
  }



  /* ------------------- Day-of-month setting ----------------------- */
  if (FlagSetupClock[SETUP_DAY_OF_MONTH] == FLAG_ON)
  {
    /* We are setting up day-of-month. */
    FlagSetupRTC = FLAG_ON; // indicate we must program the RTC IC.

    /* Make sure CurrentYear is correct before determining maximum day-of-month (for leap years). */
    CurrentYear = (FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart;

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "day-of-month" setup mode. */
      ++CurrentDayOfMonth;

      /* If we pass the maximum day-of-month for current month (out-of-bound), roll-back to 1. */
      if (CurrentDayOfMonth > get_month_days(CurrentYear, CurrentMonth))
      {
        CurrentDayOfMonth = 1;
      }
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "DayOfMonth" setup mode. */
      --CurrentDayOfMonth;

      /* If we go below 1 (out-of-bound), set day-of-month to the maximum for current month. */
      if (CurrentDayOfMonth == 0)
      {
        CurrentDayOfMonth = get_month_days(CurrentYear, CurrentMonth);
      }
    }
  }



  /* ------------------------ Year setting -------------------------- */
  if (FlagSetupClock[SETUP_YEAR] == FLAG_ON)
  {
    /* We are setting up the "Year". */
    FlagSetupRTC = FLAG_ON; // indicate we must program the RTC IC.

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "up" (middle) button while in "year" setup mode. */
      ++CurrentYearLowPart;

      /* If we exceed the two digits maximum range ("99"). */
      if (CurrentYearLowPart == 100)
      {
        /* Roll-back to 0 for decimal last two digits... */
        CurrentYearLowPart = 0;

        /* ...and add one more to the centiles. */
        ++FlashConfig.CurrentYearCentile;
      }
    }
    else
    {
      /* User pressed the "down" (bottom) button while in "year" setup mode. */
      --CurrentYearLowPart;

      /* If we go below zero for the last two digits of the year. */
      if (CurrentYearLowPart == 255)
      {
        /* Set last two digits to 99... */
        CurrentYearLowPart = 99;

        /* ...and substract one from the centiles. */
        --FlashConfig.CurrentYearCentile;
      }
    }
  }



  /* ----------------- Daylight Saving Time setting ----------------- */
  if (FlagSetupClock[SETUP_DST] == FLAG_ON)
  {
    /* If we are in setup mode, in "Daylight Saving Time" setup step,
       go through different DST options. */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++FlashConfig.DSTCountry;
      if (FlashConfig.DSTCountry >= DST_HI_LIMIT)
        FlashConfig.DSTCountry = DST_NONE;
    }
    else
    {
      /* Must be the "Down"  button. */
      --FlashConfig.DSTCountry;
      if (FlashConfig.DSTCountry == 255)
        FlashConfig.DSTCountry = DST_HI_LIMIT - 1;
    }
  }



  /* ---------------------- Timezone setting ---------------------- */
  if (FlagSetupClock[SETUP_UTC] == FLAG_ON)
  {
    /* If we are in setup mode, in "Timezone" setup step, get Timezone value. */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++FlashConfig.Timezone;
      if (FlashConfig.Timezone > 14) FlashConfig.Timezone = -12;
    }
    else
    {
      /* Must be "Button down". */
      --FlashConfig.Timezone;
      if (FlashConfig.Timezone < -12) FlashConfig.Timezone = 14;
    }
  }



  /* ----------------------- Keyclick setting ----------------------- */
  /* Check if we are in "Keyclick" setup step. */
  if (FlagSetupClock[SETUP_KEYCLICK] == FLAG_ON)
  {
    if (FlagTone == FLAG_ON)
    {
      /* If we are in setup mode, in "Keyclick" setup step, toggle the flag. */
      if (FlashConfig.FlagKeyclick == FLAG_ON)
        FlashConfig.FlagKeyclick = FLAG_OFF;
      else
        FlashConfig.FlagKeyclick = FLAG_ON;
    }

    /* Check if we should make a "keyclick tone".
       It's usually done in the timer callback routine, but since we just changed the setting,
       it's a good idea to provide an immediate feedback. */
    if (FlashConfig.FlagKeyclick == FLAG_ON)
    {
      for (Loop1UInt8 = 0; Loop1UInt8 < TONE_KEYCLICK_REPEAT2; ++Loop1UInt8)
      {
        sound_queue_active(TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1);
      }
      sound_queue_active(50, SILENT);
    }
  }



  /* ------------------- Temperature unit setting ------------------- */
  /* Check if we are in "Celsius or Fahrenheit" setup step. */
  if (FlagSetupClock[SETUP_TEMP_UNIT] == FLAG_ON)
  {
    /* If we are in setup mode, in "Keyclick" setup step, toggle the flag. */
    FlashConfig.TemperatureUnit = !FlashConfig.TemperatureUnit; // toggle temperature unit.
    if (FlashConfig.TemperatureUnit == CELSIUS)
    {
      IndicatorCelsiusOn;
      IndicatorFrnhtOff;
    }
    else
    {
      IndicatorFrnhtOn;
      IndicatorCelsiusOff;
    }
  }



  /* ------------------------ Scroll setting ------------------------ */
  if (FlagSetupClock[SETUP_SCROLLING] == FLAG_ON)
  {
    /* If we are in setup mode, in "Scroll" setup step, toggle the "scroll enable" flag On / Off. */
    if (FlashConfig.FlagScrollEnable == FLAG_OFF)
    {
      FlashConfig.FlagScrollEnable = FLAG_ON;
      IndicatorScrollOn;
    }
    else
    {
      FlashConfig.FlagScrollEnable = FLAG_OFF;
      IndicatorScrollOff;
    }
  }



  /* ---------------------- Language setting ------------------------ */
  if (FlagSetupClock[SETUP_LANGUAGE] == FLAG_ON)
  {
    /* If we are in setup mode, in "Language" setup step,
       go through different language options (French / English / German ...) */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++FlashConfig.Language;
      if (FlashConfig.Language >= LANGUAGE_HI_LIMIT)
        FlashConfig.Language = LANGUAGE_LO_LIMIT + 1;
    }
    else
    {
      /* Must be "Button down". */
      --FlashConfig.Language;
      if (FlashConfig.Language <= LANGUAGE_LO_LIMIT)
        FlashConfig.Language = LANGUAGE_HI_LIMIT - 1;
    }

     /* Setup order in some languages is day-month(-year), as opposed to English month-day(-year). */
    if ((FlashConfig.Language == CZECH) || (FlashConfig.Language == FRENCH) || (FlashConfig.Language == GERMAN) || (FlashConfig.Language == SPANISH))
    {
      SETUP_DAY_OF_MONTH = 0x03;
      SETUP_MONTH = 0x04;
    }

    if (FlashConfig.Language == ENGLISH)
    {
      SETUP_MONTH = 0x03;
      SETUP_DAY_OF_MONTH = 0x04;
    }
  }



  /* --------------------- Time format setting ---------------------- */
  /* Select time display format (12-hours or 24-hours). */
  if (FlagSetupClock[SETUP_TIME_FORMAT] == FLAG_ON)
  {
    if (FlashConfig.TimeDisplayMode == H12)
    {
      /* Toggle from 12-hours to 24-hours display mode. */
      FlashConfig.TimeDisplayMode = H24;
      IndicatorAmOff;
      IndicatorPmOff;
    }
    else
    {
      /* Change from 24-hours to 12-hours display mode. */
      FlashConfig.TimeDisplayMode = H12;

      /* Adjust current hour and AM / PM indicators accordingly. */
      if (FlashConfig.TimeDisplayMode == H12)
      {
        CurrentHourDisplay = convert_h24_to_h12(CurrentHour, &AmFlag, &PmFlag);
        (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
        (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
      }
    }
  }



  /* --------------------- Hourly chime setting --------------------- */
  if (FlagSetupClock[SETUP_HOURLY_CHIME] == FLAG_ON)
  {
    /* If we are in setup mode, in "Hourly chime" setup step, go through different status (OFF / ON / DAY). */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++FlashConfig.ChimeMode;
      if (FlashConfig.ChimeMode > CHIME_DAY)
        FlashConfig.ChimeMode = CHIME_OFF;
    }

    if (FlagButtonSelect == FLAG_DOWN)
    {
      --FlashConfig.ChimeMode;
      if (FlashConfig.ChimeMode == 255)
        FlashConfig.ChimeMode = CHIME_DAY;
    }
  }



  /* ------------------- Chime time on setting ---------------------- */
  /* Check if we are setting up the Chime time on */
  if (FlagSetupClock[SETUP_CHIME_TIME_ON] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in Chime time on setup mode. */
      ++FlashConfig.ChimeTimeOn;
      if (FlashConfig.ChimeTimeOn == 24)
        FlashConfig.ChimeTimeOn = 0; // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in Chime time on setup mode. */
      --FlashConfig.ChimeTimeOn;
      if (FlashConfig.ChimeTimeOn == 255)
        FlashConfig.ChimeTimeOn = 23; // if out-of-bound, revert to 23.
    }
  }



  /* -------------------- Chime time off setting -------------------- */
  /* Check if we are setting up the Chime time off. */
  if (FlagSetupClock[SETUP_CHIME_TIME_OFF] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in Chime time off setup mode. */
      ++FlashConfig.ChimeTimeOff;
      if (FlashConfig.ChimeTimeOff == 24)
        FlashConfig.ChimeTimeOff = 0; // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in Chime time off setup mode. */
      --FlashConfig.ChimeTimeOff;
      if (FlashConfig.ChimeTimeOff == 255)
        FlashConfig.ChimeTimeOff = 23; // if out-of-bound, revert to 23.
    }
  }



  /* --------------------- Night light setting ---------------------- */
  if (FlagSetupClock[SETUP_NIGHT_LIGHT] == FLAG_ON)
  {
    /* If we are in setup mode, in "Night light" setup step, go through different status (OFF / ON / NIGHT). */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++FlashConfig.NightLightMode;
      if (FlashConfig.NightLightMode > NIGHT_LIGHT_AUTO)
        FlashConfig.NightLightMode = NIGHT_LIGHT_OFF;
    }

    if (FlagButtonSelect == FLAG_DOWN)
    {
      --FlashConfig.NightLightMode;
      if (FlashConfig.NightLightMode == 255)
        FlashConfig.NightLightMode = NIGHT_LIGHT_AUTO;
    }
  }



  /* ----------------- Night light time on setting ------------------ */
  /* Check if we are setting up the Night light time on */
  if (FlagSetupClock[SETUP_NIGHT_LIGHT_TIME_ON] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in Night light time on setup mode. */
      ++FlashConfig.NightLightTimeOn;
      if (FlashConfig.NightLightTimeOn == 24)
        FlashConfig.NightLightTimeOn = 0; // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in Night light time on setup mode. */
      --FlashConfig.NightLightTimeOn;
      if (FlashConfig.NightLightTimeOn == 255)
        FlashConfig.NightLightTimeOn = 23; // if out-of-bound, revert to 23.
    }
  }



  /* ----------------- Night light time off setting ----------------- */
  /* Check if we are setting up the Night light time off. */
  if (FlagSetupClock[SETUP_NIGHT_LIGHT_TIME_OFF] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in Night light time off setup mode. */
      ++FlashConfig.NightLightTimeOff;
      if (FlashConfig.NightLightTimeOff == 24)
        FlashConfig.NightLightTimeOff = 0; // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in Night light time off setup mode. */
      --FlashConfig.NightLightTimeOff;
      if (FlashConfig.NightLightTimeOff == 255)
        FlashConfig.NightLightTimeOff = 23; // if out-of-bound, revert to 23.
    }
  }



  /* ------------------------ Auto brightness ----------------------- */
  if (FlagSetupClock[SETUP_AUTO_BRIGHT] == FLAG_ON)
  {
    clear_framebuffer(0);

    /* If we are in setup mode, in "Auto-brightness" setup step, toggle the "auto-brightness" On / Off. */
    if (FlashConfig.FlagAutoBrightness == FLAG_ON)
    {
      FlashConfig.FlagAutoBrightness = FLAG_OFF;
      IndicatorAutoLightOff;
    }
    else
    {
      FlashConfig.FlagAutoBrightness = FLAG_ON;
      IndicatorAutoLightOn;
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=setup_timer_frame() */
/* ------------------------------------------------------------------ *\
                  Display current timer parameters.
\* ------------------------------------------------------------------ */
void setup_timer_frame(void)
{
  UCHAR String[128];

  UINT8 Loop1UInt8;


  CurrentClockMode = MODE_TIMER_SETUP;

  if (SetupStep == SETUP_TIMER_UP_DOWN)
  {
    /* Clear clock framebuffer on entry. */
    clear_framebuffer(0);

    FlagSetupTimer[SETUP_TIMER_UP_DOWN] = FLAG_ON;
    fill_display_buffer_5X7(2, 'T');
    fill_display_buffer_5X7(8, 'M');

    if (TimerMode == TIMER_COUNT_UP)
    {
      /* Timer is currently in "Count Up" mode. */
      fill_display_buffer_4X7(13, 'U' & FlagBlinking[1]);
      fill_display_buffer_4X7(18, 'P' & FlagBlinking[1]);
      IndicatorCountUpOn;
      IndicatorCountDownOff;
    }
    else if (TimerMode == TIMER_COUNT_DOWN)
    {
      /* Timer is currently in "Count Down" mode. */
      fill_display_buffer_4X7(13, 'D' & FlagBlinking[1]);
      fill_display_buffer_4X7(18, 'N' & FlagBlinking[1]);
      IndicatorCountDownOn;
      IndicatorCountUpOff;
    }
    else if (TimerMode == TIMER_OFF)
    {
      /* Timer is currently OFF (no Count up, no Count down). */
      fill_display_buffer_4X7(13, '0' & FlagBlinking[1]);
      fill_display_buffer_4X7(18, 'F' & FlagBlinking[1]);
      IndicatorCountDownOff;
      IndicatorCountUpOff;
    }

    /* Clear the clock framebuffer when done. */
    clear_framebuffer(26);
  }
  else if ((SetupStep == SETUP_TIMER_MINUTE) || (SetupStep == SETUP_TIMER_SECOND) && (TimerMode != TIMER_OFF))
  {
    if ((SetupStep == SETUP_TIMER_MINUTE) && (TimerMode == TIMER_COUNT_DOWN))
    {
      /* Timer is currently in "Count Down" mode. */
      FlagSetupTimer[SETUP_TIMER_MINUTE] = FLAG_ON;
    }
    else if ((SetupStep == SETUP_TIMER_MINUTE) && (TimerMode == TIMER_COUNT_UP))
    {
      /* Timer is currently in "Count Up" mode. We assume that a "count-up" timer will always start from zero. */
      if (FlagSetTimerCountUp == FLAG_OFF)
      {
        TimerMinutes = 0;
        TimerSeconds = 0;
      }

      IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
      FlagSetTimerCountUp = FLAG_ON;
      fill_display_buffer_4X7(0, TimerMinutes / 10 + '0');
      fill_display_buffer_4X7(5, TimerMinutes % 10 + '0');
      fill_display_buffer_4X7(10, ':');
      fill_display_buffer_4X7(12, TimerShowSecond / 10 + '0');
      fill_display_buffer_4X7(17, TimerShowSecond % 10 + '0');

      /* Clear the clock framebuffer when done. */
      clear_framebuffer(26);
    }

    if ((SetupStep == SETUP_TIMER_SECOND) && (TimerMode == TIMER_COUNT_DOWN))
    {
      /* Timer is currently in "Count Down" mode. */
      FlagSetupTimer[SETUP_TIMER_SECOND] = FLAG_ON;
      FlagTimerCountDownEnd = FLAG_OFF;

      if (DebugBitMask & DEBUG_TIMER)
        uart_send(__LINE__, "FlagTimerCountDownEnd has been reset.\r");
    }

    if (TimerMode == TIMER_COUNT_DOWN)
    {
      /* Timer is currently in "Count Down" mode. */
      fill_display_buffer_4X7(0, TimerMinutes / 10 + '0' & FlagBlinking[2]);
      fill_display_buffer_4X7(5, TimerMinutes % 10 + '0' & FlagBlinking[2]);
      fill_display_buffer_4X7(10, ':');
      fill_display_buffer_4X7(12, TimerSeconds / 10 + '0' & FlagBlinking[3]);
      fill_display_buffer_4X7(17, TimerSeconds % 10 + '0' & FlagBlinking[3]);

      /* Clear the clock framebuffer when done. */
      clear_framebuffer(26);
      TimerShowSecond = TimerSeconds;
    }

    if ((TimerMode == TIMER_COUNT_UP) && (SetupStep == SETUP_TIMER_SECOND))
    {
      /* Timer is currently in "Count Up" mode. */
      fill_display_buffer_4X7(0, TimerMinutes / 10 + '0');
      fill_display_buffer_4X7(5, TimerMinutes % 10 + '0');
      fill_display_buffer_4X7(10, ':');
      fill_display_buffer_4X7(12, TimerSeconds / 10 + '0');
      fill_display_buffer_4X7(17, TimerSeconds % 10 + '0');

      /* Clear the clock framebuffer when done. */
      clear_framebuffer(26);
      TimerShowSecond = 0;
      TimerShowCount = 0;
      FlagSetTimerCountUp = FLAG_OFF;
    }
  }
  else if ((SetupStep == SETUP_TIMER_READY) && (TimerMode == TIMER_COUNT_DOWN) && (FlagTimerCountDownEnd == FLAG_OFF))
  {
    IdleNumberOfSeconds = 0;          // reset the number of seconds the system has been idle.
    FlagSetTimerCountDown = FLAG_ON;  // make count down timer active.
    fill_display_buffer_4X7(0, TimerMinutes / 10 + '0');
    fill_display_buffer_4X7(5, TimerMinutes % 10 + '0');
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(12, TimerShowSecond / 10 + '0');
    fill_display_buffer_4X7(17, TimerShowSecond % 10 + '0');

    /* Clear the clock framebuffer when done. */
    clear_framebuffer(26);
  }
  else
  {
    /* When reaching the end of settings (SETUP_TIMER_HI_LIMIT), proceed with cleanup. */
    if (TimerMode == TIMER_COUNT_UP)
    {
      /* Timer is currently in "Count Up" mode. */
      TimerMode = TIMER_OFF; // set timer OFF.
      TimerMinutes = 0;
      TimerSeconds = 0;
      IndicatorCountUpOff;
    }

    IdleNumberOfSeconds = 0;                  // reset the number of seconds the system has been idle.
    UpId                = 0;
    ScrollSecondCounter = 0;                  // reset number of seconds to reach time-to-scroll.
    SetupStep           = SETUP_NONE;         // reset SetupStep for next pass.
    SetupSource         = SETUP_SOURCE_NONE;  // exit timer setup mode.
    FlagSetTimer        = FLAG_OFF;           // reset "up" button.
    FlagIdleCheck       = FLAG_OFF;           // no more checking for an idle time-out.
    FlagTone            = FLAG_OFF;           // reset flag tone.
    FlagUpdateTime      = FLAG_ON;            // setup is over, it becomes time to display time.

    /* Reset all timer setup member flags. */
    for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_TIMER_HI_LIMIT; ++Loop1UInt8)
      FlagSetupTimer[Loop1UInt8] = FLAG_OFF;
  }

  return;
}





/* $PAGE */
/* $TITLE=setup_timer_variables() */
/* ------------------------------------------------------------------ *\
                  Change current timer parameters.
\* ------------------------------------------------------------------ */
void setup_timer_variables(UINT8 FlagButtonSelect)
{
  /* Select timer mode (Off / Count down / Count up). */
  if (FlagSetupTimer[SETUP_TIMER_UP_DOWN] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* We come here because user pressed the "Up" (middle) button while in "timer mode" setup. */
      ++TimerMode; // step upward through different timer modes (Off / Count down / Count up).
      if (TimerMode > TIMER_COUNT_UP)
        TimerMode = TIMER_OFF; // if reaching out-of-bound, revert to 0.
    }
    else
    {
      /* We come here because user pressed the "Down" (bottom) button while in "timer mode" setup. */
      --TimerMode; // step downward through different timer modes (Count up / Count down / Off).
      if (TimerMode == 255)
        TimerMode = TIMER_COUNT_UP; // if reaching out-of-bound, revert to 2.
    }
  }

  /* NOTE: There is no "Minute" or "Second" parameters adjustment for "Count up" timer.
           We assume that a "Count up" timer will logically always start counting from 00m00s. */

  /* Select timer "Minute" parameter. */
  if ((TimerMode == TIMER_COUNT_DOWN) && (FlagSetupTimer[SETUP_TIMER_MINUTE] == FLAG_ON))
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* We come here because user pressed the "Up" (middle) button while in timer "Minutes" parameter setup. */
      ++TimerMinutes;
      if (TimerMinutes == 60)
        TimerMinutes = 0; // if reaching out-of-bound, revert to 0.
    }
    else
    {
      /* We come here because user pressed the "Down" (bottom) button while in timer "Minutes" parameter setup.. */
      --TimerMinutes;
      if (TimerMinutes == 255)
        TimerMinutes = 59; // if reaching out-of-bound, revert to 59.
    }
  }

  /* Select timer "Second" parameter. */
  if ((TimerMode == TIMER_COUNT_DOWN) && (FlagSetupTimer[SETUP_TIMER_SECOND] == FLAG_ON))
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* We come here because user pressed the "Up" (middle) button while in timer "Seconds" parameter setup. */
      ++TimerSeconds;
      if (TimerSeconds == 60)
        TimerSeconds = 0; // if reaching out-of-bound, revert to 0.
    }
    else
    {
      /* We come here because user pressed the "Down" (bottom) button while in timer "Seconds" parameter setup. */
      --TimerSeconds;
      if (TimerSeconds == 255)
        TimerSeconds = 59; // if reaching out-of-bound, revert to 59.
    }
  }

  return;
}





/* $PAGE */
/* $TITLE=show_time() */
/* ------------------------------------------------------------------ *\
            Read the real-time clock IC and display time.
\* ------------------------------------------------------------------ */
void show_time(void)
{
  UCHAR String[128];
  char TimeBuffer[4];

  UINT8 AmFlag;
  UINT8 PmFlag;
  // clock works in 24 hr values, but convert to 12 hr or 24 hr for display
  UINT8 CurrentHourDisplay;


  /* Read the real-time clock IC. */
  Time_RTC = Read_RTC();

  CurrentHour        = bcd_to_byte(Time_RTC.hour);  // use CurrentHour until we convert to H12 or H24 if required.
  CurrentMinute      = bcd_to_byte(Time_RTC.minutes);
  CurrentSecond      = bcd_to_byte(Time_RTC.seconds);
  CurrentDayOfMonth  = bcd_to_byte(Time_RTC.dayofmonth);
  CurrentMonth       = bcd_to_byte(Time_RTC.month);
  CurrentYearLowPart = bcd_to_byte(Time_RTC.year);
  CurrentYear        = ((FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart);
  CurrentDayOfWeek   = get_day_of_week(CurrentYear, CurrentMonth, CurrentDayOfMonth);


  /* Check if we are in 12-hours or 24-hours time format. */
  if (FlashConfig.TimeDisplayMode == H12)
  {
    CurrentHourDisplay = convert_h24_to_h12(CurrentHour, &AmFlag, &PmFlag);
    (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
    (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
  }
  else
  {
    /* We are in "24-hours" display mode, nothing to do since RTC keep hours in 24-hours mode. */
    CurrentHourDisplay = CurrentHour;
  }


  /* When in 12-hour time display format, first digit is not displayed if it is zero. */
  if ((FlashConfig.TimeDisplayMode == H12) && (CurrentHourDisplay < 10))
    TimeBuffer[0] = (' ');  // hours first digit.
  else
    TimeBuffer[0] = ((CurrentHourDisplay / 10) + '0');     // hours first digit.

  TimeBuffer[1] = ((CurrentHourDisplay % 10) + '0');       // hours second digit.
  TimeBuffer[2] = ((Time_RTC.minutes / 16) + '0');  // minutes first digit.
  TimeBuffer[3] = ((Time_RTC.minutes % 16) + '0');  // minutes second digit.
  CurrentSecond = ((float)Time_RTC.seconds) / 1.5;


  /* Display "time of day" if we are not scrolling some data. */
  if (FlagScrollStart == FLAG_OFF)  // check to replace with ScrollDotCount.
  {
    CurrentClockMode = MODE_SHOW_TIME;

    fill_display_buffer_4X7(0, TimeBuffer[0]);
    fill_display_buffer_4X7(5, TimeBuffer[1]);
    fill_display_buffer_4X7(10, 0x3A); // slim ":"
    fill_display_buffer_4X7(12, TimeBuffer[2]);
    fill_display_buffer_4X7(17, TimeBuffer[3]);
  }


  /* Turn ON the weekday indicator. */
  update_top_indicators(ALL, FLAG_OFF);  // first, turn Off all days' indicators.
  update_top_indicators(CurrentDayOfWeek, FLAG_ON);

  return;
}





/* $PAGE */
/* $TITLE=sound_callback_ms() */
/* ------------------------------------------------------------------ *\
          Sound callback function (50 millisecond period).
        When called, will check if there are sounds to play.
           If not, wait for next call in 50 milliseconds.
         If there are sounds to play, manage sound timing.
\* ------------------------------------------------------------------ */
bool sound_callback_ms(struct repeating_timer *Timer50MSec)
{
  UCHAR String[128];

  static UINT16 ActiveMSeconds;
  static UINT16 ActiveMSecCounter;
  static UINT16 ActiveRepeatCount;
  static UINT16 CurrentRepeat;
  static UINT8  FlagActiveSound;
  static UINT8  FlagPassiveSound;
  static UINT16 Frequency;
  UINT16 Loop1UInt16;
  static UINT16 PassiveMSeconds;
  static UINT16 PassiveMSecCounter;

  UINT64 Timer1;
  UINT64 Timer2;


  Timer1 = time_us_64();


  /* ========================================================= *\
                     Handling of active buzzer
              (The one integrated in the Green Clock)
  \* ========================================================= */
  /* If there is a sound on-going on the active buzzer, check if it is completed. */
  if (FlagActiveSound == FLAG_ON)
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      if (ActiveRepeatCount == SILENT)
        sprintf(String, "- A-Silence     (%4u)\r", ActiveMSecCounter + 50);
      else
        sprintf(String, "- A-Sounding    (%4u)\r", ActiveMSecCounter + 50);

      uart_send(__LINE__, String);
    }

    ActiveMSecCounter += 50;  // 50 milliseconds more since last callback.

    if (ActiveMSecCounter >= ActiveMSeconds)
    {
      if (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- A-Shutoff\r");

      /* Current sound is over on active buzzer. Turn sound off to make a cut with next sound. */
      gpio_put(BUZZ, 0);

      /* One more repeat count has been done. */
      ++CurrentRepeat;

      /* Check if we reached the total repeat count required. */
      if (CurrentRepeat > ActiveRepeatCount) CurrentRepeat = 0;

      /* No active sound for now. */
      FlagActiveSound = FLAG_OFF;

      /* Reset sound elapsed time for now. */
      ActiveMSecCounter = 0;
    }
  }
  else
  {
    if (CurrentRepeat)
    {
      /* There are some more sound repeats to be done. */
      /* If RepeatCount is 0 ("SILENT"), we wanted to wait for specified duration without any sound. */
      if (ActiveRepeatCount != SILENT) gpio_put(BUZZ, 1);

      FlagActiveSound   = FLAG_ON;
      ActiveMSecCounter = 0;
    }
    else
    {
      /* Check if there are more sounds to play on active buzzer. Request duration and repeat count for next sound. */
      if (sound_unqueue_active(&ActiveMSeconds, &ActiveRepeatCount) == 0xFF)
      {
        /* Either there is no more sound, either there was an error while trying to unqueue next sound (corrupted sound queue).
        /* If sound_unqueue_active() is empty or unqueue failed, make sure audio is turned off. */
        gpio_put(BUZZ, 0);
      }
      else
      {
        if (DebugBitMask & DEBUG_SOUND_QUEUE)
          uart_send(__LINE__, "- A-Unqueued:            %5u   %5u\r", ActiveMSeconds, ActiveRepeatCount);

        /* If RepeatCount is 0 ("SILENT"), we wanted to wait for specified duration without any sound. */
        if (ActiveRepeatCount != SILENT) gpio_put(BUZZ, 1);
        FlagActiveSound   = FLAG_ON;
        CurrentRepeat     = 1;
        ActiveMSecCounter = 0;
      }
    }
  }



  #ifdef PASSIVE_PIEZO_SUPPORT
  /* ========================================================= *\
                    Handling of passive buzzer
            (If user installed one in the Green Clock)
  \* ========================================================= */
  if (FlagPassiveSound == FLAG_ON)
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      if (Frequency == SILENT)
        sprintf(String, "- P-Silence     (%4u)\r", PassiveMSecCounter + 50);
      else
        sprintf(String, "- P-Sounding    (%4u)\r", PassiveMSecCounter + 50);

      uart_send(__LINE__, String);
    }

    /* There is a sound on-going on the passive buzzer, check if it is completed. */
    PassiveMSecCounter += 50;  // 50 milliseconds more since last callback.

    if (PassiveMSecCounter >= PassiveMSeconds)
    {
      if (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- P-Shutoff\r");

      /* Current sound on passive buzzer is over. Turn sound Off to make a cut with next sound. */
      pwm_on_off(PWM_SOUND, FLAG_OFF);

      /* No active sound for now. */
      FlagPassiveSound = FLAG_OFF;

      /* Reset sound elapsed time for now. */
      PassiveMSecCounter = 0;
    }
  }
  else
  {
    /* If we asked passive sound queue to wait for active sound queue to complete and sound queue is still active, skip this callback cycle. */
    if (FlagPassiveSound == FLAG_WAIT)
    {
      /* Check if active sound queue is done for now. */
      if ((SoundActiveHead != SoundActiveTail) || (FlagActiveSound == FLAG_ON) || (CurrentRepeat != 0))
      {
        if (DebugBitMask & DEBUG_SOUND_QUEUE)
          uart_send(__LINE__, "- P-Waiting\r");

        /* Active sound queue not done yet. */
        return TRUE;
      }
      else
      {
        /* Active sound queue done for now, short pause before triggering passive buzzer. */
        FlagPassiveSound = FLAG_ON;
        PassiveMSeconds  = 400;  // 400 milliseconds pause between active buzzer and passive buzzer.

        return TRUE;
      }
    }


    /* Check if there are more sounds to play on passive buzzer. Request frequency and duration for next sound. */
    if (sound_unqueue_passive(&Frequency, &PassiveMSeconds) == 0xFF)
    {
      /* Either there is no more sound for passive buzzer, either there was an error while trying to unqueue next sound (corrupted sound queue). */
      /* If sound_unqueue_passive() failed, make sure audio PWM is turned off. */
      pwm_on_off(PWM_SOUND, FLAG_OFF);
      FlagPassiveSound = FLAG_OFF;
    }
    else
    {
      if (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- P-Unqueued:            %5u   %5u\r", Frequency, PassiveMSeconds);

      /* If frequency is 0 ("SILENT"), we wanted to wait for specified duration without any sound. */
      if (Frequency == SILENT)
      {
        if (PassiveMSeconds == WAIT_4_ACTIVE)
          FlagPassiveSound   = FLAG_WAIT;
        else
          FlagPassiveSound   = FLAG_ON;
      }
      else
      {
        /* Set new sound frequency... */
        pwm_set_frequency(Frequency);

        /* ...and turn On sound. */
        pwm_on_off(PWM_SOUND, FLAG_ON);
        FlagPassiveSound   = FLAG_ON;
      }
      PassiveMSecCounter = 0;
    }
  }
  #endif


  /***
  if (DebugBitMask & DEBUG_SOUND_QUEUE)
  {
    Timer2 = time_us_64();

    uart_send(__LINE__, "Sound callback: %5.2f msec\r", ((Timer2 - Timer1) / 1000.0));
  }
  ***/

  return TRUE;
}





/* $PAGE */
/* $TITLE=sound_queue_status_active() */
/* ------------------------------------------------------------------ *\
             Reset (clear) the active buzzer sound queue.
\* ------------------------------------------------------------------ */
void sound_queue_reset_active(void)
{
  SoundActiveHead = 0;
  SoundActiveTail = 0;
  return;
}





/* $PAGE */
/* $TITLE=sound_queue_status_active() */
/* ------------------------------------------------------------------ *\
        Return the status of the active buzzer sound queue.
\* ------------------------------------------------------------------ */
UINT8 sound_queue_status_active(void)
{
  /* Trap circular buffer corruption. */
  if ((SoundActiveHead > MAX_ACTIVE_SOUND_QUEUE) || (SoundActiveTail > MAX_ACTIVE_SOUND_QUEUE))
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- P-Corrupted:           %5u   %5u\r", SoundActiveHead, SoundActiveTail);

    SoundActiveHead = 0;
    SoundActiveTail = 0;
    return 0;
  }


  /* Check if the sound circular buffer (sound queue) is full. */
  if (((SoundActiveTail > 0) && (SoundActiveHead == (SoundActiveTail - 1))) || ((SoundActiveTail == 0) && (SoundActiveHead == (MAX_ACTIVE_SOUND_QUEUE - 1))))
  {
    /* Sound queue is full, return error code. */
    return FLAG_FULL;
  }

  if (SoundActiveHead != SoundActiveTail)
  {
    // Return FLAG_ON if buffer not empty
    return FLAG_ON;
  }
  else
  {
    return FLAG_OFF;
  }

}





/* $PAGE */
/* $TITLE=sound_queue_active() */
/* ------------------------------------------------------------------ *\
        Queue the given sound in the active buzzer sound queue.
            Use the queue algorithm where one slot is lost.
              This prevents the use of a lock mechanism.
\* ------------------------------------------------------------------ */
UINT16 sound_queue_active(UINT16 MSeconds, UINT16 RepeatCount)
{
  UCHAR String[256];


  /* Trap circular buffer corruption. */
  if ((SoundActiveHead > MAX_ACTIVE_SOUND_QUEUE) || (SoundActiveTail > MAX_ACTIVE_SOUND_QUEUE))
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- A-Corrupted:        %5u   %5u\r", SoundActiveHead, SoundActiveTail);

    SoundActiveHead = 0;
    SoundActiveTail = 0;

    return 0;
  }


  /* Check if the sound circular buffer (sound queue) is full. */
  if (((SoundActiveTail > 0) && (SoundActiveHead == SoundActiveTail - 1)) || ((SoundActiveTail == 0) && (SoundActiveHead == MAX_ACTIVE_SOUND_QUEUE - 1)))
  {
    /* Sound queue is full, return error code. */
    return MAX_ACTIVE_SOUND_QUEUE;
  }

  /* If there is at least one slot available in the queue, insert the sound to be played. */
  SoundQueueActive[SoundActiveHead].MSec        = MSeconds;
  SoundQueueActive[SoundActiveHead].RepeatCount = RepeatCount;
  ++SoundActiveHead;

  if (DebugBitMask & DEBUG_SOUND_QUEUE)
    uart_send(__LINE__, "- A-Queueing:            %5u   %5u\r", MSeconds, RepeatCount);


  /* Revert to zero when reaching out-of-bound. */
  if (SoundActiveHead >= MAX_ACTIVE_SOUND_QUEUE) SoundActiveHead = 0;

  /***
  if (DebugBitMask & DEBUG_SOUND_QUEUE)
  {
    if (SoundActiveHead > SoundActiveTail)
      uart_send(__LINE__, "Active queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_ACTIVE_SOUND_QUEUE - SoundActiveHead + SoundActiveTail - 1), SoundActiveHead, SoundActiveTail);

    if (SoundActiveHead == SoundActiveTail)
      uart_send(__LINE__, "Active queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_ACTIVE_SOUND_QUEUE - 1), SoundActiveHead, SoundActiveTail);

    if (SoundActiveHead < SoundActiveTail)
      uart_send(__LINE__, "Active queue remaining space: %5u  Head: %3u  Tail: %3u\r", (SoundActiveTail - SoundActiveHead - 1), SoundActiveHead, SoundActiveTail);
  }
  ***/

  return 0;
}





/* $PAGE */
/* $TITLE=sound_unqueue_active() */
/* ------------------------------------------------------------------ *\
         Unqueue next sound from the active buzzer sound queue.
\* ------------------------------------------------------------------ */
UINT8 sound_unqueue_active(UINT16 *MSeconds, UINT16 *RepeatCount)
{
  UCHAR String[256];

  UINT16 Loop1UInt16;

  UINT32 CurrentTail;


  /* Trap circular buffer corruption. */
  if ((SoundActiveHead > MAX_ACTIVE_SOUND_QUEUE) || (SoundActiveTail > MAX_ACTIVE_SOUND_QUEUE))
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      uart_send(__LINE__, "- A-Corrupted:        %5u   %5u\r", SoundActiveHead, SoundActiveTail);
      uart_send(__LINE__, "ACTIVE SOUND QUEUE (%u)   Head: %4u   Tail: %4u\r", MAX_ACTIVE_SOUND_QUEUE, SoundActiveHead, SoundActiveTail);
      uart_send(__LINE__, "          MSec    Repeat\r");

      for (Loop1UInt16 = 0; Loop1UInt16 < MAX_ACTIVE_SOUND_QUEUE; ++Loop1UInt16)
        uart_send(__LINE__, "(%4u)   %5u     %5u\r", Loop1UInt16, SoundQueueActive[Loop1UInt16].MSec, SoundQueueActive[Loop1UInt16].RepeatCount);
    }

    SoundActiveHead = 0;
    SoundActiveTail = 0;

    return 0xFF;
  }


  /* Check if active sound queue is empty. */
  if (SoundActiveHead == SoundActiveTail)
  {
    /* In case of empty queue or queue error, return 0 as milliseconds and repeat count. */
    *MSeconds    = 0;
    *RepeatCount = 0;

    /***
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- A-Empty:               %5u   %5u\r", SoundActiveHead, SoundActiveTail);
    ***/

    return 0xFF;
  }
  else
  {
    /* Active sound queue is not empty. */
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- A-NotEmpty:            %5u   %5u\r", SoundActiveHead, SoundActiveTail);

    /* Extract data for next sound to play. */
    *MSeconds    = SoundQueueActive[SoundActiveTail].MSec;
    *RepeatCount = SoundQueueActive[SoundActiveTail].RepeatCount;


    /* And reset this slot in the sound queue. */
    SoundQueueActive[SoundActiveTail].MSec        = 0;
    SoundQueueActive[SoundActiveTail].RepeatCount = 0;


    /***
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- A-Unqueuing:           %5u   %5u\r", *MSeconds, *RepeatCount);
    ***/


    if ((*MSeconds != 0) && (*RepeatCount <= 100))
    {
      /* The sound found in this slot is valid. */
      ++SoundActiveTail;

      /* If reaching out-of-bound, revert to zero. */
      if (SoundActiveTail >= MAX_ACTIVE_SOUND_QUEUE) SoundActiveTail = 0;

      return 0;
    }
    else
    {
      /* Sound in this slot was invalid. */
      if (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- A-Housekeeping from      %3u\r", SoundActiveTail);

      /* If this slot was corrupted, make some housekeeping. */
      CurrentTail = SoundActiveTail; // keep track of starting position in the queue.

      /* In case of empty queue or queue error, return 0 as milliseconds and repeat count. */
      *MSeconds    = 0;
      *RepeatCount = 0;


      /* Clean all active sound queue. */
      do
      {
        SoundQueueActive[SoundActiveTail].MSec        = 0;
        SoundQueueActive[SoundActiveTail].RepeatCount = 0;


        ++SoundActiveTail;

        /* If reaching out-of-bound, revert to zero. */
        if (SoundActiveTail >= MAX_ACTIVE_SOUND_QUEUE) SoundActiveTail = 0;
      } while (SoundActiveTail != CurrentTail);  // if we come back to where we started from, sound queue has all been cleaned up.

      SoundActiveHead = SoundActiveTail;

      if (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- A-Done:                %5u   %5u\r", SoundActiveHead, SoundActiveTail);

      return 0xFF;
    }
  }
}




#ifdef PASSIVE_PIEZO_SUPPORT
/* $PAGE */
/* $TITLE=sound_queue_reset_passive() */
/* ------------------------------------------------------------------ *\
             Reset (clear) the passive buzzer sound queue.
\* ------------------------------------------------------------------ */
void sound_queue_reset_passive(void)
{
  SoundPassiveHead = 0;
  SoundPassiveTail = 0;
  return;
}
#endif




#ifdef PASSIVE_PIEZO_SUPPORT
/* $PAGE */
/* $TITLE=sound_queue_status_passive() */
/* ------------------------------------------------------------------ *\
        Return the status of the passive buzzer sound queue.
\* ------------------------------------------------------------------ */
UINT8 sound_queue_status_passive(void)
{
  /* Trap circular buffer corruption. */
  if ((SoundPassiveHead > MAX_PASSIVE_SOUND_QUEUE) || (SoundPassiveTail > MAX_PASSIVE_SOUND_QUEUE))
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- P-Corrupted:           %5u   %5u\r", SoundPassiveHead, SoundPassiveTail);

    SoundPassiveHead = 0;
    SoundPassiveTail = 0;
    return 0;
  }


  /* Check if the sound circular buffer (sound queue) is full. */
  if (((SoundPassiveTail > 0) && (SoundPassiveHead == (SoundPassiveTail - 1))) || ((SoundPassiveTail == 0) && (SoundPassiveHead == (MAX_PASSIVE_SOUND_QUEUE - 1))))
  {
    /* Sound queue is full, return error code. */
    return FLAG_FULL;
  }

  if (SoundPassiveHead != SoundPassiveTail)
  {
    // Return FLAG_ON if buffer not empty
    return FLAG_ON;
  }
  else
  {
    return FLAG_OFF;
  }

}
#endif





#ifdef PASSIVE_PIEZO_SUPPORT
/* $PAGE */
/* $TITLE=sound_queue_passive() */
/* ------------------------------------------------------------------ *\
       Queue the given sound in the passive buzzer sound queue.
            Use the queue algorithm where one slot is lost.
              This prevents the use of a lock mechanism.
\* ------------------------------------------------------------------ */
UINT16 sound_queue_passive(UINT16 Frequency, UINT16 MSeconds)
{
  UCHAR String[256];


  if (DebugBitMask & DEBUG_SOUND_QUEUE)
    uart_send(__LINE__, "- P-Queueing:    (%3u)   %5u   %5u\r", SoundPassiveHead, Frequency, MSeconds);

  /* Trap circular buffer corruption. */
  if ((SoundPassiveHead > MAX_PASSIVE_SOUND_QUEUE) || (SoundPassiveTail > MAX_PASSIVE_SOUND_QUEUE))
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- P-Corrupted:           %5u   %5u\r", SoundPassiveHead, SoundPassiveTail);

    SoundPassiveHead = 0;
    SoundPassiveTail = 0;

    return 0;
  }


  /* Check if the sound circular buffer (sound queue) is full. */
  if (((SoundPassiveTail > 0) && (SoundPassiveHead == (SoundPassiveTail - 1))) || ((SoundPassiveTail == 0) && (SoundPassiveHead == (MAX_PASSIVE_SOUND_QUEUE - 1))))
  {
    /* Sound queue is full, return error code. */
    return MAX_PASSIVE_SOUND_QUEUE;
  }


  /* If there is at least one slot available in the queue, insert the sound to be played. */
  SoundQueuePassive[SoundPassiveHead].Freq = Frequency;
  SoundQueuePassive[SoundPassiveHead].MSec = MSeconds;
  ++SoundPassiveHead;


  /* Revert to zero when reaching out-of-bound. */
  if (SoundPassiveHead >= MAX_PASSIVE_SOUND_QUEUE) SoundPassiveHead = 0;


  /***
  if (DebugBitMask & DEBUG_SOUND_QUEUE)
  {
    if (SoundPassiveHead > SoundPassiveTail)
      uart_send(__LINE__, "Passive queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_PASSIVE_SOUND_QUEUE - SoundPassiveHead + SoundPassiveTail - 1), SoundPassiveHead, SoundPassiveTail);

    if (SoundPassiveHead == SoundPassiveTail)
      uart_send(__LINE__, "Passive queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_PASSIVE_SOUND_QUEUE - 1), SoundPassiveHead, SoundPassiveTail);

    if (SoundPassiveHead < SoundPassiveTail)
      uart_send(__LINE__, "Passive queue remaining space: %5u  Head: %3u  Tail: %3u\r", (SoundPassiveTail - SoundPassiveHead - 1), SoundPassiveHead, SoundPassiveTail);
  }
  ***/

  return 0;
}



/* $PAGE */
/* $TITLE=sound_unqueue_passive() */
/* ------------------------------------------------------------------ *\
        Unqueue next sound from the passive buzzer sound queue.
\* ------------------------------------------------------------------ */
UINT8 sound_unqueue_passive(UINT16 *Frequency, UINT16 *MSeconds)
{
  UCHAR  String[256];

  UINT16 Loop1UInt16;

  UINT32 CurrentTail;


  /* Trap circular buffer corruption. */
  if ((SoundPassiveHead > MAX_PASSIVE_SOUND_QUEUE) || (SoundPassiveTail > MAX_PASSIVE_SOUND_QUEUE))
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      uart_send(__LINE__, "- P-Corrupted:           %5u   %5u\r", SoundPassiveHead, SoundPassiveTail);
      uart_send(__LINE__, "PASSIVE SOUND QUEUE (%u)   Head: %4u   Tail: %4u\r", MAX_PASSIVE_SOUND_QUEUE, SoundPassiveHead, SoundPassiveTail);
      uart_send(__LINE__, "        Freq   MSec\r");

      for (Loop1UInt16 = 0; Loop1UInt16 < MAX_PASSIVE_SOUND_QUEUE; ++Loop1UInt16)
        uart_send(__LINE__, "(%4u) %5u  %5u\r", Loop1UInt16, SoundQueuePassive[Loop1UInt16].Freq, SoundQueuePassive[Loop1UInt16].MSec);
    }

    SoundPassiveHead = 0;
    SoundPassiveTail = 0;

    return 0xFF;
  }


  /* Check if passive sound queue is empty. */
  if (SoundPassiveHead == SoundPassiveTail)
  {
    /* In case of empty queue or queue error, return 0 as Frequency and Duration. */
    *Frequency = 0;
    *MSeconds  = 0;

    /***
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
      uart_send(__LINE__, "- P-Empty:               %5u   %5u\r", SoundPassiveHead, SoundPassiveTail);
    ***/

    return 0xFF;
  }
  else
  {
    /* Passive queue is not empty, return next sound. */
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      uart_send(__LINE__, "- P-NotEmpty:            %5u   %5u\r", SoundPassiveHead, SoundPassiveTail);

      // uart_send(__LINE__, "- P-Unqueuing:   [%3u]   %5u   %5u\r", SoundPassiveTail, SoundQueuePassive[SoundPassiveTail].Freq, SoundQueuePassive[SoundPassiveTail].MSec);
    }


    /* Extract data for next sound to play. */
    *Frequency = SoundQueuePassive[SoundPassiveTail].Freq;
    *MSeconds  = SoundQueuePassive[SoundPassiveTail].MSec;


    /* And reset this slot in the sound queue. */
    SoundQueuePassive[SoundPassiveTail].Freq = 0;
    SoundQueuePassive[SoundPassiveTail].MSec = 0;


    if ((*Frequency >= 60) && (*Frequency <= 8000) && (*MSeconds != 0) || (*Frequency == SILENT) || (*MSeconds == WAIT_4_ACTIVE))
    {
      /* The sound found in this slot is valid. */
      ++SoundPassiveTail;

      /* If reaching out-of-bound, revert to zero. */
      if (SoundPassiveTail >= MAX_PASSIVE_SOUND_QUEUE) SoundPassiveTail = 0;

      /***
      if  (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- P-Adjust Tail to:         %5u\r", SoundPassiveTail);
      ***/

      return 0;
    }
    else
    {
      /* If this slot was corrupted, make some housekeeping. */
      if  (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- P-Housekeeping from %u   Freq: %5u   MSec: %5u\r", SoundPassiveTail, *Frequency, *MSeconds);

      CurrentTail = SoundPassiveTail; // keep track of starting position in the queue.

      /* In case of empty queue or queue error, return 0 as Frequency and Duration. */
      *Frequency = 0;
      *MSeconds  = 0;

      do
      {
        SoundQueuePassive[SoundPassiveTail].Freq = 0;
        SoundQueuePassive[SoundPassiveTail].MSec = 0;

        /* Advance to next slot. */
        ++SoundPassiveTail;

        // If reaching out-of-bound, revert to zero.
        if (SoundPassiveTail >= MAX_PASSIVE_SOUND_QUEUE) SoundPassiveTail = 0;
      } while (SoundPassiveTail != CurrentTail);  // if we come back to where we started from, sound queue is now empty.

      SoundPassiveHead = SoundPassiveTail;

      if (DebugBitMask & DEBUG_SOUND_QUEUE)
        uart_send(__LINE__, "- P-Done:             %5u   %5u\r", SoundPassiveHead, SoundPassiveTail);
    }

    return 0xFF;
  }
}
#endif





#ifdef DEVELOPER_VERSION
#ifdef DST_DEBUG
#include "test_dst_status.cpp"
#endif  // DST_DEBUG
#endif  // DEVELOPER_VERSION





#ifdef TEST_CODE
/* $PAGE */
/* $TITLE=test_zone() */
/* ------------------------------------------------------------------ *\
     Section to keep some useful testing code for eventual re-use.
     NOTE: Clock display is logically divided in sections of 8 columns
           (8 dots). Some of these columns and / or bits of these
           columns being used by indicators.

      Depending on which tests are performed, it may be useful to
       turn off Display scrolling when performing tests, so that
             scrolling does not interfere with the tests.
\* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ *\
    === WARNING === WARNING === WARNING === WARNING === WARNING ===
    === WARNING === WARNING === WARNING === WARNING === WARNING ===
    === WARNING === WARNING === WARNING === WARNING === WARNING ===

     The pieces of code below have been very useful while testing
    and implementing new features on the clock. However, they must
     be considered as "test code" only. Even if most of the parts
     should work as expected, they have not been tested under all
    conditions and some use cases have certainly not been covered.
    So, most likely, you will need to modify and adapt it to cover
   your own test procedure. Whatever the case, I hope those pieces
                        will be helpful - ASL

    === WARNING === WARNING === WARNING === WARNING === WARNING ===
    === WARNING === WARNING === WARNING === WARNING === WARNING ===
    === WARNING === WARNING === WARNING === WARNING === WARNING ===
\* ------------------------------------------------------------------ */
void test_zone(UINT8 TestNumber)
{
  UCHAR CharacterBuffer;             // buffer to temporarily hold a character.
  UCHAR CrcString[64];               // buffer to test crc16() function.
  UCHAR String[DISPLAY_BUFFER_SIZE]; // string to scroll on clock display.

  UINT Status;
  UINT TrigLevel;

  UINT8 Column;
  UINT8 DisplayType;
  UINT8 Dum1UInt8;
  UINT8 *FlashBaseAddress;
  UINT8 FlashSector[4096];
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 Loop3UInt8;
  UINT8 Row;
  UINT8 TemperatureF; // temperature in Fahrenheit.
  UINT8 WeekDay;

  UINT16 ColumnInSection; // Column number relative to a specific "section" of the display.
  UINT16 ColumnPosition;  // Column number in virtual display buffer where to put the ASCII character.
  UINT16 Crc16;
  UINT16 Dum1UInt16;
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;
  UINT16 Loop3UInt16;
  UINT16 RowNumber;     // Row number on the display (row number 0 corresponds to indicators).
  UINT16 SectionNumber; // Section number on the display (display is logically divided in section numbers of 8 dots).

  UINT32 Dum1UInt32;
  UINT32 Loop1UInt32;
  UINT32 Loop2UInt32;
  UINT32 Offset;

  UINT64 Dum1UInt64;

  float Humidity;
  float Temperature;


  /* Configure the GPIO associated to the three push-buttons of the clock in "input" mode to use them while testing. */
  /* Tests are usually performed before gpio initialization. */
  gpio_set_dir(SET_BUTTON,  GPIO_IN);
  gpio_set_dir(UP_BUTTON,   GPIO_IN);
  gpio_set_dir(DOWN_BUTTON, GPIO_IN);

  /* Setup a pull-up on those three GPIOs. */
  gpio_pull_up(SET_BUTTON);
  gpio_pull_up(UP_BUTTON);
  gpio_pull_up(DOWN_BUTTON);


  /* ---------------------------------------------------------------- *\
                  Display time before beginning tests.
  \* ---------------------------------------------------------------- */
  /* Single tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Clear framebuffer. */
  clear_framebuffer(0);

  show_time();
  sleep_ms(1000);
  /* ---------------------------------------------------------------- *\
                          END - Display time.
  \* ---------------------------------------------------------------- */


  /* ---------------------------------------------------------------- *\
       Jump to the specific test number if one has been specified.
              (0 means to execute all tests in sequence).
  \* ---------------------------------------------------------------- */
  if (TestNumber != 0)
  {
    switch (TestNumber)
    {
      case (1):
        goto Test1;
      break;

      case (2):
        goto Test2;
      break;

      case (3):
        goto Test3;
      break;

      case (4):
        goto Test4;
      break;

      case (5):
        goto Test5;
      break;

      case (6):
        goto Test6;
      break;

      case (7):
        goto Test7;
      break;

      case (8):
        goto Test8;
      break;

      case (9):
        goto Test9;
      break;

      case (10):
        goto Test10;
      break;

      case (11):
        goto Test11;
      break;

      case (12):
        goto Test12;
      break;

      case (13):
        goto Test13;
      break;

      case (14):
        goto Test14;
      break;

      case (15):
        goto Test15;
      break;

      case (16):
        goto Test16;
      break;

      case (17):
        goto Test17;
      break;

      case (18):
        goto Test18;
      break;

      default:
        goto Test1;
      break;
    }
  }

  /* ---------------------------------------------------------------- *\
               Test 1 - Display all 4 X 7 character set.
  \* ---------------------------------------------------------------- */
Test1:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);

  /* Announce test number. */
  scroll_string(24, "Test #1 - 4X7 character set");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Initialize clock mode. */
  CurrentClockMode = MODE_TEST;

  /* Read RTC IC registers. */
  ds3231_register_read();

  /* Display RTC registers. */
  sprintf(String, "1: %2.2X   2: %2.2X   3: %2.2X   4: %2.2X   5: %2.2X   6: %2.2X   7: %2.2X",
          Ds3231ReadRegister[0], Ds3231ReadRegister[1], Ds3231ReadRegister[2], Ds3231ReadRegister[3], Ds3231ReadRegister[4], Ds3231ReadRegister[5], Ds3231ReadRegister[6]);
  scroll_string(24, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "8: %2.2X   9: %2.2X   10: %2.2X   11: %2.2X   12: %2.2X   13: %2.2X   14: %2.2X",
          Ds3231ReadRegister[7], Ds3231ReadRegister[8], Ds3231ReadRegister[9], Ds3231ReadRegister[10], Ds3231ReadRegister[11], Ds3231ReadRegister[12], Ds3231ReadRegister[13]);
  scroll_string(24, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  return;

  /* Display all 4 X 7 character bitmaps. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 89; ++Loop1UInt8)
  {
    clear_framebuffer(0);
    fill_display_buffer_4X7(0, 0x30 + (Loop1UInt8 / 10));
    fill_display_buffer_4X7(5, 0x30 + (Loop1UInt8 % 10));
    fill_display_buffer_4X7(15, (Loop1UInt8 + 0x2D));
    sleep_ms(2000);
  }

  return;
  /* ---------------------------------------------------------------- *\
             END - Test 1 - Display all 4 X 7 character set.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
                    Test 2 - Flash storage tests.
  \* ---------------------------------------------------------------- */
  /* ---------------------------------------------------------------- *\
     NOTES:
     Size of flash memory:    2 MB
     Page size:             256 KB
     Sector size:          4096 KB
  \* ---------------------------------------------------------------- */

Test2:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number.
     NOTE: Too much data to survive with Green clock scrolling display. An external monitor is required. */
  /*
  scroll_string(24, "Test #2 - Flash storage");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.
  */

  /* Initialize clock mode during tests. */
  CurrentClockMode = MODE_TEST;

  uart_send(__LINE__, "Flash storage tests.\r");

  /* For reference purposes, display maximum value of each type of unsigned variables. */
  uart_send(__LINE__, "Keep track of a few specific numbers for reference purposes...\r");

  Dum1UInt8 = 0;
  --Dum1UInt8;
  uart_send(__LINE__, "Maximum value for  8 bits unsigned:               0x%X                    %u\r", Dum1UInt8, Dum1UInt8);

  Dum1UInt16 = 0;
  --Dum1UInt16;
  uart_send(__LINE__, "Maximum value for 16 bits unsigned:             0x%X                  %u\r", Dum1UInt16, Dum1UInt16);

  Dum1UInt32 = 0;
  --Dum1UInt32;
  uart_send(__LINE__, "Maximum value for 32 bits unsigned:         0x%lX             %lu\r", Dum1UInt32, Dum1UInt32);

  Dum1UInt64 = 0;
  --Dum1UInt64;
  uart_send(__LINE__, "Maximum value for 64 bits unsigned: 0x%llX   %llu\r", Dum1UInt64, Dum1UInt64);

  /* Display other useful information. */
  uart_send(__LINE__, "Address of malloc() pointer returned (for variable FlashData): %p\r", FlashData);

  uart_send(__LINE__, "FLASH_PAGE_SIZE: %u\r", FLASH_PAGE_SIZE);

  uart_send(__LINE__, "FLASH_SECTOR_SIZE: %u\r", FLASH_SECTOR_SIZE);

  uart_send(__LINE__, "Address of beginning of flash (XIP_BASE): %X\r\r", XIP_BASE);

  /* ================================================================ *\
          Display Raspberry Pi Pico manufacturing test results.
  \* ================================================================ */
  uart_send(__LINE__, "Display Pico's manufacturing test results.\r");
  flash_display(0x7F000, 0x6A);

  /* Erase data written to flash from previous tests. */
  flash_erase(0x1FF000);

  /* Test flash_write() function. */
  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 1 -------------------------\r");
  memset(FlashSector, 0x00, sizeof(FlashSector));
  flash_write(0x1FF000, FlashSector, 1);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 1 X 0x00 at offset 0x1FF000.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 2 -------------------------\r");
  memset(FlashSector, 0x01, sizeof(FlashSector));
  flash_write(0x1FF001, FlashSector, 1);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 1 X 0x01 at offset 0x1F001.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 3 -------------------------\r");
  memset(FlashSector, 0x02, sizeof(FlashSector));
  flash_write(0x1FF002, FlashSector, 1);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 1 X 0x02 at offset 0x1FF002.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 4 -------------------------\r");
  memset(FlashSector, 0x03, sizeof(FlashSector));
  flash_write(0x1FF003, FlashSector, 1);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 1 X 0x03 at offset 0x1FF003.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 5 -------------------------\r");
  memset(FlashSector, 0x04, sizeof(FlashSector));
  flash_write(0x1FF004, FlashSector, 1);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 1 X 0x04 at offset 0x1FF004.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 6 -------------------------\r");
  memset(FlashSector, 0x00, sizeof(FlashSector));
  flash_write(0x1FF005, FlashSector, 5);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 5 X 0x00 at offset 0x1FF005.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 7 -------------------------\r");
  memset(FlashSector, 0x11, sizeof(FlashSector));
  flash_write(0x1FF009, FlashSector, 25);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 25 X 0x11 at offset 0x1FF009.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 8 -------------------------\r");
  memset(FlashSector, 0x22, sizeof(FlashSector));
  flash_write(0x1FF044, FlashSector, 65);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 65 X 0x22 at offset 0x1FF044.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 9 -------------------------\r");
  memset(FlashSector, 0x33, sizeof(FlashSector));
  flash_write(0x1FF0E0, FlashSector, 50);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 50 X 0x33 at offset 0x1FF0E0.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 10 -------------------------\r");
  memset(FlashSector, 0x00, sizeof(FlashSector));
  FlashSector[0]  = 0x50;
  FlashSector[1]  = 0x51;
  FlashSector[2]  = 0x52;
  FlashSector[3]  = 0x53;
  FlashSector[4]  = 0x54;
  FlashSector[5]  = 0x55;
  FlashSector[6]  = 0x56;
  FlashSector[7]  = 0x57;
  FlashSector[8]  = 0x58;
  FlashSector[9]  = 0x59;
  FlashSector[10] = 0x5A;
  FlashSector[11] = 0x5B;
  FlashSector[12] = 0x5C;
  FlashSector[13] = 0x5D;
  FlashSector[14] = 0x5E;
  FlashSector[15] = 0x5F;
  flash_write(0x1FF0F3, FlashSector, 16);

  /* Display flash content after flash_write(). */
  uart_send(__LINE__, "Display flash content after writing 16 increment of 50 at offset 0x1FF0F3.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 11 -------------------------\r");

  /* NOTE: Should give an error and write nothing. */
  memset(FlashSector, 0xAA, sizeof(FlashSector));
  flash_write(0x1FF101, FlashSector, 4096);

  /* Display flash content. */
  uart_send(__LINE__, "Display flash content after writing 1024 X 0xAA at offset 0x1FF105.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 12 -------------------------\r");

  /* NOTE: Should give an error and write nothing. */
  memset(FlashSector, 0xBB, sizeof(FlashSector));
  flash_write(0x1FF000, FlashSector, 4097);

  /* Display flash content. */
  uart_send(__LINE__, "Display flash content after writing 1025 X 0xBB at offset 0x1FF200.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 13 -------------------------\r");
  memset(FlashSector, 0x11, sizeof(FlashSector));
  flash_write(0x1FF300, FlashSector, 256);

  /* Display flash content. */
  uart_send(__LINE__, "Display flash content after writing 256 X 0x11 at offset 0x1FF300.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 14 -------------------------\r");
  memset(FlashSector, 0x33, sizeof(FlashSector));
  flash_write(0x1FF355, FlashSector, 48);

  /* Display flash content. */
  uart_send(__LINE__, "Display flash content after writing 48 X 0x33 at offset 0x1FF355.\r");
  flash_display((0x1FF000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 15 -------------------------\r");
  memset(FlashSector, 0xFF, sizeof(FlashSector));
  flash_write(0x7F0FF, FlashSector, 1);

  /* Display flash content. */
  uart_send(__LINE__, "Display flash content after writing 148 X 0xFF at offset 0x7F06B.\r");
  flash_display((0x7F000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 16 -------------------------\r");
  memset(FlashSector, 0xFF, sizeof(FlashSector));
  flash_write(0x14000, FlashSector, 4096);

  /* Display flash content. */
  uart_send(__LINE__, "Display flash content after writing 148 X 0xFF at offset 0x7F06B.\r");
  flash_display((0x14000 - 64), (4096 + 128));

  /* Set the data to write to flash. */
  uart_send(__LINE__, "------------------------- BEGINNING OF TEST 17 -------------------------\r");
  memset(FlashSector, 0xFF, sizeof(FlashSector));
  flash_write(0x12000, FlashSector, 4096);

  /* Display flash content. */
  uart_send(__LINE__, "Display flash content after writing 148 X 0xFF at offset 0x7F06B.\r");
  flash_display((0x12000 - 64), (4096 + 128));

  /* Find areas beyond firmware memory space with remaining data in flask (end of code was around 0x10013A88). */
  /*
  FlashBaseAddress = (UINT8 *)(XIP_BASE);
  Dum1UInt16 = 0;
  for (Loop1UInt32 = ((2 * 1024 * 1024) - 1); Loop1UInt32 > 0x14600; --Loop1UInt32)
  {
    if (FlashBaseAddress[Loop1UInt32] != 0xFF)
    {
      ++Dum1UInt16;
      uart_send(__LINE__, "Address: %8.8X   Content: %2.2X\r", (XIP_BASE + Loop1UInt32), FlashBaseAddress[Loop1UInt32]);
    }
  }
  */
  uart_send(__LINE__, "End of flash tests...\r\r\r");

  /*
  // Display the complete Pico's 2MB flash address space.
  uart_send(__LINE__, "Flash content:\r\r");

  flash_display(0, 0x1FFFFF);

  uart_send(__LINE__, "End of flash content\r\r");
  */

  /* Tests to save Green clock configuration to flash. */
  uart_send(__LINE__, "sizeof(FlashConfig): 0x%X (%u)\r", sizeof(FlashConfig), sizeof(FlashConfig));

  /* Read current Green Clock configuration from flash memory. */
  FlashBaseAddress = (UINT8 *)(XIP_BASE);
  for (Loop1UInt16 = 0; Loop1UInt16 < 256; ++Loop1UInt16)
  {
    ((UINT8 *)(FlashConfig.Version))[Loop1UInt16] = FlashBaseAddress[FLASH_CONFIG_OFFSET + Loop1UInt16];
  }

  /* Display configuration values retrieved from flash memory. */
  uart_send(__LINE__, "Display configuration retrieved from flash memory:\r");
  display_data((UINT8 *)&FlashConfig, 256);

  /* Display details of configuration read from flash. */
  uart_send(__LINE__, "NOTE: FLAG_OFF: %u     FLAG_ON: %u\r", FLAG_OFF, FLAG_ON);
  uart_send(__LINE__, "[%X] Firmware version:  %s\r", &FlashConfig.Version, FlashConfig.Version);
  uart_send(__LINE__, "[%X] CurrentYearCentile: %3u\r", &FlashConfig.CurrentYearCentile, FlashConfig.CurrentYearCentile);
  uart_send(__LINE__, "[%X] DSTCountry:         %3u\r", &FlashConfig.DSTCountry, FlashConfig.DSTCountry);
  uart_send(__LINE__, "[%X] ChimeTimeOn:        %3u\r", &FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOn);
  uart_send(__LINE__, "[%X] ChimeTimeOff:       %3u\r", &FlashConfig.ChimeTimeOff, FlashConfig.ChimeTimeOff);
  uart_send(__LINE__, "[%X] FlagAutoBrightness: %3u\r", &FlashConfig.FlagAutoBrightness, FlashConfig.FlagAutoBrightness);
  uart_send(__LINE__, "[%X] FlagKeyclick:       %3u\r", &FlashConfig.FlagKeyclick, FlashConfig.FlagKeyclick);
  uart_send(__LINE__, "[%X] FlagScrollEnable:   %3u\r", &FlashConfig.FlagScrollEnable, FlashConfig.FlagScrollEnable);
  uart_send(__LINE__, "[%X] ChimeMode:          %3u\r", &FlashConfig.ChimeMode, FlashConfig.ChimeMode);
  uart_send(__LINE__, "[%X] Language:           %3u\r", &FlashConfig.Language, FlashConfig.Language);
  uart_send(__LINE__, "[%X] NightLightMode:     %3u\r", &FlashConfig.NightLightMode, FlashConfig.NightLightMode);
  uart_send(__LINE__, "[%X] NightLightTimeOn:   %3u\r", &FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOn);
  uart_send(__LINE__, "[%X] NightLightTimeOff:  %3u\r", &FlashConfig.NightLightTimeOff, FlashConfig.NightLightTimeOff);
  uart_send(__LINE__, "[%X] TemperatureUnit:    %3u\r", &FlashConfig.TemperatureUnit, FlashConfig.TemperatureUnit);
  uart_send(__LINE__, "[%X] TimeDisplayMode:    %3u\r\r\r", &FlashConfig.TimeDisplayMode, FlashConfig.TimeDisplayMode);
  uart_send(__LINE__, "[%X] CRC16 read from flash          0x%X (%u)\r", &FlashConfig.Crc16, FlashConfig.Crc16, FlashConfig.Crc16);

  /* Validate CRC16 read from flash. */
  Dum1UInt16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  uart_send(__LINE__, "CRC16 computed from data read: 0x%X (%u)\r", Dum1UInt16, Dum1UInt16);

  /* If CRC16 read from flash does not correspond to the data read, assign default values and save new configuration. */
  if (FlashConfig.Crc16 == Dum1UInt16)
    uart_send(__LINE__, "Flash configuration is valid. Flash memory tests completed.\r\r\r");
  else
    uart_send(__LINE__, "Flash configuration is corrupted. Save a new configuration to flash.\r\r\r");

  /* If CRC16 computed from retrieved data is different from CRC16 read from flash...
     Assign default values and save a new configuration to flash. */
  sprintf(FlashConfig.Version, FIRMWARE_VERSION);         // firmware version number.
  FlashConfig.CurrentYearCentile = 20;                    // assume we are in years 20xx (could be changed in clock setup, but will revert to 20 at each power-up).
  FlashConfig.ChimeTimeOff       = CHIME_TIME_OFF;        // hourly chime will begin at this hour.
  FlashConfig.ChimeTimeOn        = CHIME_TIME_ON;         // hourly chime will begin at this hour.
  FlashConfig.DSTCountry         = DST_COUNTRY;           // specifies how to handle the daylight saving time depending on country (see User Guide).
  FlashConfig.FlagAutoBrightness = FLAG_ON;               // flag indicating we are in "Auto Brightness" mode.
  FlashConfig.FlagKeyclick       = FLAG_OFF;              // flag for keyclick ("button-press" tone)
  FlashConfig.FlagScrollEnable   = SCROLL_DEFAULT;        // flag indicating the clock will scroll the date and temperature at regular intervals on the display.
  FlashConfig.ChimeMode          = CHIME_DEFAULT;         // chime mode (Off / On / Day).
  FlashConfig.Language           = DEFAULT_LANGUAGE;      // language used for data display (including date scrolling).
  FlashConfig.NightLightMode     = NIGHT_LIGHT_DEFAULT;   // night light mode (On / Off / Auto / Night).
  FlashConfig.NightLightTimeOff  = NIGHT_LIGHT_TIME_OFF;  // default night light time off.
  FlashConfig.NightLightTimeOn   = NIGHT_LIGHT_TIME_ON;   // default night light time on.
  FlashConfig.TemperatureUnit    = TEMPERATURE_DEFAULT;   // CELSIUS or FAHRENHEIT default value (see clock options above).
  FlashConfig.TimeDisplayMode    = TIME_DISPLAY_DEFAULT;  // H24 or H12 default value (see clock options above).

  /* Write Green Clock configuration to flash. */
  uart_send(__LINE__, "Write Green Clock configuration to flash.\r");
  uart_send(__LINE__, "Pointer to FlashConfig.Version: %X\r", FlashConfig.Version);
  uart_send(__LINE__, "Pointer to FlashConfig.Crc16:   %X\r", &FlashConfig.Crc16);
  uart_send(__LINE__, "&FlashConfig.Crc16 - &FlashConfig.Version: %X (%u)\r\r\r", (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);

  /* Display configuration being saved. */
  uart_send(__LINE__, "NOTE: FLAG_OFF: %u     FLAG_ON: %u\r", FLAG_OFF, FLAG_ON);
  uart_send(__LINE__, "[%X] Firmware version:  %s\r", &FlashConfig.Version, FlashConfig.Version);
  uart_send(__LINE__, "[%X] CurrentYearCentile: %3u\r", &FlashConfig.CurrentYearCentile, FlashConfig.CurrentYearCentile);
  uart_send(__LINE__, "[%X] DSTCountry:         %3u\r", &FlashConfig.DSTCountry, FlashConfig.DSTCountry);
  uart_send(__LINE__, "[%X] ChimeTimeOn:        %3u\r", &FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOn);
  uart_send(__LINE__, "[%X] ChimeTimeOff:       %3u\r", &FlashConfig.ChimeTimeOff, FlashConfig.ChimeTimeOff);
  uart_send(__LINE__, "[%X] FlagAutoBrightness: %3u\r", &FlashConfig.FlagAutoBrightness, FlashConfig.FlagAutoBrightness);
  uart_send(__LINE__, "[%X] FlagKeyclick:       %3u\r", &FlashConfig.FlagKeyclick, FlashConfig.FlagKeyclick);
  uart_send(__LINE__, "[%X] FlagScrollEnable:   %3u\r", &FlashConfig.FlagScrollEnable, FlashConfig.FlagScrollEnable);
  uart_send(__LINE__, "[%X] ChimeMode:          %3u\r", &FlashConfig.ChimeMode, FlashConfig.ChimeMode);
  uart_send(__LINE__, "[%X] Language:           %3u\r", &FlashConfig.Language, FlashConfig.Language);
  uart_send(__LINE__, "[%X] NightLightMode:     %3u\r", &FlashConfig.NightLightMode, FlashConfig.NightLightMode);
  uart_send(__LINE__, "[%X] NightLightTimeOn:   %3u\r", &FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOn);
  uart_send(__LINE__, "[%X] NightLightTimeOff:  %3u\r", &FlashConfig.NightLightTimeOff, FlashConfig.NightLightTimeOff);
  uart_send(__LINE__, "[%X] TemperatureUnit:    %3u\r", &FlashConfig.TemperatureUnit, FlashConfig.TemperatureUnit);
  uart_send(__LINE__, "[%X] TimeDisplayMode:    %3u\r\r\r", &FlashConfig.TimeDisplayMode, FlashConfig.TimeDisplayMode);
  uart_send(__LINE__, "Calculating CRC16\r");

  FlashConfig.Crc16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  uart_send(__LINE__, "Pointer: %X   Size: %u   CRC16: %X (%u)\r\r\r", (UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version, FlashConfig.Crc16, FlashConfig.Crc16);

  flash_write(FLASH_CONFIG_OFFSET, (UINT8 *)FlashConfig.Version, sizeof(FlashConfig));

  /* Display flash configuration as saved. */
  uart_send(__LINE__, "Display flash configuration as saved:\r");
  flash_display((FLASH_CONFIG_OFFSET - 64), (4096 + 128));

  return;
  /* ---------------------------------------------------------------- *\
                   END - Test 2 - Flash memory tests.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
                   Test 3 - Tests for CRC16 function.
  \* ---------------------------------------------------------------- */
Test3:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #3 - CRC16 tests");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(CrcString, "FF101A0B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  sprintf(CrcString, "FF101A8B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  sprintf(CrcString, "FF101B0B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  sprintf(CrcString, "FF101B8B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  sprintf(CrcString, "FF101C0B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  sprintf(CrcString, "FF101C8B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  sprintf(CrcString, "FF101D0B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  sprintf(CrcString, "FF101D8B");
  Dum1UInt16 = crc16(CrcString, 8);
  uart_send(__LINE__, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);

  uart_send(__LINE__, "End of CRC16 tests...\r\r\r");

  return;
  /* ---------------------------------------------------------------- *\
                END - Test 3 - Tests for CRC16 function.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
                  Test 4 - Display brightness tests.
  \* ---------------------------------------------------------------- */
Test4:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #4 - LED matrix brightness");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  while (1)
  {
    sprintf(String, "%4.4u", AverageLightLevel);

    fill_display_buffer_4X7(1,  String[0]);
    fill_display_buffer_4X7(6,  String[1]);
    fill_display_buffer_4X7(11, String[2]);
    fill_display_buffer_4X7(16, String[3]);
    sleep_ms(300);

    /* Press the "Top" button to exit the while() loop. */
    if (gpio_get(SET_BUTTON) == 0)
      break;
  }

  return;
  /* ---------------------------------------------------------------- *\
                END - Test 4 - Display brightness tests.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
                  Test 5 - Display scrolling tests
  \* ---------------------------------------------------------------- */
Test5:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #5 - Display scrolling");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Fill DisplayBuffer[] with bitmap corresponding to the ASCII string. */
  sprintf(String, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  ColumnPosition = 24; // put the bitmap beginning at position 22 + 2, just beyond the visible clock.

  /* Transfer the bitmap of each ASCII character to the display buffer. */
  for (Loop1UInt8 = 0; Loop1UInt8 < strlen(String); ++Loop1UInt8)
  {
    SectionNumber = ColumnPosition / 8;   // determine the target section number.
    ColumnInSection = ColumnPosition % 8; // determine the target column number in this section.

    if (ColumnPosition > (UINT16)DISPLAY_BUFFER_SIZE)
      break; // get out of the loop if the string is longer than allowed by display buffer size.

    for (RowNumber = 1; RowNumber < 8; ++RowNumber)
    {
      if (ColumnInSection > 0)
      {
        DisplayBuffer[(SectionNumber * 8) + RowNumber] = (DisplayBuffer[(SectionNumber * 8) + RowNumber] & (0xFF >> (8 - ColumnInSection))) | ((reverse_bits(CharMap[((String[Loop1UInt8] - 0x1E) * 7) + RowNumber - 1])) << ColumnInSection);
        if (SectionNumber < (sizeof(DisplayBuffer) / 8) - 1)
        {
          DisplayBuffer[(SectionNumber * 8) + 8 + RowNumber] = (DisplayBuffer[(SectionNumber * 8) + 8 + RowNumber] & (0xFF << (8 - ColumnInSection))) | ((reverse_bits(CharMap[((String[Loop1UInt8] - 0x1E) * 7) + RowNumber - 1])) >> (8 - ColumnInSection));
        }
      }
      else
      {
        DisplayBuffer[(SectionNumber * 8) + RowNumber] = (reverse_bits(CharMap[((String[Loop1UInt8] - 0x1E) * 7) + RowNumber - 1]));
      }
    }

    ColumnPosition += CharWidth[String[Loop1UInt8] - 0x1E] + 1; // add 1 space between characters.
  }

  /* ---------------------------------------------------------------- *\
                Then, scroll the bitmap on the display
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 1; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
  {
    for (RowNumber = 1; RowNumber < 8; ++RowNumber)
    {
      CharacterBuffer = DisplayBuffer[RowNumber] & 0x03;

      for (SectionNumber = 0; SectionNumber < sizeof(DisplayBuffer) / 8; ++SectionNumber)
      {
        if (SectionNumber < sizeof(DisplayBuffer) / 8 - 1)
          DisplayBuffer[SectionNumber * 8 + RowNumber] = DisplayBuffer[SectionNumber * 8 + RowNumber] >> 1 | DisplayBuffer[SectionNumber * 8 + RowNumber + 8] << 7;
        else
          DisplayBuffer[SectionNumber * 8 + RowNumber] = DisplayBuffer[SectionNumber * 8 + RowNumber] >> 1;
      }
      DisplayBuffer[RowNumber] = (DisplayBuffer[RowNumber] & (~0x03)) | CharacterBuffer;
    }
    sleep_ms(SCROLL_DOT_TIME);
  }
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  return;
  /* ---------------------------------------------------------------- *\
                END - Test 5 - Display scrolling tests
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
       Test 6 - Scroll all 5 X 7 characters to test variable width
                       and character spacing.
  \* ---------------------------------------------------------------- */
Test6:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);

  /* Announce test number. */
  scroll_string(24, "Test #6 - 5 X 7 character set");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete current scrolling.

  /* Scan all ASCII characters that are defined in the bitmap table. */
  for (Loop1UInt8 = 0x80; Loop1UInt8 <= 0x91; ++Loop1UInt8)  /// 0x1E = start character
  {
    /* Indicate which ASCII character we are scrolling. */
    sprintf(String, "0x%2.2X - ", Loop1UInt8);

    /* Put 10 characters side by side to see character width and spacing. */
    for (Loop2UInt8 = 7; Loop2UInt8 < 17; ++Loop2UInt8)
    {
      String[Loop2UInt8] = Loop1UInt8;
    }

    String[17] = 0x00;         // add end-of-string.
    scroll_string(22, String); // and scroll the data.

    while (ScrollDotCount)
      sleep_ms(100); // let the time to complete scrolling.
  }

  return;
  /* ---------------------------------------------------------------- *\
    END - Test 6 - Scroll all 5 X 7 characters to test variable width
                        and character spacing.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
              Test 7 - Verify the 4 X 7 character bitmaps.
  \* ---------------------------------------------------------------- */
Test7:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #7 - 4 X 7 character bitmap");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  for (Loop1UInt8 = 0x2D; Loop1UInt8 < 0x2D + 53; ++Loop1UInt8)
  {
    /* Clear framebuffer. */
    clear_framebuffer(0);
    fill_display_buffer_4X7(8, Loop1UInt8);
    sleep_ms(1000);
  }

  return;
  /* ---------------------------------------------------------------- *\
           END - Test 7 - Verify the 4 X 7 character bitmaps.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
          Test 8 - Display all characters of the 5 X 7 bitmap,
             using three different chunk of scrolling text.
  \* ---------------------------------------------------------------- */
Test8:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);

  /* Announce test number. */
  scroll_string(24, "Test #8 - 5 X 7 character bitmap");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Wipe String[] on entry. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    String[Loop1UInt8] = 0x00;

  /* Scroll first bunch of characters on the display. */
  String[0]  = 0x1E;
  String[1]  = 0x1F;
  String[2]  = 0x20;
  String[3]  = 0x21;
  String[4]  = 0x22;
  String[5]  = 0x23;
  String[6]  = 0x24;
  String[7]  = 0x25;
  String[8]  = 0x26;
  String[9]  = 0x27;
  String[10] = 0x28;
  String[11] = 0x29;
  String[12] = 0x2A;
  String[13] = 0x2B;
  String[14] = 0x2C;
  String[15] = 0x2D;
  String[16] = 0x2E;
  String[17] = 0x2F;
  String[18] = 0x30;
  String[19] = 0x31;
  String[20] = 0x32;
  String[21] = 0x33;
  String[22] = 0x34;
  String[23] = 0x35;
  String[24] = 0x36;
  String[25] = 0x37;
  String[26] = 0x38;
  String[27] = 0x39;
  scroll_string(24, String);
  sleep_ms(1000);

  /* Clear framebuffer. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Wipe String[] on entry. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    String[Loop1UInt8] = 0x00;

  /* Display another bunch of characters on the display. */
  String[0]  = 0x3A;
  String[1]  = 0x3B;
  String[2]  = 0x3C;
  String[3]  = 0x3D;
  String[4]  = 0x3E;
  String[5]  = 0x3F;
  String[6]  = 0x40;
  String[7]  = 0x41;
  String[8]  = 0x42;
  String[9]  = 0x43;
  String[10] = 0x44;
  String[11] = 0x45;
  String[12] = 0x46;
  String[13] = 0x47;
  String[14] = 0x48;
  String[15] = 0x49;
  String[16] = 0x4A;
  String[17] = 0x4B;
  String[18] = 0x4C;
  String[19] = 0x4D;
  String[20] = 0x4E;
  String[21] = 0x4F;
  String[22] = 0x50;
  String[23] = 0x51;
  String[24] = 0x52;
  String[25] = 0x53;
  String[26] = 0x54;
  String[27] = 0x55;
  String[28] = 0x56;
  String[29] = 0x57;
  String[30] = 0x58;
  String[31] = 0x59;
  String[32] = 0x5A;
  scroll_string(24, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Clear framebuffer. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Wipe String[] on entry. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    String[Loop1UInt8] = 0x00;

  /* Display last bunch of characters. */
  String[0] = 0x5B;
  String[1] = 0x5C;
  String[2] = 0x5D;
  String[3] = 0x5E;
  String[4] = 0x5F;
  String[0] = 0x60;
  String[0] = 0x61;
  String[1] = 0x62;
  String[2] = 0x63;
  String[3] = 0x64;
  String[4] = 0x65;
  String[5] = 0x66;
  String[6] = 0x67;
  String[7] = 0x68;
  String[8] = 0x69;
  String[9] = 0x6A;
  String[10] = 0x6B;
  String[11] = 0x6C;
  String[12] = 0x6D;
  String[13] = 0x6E;
  String[14] = 0x6F;
  String[15] = 0x70;
  String[16] = 0x71;
  String[17] = 0x72;
  String[18] = 0x73;
  String[19] = 0x74;
  String[20] = 0x75;
  String[21] = 0x76;
  String[22] = 0x77;
  String[23] = 0x78;
  String[24] = 0x79;
  String[25] = 0x7A;
  String[26] = 0x7B;
  String[27] = 0x7C;
  String[28] = 0x7D;
  String[29] = 0x7E;
  String[30] = 0x7F;
  String[29] = 0x80;
  String[30] = 0x81;
  scroll_string(24, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  return;
  /* ---------------------------------------------------------------- *\
        END - Test 8 - Display all characters of the 5 X 7 bitmap,
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
                   Test 9 - Tests with sound queues.
  \* ---------------------------------------------------------------- */

Test9:

  /* Tone to announce entering a new test. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 4; ++Loop1UInt8)
  {
    tone(10);
    sleep_ms(50);
  }

  /* Announce test number. */
  uart_send(__LINE__, "==========---------- Test #9 ----------==========\r");
  uart_send(__LINE__, "              PWM for Passive buzzer\r\r\r");



  /* Cancel sound callback during tests. -3.1416 marker- */
  cancel_repeating_timer(&Timer50MSec);


  while (1)
  {
    display_pwm(&Pwm[PWM_SOUND], "Current values for sound PWM.\r");

    printf("\r\r\r\r");


    printf("1- Change sound On / Off\r");
    printf("2- Change clock divider value\r");
    printf("3- Change counter wrap value\r");
    printf("4- Change level value\r");
    printf("5- Change duty cycle value\r");
    printf("6- Phase correct mode\r");
    printf("7- Change counter wrap value with duty cycle following\r");
    printf("\r");


    printf("Enter an option: ");
    input_string(String);
    if (String[0] == 0x0D)
    {
      printf("\r");
      break;
    }
    Dum1UInt8 = atoi(String);


    switch(Dum1UInt8)
    {
      case (1):
        /* Change sound On / Off. */
        printf("Enter 0 for Off and 1 for On (8 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Dum1UInt8 = atoi(String);
        if (Dum1UInt8 == 0)
        {
          Pwm[PWM_SOUND].OnOff = FLAG_OFF;
          pwm_set_enabled(Pwm[PWM_SOUND].Slice, FALSE);
        }

        if (Dum1UInt8 == 1)
        {
          Pwm[PWM_SOUND].OnOff = FLAG_ON;
          pwm_set_enabled(Pwm[PWM_SOUND].Slice, TRUE);
        }
        printf("\r\r");
      break;

      case (2):
        /* Change clock divider value. */
        printf("Enter new clock divider value (float): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_SOUND].ClockDivider = atof(String);
        pwm_set_clkdiv(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].ClockDivider);
        printf("\r\r");
      break;

      case (3):
        /* Change counter wrap value. */
        printf("Enter new counter wrap value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_SOUND].Wrap = atoi(String);
        pwm_set_wrap(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].Wrap);
        printf("\r\r");
      break;

      case (4):
        /* Change level value. */
        printf("Enter new level value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_SOUND].Level = atoi(String);
        pwm_set_chan_level(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].Channel, Pwm[PWM_SOUND].Level);
        printf("\r\r");
      break;

      case (5):
        /* Change duty cycle value. */
        printf("Enter new duty cycle value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_SOUND].Cycles = atoi(String);
        if (Pwm[PWM_SOUND].Cycles > 100) Pwm[PWM_SOUND].Cycles = 100;  // Validate value specified for duty cycle.
        Pwm[PWM_SOUND].Level = (UINT16)(Pwm[PWM_SOUND].Wrap * (Pwm[PWM_SOUND].Cycles / 100.0));
        pwm_set_chan_level(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].Channel, Pwm[PWM_SOUND].Level);
        printf("\r\r");
      break;

      case (6):
        /* Change phase correct mode. */
        printf("Enter 0 for no phase correct and 1 for phase correct (8 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Dum1UInt8 = atoi(String);
        if (Dum1UInt8 == 0) pwm_set_phase_correct(Pwm[PWM_SOUND].Slice, FALSE);
        if (Dum1UInt8 == 1) pwm_set_phase_correct(Pwm[PWM_SOUND].Slice, TRUE);
        printf("\r\r");
      break;

      case (7):
        /* Change counter wrap value with duty cycle following. */
        printf("Enter new counter wrap value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_SOUND].Wrap = atoi(String);
        pwm_set_wrap(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].Wrap);
        /* Set duty cycle to 50%. */
        Pwm[PWM_SOUND].Level = (UINT16)(Dum1UInt16 / 2);
        pwm_set_chan_level(Pwm[PWM_SOUND].Slice, Pwm[PWM_SOUND].Channel, Pwm[PWM_SOUND].Level);
        printf("\r\r");
      break;
    }
  }

  /* Restart sound callback when done with tests. */
  add_repeating_timer_ms(-50, sound_callback_ms, NULL, &Timer50MSec);


  #ifdef PASSIVE_PIEZO_SUPPORT
  /* Passive sound queue tests. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(60, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(100, 15);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(250, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(400, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  /* Passive sound queue tests. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(1000, 50);
    sound_queue_passive(SILENT, 50);
  }


  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(1500, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(2500, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(4000,  50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(5000, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(6000, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(7000, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");


  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    sound_queue_passive(8000, 50);
    sound_queue_passive(SILENT, 50);
  }

  sleep_ms(2200);
  printf("Press <Enter> to continue: ");
  input_string(String);
  printf("\r");

  return;
  #endif // PASSIVE_PIEZO_SUPPORT



  /* Active sound queue tests. */
  sound_queue_active(200, 5);
  sound_queue_active(700, SILENT);

  sound_queue_active(40, 4);
  sound_queue_active(700, SILENT);

  sound_queue_active(50, 3);
  sound_queue_active(700, SILENT);

  sound_queue_active(50, 15);
  sound_queue_active(700, SILENT);

  sound_queue_active(40, 5);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 4);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 3);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 2);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 1);

  sound_queue_active(2000, SILENT);

  sound_queue_active(60, 5);
  sound_queue_active(700, SILENT);
  sound_queue_active(60, 4);
  sound_queue_active(700, SILENT);
  sound_queue_active(60, 3);
  sound_queue_active(700, SILENT);
  sound_queue_active(60, 2);
  sound_queue_active(700, SILENT);
  sound_queue_active(60, 1);

  sound_queue_active(2000, SILENT);

  sound_queue_active(50, 3);
  sound_queue_active(700, SILENT);
  sound_queue_active(50, 3);
  sound_queue_active(700, SILENT);
  sound_queue_active(50, 3);

  sound_queue_active(2000, SILENT);

  sound_queue_active(40, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(40, 1);

  sound_queue_active(2000, SILENT);

  /* Minimum sound duration is 50 msec. */
  sound_queue_active(20, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(20, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(20, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(20, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(20, 1);

  sound_queue_active(2000, SILENT);

  sound_queue_active(300, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(300, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(300, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(300, 1);
  sound_queue_active(700, SILENT);
  sound_queue_active(300, 1);

  sound_queue_active(2000, SILENT);

  return;
  /* ---------------------------------------------------------------- *\
                END - Test 9 - Tests with sound queues.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
                   Test 10 - Tests with indicators.
  \* ---------------------------------------------------------------- */
Test10:

  /* NOTE: Test 10 will turn on each bit of each byte of the main display matrix
           (excluding the indicators), and then blink the 2 white LEDs
           and display a prompt on the display, waiting for a "top button" press.
           Once the "top button" has been pressed, process will continue with next
           byte of the display. Source code will give more information about how
           the program is built and the exceptions covered. */

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #10 - Indicators test");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Scan all bytes of the clock display matrix. */
  for (Loop1UInt8 = 1; Loop1UInt8 < 24; ++Loop1UInt8)
  {
    if (Loop1UInt8 == 16)
      IndicatorButtonLightsOn; // To check two while LEDs indicators.

    /* DisplayBuffer[8] and [16] are reserved for indicators bitmap, so they are skipped. */
    if ((Loop1UInt8 == 8) || (Loop1UInt8 == 16))
    {
      tone(60); // tone the user to indicate that a byte has been skipped.
      sleep_ms(1200);
      continue;
    }

    /* Use "OR" so that the indicators remains untouched on the display. */
    /* We also cumulate the dots on the clock display. */
    if (Loop1UInt8 > 8)
    {
      /* First display chunk uses bits 0 and 1 for indicators, but not chunk 1 and not chunk 2. */
      DisplayBuffer[Loop1UInt8] |= 0x01;
      sleep_ms(100);

      DisplayBuffer[Loop1UInt8] |= 0x02;
      sleep_ms(100);
    }

    DisplayBuffer[Loop1UInt8] |= 0x04;
    sleep_ms(100);

    DisplayBuffer[Loop1UInt8] |= 0x08;
    sleep_ms(100);

    DisplayBuffer[Loop1UInt8] |= 0x10;
    sleep_ms(100);

    DisplayBuffer[Loop1UInt8] |= 0x20;
    sleep_ms(100);

    DisplayBuffer[Loop1UInt8] |= 0x40;
    sleep_ms(100);

    DisplayBuffer[Loop1UInt8] |= 0x80;
    sleep_ms(1300);

    /* Clear framebuffer. */
    clear_framebuffer(0);

    /* Wait for "Set" (top) button to be pressed... */
    /* Blink the two white LEDs near the buttons to show that we are waiting for "Set" button press,
       then put a "prompt" on the clock display. */
    IndicatorButtonLightsOn;
    sleep_ms(500);
    IndicatorButtonLightsOff;
    fill_display_buffer_5X7(2, '>');
    while (gpio_get(SET_BUTTON))
      ;

    /* Clear framebuffer. */
    clear_framebuffer(0);

    sleep_ms(300);
  }
  sleep_ms(1000);

  return;
  /* ---------------------------------------------------------------- *\
                END - Test 10 - Tests with indicators.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
         Test 11 - Test to visualize variable character width.
  \* ---------------------------------------------------------------- */
Test11:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #11 - Variable character width");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Put a left frame. */
  for (Loop1UInt8 = 9; Loop1UInt8 < 16; ++Loop1UInt8)
    DisplayBuffer[Loop1UInt8] |= 0x40;
  sleep_ms(500);

  /* Put a right frame. */
  for (Loop1UInt8 = 17; Loop1UInt8 < 24; ++Loop1UInt8)
    DisplayBuffer[Loop1UInt8] |= 0x10;
  sleep_ms(500);

  /* Now, display each character in between those frames. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 98; ++Loop1UInt8)
  {
    fill_display_buffer_4X7(0, Loop1UInt8 / 10 + '0');  // display first digit of character number.
    fill_display_buffer_4X7(5, Loop1UInt8 % 10 + '0');  // display second digit of character number.

    /* Erase first column of the digit to be displayed. */
    for (Loop2UInt8 = 9; Loop2UInt8 < 16; ++Loop2UInt8)
      DisplayBuffer[Loop2UInt8] &= 0x7F;

    /* Erase last four columns of the digit to be displayed. */
    for (Loop2UInt8 = 17; Loop2UInt8 < 24; ++Loop2UInt8)
      DisplayBuffer[Loop2UInt8] &= 0xF0;

    sleep_ms(500);
    fill_display_buffer_5X7(15, (UINT8)(0x1E + Loop1UInt8));
    sleep_ms(3000);
    // while (gpio_get(UP_BUTTON));  // wait for button press.
  }

  return;
  /* ---------------------------------------------------------------- *\
       END - Test 11 - Test to visualize variable character width.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
                  Test 12 - Test 5 X 7 character bitmap
  \* ---------------------------------------------------------------- */
Test12:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #12 - 5 X 7 character bitmap");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Scan all available ASCII characters. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 100; ++Loop1UInt8)
  {
    /* Clear framebuffer. */
    clear_framebuffer(0);

    for (RowNumber = 1; RowNumber < 8; ++RowNumber)
    {
      /* We need to reverse bit order of the character bitmap since the left-most column
         on the clock display corresponds to the lowest bit. */
      DisplayBuffer[8 + RowNumber] = reverse_bits(CharMap[(Loop1UInt8 * 7) + RowNumber - 1]);
    }
    sleep_ms(500);
  }
  sleep_ms(500);

  /* Wait for "Set" button to be pressed... */
  /* Blink the two white LEDs near the buttons to show that we are waiting for "Set" button press,
     then put a "prompt" on the clock display. */
  IndicatorButtonLightsOn;
  sleep_ms(500);
  IndicatorButtonLightsOff;
  sleep_ms(500);
  fill_display_buffer_5X7(2, '>');
  while (gpio_get(SET_BUTTON))
    ;

  return;
  /* ---------------------------------------------------------------- *\
                END - Test 12 - Test 5 X 7 character map
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
              Test 13 - Scrolling test with 5 X 7 matrix.
  \* ---------------------------------------------------------------- */
Test13:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #13 - 5 X 7 characters scrolling");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Wipe String[] on entry. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    String[Loop1UInt8] = 0x00;

  sprintf(String, "A");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "B");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "AA");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "BB");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "AB");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "ABAB");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Wait for "Set" button to be pressed... */
  /* Blink the two white LEDs near the buttons to show that we are waiting for "Set" button press,
     then put a "prompt" on the clock display. */
  IndicatorButtonLightsOn;
  sleep_ms(500);
  IndicatorButtonLightsOff;
  sleep_ms(500);
  fill_display_buffer_5X7(2, '>');
  while (gpio_get(SET_BUTTON))
    ;

  return;
  /* ---------------------------------------------------------------- *\
           END - Test 13 - Scrolling test with 5 X 7 matrix.
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
              Test 14 - Scrolling test with 4 X 7 matrix.
  \* ---------------------------------------------------------------- */
Test14:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Prevent dots blinking in the clock. */
  CurrentClockMode = MODE_TEST;

  /* Announce test number. */
  scroll_string(4, "Test #14 - 4 X 7 characters scrolling");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  fill_display_buffer_5X7(5,   'A');
  fill_display_buffer_5X7(11,  'B');
  fill_display_buffer_5X7(17,  'C');
  fill_display_buffer_5X7(23,  'D');
  fill_display_buffer_5X7(29,  'E');
  fill_display_buffer_5X7(35,  'F');
  fill_display_buffer_5X7(41,  'G');
  fill_display_buffer_5X7(47,  'H');
  fill_display_buffer_5X7(53,  'I');
  fill_display_buffer_5X7(59,  'J');
  fill_display_buffer_5X7(65,  'K');
  fill_display_buffer_5X7(71,  'L');
  fill_display_buffer_5X7(77,  'M');
  fill_display_buffer_5X7(83,  'N');
  fill_display_buffer_5X7(90,  'O');
  fill_display_buffer_5X7(96,  'P');
  fill_display_buffer_5X7(102, 'Q');
  fill_display_buffer_5X7(108, 'R');
  fill_display_buffer_5X7(114, 'S');
  fill_display_buffer_5X7(120, 'T');
  fill_display_buffer_5X7(126, 'U');
  fill_display_buffer_5X7(132, 'V');
  fill_display_buffer_5X7(138, 'W');
  fill_display_buffer_5X7(144, 'X');
  fill_display_buffer_5X7(150, 'Y');
  fill_display_buffer_5X7(156, 'Z');
  sleep_ms(1000);

  /* ---------------------------------------------------------------- *\
                 Scroll the framebuffer on the display.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
  {
    /* Scan the 7 rows of data. Row 0 is reserved for "indicators". */
    for (RowNumber = 1; RowNumber < 8; ++RowNumber)
    {
      CharacterBuffer = DisplayBuffer[RowNumber] & 0x03; // keep track of the "indicators" status (first 2 bits).

      sleep_ms(10); // value can be increase (up tp 255) to better see scroll movement.

      /* Scan all "sections" of the framebuffer. One section is 8 bits width. */
      for (SectionNumber = 0; SectionNumber < (sizeof(DisplayBuffer) / 8); ++SectionNumber)
      {
        /* If we're not at the last section of the virtual display buffer, move one bit to the left and add the upmost bit of the next section. */
        if (SectionNumber < sizeof(DisplayBuffer) / 8 - 1)
          DisplayBuffer[(SectionNumber * 8) + RowNumber] = DisplayBuffer[(SectionNumber * 8) + RowNumber] >> 1 | DisplayBuffer[(SectionNumber * 8) + RowNumber + 8] << 7;
        else
          DisplayBuffer[(SectionNumber * 8) + RowNumber] = DisplayBuffer[(SectionNumber * 8) + RowNumber] >> 1;
      }
      DisplayBuffer[RowNumber] = (DisplayBuffer[RowNumber] & (~0x03)) | CharacterBuffer; // restore the "indicators" status.
    }
    sleep_ms(100);
  }

  /* Display time for a few seconds. */
  show_time();
  sleep_ms(5000);

  /* Prevent dots blinking in the clock. */
  CurrentClockMode = MODE_TEST;

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Test the fill_display_buffer_4X7() function for 4 X 7 characters...
     ... first in the "physical" section of the display memory. */
  fill_display_buffer_4X7(0,  '0');
  sleep_ms(500);
  fill_display_buffer_4X7(5,  '1');
  sleep_ms(500);
  fill_display_buffer_4X7(10, '2');
  sleep_ms(500);
  fill_display_buffer_4X7(15, '3');
  sleep_ms(500);
  fill_display_buffer_4X7(20, '4');
  sleep_ms(5000);
  ScrollSecondCounter = 0; // delay eventual date display on clock display
  ScrollDotCount = 24;
  FlagScrollStart = FLAG_ON;

  /* Pause before next test. */
  sleep_ms(5000);

  /* Tone to announce end of test. */
  tone(10);
  sleep_ms(100);

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Prevent dots blinking in the clock. */
  CurrentClockMode = MODE_TEST;

  /* Do the same thing, this time in the virtual section of the display memory. */
  fill_display_buffer_4X7(23, 'A');
  fill_display_buffer_4X7(28, 'B');
  fill_display_buffer_4X7(33, 'C');
  fill_display_buffer_4X7(38, 'D');
  fill_display_buffer_4X7(43, 'E');
  fill_display_buffer_4X7(48, 'F');
  fill_display_buffer_4X7(53, 'G');
  fill_display_buffer_4X7(58, 'H');
  fill_display_buffer_4X7(63, 'I');
  ScrollDotCount = 66;

  FlagScrollData = FLAG_OFF; // we don't want to fill display buffer with the date before scrolling.
  FlagScrollStart = FLAG_ON;

  /* Pause before next test. */
  sleep_ms(5000);

  /* Tone to announce end of test. */
  tone(10);
  sleep_ms(100);

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Prevent dots blinking in the clock. */
  CurrentClockMode = MODE_TEST;

  /* Put characters 0 to 9 in framebuffer. */
  for (Loop1UInt8 = 0; Loop1UInt8 <= 9; ++Loop1UInt8)
    fill_display_buffer_4X7((Loop1UInt8 * 6) + 10, Loop1UInt8 + 0x30);

  /* Then scroll them... */
  for (Loop1UInt8 = 1; Loop1UInt8 < ((strlen(DisplayBuffer) * 6) + 69); ++Loop1UInt8)
  {
    for (Loop2UInt8 = 1; Loop2UInt8 < 8; ++Loop2UInt8)
    {
      CharacterBuffer = DisplayBuffer[Loop2UInt8] & 0x03; // keep track of "indicators" status (first 2 bits).

      for (Loop3UInt8 = 0; Loop3UInt8 < sizeof(DisplayBuffer) / 8; ++Loop3UInt8)
      {
        if (Loop3UInt8 < sizeof(DisplayBuffer) / 8 - 1)
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1 | DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8 + 8] << 7;
        else
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1;
      }

      DisplayBuffer[Loop2UInt8] = (DisplayBuffer[Loop2UInt8] & (~0x03)) | CharacterBuffer; // restore "indicators" status.
    }
    sleep_ms(SCROLL_DOT_TIME);
  }

  /* Pause before next test. */
  sleep_ms(5000);

  /* Clear framebuffer before next test. */
  clear_framebuffer(0);

  /* Put characters of the alphabet (+ one extra character to show out-of-bound) in framebuffer. */
  for (Loop1UInt8 = 0; Loop1UInt8 <= 26; ++Loop1UInt8)
    fill_display_buffer_4X7((Loop1UInt8 * 6) + 10, Loop1UInt8 + 0x41);

  /* Then scroll them... */
  for (Loop1UInt8 = 1; Loop1UInt8 < ((strlen(DisplayBuffer) * 6) + 171); ++Loop1UInt8)
  {
    for (Loop2UInt8 = 1; Loop2UInt8 < 8; ++Loop2UInt8)
    {
      CharacterBuffer = DisplayBuffer[Loop2UInt8] & 0x03; // keep track of "indicators" status (first 2 bits).

      for (Loop3UInt8 = 0; Loop3UInt8 < sizeof(DisplayBuffer) / 8; ++Loop3UInt8)
      {
        if (Loop3UInt8 < sizeof(DisplayBuffer) / 8 - 1)
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1 | DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8 + 8] << 7;
        else
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1;
      }

      DisplayBuffer[Loop2UInt8] = (DisplayBuffer[Loop2UInt8] & (~0x03)) | CharacterBuffer; // restore "indicators" status.
    }
    sleep_ms(SCROLL_DOT_TIME); // time delay to scroll one dot to the left.
  }
  sleep_ms(5000);

  return;
  /* ---------------------------------------------------------------- *\
           END - Test 14 - Scrolling test with 4 X 7 matrix.
  \* ---------------------------------------------------------------- */

/* ------------------------------------------------------------------ *\
            Test 15 - Display some info on clock display.
\* ------------------------------------------------------------------ */
Test15:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);

  /* Announce test number. */
  scroll_string(24, "Test #15 - Display technical information");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "strlen(DisplayBuffer) = %u", strlen(DisplayBuffer));
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "sizeof(DisplayBuffer) = %u", sizeof(DisplayBuffer));
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "Value of TRUE = %u", TRUE);
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  sprintf(String, "Value of FALSE = %u", FALSE);
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  return;
/* ------------------------------------------------------------------ *\
          END - Test 15 - Display some info on clock display.
\* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ *\
               Test 16 - Turn ON and OFF in sequence
             each bit of each byte of the DisplayBuffer.
\* ------------------------------------------------------------------ */
Test16:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #16 - Testing LED matrix bitmap");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  // Wipe DisplayBuffer on entry.
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    DisplayBuffer[Loop1UInt8] = 0x00;

  /* Scan all bytes of the visible display buffer. */
  // for (Loop1UChar = 0; Loop1UChar < 24; ++Loop1UChar)
  for (Loop1UInt8 = 0; Loop1UInt8 < 5; ++Loop1UInt8)
  {
    // First, display which byte of the DisplayBuffer[] we are testing.
    sprintf(String, "[%u]", Loop1UInt8);

    /* Wait for "Set" button to be pressed... */
    /* Blink the two white LEDs near the buttons to show that we are waiting
       for "Set" button press, then put a "prompt" on clock display. */
    IndicatorButtonLightsOn;
    sleep_ms(500);
    IndicatorButtonLightsOff;
    // sleep_ms(500);
    fill_display_buffer_5X7(2, '>');
    while (gpio_get(SET_BUTTON))
      ;

    ColumnPosition = 1; // will be auto-incremented in the fill_display_buffer_5X7() function.
    for (Loop2UInt8 = 0; Loop2UInt8 < strlen(String); ++Loop2UInt8)
    {
      if (ColumnPosition > (UINT16)DISPLAY_BUFFER_SIZE)
        break; // get out of the loop if the string is longer than allowed by display buffer size.

      ColumnPosition = fill_display_buffer_5X7(++ColumnPosition, String[Loop2UInt8]);
    }
    sleep_ms(1000); // give time for user to read the byte under test.

    /* Blank the whole display. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0;
    sleep_ms(200); // wait some time before scrolling the dot on the display.

    /* Now, test each bit of the byte under test. */
    for (Loop2UInt8 = 0; Loop2UInt8 < 8; ++Loop2UInt8)
    {
      DisplayBuffer[Loop1UInt8] = 1 << Loop2UInt8;
      sleep_ms(400);
    }

    /* Blank the whole display again when done with this byte. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0;
  }

  return;
/* ------------------------------------------------------------------ *\
             END - Test 16 - Turn ON and OFF in sequence
             each bit of each byte of the DisplayBuffer.
\* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ *\
          Test 17 - Play with weekday indicators in sequence.
\* ------------------------------------------------------------------ */
Test17:

  /* Temporary section below to see impact of certain actions on clock display. */
  uart_send(__LINE__, "Entering Test 17\r");
  sleep_ms(5000);


  uart_send(__LINE__, "Will clear framebuffer in 5 seconds\r");
  sleep_ms(5000);


  /* Clear framebuffer. */
  clear_framebuffer(0);
  sleep_ms(5000);


  uart_send(__LINE__, "Will update indicators in 5 seconds\r");
  sleep_ms(5000);


  update_left_indicators();
  sleep_ms(5000);


  uart_send(__LINE__, "Will make CurrentClockMode to UNDEFINED in 5 seconds\r");
  sleep_ms(5000);


  CurrentClockMode = MODE_UNDEFINED;
  sleep_ms(5000);


  uart_send(__LINE__, "Will clear all leds in 5 seconds\r");
  sleep_ms(5000);


  /* Clear all leds. */
  clear_all_leds();
  sleep_ms(5000);


  uart_send(__LINE__, "Will read DHT22 in 5 seconds\r");
  sleep_ms(5000);


  #ifdef DHT_SUPPORT
  if (read_dht(&Temperature, &Humidity))
    uart_send(__LINE__, "Error while trying to read DHT22\r");
  sleep_ms(5000);
  #endif


  uart_send(__LINE__, "Will restart clock in 5 seconds\r");
  sleep_ms(5000);


  show_time();
  sleep_ms(5000);


  uart_send(__LINE__, "Will update indicators in 5 seconds\r");
  sleep_ms(5000);


  update_left_indicators();
  sleep_ms(5000);


  return;




  uart_send(__LINE__, "Will stop TimerMSec timer in 5 seconds\r");
  sleep_ms(5000);

  cancel_repeating_timer(&TimerMSec);
  sleep_ms(5000);




  uart_send(__LINE__, "Will restart TimerMSec timer in 5 seconds\r");
  sleep_ms(5000);

  add_repeating_timer_ms(-1, timer_callback_ms, NULL, &TimerMSec);
  sleep_ms(5000);




  uart_send(__LINE__, "Will restart clock in 5 seconds\r");
  sleep_ms(5000);

  show_time();
  sleep_ms(5000);




  uart_send(__LINE__, "Will stop TimerSec timer in 5 seconds\r");
  sleep_ms(5000);

  cancel_repeating_timer(&TimerSec);
  sleep_ms(5000);




  uart_send(__LINE__, "Will restart TimerSec timer in 5 seconds\r");
  sleep_ms(5000);

  add_repeating_timer_ms(-1000, timer_callback_ms, NULL, &TimerSec);
  sleep_ms(5000);




  uart_send(__LINE__, "Will restart clock in 5 seconds\r");
  sleep_ms(5000);

  show_time();
  sleep_ms(5000);




  /* Tone to announce entering a new test. */
  tone(10);

  /* Announce test number. */
  scroll_string(24, "Test #17 - Tests with weekday indicators");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  // Turn ON, then OFF, each weekday indicator (2 LEDs per indicator).
  DisplayBuffer[0] |= (1 << 3) | (1 << 4); // Monday
  sleep_ms(500);
  DisplayBuffer[0] &= ~((1 << 3) | (1 << 4));
  sleep_ms(500);

  DisplayBuffer[0] |= (1 << 6) | (1 << 7); // Tuesday
  sleep_ms(500);
  DisplayBuffer[0] &= ~((1 << 6) | (1 << 7));
  sleep_ms(500);

  DisplayBuffer[8] |= (1 << 1) | (1 << 2); // Wednesday
  sleep_ms(500);
  DisplayBuffer[8] &= ~((1 << 1) | (1 << 2));
  sleep_ms(500);

  DisplayBuffer[8] |= (1 << 4) | (1 << 5); // Thursday
  sleep_ms(500);
  DisplayBuffer[8] &= ~((1 << 4) | (1 << 5));
  sleep_ms(500);

  DisplayBuffer[8]  |= (1 << 7); // Friday
  DisplayBuffer[16] |= (1 << 0);
  sleep_ms(500);
  DisplayBuffer[8]  &= ~(1 << 7);
  DisplayBuffer[16] &= ~(1 << 0);
  sleep_ms(500);

  DisplayBuffer[16] |= (1 << 2) | (1 << 3); // Saturday
  sleep_ms(500);
  DisplayBuffer[16] &= ~((1 << 2) | (1 << 3));
  sleep_ms(500);

  DisplayBuffer[16] |= (1 << 5) | (1 << 6); // Sunday
  sleep_ms(500);
  DisplayBuffer[16] &= ~((1 << 5) | (1 << 6));
  sleep_ms(500);

  // Now do the same with all other display indicators.
  DisplayBuffer[0] |= 0X03; // scroll indicator
  sleep_ms(500);
  DisplayBuffer[0] &= ~0X03;
  sleep_ms(500);
  DisplayBuffer[1] |= 0X03; // alarm indicator
  sleep_ms(500);
  DisplayBuffer[1] &= ~0x03;
  sleep_ms(500);
  DisplayBuffer[2] |= 0X03; // count down timer indicator
  sleep_ms(500);
  DisplayBuffer[2] &= ~0x03;
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 0); // Farenheit indicator
  sleep_ms(500);
  DisplayBuffer[3] &= ~(1 << 0);
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 1); // Celsius indicator
  sleep_ms(500);
  DisplayBuffer[3] &= ~(1 << 1);
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 0); // AM indicator
  sleep_ms(500);
  DisplayBuffer[4] &= ~(1 << 0);
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 1); // PM indicator
  sleep_ms(500);
  DisplayBuffer[4] &= ~(1 << 1);
  sleep_ms(500);
  DisplayBuffer[5] |= 0X03; // count up timer indicator
  sleep_ms(500);
  DisplayBuffer[5] &= ~0x03;
  sleep_ms(500);
  DisplayBuffer[6] |= 0X03; // hourly chime indicator
  sleep_ms(500);
  DisplayBuffer[6] &= ~0X03;
  sleep_ms(500);
  DisplayBuffer[7] |= 0X03; // auto brightness indicator
  sleep_ms(500);
  DisplayBuffer[7] &= ~0X03;
  sleep_ms(500);
  DisplayBuffer[0] |= (1 << 2) | (1 << 5); // two white LEDs near the buttons inside the clock
  sleep_ms(500);
  DisplayBuffer[0] &= ~((1 << 2) | (1 << 5));
  sleep_ms(500);

  /* Turn ON all weekday indicators, one after the other. */
  DisplayBuffer[0] |= (1 << 3) | (1 << 4); // Monday
  sleep_ms(500);
  DisplayBuffer[0] |= (1 << 6) | (1 << 7); // Tuesday
  sleep_ms(500);
  DisplayBuffer[8] |= (1 << 1) | (1 << 2); // Wednesday
  sleep_ms(500);
  DisplayBuffer[8] |= (1 << 4) | (1 << 5); // Thursday
  sleep_ms(500);
  DisplayBuffer[8] |= (1 << 7); // Friday
  DisplayBuffer[16] |= (1 << 0);
  sleep_ms(500);
  DisplayBuffer[16] |= (1 << 2) | (1 << 3); // Saturday
  sleep_ms(500);
  DisplayBuffer[16] |= (1 << 5) | (1 << 6); // Sunday
  sleep_ms(500);

  /* Turn ON all left-side indicators, one after the other. */
  DisplayBuffer[0] |= 0X03; // scroll indicator
  sleep_ms(500);
  DisplayBuffer[1] |= 0X03; // alarm indicator
  sleep_ms(500);
  DisplayBuffer[2] |= 0X03; // timer indicator
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 0); // Farenheit indicator
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 1); // Celsius indicator
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 0); // AM indicator
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 1); // PM indicator
  sleep_ms(500);
  DisplayBuffer[5] |= 0X03; // count up indicator ??
  sleep_ms(500);
  DisplayBuffer[6] |= 0X03; // hourly chime indicator
  sleep_ms(500);
  DisplayBuffer[7] |= 0X03; // auto light indicator
  sleep_ms(500);
  DisplayBuffer[0] |= (1 << 2) | (1 << 5); // back light indicator
  sleep_ms(500);

  /* Clear DisplayBuffer when done. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    DisplayBuffer[Loop1UInt8] = 0;
  sleep_ms(500);

  /* Display clock when done. */
  show_time();
  sleep_ms(500);

  /* Do the same thing again, using the defined macros. */
  IndicatorMondayOn;
  sleep_ms(500);
  IndicatorMondayOff;
  sleep_ms(500);
  IndicatorTuesdayOn;
  sleep_ms(500);
  IndicatorTuesdayOff;
  sleep_ms(500);
  IndicatorWednesdayOn;
  sleep_ms(500);
  IndicatorWednesdayOff;
  sleep_ms(500);
  IndicatorThursdayOn;
  sleep_ms(500);
  IndicatorThursdayOff;
  sleep_ms(500);
  IndicatorFridayOn;
  sleep_ms(500);
  IndicatorFridayOff;
  sleep_ms(500);
  IndicatorSaturdayOn;
  sleep_ms(500);
  IndicatorSaturdayOff;
  sleep_ms(500);
  IndicatorSundayOn;
  sleep_ms(500);
  IndicatorSundayOff;
  sleep_ms(500);
  IndicatorScrollOn;
  sleep_ms(500);
  IndicatorScrollOff;
  sleep_ms(500);
  IndicatorAlarmOn;
  sleep_ms(500);
  IndicatorAlarmOff;
  sleep_ms(500);
  IndicatorCountDownOn;
  sleep_ms(500);
  IndicatorCountDownOff;
  sleep_ms(500);
  IndicatorFrnhtOn;
  sleep_ms(500);
  IndicatorFrnhtOff;
  sleep_ms(500);
  IndicatorCelsiusOn;
  sleep_ms(500);
  IndicatorCelsiusOff;
  sleep_ms(500);
  IndicatorAmOn;
  sleep_ms(500);
  IndicatorAmOff;
  sleep_ms(500);
  IndicatorPmOn;
  sleep_ms(500);
  IndicatorPmOff;
  sleep_ms(500);
  IndicatorCountUpOn;
  sleep_ms(500);
  IndicatorCountUpOff;
  sleep_ms(500);
  IndicatorHourlyChimeOn;
  sleep_ms(500);
  IndicatorHourlyChimeOff;
  sleep_ms(500);
  IndicatorAutoLightOn;
  sleep_ms(500);
  IndicatorAutoLightOff;
  sleep_ms(500);
  IndicatorButtonLightsOn;
  sleep_ms(500);
  IndicatorButtonLightsOff;
  sleep_ms(500);

  /* Clear DisplayBuffer when done. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    DisplayBuffer[Loop1UInt8] = 0;
  sleep_ms(500);

  /* Display clock when done. */
  show_time();
  sleep_ms(500);

  return;
/* ------------------------------------------------------------------ *\
       END - Test 17 - Play with weekday indicators in sequence.
\* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ *\
           Test 18 - Test PWM for clock display brightness
\* ------------------------------------------------------------------ */
  UINT8 ColumnUp[23];
  UINT8 ColumnDown[23];
  UINT8 DurationSeconds;

  UINT64 CurrentTimer;
  UINT64 StartTimer;

Test18:

  /* Tone to announce entering a new test. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 4; ++Loop1UInt8)
  {
    tone(10);
    sleep_ms(50);
  }

  /* Announce test number. */
  uart_send(__LINE__, "==========---------- Test #18 ----------==========\r");
  uart_send(__LINE__, "      PWM to drive clock display brightness\r\r\r");


  /* Prevent blinking dots when scrolling is over. */
  CurrentClockMode = MODE_TEST;


  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Quick display test with 4 X 7 character bitmap. -6.28 Marker- */
  fill_display_buffer_4X7(1,  '1');
  fill_display_buffer_4X7(6,  '2');
  fill_display_buffer_4X7(11, '3');
  fill_display_buffer_4X7(16, '4');


  while (1)
  {
    display_pwm(&Pwm[PWM_BRIGHTNESS], "Current values for clock display PWM\r");


    printf("\r\r\r\r");


    printf("1- Change PWM On / Off\r");
    printf("2- Change clock divider value\r");
    printf("3- Change counter wrap value\r");
    printf("4- Change level value\r");
    printf("5- Change duty cycle value\r");
    printf("6- Phase correct mode\r");
    printf("7- Change counter wrap value with duty cycle following\r");
    printf("\r");
    printf("Enter an option: ");
    input_string(String);
    if (String[0] == 0x0D)
    {
      printf("\r");
      break;
    }
    Dum1UInt8 = atoi(String);
    switch(Dum1UInt8)
    {
      case (1):
        /* Change PWM On / Off. */
        printf("Enter 0 for Off and 1 for On (8 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Dum1UInt8 = atoi(String);
        if (Dum1UInt8 == 0)
        {
          Pwm[PWM_BRIGHTNESS].OnOff = FLAG_OFF;
          pwm_set_enabled(Pwm[PWM_BRIGHTNESS].Slice, FALSE);
        }

        if (Dum1UInt8 == 1)
        {
          Pwm[PWM_BRIGHTNESS].OnOff = FLAG_ON;
          pwm_set_enabled(Pwm[PWM_BRIGHTNESS].Slice, TRUE);
        }
        printf("\r\r");
      break;

      case (2):
        /* Change clock divider value. */
        printf("Enter new clock divider value (float): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_BRIGHTNESS].ClockDivider = atof(String);
        pwm_set_clkdiv(Pwm[PWM_BRIGHTNESS].Slice, Pwm[PWM_BRIGHTNESS].ClockDivider);
        printf("\r\r");
      break;

      case (3):
        /* Change counter wrap value. */
        printf("Enter new counter wrap value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_BRIGHTNESS].Wrap = atoi(String);
        pwm_set_wrap(Pwm[PWM_BRIGHTNESS].Slice, Pwm[PWM_BRIGHTNESS].Wrap);
        printf("\r\r");
      break;

      case (4):
        /* Change level value. */
        printf("Enter new level value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_BRIGHTNESS].Level = atoi(String);
        pwm_set_chan_level(Pwm[PWM_BRIGHTNESS].Slice, Pwm[PWM_BRIGHTNESS].Channel, Pwm[PWM_BRIGHTNESS].Level);
        printf("\r\r");
      break;

      case (5):
        /* Change duty cycle value. */
        printf("Enter new duty cycle value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_BRIGHTNESS].Cycles = atoi(String);
        if (Pwm[PWM_BRIGHTNESS].Cycles > BRIGHTNESS_LIGHTLEVELSTEP)
          Pwm[PWM_BRIGHTNESS].Cycles = BRIGHTNESS_LIGHTLEVELSTEP;  // Validate value specified for duty cycle.
        Pwm[PWM_BRIGHTNESS].Level = (UINT16)(Pwm[PWM_BRIGHTNESS].Wrap * ((BRIGHTNESS_LIGHTLEVELSTEP - Pwm[PWM_BRIGHTNESS].Cycles) / (BRIGHTNESS_LIGHTLEVELSTEP * 1.0)));
        pwm_set_chan_level(Pwm[PWM_BRIGHTNESS].Slice, Pwm[PWM_BRIGHTNESS].Channel, Pwm[PWM_BRIGHTNESS].Level);
        printf("\r\r");
      break;

      case (6):
        /* Change phase correct mode. */
        printf("Enter 0 for no phase correct and 1 for phase correct (8 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Dum1UInt8 = atoi(String);
        if (Dum1UInt8 == 0) pwm_set_phase_correct(Pwm[PWM_BRIGHTNESS].Slice, FALSE);
        if (Dum1UInt8 == 1) pwm_set_phase_correct(Pwm[PWM_BRIGHTNESS].Slice, TRUE);
        printf("\r\r");
      break;

      case (7):
        /* Change counter wrap value with duty cycle following. */
        printf("Enter new counter wrap value (16 bits): ");
        input_string(String);
        if (String[0] == 0x0D) continue;
        Pwm[PWM_BRIGHTNESS].Wrap = atoi(String);
        pwm_set_wrap(Pwm[PWM_BRIGHTNESS].Slice, Pwm[PWM_BRIGHTNESS].Wrap);
        /* Set duty cycle to 50%. */
        Pwm[PWM_BRIGHTNESS].Level = (UINT16)(Pwm[PWM_BRIGHTNESS].Wrap / 2);
        pwm_set_chan_level(Pwm[PWM_BRIGHTNESS].Slice, Pwm[PWM_BRIGHTNESS].Channel, Pwm[PWM_BRIGHTNESS].Level);
        printf("\r\r");
      break;
    }
  }

  return;



  /* Disable auto-brightness mode before tests. */
  FlashConfig.FlagAutoBrightness == FLAG_OFF;



  if (DebugBitMask & DEBUG_BRIGHTNESS)
  {
    uart_send(__LINE__, "Going to play with duty cycle");
    sleep_ms(2000);
  }

  uart_send(__LINE__, "CurrentClockMode: %u\r", CurrentClockMode);

  for (Loop1UInt16 = 100; Loop1UInt16 > 0; --Loop1UInt16)
  {
    pwm_set_cycles(Loop1UInt16);

    sprintf(String, "%4.4u", Loop1UInt16);
    fill_display_buffer_4X7(3,  String[0]);
    fill_display_buffer_4X7(8,  String[1]);
    fill_display_buffer_4X7(13, String[2]);
    fill_display_buffer_4X7(18, String[3]);

    sleep_ms(600);

    clear_framebuffer(0);
    sleep_ms(100);
  }



  /* Put back auto-brightness mode when tests are over. */
  FlashConfig.FlagAutoBrightness == FLAG_ON;



  /* Clear framebuffer when done. */
  clear_framebuffer(0);



  /* -------------------------- Snowflakes animation. -------------------------- */
  /* Initialize Up and Down limits. */
  for (Loop1UInt16 = 0; Loop1UInt16 < 23; ++Loop1UInt16)
  {
    ColumnUp[Loop1UInt16]   = 0;
    ColumnDown[Loop1UInt16] = 0;
  }


  /* Perform animation for the specified number of seconds. */
  DurationSeconds = 15;
  StartTimer = (UINT64)time_us_64();
  while ((CurrentTimer = (UINT64)time_us_64()) < (StartTimer + (DurationSeconds * 1000000)))
  {
    do
    {
      Column = ((rand() % 22) + 1);
    } while (Column % 2);

    if (DebugBitMask & DEBUG_TEMP)
      uart_send(__LINE__, "Random Column: %2u   ColumnUp[%2u]: %u   ColumnDown[%2u]: %u ", Column, Column, ColumnUp[Column], Column, ColumnDown[Column]);


    if (CurrentTimer < (StartTimer + ((DurationSeconds - 3) * 1000000)))
      uart_send(__LINE__, "- Not the time to purge yet...\r");
    else
      uart_send(__LINE__, "- Time to purge NOW...\r");


    if ((ColumnUp[Column] == 0) && (CurrentTimer < (StartTimer + ((DurationSeconds - 3) * 1000000))))
    {
      /* Empty column, set the up and down limits for it. */
      Row = ((rand() % 3) + 1);
      ColumnUp[Column]   = Row;
      ColumnDown[Column] = Row;

      set_pixel(ColumnDown[Column], Column, FLAG_ON);

      if (DebugBitMask & DEBUG_TEMP)
        uart_send(__LINE__, "- Empty column - Random Row: %u   Turn  On pixel %u, %2u\r", Row, Row, Column);
    }
    else
    {
      /* Column already occupied or five seconds before end of animation... check current down position. */
      if (DebugBitMask & DEBUG_TEMP)
        uart_send(__LINE__, "- Occupied - Up[%2u]: %u   Down[%2u]: %u ", Column, ColumnUp[Column], Column, ColumnDown[Column]);

      if ((ColumnDown[Column] < 7) && (ColumnDown[Column] != 0))
      {
        ++ColumnDown[Column];
        set_pixel(ColumnDown[Column], Column, FLAG_ON);

        if (DebugBitMask & DEBUG_TEMP)
          uart_send(__LINE__, "- Turn  On pixel %u, %2u\r", ColumnDown[Column], Column);
      }
      else
      {
        /* We already reached the lowest down mark, erase falling flake. */
        if (DebugBitMask & DEBUG_TEMP)
          uart_send(__LINE__, "- Turn Off pixel %u, %2u\r", ColumnUp[Column], Column);

        set_pixel(ColumnUp[Column], Column, FLAG_OFF);
        ++ColumnUp[Column];

        /* If this column is totally erased, re-initialize it. */
        if (ColumnUp[Column] == 8)
        {
          ColumnUp[Column]   = 0;
          ColumnDown[Column] = 0;
        }
      }
    }
    sleep_ms(10);
  }

  /* Return to normal time display mode. */
  show_time();

  return;
  /* ------------------------------------------------------------------ *\
          END - Test 18 - PWM tests for clock display brightness.
  \* ------------------------------------------------------------------ */
}
#endif





/* $PAGE */
/* $TITLE=timer_callback_ms() */
/* ------------------------------------------------------------------ *\
           Timer callback function (1 millisecond period).
    As with any ISR ("Interrupt Service Routine"), we must keep the
    processing as quick as possible since we want to make sure that
     the routine has completed before the next interrupt comes in.
\* ------------------------------------------------------------------ */
bool timer_callback_ms(struct repeating_timer *TimerMSec)
{
  UCHAR String[128];

  UINT8 Dum1UInt8;
  UINT8 Loop1UInt8;


  Dum1UInt8 = 0;

  adjust_clock_brightness();  // read ambient light every millisecond and calculate average for last 60 seconds.
  evaluate_blinking_time();   // evaluate if it is time to toggle blinking some data on clock display.
  evaluate_scroll_time();     // evaluate if it is time to scroll one dot to the left on clock display.


  if (CurrentClockMode == MODE_TEST) goto matrix_scan;

  /* ................................................................ *\
              Manage any two keys being pressed together
  \* ................................................................ */
  // This disables the other funtions when not in the setup mode
  if (SetupSource == SETUP_SOURCE_NONE && ((TopKeyPressTime > 0 && MiddleKeyPressTime > 0) || (MiddleKeyPressTime > 0 && BottomKeyPressTime > 0)  || (TopKeyPressTime > 0 && BottomKeyPressTime > 0))) {
    Two_Keys_Pressed = FLAG_ON;
  }
  if (Two_Keys_Pressed == FLAG_ON && gpio_get(SET_BUTTON) != 0 && gpio_get(UP_BUTTON) != 0 && gpio_get(DOWN_BUTTON) != 0) {
    // Clear the accumulated timers so that the other button controls aren't triggered
    TopKeyPressTime = 0;
    MiddleKeyPressTime = 0;
    BottomKeyPressTime = 0;
    // We have 2 keys pressed and released, so toggle the temporary alarm disable
    if (Temp_Disable_Alarm == FLAG_OFF) {
      Temp_Disable_Alarm = FLAG_ON;
      // disable alarms for ALARM_DISABLE_WINDOW hours
      Temp_Disable_Hour = ALARM_DISABLE_WINDOW;
      Temp_Disable_Min = CurrentMinute;
      // Disable the alarm indicator
      IndicatorAlarm0Off;
      IndicatorAlarm1Off;
    } else {
      // Re-enable the alarms
      Temp_Disable_Alarm = FLAG_OFF;
      // Set the alarm indicator if any alarm is enabled
      for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
        if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_ON)
          Temp_Disable_Alarm = FLAG_ON;

      if (Temp_Disable_Alarm == FLAG_ON) {
        IndicatorAlarm0On;
        IndicatorAlarm1On;
      } else {
        IndicatorAlarm0Off;
        IndicatorAlarm1Off;
      }
      // Clear down variables
      Temp_Disable_Alarm = FLAG_OFF;
      Temp_Disable_Hour = 0;
      Temp_Disable_Min = 0;
    }
    // Clear the keys pressed flag
    Two_Keys_Pressed = FLAG_OFF;
  }


  /* ................................................................ *\
                        Manage "Set" (top) button.
  \* ................................................................ */
  /* Check if the "Set" (top) button is pressed. */
  if (gpio_get(SET_BUTTON) == 0)
  {
    /* The "Set" (top) button is pressed... cumulate the number of milliseconds it is pressed. */
    ++TopKeyPressTime;
    // Saturate if held for a long while
    if (TopKeyPressTime > 500)
      TopKeyPressTime = 500;
  }
  else
  {
    /* If "Set" key has been pressed while one or more of the nine (9) clock alarms were ringing, reset all alarms and continue.
       If "Set" key has been pressed while the count-down timer was ringing, shut off count-down timer and continue.
       When one or more of the nine (9) alarms is ringing, or count-down timer is ringing "Set" key is simply used for alarms and count-down timer shutoff. */
    if (TopKeyPressTime > 0 && Two_Keys_Pressed == FLAG_OFF)
    {
      /* If "Set" key has been pressed while some clock alarms are ringing, reset those alarms so that they stop ringing. */
      if (AlarmReachedBitMask || FlagTimerCountDownEnd)
      {
        if (AlarmReachedBitMask)
          AlarmReachedBitMask = 0;

        /* If "Set" key has been pressed while count-down timer alarm is ringing, reset count-down timer alarm so that it stops ringing. */
        if (FlagTimerCountDownEnd == FLAG_ON)
        {
          FlagTimerCountDownEnd = FLAG_OFF;

          if (DebugBitMask & DEBUG_TIMER)
            uart_send(__LINE__, "FlagTimerCountDownEnd has been reset.\r");
        }
        // Clear down any alarm sounds
        sound_queue_reset_active();
        #ifdef PASSIVE_PIEZO_SUPPORT
        sound_queue_reset_passive();
        #endif
      }
      else
      {
        /* If the "Set" (top) button is not pressed, but cumulative time is not zero, it means that the
           "Set" button has just been released and it is a "short press" (between 50 and 300 milliseconds). */
        if ((TopKeyPressTime > 50) && (TopKeyPressTime < 300) && (ScrollDotCount == 0))
        {
          IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
          if (FlashConfig.FlagKeyclick == FLAG_ON)
          {
            for (Loop1UInt8 = 0; Loop1UInt8 < TONE_KEYCLICK_REPEAT2; ++Loop1UInt8)
            {
              sound_queue_active(TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1);
            }
            sound_queue_active(50, SILENT);
          }

          /* If we are already in a setup mode, perform housekeeping for current SetupStep and jump on to next SetupStep. */
          switch (SetupSource)
          {
            case SETUP_SOURCE_ALARM:
              set_mode_out();
              FlagSetAlarm = FLAG_ON;
              ++SetupStep;
              /* Cleanup will be done in setup_alarm_frame() when done. */
            break;

            case SETUP_SOURCE_CLOCK:
              set_mode_out();
              FlagSetClock = FLAG_ON;
              ++SetupStep;
              /* Cleanup will be done in setup_clock_frame() when done. */
            break;

            case SETUP_SOURCE_TIMER:
              set_mode_out();
              FlagSetTimer = FLAG_ON;
              ++SetupStep;
              /* Cleanup will be done in setup_timer_frame() when done. */
            break;

            default:
              if (FlashConfig.ShortSetKey == FLAG_ON) {
                /* If not already in a setup mode, "Set" button triggers entering in clock setup mode. */
                FlagSetClock = FLAG_ON;
                SetupSource = SETUP_SOURCE_CLOCK;
              }
              else {
                /* If we are not already in setup mode, enter alarm setup mode. */
                FlagSetAlarm = FLAG_ON;  // enter alarm setup mode.
                SetupSource  = SETUP_SOURCE_ALARM;
              }
              ++SetupStep;
            break;
          }
        }
        else if ((TopKeyPressTime > 300) && (SetupStep == SETUP_NONE))
        {
          /* If the "Set" (top) button is not pressed, but cumulative time is not zero, it means that the
             "Set" button has just been released and it is a "long press" (longer than 300 milliseconds). */
          IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
          if (FlashConfig.FlagKeyclick == FLAG_ON)
          {
            for (Loop1UInt8 = 0; Loop1UInt8 < TONE_KEYCLICK_REPEAT2; ++Loop1UInt8)
            {
              sound_queue_active(TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1);
            }
            sound_queue_active(50, SILENT);
          }
          set_mode_out(); // housekeeping;

          if (FlashConfig.ShortSetKey == FLAG_ON) {
            /* If we are not already in setup mode, enter alarm setup mode. */
            FlagSetAlarm = FLAG_ON;  // enter alarm setup mode.
            SetupSource  = SETUP_SOURCE_ALARM;
          }
          else {
            /* If not already in a setup mode, "Set" button triggers entering in clock setup mode. */
            FlagSetClock = FLAG_ON;
            SetupSource = SETUP_SOURCE_CLOCK;
          }
          ++SetupStep; // process through all alarm setup steps.
        }
      }
    }

    /* Reset "Set" (top) button cumulative press time. */
    TopKeyPressTime = 0;
  }


  /* ................................................................ *\
                       Manage "Up" (middle) button.
  \* ................................................................ */
  /* Check if the "Up" (middle) button is pressed. */
  if (gpio_get(UP_BUTTON) == 0)
  {
    /* The "Up" (middle) button is pressed... cumulate the time it is pressed. */
    ++MiddleKeyPressTime;
    // Saturate if held for a long while
    if (MiddleKeyPressTime > 500)
      MiddleKeyPressTime = 500;
  }
  else
  {
    /* If "Up" key has been pressed while one or more of the nine (9) clock alarms were ringing, reset all alarms and continue.
       If "Up" key has been pressed while the count-down timer was ringing, shut off count-down timer and continue.
       When one or more of the nine (9) alarms is ringing, or count-down timer is ringing "Up" key is simply used for alarms and count-down timer shutoff.
       If the "Up" and "Down" keys have both been pressed, then toggle a temprary alarm disable flag and reset a counter if being disabled */
    if (MiddleKeyPressTime > 0 && Two_Keys_Pressed == FLAG_OFF)
    {
      /* If "Up" key has been pressed while some clock alarms are ringing, reset those alarms so that they stop ringing. */
      if (AlarmReachedBitMask || FlagTimerCountDownEnd)
      {
        if (AlarmReachedBitMask)
          AlarmReachedBitMask = 0;


        /* If "Up" key has been pressed while count-down timer alarm is ringing, reset count-down timer alarm so that it stops ringing. */
        if (FlagTimerCountDownEnd == FLAG_ON)
        {
          FlagTimerCountDownEnd = FLAG_OFF;

          if (DebugBitMask & DEBUG_TIMER)
            uart_send(__LINE__, "FlagTimerCountDownEnd has been reset.\r");
        }
        // Clear down any alarm sounds
        sound_queue_reset_active();
        #ifdef PASSIVE_PIEZO_SUPPORT
        sound_queue_reset_passive();
        #endif
      }
      else
      {
        /* If the "Up" (middle) button is not pressed, but cumulative time is not zero, it means that the
           "Up" button has just been released and it is a "short press" (between 50 and 300 milliseconds). */
        if ((MiddleKeyPressTime > 50) && (MiddleKeyPressTime < 300))
        {
          IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
          if (FlashConfig.FlagKeyclick == FLAG_ON)
          {
            for (Loop1UInt8 = 0; Loop1UInt8 < TONE_KEYCLICK_REPEAT2; ++Loop1UInt8)
            {
              sound_queue_active(TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1);
            }
            sound_queue_active(50, SILENT);
          }

          /* If we are already in a setup mode, perform housekeeping for current SetupStep and jump on to next SetupStep. */
          switch (SetupSource)
          {
            case SETUP_SOURCE_ALARM:
              setup_alarm_variables(FLAG_UP); // perform alarm settings.
              set_mode_out();                 // program RTC IC if required.
              FlagSetAlarm = FLAG_ON;         // make sure main program loop displays new value.
            break;

            case SETUP_SOURCE_CLOCK:
              setup_clock_variables(FLAG_UP); // perform clock settings.
              set_mode_out();                 // program RTC IC if required.
              FlagSetClock = FLAG_ON;         // make sure main program loop displays new value.
            break;

            case SETUP_SOURCE_TIMER:
              setup_timer_variables(FLAG_UP); // perform timer settings.
              set_mode_out();                 // housekeeping.
              FlagSetTimer = FLAG_ON;         // make sure main program loop displays new value.
            break;

            default:
              /* NOTE: Temperature unit toggling has been transferred to the list of clock setup parameters. */
              /* Blink Night light On / Off and display external temperature if a sensor has been installed by user. */
              Dum1UInt8 = (1 << 2) | (1 << 5);
              if (DisplayBuffer[0] & Dum1UInt8)
                IndicatorButtonLightsOff;
              else
                IndicatorButtonLightsOn;

              /* If a DHT22 or a BME280 has been installed by user, display outside parameters. */
              scroll_queue(TAG_BME280_TEMP);
              scroll_queue(TAG_DHT22_TEMP);
            break;
          }
        }
        else if (MiddleKeyPressTime > 300)
        {
          if (FlashConfig.FlagKeyclick == FLAG_ON)
          {
            for (Loop1UInt8 = 0; Loop1UInt8 < TONE_KEYCLICK_REPEAT2; ++Loop1UInt8)
            {
              sound_queue_active(TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1);
            }
            sound_queue_active(50, SILENT);
          }

          if (SetupStep != SETUP_NONE)
          {
            /* Special case if we are in alarm setup mode, selecting target day-of-week for alarm. */
            if ((SetupSource == SETUP_SOURCE_ALARM) && (SetupStep == SETUP_ALARM_DAY))
            {
              setup_alarm_variables(FLAG_LONG_UP); // perform alarm settings.
              set_mode_out();
              FlagSetAlarm = FLAG_ON;
            }
            else
            {
              /* If the "Up" (middle) button is not pressed, but cumulative time is not zero, it means that the
                 "Up" button has just been released and it is a "long press" (longer than 300 milliseconds). */
              IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
              set_mode_out();          // housekeeping;
            }
          }
          else
          {
            /* If we're not already in a setup mode, long press on the "Up" button triggers timer setup mode. */
            FlagSetTimer = FLAG_ON;
            SetupSource  = SETUP_SOURCE_TIMER;
            ++SetupStep;
          }
        }
      }
    }

    /* Reset "Middle" button cumulative press time. */
    MiddleKeyPressTime = 0;
  }


  /* ................................................................ *\
                     Manage "Down" (bottom) button.
  \* ................................................................ */
  /* Check if the "Down" (bottom) button is press. */
  if (gpio_get(DOWN_BUTTON) == 0)
  {
    /* The "Down" (bottom) button is pressed... cumulate the time it is pressed. */
    ++BottomKeyPressTime;
    // Saturate if held for a long while
    if (BottomKeyPressTime > 500)
      BottomKeyPressTime = 500;
  }
  else
  {


    /* If "Down" key has been pressed while one or more of the nine (9) clock alarms were ringing, reset all alarms and continue.
       If "Down" key has been pressed while the count-down timer was ringing, shut off count-down timer and continue.
       When one or more of the nine (9) alarms is ringing, or count-down timer is ringing "Down" key is simply used for alarms and count-down timer shutoff.
       If the "Up" and "Down" keys have both been pressed, then do nothing here */
    if (BottomKeyPressTime > 0 && Two_Keys_Pressed == FLAG_OFF)
    {
      /* If "Dowwn" key has been pressed while some clock alarms are ringing, reset those alarms so that they stop ringing. */
      if (AlarmReachedBitMask || FlagTimerCountDownEnd)
      {
        if (AlarmReachedBitMask)
          AlarmReachedBitMask = 0;


        /* If "Down" key has been pressed while count-down timer alarm is ringing, reset count-down timer alarm so that it stops ringing. */
        if (FlagTimerCountDownEnd == FLAG_ON)
        {
          FlagTimerCountDownEnd = FLAG_OFF;

          if (DebugBitMask & DEBUG_TIMER)
            uart_send(__LINE__, "FlagTimerCountDownEnd has been reset.\r");
        }
        // Clear down any alarm sounds
        sound_queue_reset_active();
        #ifdef PASSIVE_PIEZO_SUPPORT
        sound_queue_reset_passive();
        #endif
      }
      else
      {
        /* If the "Down" (bottom) button is not pressed, but cumulative time is not zero, it means that the
           "Down" button has just been released and it is a "short press" (between 50 and 300 milliseconds). */
        if ((BottomKeyPressTime > 50) && (BottomKeyPressTime < 300))
        {
          IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
          if (FlashConfig.FlagKeyclick == FLAG_ON)
          {
            for (Loop1UInt8 = 0; Loop1UInt8 < TONE_KEYCLICK_REPEAT2; ++Loop1UInt8)
            {
              sound_queue_active(TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1);
            }
            sound_queue_active(50, SILENT);
          }

          /* If we are already in a setup mode, perform housekeeping for current SetupStep and jump on to next SetupStep. */
          switch (SetupSource)
          {
            case SETUP_SOURCE_ALARM:
              setup_alarm_variables(FLAG_DOWN); // perform alarm settings.
              set_mode_out();
              FlagSetAlarm = FLAG_ON;
            break;

            case SETUP_SOURCE_CLOCK:
              setup_clock_variables(FLAG_DOWN); // perform clock settings.
              set_mode_out();
              FlagSetClock = FLAG_ON;
            break;

            case SETUP_SOURCE_TIMER:
              setup_timer_variables(FLAG_DOWN); // perform timer settings.
              set_mode_out();
              FlagSetTimer = FLAG_ON;
            break;

            default:
              /* NOTE: Clock display auto-brightness toggling has been Added to the list of clock setup parameters.
                       Leave a copy of this function here for convenience and quicker access. */
              /* If we are not in setup mode, toggle the "auto-brightness" On / Off and cycle through 5 levels of manual settings. */
              if (FlashConfig.FlagAutoBrightness == FLAG_ON)
              {
                FlashConfig.FlagAutoBrightness = FLAG_OFF;
                IndicatorAutoLightOff;
                pwm_set_cycles(BRIGHTNESS_LIGHTLEVELSTEP);
              }
              else if (FlashConfig.FlagAutoBrightness == FLAG_OFF && (Pwm[PWM_BRIGHTNESS].Cycles <= BRIGHTNESS_LIGHTLEVELSTEP && Pwm[PWM_BRIGHTNESS].Cycles > BRIGHTNESS_MANUALDIV1))
              {
                FlashConfig.FlagAutoBrightness = FLAG_OFF;
                IndicatorAutoLightOff;
                pwm_set_cycles(BRIGHTNESS_MANUALDIV1);
              }
              else if (FlashConfig.FlagAutoBrightness == FLAG_OFF && (Pwm[PWM_BRIGHTNESS].Cycles <= BRIGHTNESS_MANUALDIV1 && Pwm[PWM_BRIGHTNESS].Cycles > BRIGHTNESS_MANUALDIV2))
              {
                FlashConfig.FlagAutoBrightness = FLAG_OFF;
                IndicatorAutoLightOff;
                pwm_set_cycles(BRIGHTNESS_MANUALDIV2);
              }
              else if (FlashConfig.FlagAutoBrightness == FLAG_OFF && (Pwm[PWM_BRIGHTNESS].Cycles <= BRIGHTNESS_MANUALDIV2 && Pwm[PWM_BRIGHTNESS].Cycles > BRIGHTNESS_MANUALDIV3))
              {
                FlashConfig.FlagAutoBrightness = FLAG_OFF;
                IndicatorAutoLightOff;
                pwm_set_cycles(BRIGHTNESS_MANUALDIV3);
              }
              else if (FlashConfig.FlagAutoBrightness == FLAG_OFF && (Pwm[PWM_BRIGHTNESS].Cycles <= BRIGHTNESS_MANUALDIV3 && Pwm[PWM_BRIGHTNESS].Cycles > 0))
              {
                FlashConfig.FlagAutoBrightness = FLAG_OFF;
                IndicatorAutoLightOff;
                pwm_set_cycles(0);
              }
              else
              {
                FlashConfig.FlagAutoBrightness = FLAG_ON;
                IndicatorAutoLightOn;
                pwm_set_cycles(BRIGHTNESS_LIGHTLEVELSTEP);
              }
            break;
          }
        }
        else if ((BottomKeyPressTime > 300) && (SetupStep != SETUP_NONE))
        {
          /* Special case if we are in alarm setup mode, selecting target day-of-week for alarm. */
          if ((SetupSource == SETUP_SOURCE_ALARM) && (SetupStep == SETUP_ALARM_DAY))
          {
            setup_alarm_variables(FLAG_LONG_DOWN); // perform alarm settings.
            set_mode_out();
            FlagSetAlarm = FLAG_ON;
          }
          else
          {
            /* If the "Down" (bottom) button is not pressed, but cumulative time is not zero, it means that the
               "Down" button has just been released and it is a "long press" (longer than 300 milliseconds). */
            IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
            if (FlashConfig.FlagKeyclick == FLAG_ON)
            {
              for (Loop1UInt8 = 0; Loop1UInt8 < TONE_KEYCLICK_REPEAT2; ++Loop1UInt8)
              {
                sound_queue_active(TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1);
              }
              sound_queue_active(50, SILENT);
            }

            set_mode_timeout();              // exit setup mode.
            set_mode_out();                  // make required housekeeping.
            SetupSource = SETUP_SOURCE_NONE; // no more in setup mode.
            SetupStep = SETUP_NONE;          // reset SetupStep.
          }
        }
      }
    }

    /* Reset bottom button cumulative press time. */
    BottomKeyPressTime = 0;
  }


matrix_scan:
  /* ................................................................ *\
                    Increment LED matrix scanned row.
  \* ................................................................ */
  /* Increment row number to scan on clock display (Row0 to Row7). */
  ++RowScanNumber;
  if (RowScanNumber > 7) RowScanNumber = 0;


  /* ................................................................ *\
                 Change column data on matrix display.
  \* ................................................................ */
  /* Send the three bytes (3 X 8 = 24 bits) corresponding to the 24 columns of the current row to the LED matrix controller ICs. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 4; ++Loop1UInt8)
    send_data(DisplayBuffer[8 * Loop1UInt8 + RowScanNumber]);

  /* Latch those values to the matrix controler ICs. */
  LE_HIGH;
  LE_LOW;

  /* Set A0 address line level. */
  if (RowScanNumber & 0x01)
    A0_HIGH;
  else
    A0_LOW;

  /* Set A1 address line level. */
  if (RowScanNumber & 0x02)
    A1_HIGH;
  else
    A1_LOW;

  /* Set A2 address line level. */
  if (RowScanNumber & 0x04)
    A2_HIGH;
  else
    A2_LOW;

  return TRUE;
}





/* $PAGE */
/* $TITLE=timer_callback_s() */
/* ------------------------------------------------------------------ *\
                  One second timer callback function
    As with any ISR ("Interrupt Service Routine"), we must keep the
    processing as quick as possible since we want to make sure that
     the routine has completed before the next interrupt comes in.
\* ------------------------------------------------------------------ */
bool timer_callback_s(struct repeating_timer *TimerSec)
{
  UCHAR String[128];
  UCHAR String1[64];

  UINT16 CurrentCycles;
  UINT8 Dum1UInt8;
  static UINT8 FlagChimeDone     = FLAG_OFF;  // indicates that hourly chime has already been triggered for current hour.
  static UINT8 FlagChimeHalfDone = FLAG_OFF;  // if "On", indicates that half-hour chime has already been triggered for current hour.
  static UINT8 FlagEventDone     = FLAG_OFF;  // to prevent duplication of Calendar Events.
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 TotalBeeps = 0;

  static UINT16 AlarmRingTime[MAX_ALARMS];    // cumulative number of seconds alarm has been ringing.
  UINT16 BeepLength;
  static UINT16 CountDownAlarmDuration;       // keep track of curent cumulative time (in seconds) count-down alarm has been sounding so far.
  static UINT16 CountDownDelay;               // delay (in seconds) betweek each count-down alarm sound burst.
  UINT16 LightLevel;
  static UINT16 PreviousYear;

  UINT64 Timer1;
  UINT64 Timer2;
  static UINT64 PreviousTimer;

  // Hold an alarm jingle tune value
  UINT8 Alarm_Jingle;
  UINT8 Active_Buzzer;


  /* Indicate that we just entered callback_s. */
  /***
  if ((DebugBitMask & DEBUG_SOUND_QUEUE) || (DebugBitMask & DEBUG_TIMING))
  {
    Timer1 = time_us_64();

    if (PreviousTimer != 0)
    {
      CurrentCycles = Pwm[PWM_BRIGHTNESS].Cycles;
      pwm_set_cycles(0);  // blank display so that we don't see frozen display while LED scanning is delayed by UART.

      uart_send(__LINE__, "\r\r=== CALLBACK S ===      Delay: %5llu mSec   Stack: 0x%8.8X\r\r", ((Timer1 - PreviousTimer) / 1000), &String[0]);
      pwm_set_cycles(CurrentCycles);  // restore display brightness.
    }

    PreviousTimer = Timer1;
  }
  ***/



  /* ................................................................ *\
                          Manage time of day
                      and Daylight Saving Time.
  \* ................................................................ */
  ++CurrentSecond;
  ++GlobalUnixTime;


  if ((CurrentMinute == 0) && (CurrentSecond == 1))
  {
    /* Since we just changed hour, check for an eventual change in Daylight Saving Time status.
       Wait two seconds in case there is a small discrepancy between Green Clock real-time and RTC IC. */
    update_dst_status();
  }


  if (CurrentSecond == 60)
  {
    ++CurrentMinute;
    CurrentSecond = 0; // reset second counter.

    /* Reset FlagChimeHalfDone to get prepared for next half-hour chime. */
    if (CurrentMinute == 32)
    {
      FlagChimeHalfDone = FLAG_OFF;

      if (DebugBitMask & DEBUG_CHIME)
        uart_send(__LINE__, "Resetting FlagChimeHalfDone: 0x%2.2X   Hour: %2u   Minute: %2u   CurrentSecond: %2u\r", FlagChimeHalfDone, CurrentHour, CurrentMinute, CurrentSecond);
    }

    /* Reset FlagChimeDone to get prepared for next hour chime. */
    if (CurrentMinute == 2)
    {
      FlagChimeDone = FLAG_OFF;

      if (DebugBitMask & DEBUG_CHIME)
        uart_send(__LINE__, "Resetting FlagChimeDone: 0x%2.2X   Hour: %2u   Minute: %2u   CurrentSecond: %2u\r", FlagChimeDone, CurrentHour, CurrentMinute, CurrentSecond);


      /* Perform this check at 0h02, out of peak period. */
      if (CurrentYear != PreviousYear)
      {
        /* We just changed year, find specific days for Daylight Saving Time change for this year. */
        get_dst_days();

        PreviousYear = CurrentYear;
      }
    }


    /* Reset FlagEventDone to get prepared for next iteration. */
    if (((EVENT_MINUTE1 < 57) && (CurrentMinute == (EVENT_MINUTE1 + 2))) || ((EVENT_MINUTE1 >= 57) && (CurrentMinute == 2)))
      FlagEventDone = FLAG_OFF;

    if (((EVENT_MINUTE2 < 57) && (CurrentMinute == (EVENT_MINUTE2 + 2))) || ((EVENT_MINUTE2 >= 57) && (CurrentMinute == 2)))
      FlagEventDone = FLAG_OFF;



    if (CurrentMinute == 60)
    {
      CurrentMinute = 0;  // reset minute counter.
      ++CurrentHour;

      if (CurrentHour == 24) CurrentHour = 0;  // if out-of-bound, revert to 0.
    }



    /* Log tag. */
    if (DebugBitMask & DEBUG_TIMING)
      uart_send(__LINE__, "-1");



    /* .............................................................. *\
                 Request a time update on clock display.
    \* .............................................................. */
    if ((FlagIdleCheck == FLAG_OFF) && (ScrollDotCount == 0))
    {
      /* Request a "time" update (on clock display) if we are not in some setup mode and not scrolling data. */
      FlagUpdateTime = FLAG_ON;
    }
  }



  if (DebugBitMask & DEBUG_CHIME)
  {
    CurrentCycles = Pwm[PWM_BRIGHTNESS].Cycles;
    pwm_set_cycles(0);  // blank display so that we don't see frozen display while LED scanning is delayed by UART.

    uart_send(__LINE__, "H: %2u   (HS: %2u)   M: %2u   (MS: %2u)\r", CurrentHour, CurrentHourSetting, CurrentMinute, CurrentMinuteSetting);

    pwm_set_cycles(CurrentCycles);  // restore display brightness.
  }



  /* ................................................................ *\
      If user requested a silence period, decrement its second count.
  \* ................................................................ */
  if (SilencePeriod != 0) --SilencePeriod;



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-2");



  /* ................................................................ *\
                             Feed alarm ringers.
  \* ................................................................ */
  if ((CurrentSecond % ALARM_PERIOD) == 0)
  {
    if (AlarmReachedBitMask)
    {
      if (DebugBitMask & DEBUG_ALARMS)
        uart_send(__LINE__, "AL ON\r");

      /* Calculate the total number of beeps to decide on the length of each beep. */
      TotalBeeps = 0;
      for (Loop1UInt8 = 0; Loop1UInt8 < MAX_ALARMS; ++Loop1UInt8)
      {
        // Load the current alarm jingle, if top bit set then use active buzzer and not passive one if configured
        if (FlashConfig.Alarm[Loop1UInt8].Jingle & 0x80)
        {
          Active_Buzzer = FLAG_ON;
        }
        else
        {
          Active_Buzzer = FLAG_OFF;
        }
        Alarm_Jingle = (FlashConfig.Alarm[Loop1UInt8].Jingle & 0x7F);
        if (AlarmReachedBitMask & (1 << Loop1UInt8))
        {
          if (Active_Buzzer == FLAG_OFF)
          {
            if (Alarm_Jingle < 10 && Alarm_Jingle > TotalBeeps)
            {
              // Fetch the number of beeps programmed into the alarm jingle - 0 for none and 1 to 9 otherwise
              TotalBeeps = Alarm_Jingle;
            }
            if (Alarm_Jingle > 9 && TotalBeeps < 10)
            {
              // Fetch the first active tune jingle for an active alarm
              TotalBeeps = Alarm_Jingle;
            }
          }
          else
          {
            if (Alarm_Jingle < 10 && Alarm_Jingle > TotalBeeps)
            {
              // Fetch the number of beeps programmed into the alarm jingle - 0 for none and 1 to 9 otherwise
              TotalBeeps = Alarm_Jingle;
            }
            if (Alarm_Jingle > 9 && TotalBeeps < 10)
            {
              // for a jingle set and a passive buzzer, then set to the maximum number of beeps
              TotalBeeps = 9;
            }
          }
          if (Alarm_Jingle == 0 && TotalBeeps == 0)
          {
            // If no tune was selected for any alarm, then set the alarm number as the number of beeps (incremented later)
            TotalBeeps = Loop1UInt8 + 1;
          }
        }
      }

      // Now see if the alarm is a jingle or beep tones
      if (TotalBeeps > 9)
      {
        // pick out a jingle - 1 to 7 currently
        Alarm_Jingle = TotalBeeps - 9;
        if (DebugBitMask & DEBUG_ALARMS)
          uart_send(__LINE__, "Sound alarm as jingle tune %d\r", Alarm_Jingle);
      }
      else {
        Alarm_Jingle = 0;
        /* Asign beep length. */
        BeepLength = 50; // smallest value.
        if (TotalBeeps < 11)
          BeepLength = 100;
        if (TotalBeeps < 8)
          BeepLength = 250;
        if (TotalBeeps < 6)
          BeepLength = 300;
        if (TotalBeeps < 5)
          BeepLength = 350;
        if (TotalBeeps < 4)
          BeepLength = 400;
        if (TotalBeeps < 2)
          BeepLength = 500;

        if (DebugBitMask & DEBUG_ALARMS)
          uart_send(__LINE__, "-TotalBeeps: %u  BeepLength: %u\r", TotalBeeps, BeepLength);
      }

      for (Loop1UInt8 = 0; Loop1UInt8 < MAX_ALARMS; ++Loop1UInt8)
      {
        if (AlarmReachedBitMask & (1 << Loop1UInt8))
        {
          if (DebugBitMask & DEBUG_ALARMS)
            uart_send(__LINE__, "-AL %u ON   RingTime: %u\r", Loop1UInt8 + 1, AlarmRingTime[Loop1UInt8]);

          /* Check if this alarm has been ringing for specified ring time. */
          AlarmRingTime[Loop1UInt8] += ALARM_PERIOD;
          if (AlarmRingTime[Loop1UInt8] >= MAX_ALARM_RING_TIME)
          {
            /* This alarm has been sounding for the specified ring time now... Turn it Off. */
            AlarmReachedBitMask &= ~(1 << Loop1UInt8);

            /* Reset total ringing time. */
            AlarmRingTime[Loop1UInt8] = 0;

            if (DebugBitMask & DEBUG_ALARMS)
              uart_send(__LINE__, "-Bitmask: %8.8X\r", AlarmReachedBitMask);

            break;  // skip to check next alarm.
          }


          if (Active_Buzzer == FLAG_OFF)
          {
            if (TotalBeeps > 9)
            {
              #ifdef PASSIVE_PIEZO_SUPPORT
              // Send alarm jingle only if the sound queue is empty
              if (sound_queue_status_passive() == FLAG_OFF)
              {
                // We have a jinge tune to play for this 5 second period
                play_jingle(Alarm_Jingle);
              }
              #endif
              // exit all loops
              Loop1UInt8 = MAX_ALARMS;
              break;
            }
            else
            {
              #ifdef PASSIVE_PIEZO_SUPPORT
              for (Loop2UInt8 = 0; Loop2UInt8 < TotalBeeps; ++Loop2UInt8)
              {
                sound_queue_passive(600 + (Loop1UInt8 * 150), BeepLength);
                sound_queue_passive(SILENT, 50); // separate each beep in the train of beeps.
              }
              sound_queue_passive(SILENT, 150);  // separate the train of beeps of an alarm from each other.
              #else
              sound_queue_active(BeepLength, TotalBeeps);
              sound_queue_active(500, SILENT);   // separate the train of beeps of an alarm from each other.
              #endif
            }
          }
          else
          {
            sound_queue_active(BeepLength, TotalBeeps);
            sound_queue_active(500, SILENT);   // separate the train of beeps of an alarm from each other.
          }
        }
      }
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-3");



  #ifdef PICO_W
  /* ................................................................ *\
  Print out connection status and IP address every SCROLL_PERIOD_MINUTE
  \* ................................................................ */
  if (((CurrentMinute + 2) % SCROLL_PERIOD_MINUTE) == 0 && CurrentSecond == 18)
  {
    struct netif *my_netif = &cyw43_state.netif[CYW43_ITF_STA];
    if (netif_is_link_up(my_netif)) {
      printf("%s clock WiFi connected, IP address is %s\n",netif_get_hostname(my_netif), ip4addr_ntoa(netif_ip4_addr(my_netif)));
    } else {
      printf("%s clock WiFi not connected\n",netif_get_hostname(my_netif));
    }
  }
  #endif // PICO_W



  /* ................................................................ *\
        Check alarms and turn On corresponding bit in the bitmask
                     when reaching all alarm conditions.
  \* ................................................................ */
  /* Scan all alarms. The check is done during an off-peak second (not 00, 14, 44). */
  if (CurrentSecond == 29)
  {
    // Count down of hours if the alarms have been temporarily disabled
    if (Temp_Disable_Alarm == FLAG_ON) {
      if (CurrentMinute == Temp_Disable_Min) {
        // Reduce the hour value by 1 if we are at a repeating minute value
        Temp_Disable_Hour--;
        if (Temp_Disable_Hour == 0) {
          // Re-enable the alarms
          Temp_Disable_Alarm = FLAG_OFF;
          // Set the alarm indicator if any alarm is enabled
          for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
            if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_ON)
              Temp_Disable_Alarm = FLAG_ON;

          if (Temp_Disable_Alarm == FLAG_ON) {
            IndicatorAlarm0On;
            IndicatorAlarm1On;
          } else {
            IndicatorAlarm0Off;
            IndicatorAlarm1Off;
          }
          // Clear down variables
          Temp_Disable_Alarm = FLAG_OFF;
          Temp_Disable_Min = 0;
        }
      }
    }
    if (Temp_Disable_Alarm == FLAG_OFF) {
      for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
      {
        /* If this alarm is Off, skip it. */
        if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_OFF)
        {
          if (DebugBitMask & DEBUG_ALARMS)
            uart_send(__LINE__, "-[%2.2u]: Off\r", Loop1UInt8);

          continue;
        }

        /* This alarm is On, check if current day-of-week is defined in the "day-mask". */
        if ((FlashConfig.Alarm[Loop1UInt8].Day & (1 << CurrentDayOfWeek)) == 0)
        {
          if (DebugBitMask & DEBUG_ALARMS)
            uart_send(__LINE__, "-[%2.2u]: Wrong day    (%2.2X VS %2.2X)\r", Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Day, (1 << CurrentDayOfWeek));

          continue;
        }

        /* Current day-of-week is defined in DayMask, check if current hour (in 24hr range) is the "alarm-hour". */
        if (FlashConfig.Alarm[Loop1UInt8].Hour != CurrentHour)
        {
          if (DebugBitMask & DEBUG_ALARMS)
            uart_send(__LINE__, "-[%2.2u]: Wrong hour   (%2.2u VS %2.2u)\r", Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Hour, CurrentHour);

          continue;
        }

        /* Current hour is the "alarm hour", check if current minute is the "alarm-minute". */
        if (FlashConfig.Alarm[Loop1UInt8].Minute != CurrentMinute)
        {
          if (DebugBitMask & DEBUG_ALARMS)
            uart_send(__LINE__, "-[%2.2u]: Wrong minute (%2.2u VS %2.2u)\r", Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Minute, CurrentMinute);

          continue;
        }

        /* We reach this point if we comply with all alarm conditions. */
        if (DebugBitMask & DEBUG_ALARMS)
          uart_send(__LINE__, "-[%2.2u]: **Reached**\r", Loop1UInt8);

        /* Set the bit corresponding to this alarm in the alarm bit mask. */
        AlarmReachedBitMask |= (1 << Loop1UInt8);

        if (DebugBitMask & DEBUG_ALARMS)
          uart_send(__LINE__, "-BitMask: 0x%4.4X\r\r", AlarmReachedBitMask);

        /* If there is a string associated with this alarm, scroll it on clock display. */
        if (FlashConfig.Alarm[Loop1UInt8].Text[0] != 0x00)
          scroll_string(24, FlashConfig.Alarm[Loop1UInt8].Text);
      }
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-4");



  /* ................................................................ *\
                       Hourly and half-hour chimes.
  \* ................................................................ */
  /* Hourly chime will never sound if set to Off and will always sound if set to On (except if user requested a silence period - see remote control).
     However, if Hourly chime is set to Day "OI" ("On, Intermittent" in clock setup), here is what happens:

     "Daytime workers":
     Hourly chime "normal behavior": When "Chime time On" is smaller than "Chime time Off".
     For example: "Chime time On" = 9h00 and "Chime time Off" is 21h00. Chime will sound between (and including) 9h00 and 21h00 as we would expect (and will be silent otherwise).

     "Nighttime workers":
     Hourly chime "special behavior": When "Chime time On" is greater than "Chime time Off".
     For example: "Chime time On" = 21h00 and "Chime time Off" is 9h00. Chime will sound AFTER 21h00 and BEFORE 9h00 (and will be silent otherwise).
     (That is, will not sound between 9h00 and 21h00). */
  if (DebugBitMask & DEBUG_CHIME)
  {
    CurrentCycles = Pwm[PWM_BRIGHTNESS].Cycles;
    pwm_set_cycles(0);  // blank display so that we don't see frozen display while LED scanning is delayed by UART.

    uart_send(__LINE__, "H: %2u   (HS: %2u)   M: %2u   (MS: %2u)   S: %2u   FCD: 0x%2.2X\r", CurrentHour, CurrentHourSetting, CurrentMinute, CurrentMinuteSetting, CurrentSecond, FlagChimeDone);

    uart_send(__LINE__, "ChimeMode:  %u   On: %2u   Off: %2u   FlagSetupClock[MIN]: 0x%2.2X\r\r", FlashConfig.ChimeMode, FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff, FlagSetupClock[SETUP_MINUTE]);

    pwm_set_cycles(CurrentCycles);  // restore display when done.
  }

  // Chime on/off hours are set in 24h values, so use CurrentHour to compare
  if ((SilencePeriod == 0) &&
     ((FlashConfig.ChimeMode == CHIME_ON) ||                                                                                                                                                       // always On
     ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOn  < FlashConfig.ChimeTimeOff) && (CurrentHour >= FlashConfig.ChimeTimeOn)  && (CurrentHour <= FlashConfig.ChimeTimeOff)) ||  // "normal behavior"  for daytime workers.
     ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOff < FlashConfig.ChimeTimeOn) && ((CurrentHour >= FlashConfig.ChimeTimeOff) || (CurrentHour <= FlashConfig.ChimeTimeOn)))))   // "special behavior" for nighttime workers.
  {
    /* Half-hour chime. */
    if ((CurrentMinute == 30) && (FlagChimeHalfDone == FLAG_OFF) && (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF))
    {
      /* Indicate that half-hour chime has been done for current half-hour chime. */
      FlagChimeHalfDone = FLAG_ON;

      sound_queue_active(50, 2);
    }



    /* Hourly chime. */
    if ((CurrentMinute == 0) && (FlagChimeDone == FLAG_OFF) && (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF))
    {
      /* Indicate that hourly chime has been done for current hour chime. */
      FlagChimeDone = FLAG_ON;

      if (DebugBitMask & DEBUG_CHIME)
        uart_send(__LINE__, "Queueing hourly chime...\r");

      /* If user select option where hourly chime to be equivalent to hour value. */
      if (CHIME_HOUR_COUNT)
      {
        /* Number of "beeps" correspond to hour value in 12-hour format. */
        Dum1UInt8 = CurrentHour;
        if (CurrentHour > 12) Dum1UInt8 -= 12;
        for (Loop1UInt8 = 0; Loop1UInt8 < Dum1UInt8; ++Loop1UInt8)
        {
          sound_queue_active(CHIME_HOUR_COUNT_BEEP_DURATION, 1);
          sound_queue_active(CHIME_HOUR_COUNT_BEEP_DURATION, SILENT);
        }
      }
      else
      {
        /* If CHIME_HOUR_COUNT is Off, use normal hourly chime. */
        for (Loop1UInt8 = 0; Loop1UInt8 < TONE_CHIME_REPEAT2; ++Loop1UInt8)  // second repeat level.
        {
          sound_queue_active(TONE_CHIME_DURATION, TONE_CHIME_REPEAT1);       // first repeat level.
          sound_queue_active(50, SILENT);
        }
        sound_queue_active(100, SILENT);
      }

      #ifdef PASSIVE_PIEZO_SUPPORT
      /* Let the time to complete the active buzzer hourly chime. */
      sound_queue_passive(SILENT, WAIT_4_ACTIVE);

      /* Close encounter of the third kind. */
      play_jingle(JINGLE_ENCOUNTER);
      #endif  // PASSIVE_PIEZO_SUPPORT
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-5");



  /* ................................................................ *\
                   Periodic scrolling on clock display.
              (Date, temperature, voltage scrolling, etc...)
  \* ................................................................ */
  /* Scroll information on clock display every "integer number of times" the SCROLL_PERIOD_MINUTE minutes,
     except if we are currently in some setup mode. For example, if SCROLL_PERIOD_MINUTE is set to 5, data
     will scroll at xh00, xh05, xh10, etc...
     When changing to exact hour (from xh59 to yh00), scrolling will begin 5 seconds later to give time to user
     to look at the clock display when he hears the hourly chime. */
  if (((CurrentSecond == 0) && ((CurrentMinute % SCROLL_PERIOD_MINUTE) == 0) && (CurrentMinute != 0)) ||
      ((CurrentSecond == 5) &&  (CurrentMinute == 0)))
  {
    /* ................................................................ *\
           Check for a change in current active clock configuration.
    \* ................................................................ */
    /* Make sure we're not currently in setup mode when updating clock configuration in flash.
       It's better to wait until we completed the setup process before updating flash configuration.
       Since we need to temporarily disable interrupts during flash write, perform the check near the minute change.
       This way, we can turn Off clock display and it will refresh soon after, at minute change, for a less intrusive effect. */
    if (ScrollDotCount == 0)
    {
      flash_check_config();

      if (FlashConfig.FlagScrollEnable == FLAG_ON)
      {
        if (DebugBitMask & DEBUG_SCROLL)
          uart_send(__LINE__, "Scrolling date and other periodic display...\r");

        /* PERIODIC SCROLLING CONFIGURATION. */
        /* We reached the time to trigger periodic scrolling and we are in time display mode. */
        scroll_queue(TAG_DATE);
        #ifdef RELEASE_VERSION
        scroll_queue(TAG_DS3231_TEMP);
        #ifdef PICO_W
        scroll_queue(TAG_NTP_ERRORS);
        #endif  // PICO_W
        #else   // RELEASE_VERSION
        scroll_queue(TAG_BME280_TEMP);
        scroll_queue(TAG_DHT22_TEMP);
        scroll_queue(TAG_DS3231_TEMP);
        scroll_queue(TAG_NTP_ERRORS);
        #endif  // RELEASE_VERSION
      }
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-6");



  /* ................................................................ *\
               Cumulative idle time for potential time-out
                           while in a setup mode.
  \* ................................................................ */
  /* If we are in some setup mode (clock, timer or alarm), check if it is time to declare a time-out. */
  if (FlagIdleCheck == FLAG_ON)
  {
    ++IdleNumberOfSeconds; // one more second added while the system has been idle.

    if (IdleNumberOfSeconds == TIME_OUT_PERIOD)
    {
      set_mode_timeout();      // get out of setup mode because of inaction (time-out).
      set_mode_out();          // proceed with what needs to be set and then cleanup as we get out of setup mode.
      SetupStep = SETUP_NONE;  // reset setup step.
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-7");



  /* ................................................................ *\
                            Count-down timer.
  \* ................................................................ */
  if (FlagTimerCountDownEnd == FLAG_ON)
  {
    if (DebugBitMask & DEBUG_TIMER)
      uart_send(__LINE__, "Count-down timer has been triggered.\r");

    /* Count-down timer reached zero. we sound count-down timer alarm until user pressed the "Set" button,
       or until we reach the maximum count-down alarm elapsed time. */
    ++CountDownDelay;          // one more second passed for next count-down timer alarm burst.
    ++CountDownAlarmDuration;  // one more second for total count-down alarm duration.

    if (DebugBitMask & DEBUG_TIMER)
      uart_send(__LINE__, "CountDownDelay: %u   CountDownAlarmDuration: %u   COUNT_DOWN_DELAY: %u.\r", CountDownDelay, CountDownAlarmDuration, COUNT_DOWN_DELAY);

    if (CountDownDelay >= COUNT_DOWN_DELAY)
    {
      CountDownDelay = 0;  // reset delay until next alarm burst.
      for (Loop1UInt8 = 0; Loop1UInt8 < TONE_TIMER_REPEAT2; ++Loop1UInt8)
      {
        sound_queue_active(TONE_TIMER_DURATION, TONE_TIMER_REPEAT1);
        sound_queue_active(1000, SILENT);
      }
    }


    /* If user doesn't press "Set" (top) button to shutdown count-down timer alarm, it will stop by itself
       when reaching its maximum ringing time (which is specified in minutes). */
    if (CountDownAlarmDuration >= (MAX_COUNT_DOWN_ALARM_DURATION * 60))
    {
      /* "Last call" !! */
      sound_queue_active(TONE_TIMER_DURATION, 20);
      sound_queue_active(1000, SILENT);

      CountDownAlarmDuration = 0;
      CountDownDelay         = 0;
      FlagTimerCountDownEnd  = FLAG_OFF;

      if (DebugBitMask & DEBUG_TIMER)
        uart_send(__LINE__, "FlagTimerCountDownEnd has been reset.\r");
    }
  }
  else
  {
    if (DebugBitMask & DEBUG_TIMER)
      uart_send(__LINE__, "FlagTimerCountDownEnd: %u\r", FlagTimerCountDownEnd);
  }


  if (FlagSetTimerCountDown == FLAG_ON)
  {
    if (DebugBitMask & DEBUG_TIMER)
      uart_send(__LINE__, "Count-down timer active: %2.2u:%2.2u\r", TimerMinutes, TimerSeconds);

    /* If a count-down timer has been set and reaches zero, we always sound the piezo, no matter what is the Chime Time On / Off setting. */
    if ((TimerMinutes == 0) && (TimerSeconds == 0))
    {
      if (DebugBitMask & DEBUG_TIMER)
        uart_send(__LINE__, "Count-down timer reached zero: %2.2u:%2.2u\r", TimerMinutes, TimerSeconds);

      /*** This block for a single-train alarm for count-down timer. ***
      for (Loop1UInt8 = 0; Loop1UInt8 < TONE_TIMER_REPEAT2; ++Loop1UInt8)
      {
        sound_queue_active(TONE_TIMER_DURATION, TONE_TIMER_REPEAT1);
        sound_queue_active(1000, SILENT);
      }
      ***/
      /*** Block below to ring an alarm for 30 minutes or until user presses "Set" button, whichever happens first. ***/
      CountDownDelay         = COUNT_DOWN_DELAY;  // force an alarm on first pass.
      CountDownAlarmDuration = 0;                 // count-down alarm just begins sounding.
      SetupSource            = SETUP_SOURCE_NONE;
      FlagSetTimerCountDown  = FLAG_OFF;  // reset count down timer flag.
      FlagTimerCountDownEnd  = FLAG_ON;   // flag indicating we reached final count of count down timer (0m00).
      IndicatorCountDownOff;              // turn Off "count-down" indicator when done.

      if (DebugBitMask & DEBUG_TIMER)
        uart_send(__LINE__, "Count-down timer CountDownDelay: %u   FlagTimerCountDownEnd: %u\r", CountDownDelay, FlagTimerCountDownEnd);
      /*** End of block. ***/
    }
    else
    {
      ++TimerShowCount;

      /* Decrease the "second" part of the count-down timer. */
      --TimerSeconds;

      /* We let the timer to go one second below zero so that the last second is accounted for in the laps period. */
      if (TimerSeconds == 255) TimerSeconds = 0;  // if out-of-bound, revert to zero.

      /* If there are more minutes to the count-down. */
      if ((TimerSeconds == 0) && (TimerMinutes != 0))
      {
        --TimerMinutes;
        TimerSeconds    = 59;
        TimerShowSecond = 0;  // for display purpose.
      }

      /* Update timer on clock display every so many seconds. */
      if ((TimerShowCount % 1) == 0)
      {
        TimerShowCount  = 0;
        TimerShowSecond = TimerSeconds;
      }
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-8");



  /* ................................................................ *\
                             Count-up timer.
  \* ................................................................ */
  if (FlagSetTimerCountUp == FLAG_ON)
  {
    ++TimerSeconds;  // one more second for Up timer.
    ++TimerShowCount;

    /* Add one minute when we reach 60 seconds to the timer. */
    if (TimerSeconds == 60)
    {
      TimerSeconds = 0;
      ++TimerMinutes;

      /* If reaching out-of-bound, revert to zero. */
      if (TimerMinutes == 60) TimerMinutes = 0;
    }

    /* Update timer on the display every so many seconds. */
    if ((TimerShowCount % 1) == 0)
    {
      TimerShowCount  = 0;
      TimerShowSecond = TimerSeconds;
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-9");



  /* ................................................................ *\
                             Calendar events.
              Check if we should scroll a calendar event.
         This check is done twice an hour, at XXh14m and XXh44m.
  \* ................................................................ */
  /***
  if (DebugBitMask & DEBUG_EVENT)
    uart_send(__LINE__, "CurrentMinute: %2u   CurrentSecond: %2u   Check min1: %2u  Check min2: %2u   FlagEventDone: 0x%2.2X\r", CurrentMinute, CurrentSecond, EVENT_MINUTE1, EVENT_MINUTE2, FlagEventDone);
  ***/


  if ((CurrentSecond == 0) && (FlagEventDone == FLAG_OFF) && ((CurrentMinute == EVENT_MINUTE1) || (CurrentMinute == EVENT_MINUTE2)))
  {
    /* Prevent repeats. */
    FlagEventDone = FLAG_ON;

    for (Loop1UInt8 = 0; Loop1UInt8 < MAX_EVENTS; ++Loop1UInt8)
    {
      if ((CalendarEvent[Loop1UInt8].Day == CurrentDayOfMonth) && (CalendarEvent[Loop1UInt8].Month == CurrentMonth))
      {
        /* Scroll text corresponding to this event on clock display. */
        scroll_queue(Loop1UInt8);  // when Tag number is lower than MAX_EVENTS, it means to scroll the Calendar Event Text.

        /* Sound "event tones" in compliance with clock Chime settings. */
        if ((SilencePeriod == 0) &&
           ((FlashConfig.ChimeMode == CHIME_ON) ||                                                                                                                                                                      // always On
           ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOn  < FlashConfig.ChimeTimeOff) && (CurrentHour  >= FlashConfig.ChimeTimeOn)  && (CurrentHour <= FlashConfig.ChimeTimeOff)) ||  // "normal behavior"  for daytime workers.
           ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOff < FlashConfig.ChimeTimeOn)  && ((CurrentHour >= FlashConfig.ChimeTimeOff) || (CurrentHour <= FlashConfig.ChimeTimeOn)))))   // "special behavior" for nighttime workers.
        {
          #ifdef PASSIVE_PIEZO_SUPPORT
          /* Calendar Event sounds with passive buzzer if one has been installed by user, and if a jingle is defined with this Calendar Event. */
          if (CalendarEvent[Loop1UInt8].Jingle != 0)
          {
            /* If there is a jingle defined, play it. */
            play_jingle(CalendarEvent[Loop1UInt8].Jingle);
          }
          else {
            /* Calendar Event sounds with active buzzer. */
            for (Loop2UInt8 = 0; Loop2UInt8 < TONE_EVENT_REPEAT2; ++Loop2UInt8)
            {
              sound_queue_active(TONE_EVENT_DURATION, TONE_EVENT_REPEAT1);
              sound_queue_active(100, SILENT);
            }
          }
          #else
          /* Calendar Event sounds with active buzzer. */
          for (Loop2UInt8 = 0; Loop2UInt8 < TONE_EVENT_REPEAT2; ++Loop2UInt8)
          {
            sound_queue_active(TONE_EVENT_DURATION, TONE_EVENT_REPEAT1);
            sound_queue_active(100, SILENT);
          }
          #endif  // PASSIVE_PIEZO_SUPPORT
        }
      }
    }
  }



  /* Log tag. */
  if (DebugBitMask & DEBUG_TIMING)
    uart_send(__LINE__, "-10");



  /* ................................................................ *\
                               Night light.
  \* ................................................................ */
  /* Night light (the two white LEDs beside clock's three buttons) will
     turn On at NightLightTimeOn and turn Off at NightLightTimeOff if setting
     is set to NIGHT_LIGHT_NIGHT ("OI" when setting up the clock). */

  /* Check if it is time to turn On night light. */
  /* NOTE: Make a check every minute in case we just powered On the clock and / or we recently adjusted the time. */
  // Night light on/off hours are set in 24h values, so use CurrentHour to compare
  if ((FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT) && ((CurrentHour >= FlashConfig.NightLightTimeOn) || (CurrentHour < FlashConfig.NightLightTimeOff)))
  {
    if ((CurrentSecond == 0) && (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF))
    {
      IndicatorButtonLightsOn;
    }
  }

  /* Check if it is time to turn Off night light. */
  /* Make a check every minute in case we recently adjusted the time. */
  if ((FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT) && ((CurrentHour >= FlashConfig.NightLightTimeOff) && (CurrentHour < FlashConfig.NightLightTimeOn)))
  {
    if ((CurrentSecond == 0) && (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF))
    {
      IndicatorButtonLightsOff;
    }
  }

  /* If night light is in automatic mode, check ambient light and set night light accordingly.
     A gap is left between "On" and "Off" values to prevent a twilight period of On/Off blinking.
     However, no hysteris is added for an immediate response of the night light on ambient light change. */
  if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
  {
    LightLevel = adc_read_light();

    if (LightLevel < 295)
      IndicatorButtonLightsOn;

    if (LightLevel > 400)
      IndicatorButtonLightsOff;
  }


  /* For debugging purposes, check the duration of the one-second callback function and also add current DST status. */
  /***
  if ((DebugBitMask & DEBUG_SOUND_QUEUE) || (DebugBitMask & DEBUG_DST))
  {
    Timer2 = time_us_64();

    if (FlashConfig.FlagSummerTime == FLAG_ON)
      sprintf(String1, "Summer Time");
    else
      sprintf(String1, "Winter Time");

    CurrentCycles = Pwm[PWM_BRIGHTNESS].Cycles;
    pwm_set_cycles(0);  // blank display so that we don't see frozen display while LED scanning is delayed by UART.

    uart_send(__LINE__, "\r\r=== CALLBACK S ===   Duration: %5.2f mSec\r\r", ((Timer2 - Timer1) / 1000.0));

    pwm_set_cycles(CurrentCycles);  // restore display brightness.
  }
  ***/

  return TRUE;
}





/* $PAGE */
/* $TITLE=tone() */
/* ------------------------------------------------------------------ *\
        Make a tone for the specified number of milliseconds
    The piezo used in Waveshare clock has an integrated oscillator,
    so there is no need to make a software frequency with the gpio.
\* ------------------------------------------------------------------ */
void tone(UINT16 MilliSeconds)
{
  gpio_put(BUZZ, 1);
  sleep_ms(MilliSeconds);
  gpio_put(BUZZ, 0);

  return;
}





/* $PAGE */
/* $TITLE=total_one_bits() */
/* ------------------------------------------------------------------ *\
          Return the number of "one" bits in the given integer.
\* ------------------------------------------------------------------ */
UINT8 total_one_bits(UINT32 Data, UINT8 Size)
{
  UINT8 TotalOneBits;
  UINT8 Loop1UInt8;


  TotalOneBits = 0;
  for (Loop1UInt8 = 0; Loop1UInt8 < Size; ++Loop1UInt8)
  {
    if (Data & (1 << Loop1UInt8))
      ++TotalOneBits;
  }

  return TotalOneBits;
}





/* $PAGE */
/* $TITLE=uart_send() */
/* ------------------------------------------------------------------ *\
           Send a string to VT101 monitor through Pico UART.
\* ------------------------------------------------------------------ */
void uart_send(UINT LineNumber, UCHAR *Format, ...)
{
  UCHAR Dum1Str[256];
  UCHAR TimeStamp[128];

  UINT Loop1UInt;
  UINT StartChar;

  struct human_time HumanTime;

  va_list argp;


  /* Transfer the text to print to variable Dum1Str */
  va_start(argp, Format);
  vsnprintf(Dum1Str, sizeof(Dum1Str), Format, argp);
  va_end(argp);

  /* Trap special control code for <HOME>. Replace "home" by appropriate control characters for "Home" on a VT101. */
  if (strcmp(Dum1Str, "home") == 0)
  {
    Dum1Str[0] = 0x1B; // ESC code
    Dum1Str[1] = '[';
    Dum1Str[2] = 'H';
    Dum1Str[3] = 0x00;
  }

  /* Trap special control code for <CLS>. Replace "cls" by appropriate control characters for "Clear screen" on a VT101. */
  if (strcmp(Dum1Str, "cls") == 0)
  {
    Dum1Str[0] = 0x1B; // ESC code
    Dum1Str[1] = '[';
    Dum1Str[2] = '2';
    Dum1Str[3] = 'J';
    Dum1Str[4] = 0x00;
  }

  /* Time stamp will not be printed if first character is a '-' (for title line when starting debug, for example),
     or if first character is a line feed '\r' when we simply want add line spacing in the debug log,
     or if first character is the beginning of a control stream (for example 'Home' or "Clear screen'). */
  if ((Dum1Str[0] != '-') && (Dum1Str[0] != '\r') && (Dum1Str[0] != 0x1B) && (Dum1Str[0] != '|'))
  {
    /* Send line number through UART. */
    printf("[%7u]", LineNumber);

    /* Retrieve current time stamp. */
    date_stamp(TimeStamp);

    /* Send time stamp through UART. */
    printf(TimeStamp);
  }

  /* Send string through UART. */
  printf(Dum1Str);

  return;
}





/* $PAGE */
/* $TITLE=uint64_to_binary_string() */
/* ---------------------------------------------------------------- *\
     Return the string representing the uint64_t value in binary.
\* ---------------------------------------------------------------- */
void uint64_to_binary_string(UINT64 Value, UINT8 StringLength, UCHAR *BinaryString)
{
  UINT8 Loop1UInt8;

  /* Initialize binary string with all zeroes. */
  for (Loop1UInt8 = 0; Loop1UInt8 < StringLength; ++Loop1UInt8)
    BinaryString[Loop1UInt8] = '0';
  BinaryString[Loop1UInt8] = 0x00;  // end-of-string


  /***
  uart_send(__LINE__, "=============================================================================\r");
  uart_send(__LINE__, "Converting value %10llu [0x%8.8llX] to Binary string %u\r\r", Value, Value, StringLength);
  uart_send(__LINE__, "[%s]\r\r", BinaryString);
  uart_send(__LINE__, "BinaryString en entry:\r");
  for (Loop1UInt8 = 0; Loop1UInt8 < 65; ++Loop1UInt8)
  {
    printf("0x%2.2X   ", BinaryString[Loop1UInt8]);
  }
  printf("\r\r");
  ***/


  /* Replace a '0' with a '1' where it must be replaced. */
  for (Loop1UInt8 = 0; Loop1UInt8 < StringLength; ++Loop1UInt8)
  {
    if (Value & (1 << Loop1UInt8)) BinaryString[StringLength - (Loop1UInt8 + 1)] = '1';
    // uart_send(__LINE__, "Value to convert: %10llu (0x%8.8llX)  Bit number: %2u   Binary string so far: [%s]\r", Value, Value, Loop1UInt8, BinaryString);
  }

  return;
}




/* $PAGE */
/* $TITLE=update_dst_status()) */
/* ---------------------------------------------------------------- *\
          Set DST parameters (FlagSummerTime and Timezone)
      according to current DST setting and current date and time.
\* ---------------------------------------------------------------- */
void update_dst_status()
{
  UCHAR Dum1UChar;
  UCHAR String[256];

  int8_t UtcDisplayTime;
  int8_t UtcTime;


  /* Initializations. */
  FlagDaylightSavingTime = 0x55;  // undefined value.

  /* Get DayOfYear for current date. */
  CurrentDayOfYear = get_day_of_year(CurrentYear, CurrentMonth, CurrentDayOfMonth);


  if (DebugBitMask & DEBUG_DST)
  {
    Dum1UChar = ' ';
    if (FlashConfig.Timezone >= 0) Dum1UChar = '+';

    uart_send(__LINE__, "Entering update_dst_status()\r");
    uart_send(__LINE__, "DST country setting in flash memory:       %2u\r",     FlashConfig.DSTCountry);
    uart_send(__LINE__, "Timezone setting in flash memory:         %3d\r",      FlashConfig.Timezone);
    uart_send(__LINE__, "FlagSummerTime status in flash memory:    %3d\r",      FlashConfig.FlagSummerTime);
    uart_send(__LINE__, "DST check:    %8s %2u-%s-%4.4u   DoY: %3u\r",          DayName[FlashConfig.Language][CurrentDayOfWeek], CurrentDayOfMonth, ShortMonth[ENGLISH][CurrentMonth], CurrentYear, CurrentDayOfYear);
    uart_send(__LINE__, "Local hour: %2u:%2.2u    Delta with UTC time: %c%d\r", CurrentHour, CurrentMinute, Dum1UChar, FlashConfig.Timezone);
  }



  /* ------------------------------------------------------------------ *\
      Do not spend more time if Daylight Saving Time is not supported.
  \* ------------------------------------------------------------------ */
  if (FlashConfig.DSTCountry == DST_NONE)
  {
    /* DST is set to be ignored. */
    if (DebugBitMask & DEBUG_DST)
      uart_send(__LINE__, " -> DST not supported.\r");

    return;
  }



  /* ------------------------------------------------------------------ *\
       Check Northern countries for main periods between time change.
  \* ------------------------------------------------------------------ */
  if (DstParameters[FlashConfig.DSTCountry].StartMonth < DstParameters[FlashConfig.DSTCountry].EndMonth)  // Northern countries
  {
    /* Check if we are during Summer time. */
    if ((CurrentDayOfYear > DstParameters[FlashConfig.DSTCountry].StartDayOfYear) && (CurrentDayOfYear < DstParameters[FlashConfig.DSTCountry].EndDayOfYear))
    {
      if (DebugBitMask & DEBUG_DST)
        uart_send(__LINE__, " -> Currently Summer Time.\r");

      FlagDaylightSavingTime = FLAG_ON;
    }

    /* Check if we are during Winter time. */
    if ((CurrentDayOfYear > DstParameters[FlashConfig.DSTCountry].EndDayOfYear) || (CurrentDayOfYear < DstParameters[FlashConfig.DSTCountry].StartDayOfYear))
    {
      if (DebugBitMask & DEBUG_DST)
        uart_send(__LINE__, " -> Currently Winter Time.\r");

      FlagDaylightSavingTime = FLAG_OFF;
    }
  }



  /* ------------------------------------------------------------------ *\
       Check Southern countries for main periods between time change.
  \* ------------------------------------------------------------------ */
  if (DstParameters[FlashConfig.DSTCountry].StartMonth > DstParameters[FlashConfig.DSTCountry].EndMonth)  // Southern countries.
  {
    /* Check if we are during Summer time. */
    if ((CurrentDayOfYear > DstParameters[FlashConfig.DSTCountry].StartDayOfYear) || (CurrentDayOfYear < DstParameters[FlashConfig.DSTCountry].EndDayOfYear))
    {
      if (DebugBitMask & DEBUG_DST)
        uart_send(__LINE__, " -> Currently Summer Time.\r");

      FlagDaylightSavingTime = FLAG_ON;
    }

    /* Check if we are during Winter time. */
    if ((CurrentDayOfYear > DstParameters[FlashConfig.DSTCountry].EndDayOfYear) && (CurrentDayOfYear < DstParameters[FlashConfig.DSTCountry].StartDayOfYear))
    {
      if (DebugBitMask & DEBUG_DST)
        uart_send(__LINE__, " -> Currently Winter Time.\r");

      FlagDaylightSavingTime = FLAG_OFF;
    }
  }



  /* ------------------------------------------------------------------ *\
                   Check if today is the date to change
                     from Winter Time to Summer Time.
          Current time can't be in the period from "Start Hour" to
                      "Start Hour + Switch Minutes".
  \* ------------------------------------------------------------------ */
  if (CurrentDayOfYear == DstParameters[FlashConfig.DSTCountry].StartDayOfYear)
  {
    /* Today is the day we change from Summer Time to Winter Time... */
    if (DebugBitMask & DEBUG_DST)
      uart_send(__LINE__, " -> Today is time to change from Winter Time to Summer Time.\r");


    switch (FlashConfig.DSTCountry)
    {
      case (DST_EUROPE):
        /* Special case for countries where Spring-Forward is based on UTC. */
        UtcTime = CurrentHour - FlashConfig.Timezone;
        UtcDisplayTime = UtcTime;
        if (UtcDisplayTime < 0) UtcDisplayTime += 24;  // UTC time is still "Yesterday".
        if (DebugBitMask & DEBUG_DST)
          uart_send(__LINE__, " -> Handling European countries. Time change occurs at UTC time = 1h00 (current UTC time: %d:%2.2u).\r", UtcDisplayTime, CurrentMinute);

        if ((UtcTime < 0) || (UtcTime < DstParameters[FlashConfig.DSTCountry].StartHour))
        {
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Still Winter Time.\r");

          FlagDaylightSavingTime = FLAG_OFF;
        }
        else
        {
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Now Summer Time.\r");

          FlagDaylightSavingTime = FLAG_ON;
        }
      break;

      case (DST_CHILE):
        /* Chile is a special case since time change happens at "24h00. */
        if (CurrentHour < DstParameters[FlashConfig.DSTCountry].StartHour)
        {
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Still Winter Time.\r");

          FlagDaylightSavingTime = FLAG_OFF;
        }
      break;

      default:
        /* Other countries change from Winter Time to Summer Time based on local time (not UTC). */
        if (CurrentHour < DstParameters[FlashConfig.DSTCountry].StartHour)
        {
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Still Winter Time.\r");

          FlagDaylightSavingTime = FLAG_OFF;
        }
        else
        {
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Now Summer Time.\r");

          FlagDaylightSavingTime = FLAG_ON;
        }
      break;
    }
  }



  /* ------------------------------------------------------------------------------------------------------------------ *\
                            Check if today is the date to change from Summer Time to Winter Time.
                 If current time is during the period from "Start Hour" to "Start Hour + Switch Minutes":
           1) If the clock is running normally (CurrentClockMode = MODE_DISPLAY_TIME), update DST accordingly.
           2) If the clock is being powered-up, there is no way to know if hour change has already been done or not.
              In this case, let's assume that FlashConfig.FlagSummerTime is right.
  \* ------------------------------------------------------------------------------------------------------------------ */
  if (CurrentDayOfYear == DstParameters[FlashConfig.DSTCountry].EndDayOfYear)
  {
    /* Today is the day we change from Summer Time to Winter Time... */
    if (DebugBitMask & DEBUG_DST)
      uart_send(__LINE__, " -> Today is time to change from Summer Time to Winter Time.\r");


    switch (FlashConfig.DSTCountry)
    {
      case (DST_EUROPE):
        /* Special case for countries where Fall-Back is based on UTC time. */
        UtcTime = CurrentHour - FlashConfig.Timezone;
        UtcDisplayTime = UtcTime;
        if (UtcDisplayTime < 0) UtcDisplayTime += 24;
        if (DebugBitMask & DEBUG_DST)
          uart_send(__LINE__, " -> Handling European countries. Time change occurs at UTC time = 1h00 (current UTC time: %d:%2.2u).\r", UtcDisplayTime, CurrentMinute);

        if (UtcTime > DstParameters[FlashConfig.DSTCountry].EndHour)
        {
          /* We are sure that we are in Winter Time. */
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Now Winter Time.\r");

          FlagDaylightSavingTime = FLAG_OFF;
        }

        if (UtcTime < (int8_t)(DstParameters[FlashConfig.DSTCountry].EndHour - 1))
        {
          /* Soon enough to be sure we are still in Summer Time. */
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Still Summer Time.\r");

          FlagDaylightSavingTime = FLAG_ON;
        }

        /* We reach one hour before time change to Winter Time, we need to check if it has already occured one hour ago. */
        if (UtcTime == (DstParameters[FlashConfig.DSTCountry].EndHour - 1))
        {
          if (FlashConfig.FlagSummerTime == FLAG_OFF)
          {
            /* Hour change already occured one hour ago.*/
            FlagDaylightSavingTime = FLAG_OFF;
            uart_send(__LINE__, " -> Time change occured one hour ago. Now Winter Time.\r");
          }
          else
          {
            /* Hour change did not occur yet. Will be in one hour. */
            FlagDaylightSavingTime = FLAG_ON;
            uart_send(__LINE__, " -> Time change will occur in one hour. Now still Summer Time.\r");
          }
        }

        /* We reach hour to change to Winter Time, we need to check if it has already occured one hour ago. */
        if (UtcTime == DstParameters[FlashConfig.DSTCountry].EndHour)
        {
          if (FlashConfig.FlagSummerTime == FLAG_OFF)
          {
            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, " -> Time change already occured one hour ago.\r");
          }
          else
          {
            /* Either we are during normal clock mode, either we are during power-up and hour change has not occured yet. */
            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, " -> Now time to change to Winter Time.\r");
          }
          FlagDaylightSavingTime = FLAG_OFF;
        }
      break;

      case (DST_CHILE):
        /* Chile is a special case since time change occurs at 24h00. It means that when we fall back from 0h00 (24h00) to 23h00, we would trigger
           two (2) day changes for the same day. Let's return to Yesterday's date and let the second time we cross midnight the date will change. */
        if (CurrentHour < 23)
        {
          FlagDaylightSavingTime = FLAG_ON;

          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Still Summer Time.\r");
        }
        else
        {
          if (FlashConfig.FlagSummerTime == FLAG_ON)
          {
            /* This is the first time we cross 23h00, we are still in Summer Time. */
            FlagDaylightSavingTime = FLAG_ON;

            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, " -> Still Summer Time.\r");
          }
          else
          {
            /* This is the second time we cross 23h00, we are now in Winter Time. */
            FlagDaylightSavingTime = FLAG_OFF;

            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, " -> Now Winter Time.\r");
          }
        }
      break;

      default:
        /* Other countries change time based on local time (not UTC) */
        if (CurrentHour > DstParameters[FlashConfig.DSTCountry].EndHour)
        {
          /* We are sure that we are in Winter Time. */
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Now Winter Time.\r");

          FlagDaylightSavingTime = FLAG_OFF;
        }

        if (CurrentHour < (DstParameters[FlashConfig.DSTCountry].EndHour - 1))
        {
          /* Soon enough to be sure we are still in Summer Time. */
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, " -> Still Summer Time.\r");

          FlagDaylightSavingTime = FLAG_ON;
        }

        /* We reach one hour before time change to Winter Time, we need to check if it has already occured one hour ago. */
        if (CurrentHour == (DstParameters[FlashConfig.DSTCountry].EndHour - 1))
        {
          if (FlashConfig.FlagSummerTime == FLAG_OFF)
          {
            /* Hour change already occured one hour ago.*/
            FlagDaylightSavingTime = FLAG_OFF;
            uart_send(__LINE__, " -> Time change occured one hour ago. Now Winter Time.\r");
          }
          else
          {
            /* Hour change did not occur yet. Will be in one hour. */
            FlagDaylightSavingTime = FLAG_ON;
            uart_send(__LINE__, " -> Time change will occur in one hour. Now still Summer Time.\r");
          }
        }

        /* We reach hour to change to Winter Time, we need to check if it has already occured one hour ago. */
        if (CurrentHour == DstParameters[FlashConfig.DSTCountry].EndHour)
        {
          if (FlashConfig.FlagSummerTime == FLAG_OFF)
          {
            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, " -> Time change already occured one hour ago.\r");
          }
          else
          {
            /* Either we are during normal clock mode, either we are during power-up and hour change has not occured yet. */
            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, " -> Now time to change to Winter Time.\r");
          }
          FlagDaylightSavingTime = FLAG_OFF;
        }
      break;
    }
  }



  if (FlagDaylightSavingTime == 0x55)
    if (DebugBitMask & DEBUG_DST)
      uart_send(__LINE__, " -> CASE NOT COVERED... Please report this problem and how to reproduce it.\r");



  /* ------------------------------------------------------------------ *\
         Now that we know if we are in Summer Time or Winter Time,
           set the Daylight Saving Time parameters accordingly.
  \* ------------------------------------------------------------------ */
  if (FlagDaylightSavingTime == FLAG_ON)
  {
    if (FlashConfig.FlagSummerTime == FLAG_OFF)
    {
      /* We are in Summer Time, but flash indicates that we are still in Winter Time... We need to adjust Timezone and clock time. */
      ++FlashConfig.Timezone;

      if (DebugBitMask & DEBUG_DST)
      {
        uart_send(__LINE__, "  --------------------------------------------------------===> Changing from Winter Time to Summer Time.\r");
        uart_send(__LINE__, "DST country code: %u   Number of minutes to shift: %u\r",
                             FlashConfig.DSTCountry, DstParameters[FlashConfig.DSTCountry].ShiftMinutes);
      }

      /* Check shift minutes, just in case we have to cover other than 60 minutes shifts later on. */
      if (DstParameters[FlashConfig.DSTCountry].ShiftMinutes == 60)
      {
        if (DebugBitMask & DEBUG_DST)
          uart_send(__LINE__, "Current Hour that will be incremented: %u:%2.2u\r", CurrentHour, CurrentMinute);

        ++CurrentHour;
        CurrentHourSetting = CurrentHour;
        set_hour(CurrentHour);
      }
      else
      {
        if (DebugBitMask & DEBUG_DST)
          uart_send(__LINE__, "Current minutes that will be incremented: %u\r", CurrentHour);

        CurrentMinute += DstParameters[FlashConfig.DSTCountry].ShiftMinutes;
        CurrentMinuteSetting = CurrentMinute;
        set_minute(CurrentMinuteSetting);
        if (DebugBitMask & DEBUG_DST)
          uart_send(__LINE__, "Current Time after hour change: %u:%2.2u\r", CurrentHour, CurrentMinute);
      }
      show_time();  // update clock display to show time change.
    }
    FlashConfig.FlagSummerTime = FLAG_ON;
  }



  if (FlagDaylightSavingTime == FLAG_OFF)
  {
    if (FlashConfig.FlagSummerTime == FLAG_ON)
    {
      /* We are in Winter Time, but flash indicates that we are still in Summer Time... We need to adjust Timezone and clock time. */
      if (DebugBitMask & DEBUG_DST)
      {
        uart_send(__LINE__, " ---------------------------------------------------------===> Changing from Summer Time to Winter Time.\r");
        uart_send(__LINE__, "DST country code: %u   Number of minutes to shift: %u\r",
                             FlashConfig.DSTCountry, DstParameters[FlashConfig.DSTCountry].ShiftMinutes);
      }

      switch (FlashConfig.DSTCountry)
      {
        case (DST_CHILE):
          /* Special case for Chile since they are changing time at "24h00", whereas we are already "tomorrow". */
          CurrentHour = 23;

          --CurrentDayOfMonth;
          if (CurrentDayOfMonth == 0)
          {
            --CurrentMonth;
            CurrentDayOfMonth = get_month_days(CurrentYear, CurrentMonth);
          }

          CurrentHourSetting = CurrentHour;
          set_hour(CurrentHour);
          set_day_of_month(CurrentDayOfMonth);
          set_month(CurrentMonth);

          /* We are in Winter Time, but flash indicates that we are in Summer Time... We need to adjust DST parameters. */
          --FlashConfig.Timezone;
        break;

        case (DST_LEBANON):
        case (DST_PARAGUAY):
          /* Special case for Lebanon and Paraguay since we are changing time at "0h00", and moving it back to 23h00 Yesterday.
             We need to make some housekeeping so that we don't change the day (date, DayOfMonth) twice while crossing 0h00 twice. */
          CurrentHour = 23;

          --CurrentDayOfMonth;
          if (CurrentDayOfMonth == 0)
          {
            --CurrentMonth;
            CurrentDayOfMonth = get_month_days(CurrentYear, CurrentMonth);
          }

          CurrentHourSetting = CurrentHour;
          set_hour(CurrentHour);
          set_day_of_month(CurrentDayOfMonth);
          set_month(CurrentMonth);

          /* We are in Winter Time, but flash indicates that we are in Summer Time... We need to adjust DST parameters. */
          --FlashConfig.Timezone;
        break;

        default:
          if (DebugBitMask & DEBUG_DST)
            uart_send(__LINE__, "Current Hour that will be decremented: %u:%2.2u\r", CurrentHour, CurrentMinute);

          /* We are in Winter Time, but flash indicates that we are in Summer Time... We need to adjust DST parameters. */
          --FlashConfig.Timezone;

          /* Check shift minutes, just in case we have to cover other than 60 minutes shifts later on. */
          if (DstParameters[FlashConfig.DSTCountry].ShiftMinutes == 60)
          {
            --CurrentHour;
            CurrentHourSetting = CurrentHour;
            set_hour(CurrentHour);
          }
          else
          {
            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, "Current Minute that will be decremented: %u:%2.2u\r", CurrentHour, CurrentMinute);

            CurrentMinute -= DstParameters[FlashConfig.DSTCountry].ShiftMinutes;
            if (CurrentMinute > 59)  // went below 0.
            {
              CurrentMinute += 60;
              --CurrentHour;
            }
            CurrentHourSetting   = CurrentHour;
            CurrentMinuteSetting = CurrentMinute;
            set_hour(CurrentHour);
            set_minute(CurrentMinuteSetting);
            if (DebugBitMask & DEBUG_DST)
              uart_send(__LINE__, "Current Time after hour change: %u:%2.2u\r", CurrentHour, CurrentMinute);
          }
        break;
      }
      show_time();  // update clock display to show time change.
    }
    FlashConfig.FlagSummerTime = FLAG_OFF;
  }

  uart_send(__LINE__, "\r");

  return;
}





/* $PAGE */
/* $TITLE=update_left_indicators() */
/* ------------------------------------------------------------------ *\
           Update indicators on the left of clock display.
\* ------------------------------------------------------------------ */
void update_left_indicators(void)
{
  UCHAR String[256];

  UINT8 Loop1UInt8;


  /* ---------------------------------------------------------------- *\
      Check if "Move On" (scrolling) indicator should be turned On.
  \* ---------------------------------------------------------------- */
  if (FlashConfig.FlagScrollEnable == FLAG_ON)
    IndicatorScrollOn;

  if (DebugBitMask & DEBUG_INDICATORS)
    uart_send(__LINE__, "FlagScrollEnable:   0x%2.2X\r", FlashConfig.FlagScrollEnable);


  /* ---------------------------------------------------------------- *\
         Turn on "Alarm On" indicator if at least one alarm is On.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
  {
    if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_ON)
    {
      if (DebugBitMask & DEBUG_INDICATORS)
        uart_send(__LINE__, "Alarm[%1u]:   0x%2.2X\r", Loop1UInt8, FlashConfig.Alarm[Loop1UInt8]);

      IndicatorAlarmOn;
      break; // get out of for loop as soon as alarm indicator is On.
    }
  }



  /* ---------------------------------------------------------------- *\
                 Turn on selected temperature indicator.
  \* ---------------------------------------------------------------- */
  if (FlashConfig.TemperatureUnit == FAHRENHEIT)
  {
    IndicatorFrnhtOn;
    IndicatorCelsiusOff;
  }
  else
  {
    IndicatorCelsiusOn;
    IndicatorFrnhtOff;
  }

  if (DebugBitMask & DEBUG_INDICATORS)
    uart_send(__LINE__, "Temperature unit:   0x%2.2X\r", FlashConfig.TemperatureUnit);


  /* ---------------------------------------------------------------- *\
               Turn on appropriate Hourly chime indicator.
  \* ---------------------------------------------------------------- */
  switch (FlashConfig.ChimeMode)
  {
    case (CHIME_OFF):
      IndicatorHourlyChimeOff;
    break;

    case (CHIME_ON):
      IndicatorHourlyChimeOn;
    break;

    case (CHIME_DAY):
      IndicatorHourlyChimeDay;
    break;
  }

  if (DebugBitMask & DEBUG_INDICATORS)
    uart_send(__LINE__, "ChimeMode:             %u\r", FlashConfig.ChimeMode);



  /* ---------------------------------------------------------------- *\
            Turn On auto-brightness indicator if appropriate.
  \* ---------------------------------------------------------------- */
  if (FlashConfig.FlagAutoBrightness == FLAG_ON)
    IndicatorAutoLightOn;

  if (DebugBitMask & DEBUG_INDICATORS)
    uart_send(__LINE__, "FlagAutoBrightness: 0x%2.2X\r\r", FlashConfig.FlagAutoBrightness);

  return;
}





/* $PAGE */
/* $TITLE=update_top_indicators() */
/* ------------------------------------------------------------------ *\
           Update DayOfWeek indicators on the clock display.
\* ------------------------------------------------------------------ */
void update_top_indicators(UINT8 DayOfWeek, UINT8 Flag)
{
  if (Flag == FLAG_ON)
  {
    /* Turn On specified DayOfWeek. */
    switch (DayOfWeek)
    {
      case (SUN):
        DisplayBuffer[16] |= (1 << 5) | (1 << 6);  // turn ON both LEDs of Sunday indicator
      break;

      case (MON):
        DisplayBuffer[0] |= (1 << 3) | (1 << 4);  // turn ON both LEDs of Monday indicator
      break;

      case (TUE):
        DisplayBuffer[0] |= (1 << 6) | (1 << 7);  // turn ON  both LEDs of Tuesday indicator
      break;

      case (WED):
        DisplayBuffer[8] |= (1 << 1) | (1 << 2);  // turn ON  both LEDs of Wednesday indicator
      break;

      case (THU):
        DisplayBuffer[8] |= (1 << 4) | (1 << 5);  // turn ON  both LEDs of Thursday indicator
      break;

      case (FRI):
        DisplayBuffer[8]  |= (1 << 7);  // turn ON first  LED of Friday indicator
        DisplayBuffer[16] |= (1 << 0);  // turn ON second LED of Friday indicator
      break;

      case (SAT):
        DisplayBuffer[16] |= (1 << 2) | (1 << 3);  // turn ON  both LEDs of Saturday indicator
      break;

      case (ALL):
      default:
        /* For "ALL", or in case of error, turn On all DayOfWeek indicators. */
        DisplayBuffer[16] |= (1 << 5) | (1 << 6);  // turn ON both   LEDs of Sunday    indicator
        DisplayBuffer[0]  |= (1 << 3) | (1 << 4);  // turn ON both   LEDs of Monday    indicator
        DisplayBuffer[0]  |= (1 << 6) | (1 << 7);  // turn ON both   LEDs of Tuesday   indicator
        DisplayBuffer[8]  |= (1 << 1) | (1 << 2);  // turn ON both   LEDs of Wednesday indicator
        DisplayBuffer[8]  |= (1 << 4) | (1 << 5);  // turn ON both   LEDs of Thursday  indicator
        DisplayBuffer[8]  |= (1 << 7);             // turn ON first  LED  of Friday    indicator
        DisplayBuffer[16] |= (1 << 0);             // turn ON second LED  of Friday    indicator
        DisplayBuffer[16] |= (1 << 2) | (1 << 3);  // turn ON both   LEDs of Saturday  indicator
      break;
    }
  }


  if (Flag == FLAG_OFF)
  {
    /* Turn Off specified DayOfWeek indicator. */
    switch (DayOfWeek)
    {
      case (SUN):
        DisplayBuffer[16] &= ~((1 << 5) | (1 << 6));  // turn OFF both LEDs of Sunday indicator
      break;

      case (MON):
        DisplayBuffer[0] &= ~((1 << 3) | (1 << 4));  // turn OFF both LEDs of Monday indicator
      break;

      case (TUE):
        DisplayBuffer[0] &= ~((1 << 6) | (1 << 7));  // turn OFF both LEDs of Tuesday indicator
      break;

      case (WED):
        DisplayBuffer[8] &= ~((1 << 1) | (1 << 2));  // turn OFF both LEDs of Wednesday indicator
      break;

      case (THU):
        DisplayBuffer[8] &= ~((1 << 4) | (1 << 5));  // turn OFF both LEDs of Thursday indicator
      break;

      case (FRI):
        DisplayBuffer[8]  &= ~(1 << 7);  // turn OFF first  LED of Friday indicator
        DisplayBuffer[16] &= ~(1 << 0);  // turn OFF second LED of Friday indicator
      break;

      case (SAT):
        DisplayBuffer[16] &= ~((1 << 2) | (1 << 3));  // turn OFF both LEDs of Saturday indicator
      break;

      case (ALL):
        /* Turn Off all day-of-week indicators. */
        DisplayBuffer[16] &= ~((1 << 5) | (1 << 6));  // Sunday
        DisplayBuffer[0]  &= ~((1 << 3) | (1 << 4));  // Monday
        DisplayBuffer[0]  &= ~((1 << 6) | (1 << 7));  // Tuesday
        DisplayBuffer[8]  &= ~((1 << 1) | (1 << 2));  // Wednesday
        DisplayBuffer[8]  &= ~((1 << 4) | (1 << 5));  // Thursday
        DisplayBuffer[8]  &= ~(1 << 7);               // Friday - a
        DisplayBuffer[16] &= ~(1 << 0);               // Friday - b
        DisplayBuffer[16] &= ~((1 << 2) | (1 << 3));  // Saturday
      break;

      default:
        /* In case of error, turn On all indicators. */
        DisplayBuffer[16] |= (1 << 5) | (1 << 6);  // turn On both   LEDs of Sunday    indicator
        DisplayBuffer[0]  |= (1 << 3) | (1 << 4);  // turn On both   LEDs of Monday    indicator
        DisplayBuffer[0]  |= (1 << 6) | (1 << 7);  // turn On both   LEDs of Tuesday   indicator
        DisplayBuffer[8]  |= (1 << 1) | (1 << 2);  // turn On both   LEDs of Wednesday indicator
        DisplayBuffer[8]  |= (1 << 4) | (1 << 5);  // turn On both   LEDs of Thursday  indicator
        DisplayBuffer[8]  |= (1 << 7);             // turn On first  LED  of Friday    indicator
        DisplayBuffer[16] |= (1 << 0);             // turn On second LED  of Friday    indicator
        DisplayBuffer[16] |= (1 << 2) | (1 << 3);  // turn On both   LEDs of Saturday  indicator
      break;
    }
  }

  return;
}

/* ------------------------------------------------------------------ *\
           Web control and status information routines.
\* ------------------------------------------------------------------ */
// Fetch the hostname from the flash config
UCHAR* wfetch_hostname(void) {
  return &FlashConfig.Hostname[4];
}

// Fetch the WiFi SSID from the flash config
UCHAR* wfetch_wifissid(void) {
  return &FlashConfig.SSID[4];
}

// Fetch the WiFi passphrase from the flash config
UCHAR* wfetch_wifipass(void) {
  return &FlashConfig.Password[4];
}

// Write the hostname to the flash config fairly soon and restart the network
void wwrite_hostname(UCHAR * new_hostname) {
  // Update the configured values
  sprintf(&FlashConfig.Hostname[4], new_hostname);
  // Perform a flash configuration update check soon
  FlagWebRequestFlashCheck = FLAG_ON;
  return;
}

// Write the WiFi SSID and passphrase to the flash config fairly soon and restart the network
void wwrite_networkcfg(UCHAR * new_wifissid, UCHAR * new_wifipass) {
  // Update the configured values
  sprintf(&FlashConfig.Password[4], new_wifipass);
  sprintf(&FlashConfig.SSID[4], new_wifissid);
  // Perform a flash configuration update check soon
  FlagWebRequestFlashCheck = FLAG_ON;
  return;
}

// Fetch the current time from the main clock routine
struct human_time wfetch_current_datetime(void) {
  struct human_time current_time;
  current_time.Hour = CurrentHour;
  current_time.Minute = CurrentMinute;
  current_time.Second = CurrentSecond;
  current_time.DayOfMonth = CurrentDayOfMonth;
  current_time.Month = CurrentMonth;
  current_time.Year = CurrentYear;
  current_time.DayOfWeek = CurrentDayOfWeek;
  current_time.DayOfYear = CurrentDayOfYear;
  current_time.FlagDst = FlashConfig.FlagSummerTime;
  return current_time;
}

// Write a new date and time to the RTC and display
void wwrite_current_datetime(struct human_time new_time) {
  UINT8 shortyear;
  UINT16 longyear;
  // Update the current time, as if the set buttons were used
  CurrentSecond = new_time.Second;
  CurrentMinute = new_time.Minute;
  CurrentHour = new_time.Hour;
  CurrentDayOfMonth = new_time.DayOfMonth;
  CurrentMonth = new_time.Month;
  CurrentYearLowPart = (new_time.Year % 100);
  CurrentYear = new_time.Year;
  CurrentDayOfWeek = get_day_of_week(CurrentYear, CurrentMonth, CurrentDayOfMonth);
  CurrentDayOfYear = get_day_of_year(CurrentYear, CurrentMonth, CurrentDayOfMonth);
  // Set the RTC using the same methods as the PicoW NTP synchronisation
  set_time(CurrentSecond, CurrentMinute, CurrentHour, CurrentDayOfWeek, CurrentDayOfMonth, CurrentMonth, CurrentYearLowPart);
  // Request time display update when all completed
  FlagUpdateTime = FLAG_ON;
  return;
}

// Request an NTP synchronisation
void wrequest_NTP_Sync() {
  FlagWebRequestNTPSync = FLAG_ON;
  return;
}

// Return NTP access error count
UINT32 wfetch_NTP_Errors(void){
  return NTPData.NTPErrors;
}

// Clear down NTP access error count
void wwrite_clear_ntp_error(){
  NTPData.NTPErrors = 0;
}

// Fetch the current language index from the flash config
UINT8 wfetch_current_language(void) {
  return FlashConfig.Language;
}

// Fetch the day name text in the current language from the main routine's table
UCHAR* wfetch_DayName(UINT8 the_language, UINT16 the_dayofweek) {
  return DayName[the_language][the_dayofweek];
}

// Fetch the month name text in the current language from the main routine's table
UCHAR* wfetch_MonthName(UINT8 the_language, UINT16 the_month) {
  return MonthName[the_language][the_month];
}

// Fetch the 12HR or 24HR hour display mode from the flash config
UINT8 wfetch_current_hour_mode(void){
  return FlashConfig.TimeDisplayMode;
}

// Fetch the currently programmed data for an alarm from the flash config
struct alarm wfetch_alarm(UINT8 alarm_to_fetch) {
  struct alarm my_alarm;
  my_alarm.FlagStatus = FlashConfig.Alarm[alarm_to_fetch].FlagStatus;
  my_alarm.Second = FlashConfig.Alarm[alarm_to_fetch].Second;
  my_alarm.Minute = FlashConfig.Alarm[alarm_to_fetch].Minute;
  my_alarm.Hour = FlashConfig.Alarm[alarm_to_fetch].Hour;
  my_alarm.Day = FlashConfig.Alarm[alarm_to_fetch].Day;
  #ifdef PASSIVE_PIEZO_SUPPORT
  my_alarm.Jingle = FlashConfig.Alarm[alarm_to_fetch].Jingle;
  #else
  // For no passive buzzer support, force the top bit to be set
  if (FlashConfig.Alarm[alarm_to_fetch].Jingle < 128) {
    my_alarm.Jingle = FlashConfig.Alarm[alarm_to_fetch].Jingle | 0x80;
  } else {
    my_alarm.Jingle = FlashConfig.Alarm[alarm_to_fetch].Jingle;
  }
  #endif
  sprintf(my_alarm.Text, FlashConfig.Alarm[alarm_to_fetch].Text);
  return my_alarm;
}

// write a new alarm data to the flash config and update the alarm set display indicator
void wwrite_alarm(UINT8 alarm_to_write, struct alarm alarm_data) {
  UINT8 Dum1UInt8;
  UINT8 Loop1UInt8;
  // Set values into alarm structure, text data will be pre-checked for length so we only need one update.
  FlashConfig.Alarm[alarm_to_write] = alarm_data;
  // Now update the display as this is only handled in the setting routine (which is bypassed)
  /* We now have 9 alarms available... Check if at least one alarm is On and if ever the case, turn On both alarm indicators. */
  Dum1UInt8 = 0;
  for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
    if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_ON)
      Dum1UInt8 = 1;

  if (Dum1UInt8)
  {
    IndicatorAlarm0On;
    IndicatorAlarm1On;
  }
  else
  {
    IndicatorAlarm0Off;
    IndicatorAlarm1Off;
  }
  return;
}

// Fetch a set of status data on the display dimming levels to help calibrate the minimum setting
struct web_light_value wfetch_light_adc_level(void) {
  struct web_light_value dimmer_light_values;
  dimmer_light_values.adc_current_value = adc_read_light();
  dimmer_light_values.AverageLightLevel = Dim_AverageLightLevel;
  dimmer_light_values.MinLightLevel = FlashConfig.DimmerMinLightLevel;
  dimmer_light_values.DutyCycle = Dim_DutyCycle;
  dimmer_light_values.Min_adc_value = Dim_Min_avgadc_value;
  dimmer_light_values.Max_adc_value = Dim_Max_avgadc_value;
  return dimmer_light_values;
}

// Write a new value for the minimum display very dim level to the flash config
void wwrite_dimminlightlevel(UINT16 new_lightlevel) {
  FlashConfig.DimmerMinLightLevel = new_lightlevel;
  return;
}

// Fetch the display dimming mode from the flash config
UINT8 fetch_AutoBrightness(void) {
  return FlashConfig.FlagAutoBrightness;
}

// Fetch the display manual dimming value from the current PWM setting
UINT16 fetch_ManualBrightness(void) {
  return Pwm[PWM_BRIGHTNESS].Cycles;
}

// Fetch the button press beep from the flash config
UINT8 fetch_Keyclick(void) {
  return FlashConfig.FlagKeyclick;
}

// Fetch the scrolling display mode from the flash config
UINT8 fetch_ScrollEnable(void) {
  return FlashConfig.FlagScrollEnable;
}

// Fetch the scrolling display mode from the flash config
UINT8 fetch_ShortSeyKey(void){
  return FlashConfig.ShortSetKey;
}

// Fetch the DST active flag from the flash config
UINT8 fetch_SummerTime(void) {
  return FlashConfig.FlagSummerTime;
}

// Fetch the DST configuration from the flash config
UCHAR fetch_DSTCountry(void) {
  return FlashConfig.DSTCountry;
}

// Fetch the UTC offset timezone from the flash config
// When DST summertime is active, the timezone is incremented, so decrement before returning the value
int8_t fetch_Timezone(void) {
  int8_t BaseTimezone;
  BaseTimezone = FlashConfig.Timezone;
  if (FlashConfig.FlagSummerTime == FLAG_ON) {
    BaseTimezone = BaseTimezone - 1;
  }
  return BaseTimezone;
}

// Fetch the 12/24HR display mode from the flash config
UINT8 fetch_ClockHourMode(void) {
  return FlashConfig.TimeDisplayMode;
}

UINT8 fetch_ChimeMode(void) {
  return FlashConfig.ChimeMode;
}
UINT8 fetch_ChimeStart(void) {
  return FlashConfig.ChimeTimeOn;
}
UINT8 fetch_ChimeStop(void) {
  return FlashConfig.ChimeTimeOff;
}
UINT8 fetch_NightLightMode(void) {
  return FlashConfig.NightLightMode;
}
UINT8 fetch_NightLightStart(void) {
  return FlashConfig.NightLightTimeOn;
}
UINT8 fetch_NightLightStop(void) {
  return FlashConfig.NightLightTimeOff;
}


// Write a new value for display dimming mode to the flash config, set as enabled/disabled or one of 4 manual levels - bright (1), mid (2), low (3) or dimmest (4)
void wwriteAutoBrightness(UINT8 new_AutoBrightness, UINT8 new_ManualLevel) {
  /* Set the "auto-brightness" On / Off and cycle through 5 levels of manual settings. */
  if (new_AutoBrightness == FLAG_ON)
  {
    FlashConfig.FlagAutoBrightness = FLAG_ON;
    IndicatorAutoLightOn;
    pwm_set_cycles(BRIGHTNESS_LIGHTLEVELSTEP);
  }
  else if (new_AutoBrightness == FLAG_OFF && new_ManualLevel == 1)
  {
    FlashConfig.FlagAutoBrightness = FLAG_OFF;
    IndicatorAutoLightOff;
    pwm_set_cycles(BRIGHTNESS_LIGHTLEVELSTEP);
  }
  else if (new_AutoBrightness == FLAG_OFF && new_ManualLevel == 2)
  {
    FlashConfig.FlagAutoBrightness = FLAG_OFF;
    IndicatorAutoLightOff;
    pwm_set_cycles(BRIGHTNESS_MANUALDIV1);
  }
  else if (new_AutoBrightness == FLAG_OFF && new_ManualLevel == 3)
  {
    FlashConfig.FlagAutoBrightness = FLAG_OFF;
    IndicatorAutoLightOff;
    pwm_set_cycles(BRIGHTNESS_MANUALDIV2);
  }
  else if (new_AutoBrightness == FLAG_OFF && new_ManualLevel == 4)
  {
    FlashConfig.FlagAutoBrightness = FLAG_OFF;
    IndicatorAutoLightOff;
    pwm_set_cycles(BRIGHTNESS_MANUALDIV3);
  }
  else if (new_AutoBrightness == FLAG_OFF && new_ManualLevel == 5)
  {
    FlashConfig.FlagAutoBrightness = FLAG_OFF;
    IndicatorAutoLightOff;
    pwm_set_cycles(0);
  }
  else
  {
    FlashConfig.FlagAutoBrightness = FLAG_ON;
    IndicatorAutoLightOn;
    pwm_set_cycles(BRIGHTNESS_LIGHTLEVELSTEP);
  }
  return;
}

// Write a new value for button press beep enable to the flash config
void wwriteKeyclick(UINT8 new_Keyclick) {
  if ((new_Keyclick == FLAG_OFF) || (new_Keyclick == FLAG_ON)) {
    FlashConfig.FlagKeyclick = new_Keyclick;
  }
  return;
}

// Write a new value for scrolling display enable to the flash config
void wwriteScrollEnable(UINT8 new_ScrollEnable) {
  if ((new_ScrollEnable == FLAG_OFF) || (new_ScrollEnable == FLAG_ON)) {
    FlashConfig.FlagScrollEnable = new_ScrollEnable;
    /* Setup the "Scroll" set indicator */
    if (new_ScrollEnable == FLAG_ON) {
      IndicatorScrollOn;
    }
    else {
      IndicatorScrollOff;
    }
  }
  return;
}

// Write a new value for teh short set key operation to the flash config
void wwriteShortSeyKey(UINT8 new_SetKeyMode){
  if ((new_SetKeyMode == FLAG_OFF) || (new_SetKeyMode == FLAG_ON)) {
    FlashConfig.ShortSetKey = new_SetKeyMode;
  }
  return;
}

// Write a new value for DST active flag to the flash config
void wwriteSummerTime(UINT8 new_SummerTime) {
  int8_t BaseTimezone;
  BaseTimezone = FlashConfig.Timezone;
  // Now set the summertime flag to the chosen value. If it is changing, then the timezone offset also needs to be altered.
  if (new_SummerTime == FLAG_OFF) {
    if (FlashConfig.FlagSummerTime == FLAG_ON) {
      // If we are changing from summer to winter, then reduce the timezone offset by 1 hour
      BaseTimezone = BaseTimezone - 1;
    }
    FlashConfig.FlagSummerTime = new_SummerTime;
    FlashConfig.Timezone = BaseTimezone;
  }
  if (new_SummerTime == FLAG_ON) {
    if (FlashConfig.FlagSummerTime == FLAG_OFF) {
      // If we are changing from winter to summer, then increase the timezone offset by 1 hour
      BaseTimezone = BaseTimezone + 1;
    }
    FlashConfig.FlagSummerTime = new_SummerTime;
    FlashConfig.Timezone = BaseTimezone;
  }
  return;
}

// Write a new value for the DST configuration to the flash config
void mwrite_DSTCountry(UCHAR new_DST_Country) {
  FlashConfig.DSTCountry = new_DST_Country;
  return;
}

// Write a new value for UTC offset timezone to the flash config
// When DST summertime is active, the timezone is incremented from the requested value
void wwriteTimezone(int8_t new_Timezone) {
  int8_t BaseTimezone;
  BaseTimezone = new_Timezone;
  if (FlashConfig.FlagSummerTime == FLAG_ON) {
    BaseTimezone = BaseTimezone + 1;
  }
  FlashConfig.Timezone = BaseTimezone;
  return;
}

// Write a new value for language to the flash config if in the correct range
void mwritelanguage(UINT8 new_language) {
  if (new_language > LANGUAGE_LO_LIMIT && new_language < LANGUAGE_HI_LIMIT) {
    FlashConfig.Language = new_language;
  }
  return;
}

void wwrite_ClockHourMode(UINT8 new_hourmode){
  if ((new_hourmode == H12) || (new_hourmode == H24)) {
    FlashConfig.TimeDisplayMode = new_hourmode;
    // Request time display update when all completed
    FlagUpdateTime = FLAG_ON;
  }
  return;
}

void wwrite_ChimeMode(UINT8 new_ChimeMode) {
  if (new_ChimeMode == CHIME_OFF) {
    FlashConfig.ChimeMode = CHIME_OFF;
    IndicatorHourlyChimeOff;
  }
  else if (new_ChimeMode == CHIME_ON) {
    FlashConfig.ChimeMode = CHIME_ON;
    IndicatorHourlyChimeOn;
  }
  else if (new_ChimeMode == CHIME_DAY) {
    FlashConfig.ChimeMode = CHIME_DAY;
    IndicatorHourlyChimeDay;
  }
  return;
}

void wwrite_ChimeStart(UINT8 new_ChimeStart) {
  FlashConfig.ChimeTimeOn = new_ChimeStart;
  return;
}

void wwrite_ChimeStop(UINT8 new_ChimeStop) {
  FlashConfig.ChimeTimeOff = new_ChimeStop;
  return;
}

void wwrite_NightLightMode(UINT8 new_NightLightMode) {
  if (new_NightLightMode == NIGHT_LIGHT_OFF) {
    FlashConfig.NightLightMode = NIGHT_LIGHT_OFF;
    IndicatorButtonLightsOff;
  }
  else if (new_NightLightMode == NIGHT_LIGHT_ON){
    FlashConfig.NightLightMode = NIGHT_LIGHT_ON;
    IndicatorButtonLightsOn;
  }
  else if (new_NightLightMode == NIGHT_LIGHT_AUTO) {
    FlashConfig.NightLightMode = NIGHT_LIGHT_AUTO;
    UINT16 LightLevel = adc_read_light();
    if (LightLevel < 295) {
      IndicatorButtonLightsOn;
    }
    if (LightLevel > 400) {
      IndicatorButtonLightsOff;
    }
  }
  else if (new_NightLightMode == NIGHT_LIGHT_NIGHT) {
    FlashConfig.NightLightMode = NIGHT_LIGHT_NIGHT;
    if ((CurrentHour >= FlashConfig.NightLightTimeOn) || (CurrentHour < FlashConfig.NightLightTimeOff)) {
      IndicatorButtonLightsOn;
    }
    if ((CurrentHour >= FlashConfig.NightLightTimeOff) && (CurrentHour < FlashConfig.NightLightTimeOn)) {
      IndicatorButtonLightsOff;
    }
  }
  return;
}

void wwrite_NightLightStart(UINT8 new_NightLightStart) {
  FlashConfig.NightLightTimeOn = new_NightLightStart;
  return;
}

void wwrite_NightLightStop(UINT8 new_NightLightStop) {
  FlashConfig.NightLightTimeOff = new_NightLightStop;
  return;
}


