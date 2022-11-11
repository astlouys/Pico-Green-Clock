/* ================================================================== *\
   Pico-Clock-Green.c
   St-Louys Andre - February 2022
   astlouys@gmail.com
   Revision 25-OCT-2022
   Compiler: arm-none-eabi-gcc 7.3.1
   Version 6.00

   Raspberry Pi Pico firmware to drive the Waveshare Pico Green Clock.
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
   04-FEB-2022 1.00 - Initial release from Waveshare.

   10-FEB-2022 2.00 - Global code reformatting and rework.
                    - Fix a bug allowing "DayOfMonth" to be set to 0.
                    - Fix a bug allowing "DayOfMonth" to go higher than the maximum of 28, 29, 30 or 31 (depending of the  month).
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

   24-MAR-2022 3.00 - More code rework and optimisation.
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

   04-APR-2022 4.00 - Some code optimization (there is much more to do !).
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
                    - Modify get_ads1015() function so that light value returned is intuitive: higher number means more light. Auto-brightness and
                      automatic night-light functions have been adapted accordingly.

   19-JUN-2022 5.00 - Uncomment matrix_test() and pixel_twinkling() during power-up sequence that were left mistakenly commented-out in release 4.00.
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

   25-AUG-2022 6.00 - Add Pico's flash memory read, write and erase functions.
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
    - Replace the Pico with a Pico W and use WiFi to set clock time by
      reading it from a time reference source on the Internet (NTP).
      (RTC IC is said to be very accurate, but I haven't tested it over
      a long period of time. Not sure what is the long-term precision).
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
           protocol can be reproduced on the Pico Green Clock.
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
           according to user needs. It can be found below,
        above the "Global variables declaration / definition".
\* ================================================================== */
/* Make selective choices of some options based on "Release version" or "Developer version". */
#define RELEASE_VERSION  ///
/// #define DEVELOPER_VERSION

/* Conditional compile to allow a quicker power-up sequence by-passing some device tests.
   NOTE: You may decide below (in "main()") which sections of code to by-pass. */
#ifndef RELEASE_VERSION
#define QUICK_START  ///
#endif

/* Firmware version. */
#define FIRMWARE_VERSION "6.00"

/* Specify the file containing the remote control protocol to be used. */
#define REMOTE_FILENAME "memorex.cpp"

/* Specify the filename of calendar events to merge with this version of firmware. */
#ifdef DEVELOPER_VERSION
#define CALENDAR_FILENAME "CalendarEventsAndre.cpp"
#else
#define CALENDAR_FILENAME "CalendarEventsGeneric.cpp"
#endif

/* Silence period request unit (in minutes). Needs remote control. */
#define SILENCE_PERIOD_UNIT 30

/* Select the language for data display. */
#define DEFAULT_LANGUAGE ENGLISH // choices for now are FRENCH and ENGLISH.

/* If NO_SOUND is defined below (uncommented), it allows turning Off >>> ABSOLUTELY ALL SOUNDS <<<.
   (For example, during development phase, if your wife is sleeping while you work late in the night as was my case - smile). */
#ifndef RELEASE_VERSION
/// #define NO_SOUND ///
#endif

/* Maximum number of seconds an alarm will ring. It will be automatically shut Off after this period in seconds
   if user has not pressed yet the "Set" (top) button. */
#define MAX_ALARM_RING_TIME 3600 // (in seconds) alarm will be automatically shut Off after ringing for one hour (3600 seconds).

/* Night light default values. "AUTO" is based on ambient light reading to turn night light On or Off. */
#define NIGHT_LIGHT_DEFAULT   NIGHT_LIGHT_AUTO  // choices are NIGHT_LIGHT_OFF / NIGHT_LIGHT_ON / NIGHT_LIGHT_NIGHT / NIGHT_LIGHT_AUTO
#define NIGHT_LIGHT_TIME_ON   21                // if "NIGHT_LIGHT_NIGHT", LEDs will turn On  at this time (in the evening).
#define NIGHT_LIGHT_TIME_OFF   8                // if "NIGHT_LIGHT_NIGHT", LEDs will turn Off at this time (in the morning).

/* Hourly chime default values. */
#define CHIME_DEFAULT         CHIME_DAY  // choices are CHIME_OFF / CHIME_ON / CHIME_DAY
#define CHIME_TIME_ON          9         // if "CHIME_DAY", "hourly chime" and "calendar event" will sound beginning at this time (in the morning).
#define CHIME_TIME_OFF        21         // if "CHIME_DAY", "hourly chime" and "calendar event" will sound for the last time at this time (in the evening).
/* NOTE: See also revision history above (or User guide) about "night time workers" support for hourly chime. */


/* Determine if date scrolling will be enable by default when the clock starts. */
#define SCROLL_DEFAULT FLAG_ON  // choices are FLAG_ON / FLAG_OFF

/* Scroll one dot to the left every such milliseconds (80 is a good start point. lower = faster). */
#define SCROLL_DOT_TIME 75      // this is a UINT8 (must be between 0 and 255).

/* Date, temperature and power supply voltage will scroll at this frequency
   (in minutes - we must leave enough time for the previous scroll to complete). */
#define SCROLL_PERIOD_MINUTE 5

/* Default temperature unit to display on the clock. */
#define TEMPERATURE_DEFAULT CELSIUS  // choices are CELSIUS and FAHRENHEIT.

/* Time will be displayed in 24-hours format by default. */
#define TIME_DISPLAY_DEFAULT H24     // choices are H12 and H24.

/* Exit setup mode after this period of inactivity (in seconds). (for "clock setup", "alarm setup" or "timer setup"). */
#define TIME_OUT_PERIOD 20

/* For active buzzer integrated in Pico Green Clock. Number of "tones" for each "sound pack" (first level of repetition). */
#define TONE_CHIME_REPEAT1     2
#define TONE_EVENT_REPEAT1     5
#define TONE_KEYCLICK_REPEAT1  1
#define TONE_TIMER_REPEAT1    10

/* For active buzzer integrated in Pico Green Clock.
   Number of times the "sound pack" above will repeat itself for each tone type (second level of repetition).
   for example, if TONE_CHIME_REPEAT_1 is set to 3 and TONE_CHIME_REPEAT_2 is set to 2, we will hear:
   beep-beep-beep..........beep-beep-beep (three beeps, twice). */
#define TONE_CHIME_REPEAT2     3
#define TONE_EVENT_REPEAT2     3
#define TONE_KEYCLICK_REPEAT2  1
#define TONE_TIMER_REPEAT2     2

/* For active buzzer integrated in Pico Green Clock.
   Time duration for different tone types (in milliseconds). This is the time of each sound inside one "sound pack".
   Will be rounded-up by the clock to the next 50msec multiple.
   We can separate sounds - or group of sound - by adding a silence (for example: "sound_queue_active(50, SILENT);"). */
#define TONE_CHIME_DURATION     50   // when sounding an hourly chime.
#define TONE_EVENT_DURATION    200   // when scrolling a calendar event.
#define TONE_KEYCLICK_DURATION  50   // when pressing a button ("keyclick").
#define TONE_TIMER_DURATION    100   // when count-down timer reaches 00m00s.

/* Flag to handle automatically the daylight saving time.
   Since the moment to adjust time may be different depending in which country we are,
   more options could be added without disrupting algorithms already implemented. */
#define DAYLIGHT_SAVING_TIME DST_NORTH_AMERICA // choices for now are DST_NONE and DST_NORTH_AMERICA.


/* Flag to include test code in the executable (conditional compile).
   Tests can be run before falling in normal clock ("time display") mode.
   Commenting out the #define below will exclude test code and make the executable smaller. */
#ifdef DEVELOPER_VERSION
#define TEST_CODE
#endif


#ifdef DEVELOPER_VERSION
/* Support for an optional passive piezo / buzzer. This buzzer must be provided by user and is not included with the Green Clock.
   It allows variable frequency sounds on the clock. */
/// #define PASSIVE_PIEZO_SUPPORT       // if a passive buzzer has been installed by user.
#endif


#ifdef DEVELOPER_VERSION
/* Support of an optional (external) temperature and humidity sensor (DHT22) or temperature, humidity and barometric pressure sensor (BME280)
   to read and display those parameters. The sensors must be bought and installed by user. They are not included with the Pico Green Clock.
   >>> WARNING <<< See text in the user guide about DHT22. */
/// #define DHT22_SUPPORT // if a DHT22 outside temperature and humidity sensor has been installed by user.
/// #define BME280_SUPPORT                // if a BME280 outside temperature, humidity and barometric pressure sensor has been installed by user.
#endif


#ifdef DEVELOPER_VERSION
/* Support of an optional remote control to interact with the clock remotely, for example when the clock is installed
   too high and is out of reach. There is no remote control provided with the clock. It must be bought by the user.
   Current support is for a Memorex remote control, model MCR 5221 that I had readily available. If another
   brand of remote control is to be used, user will have to decode its protocol and implement it in the Green Clock firmware.
   (a file similar to "memorex.cpp" must be created to support the new remote control). */
/// #define IR_SUPPORT // if an infrared sensor (VS1838B-type) has been installed by the user and IR protocol analyzed and implemented.
#endif

/* ================================================================== *\
            ===== END OF CLOCK CONFIGURATION OR OPTIONS =====
\* ================================================================== */





/* $TITLE=Definitions and include files. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
                    Definitions and include files
                       (in alphabetical order)
\* ------------------------------------------------------------------ */
#define ALARM_PERIOD            5         // alarm ringer restart every x seconds (part of the whole alarm algorithm).
#define CELSIUS                 0x00
#define CHIME_DAY               0x02      // hourly chime is ON during defined daily hours (between CHIME_TIME_ON and CHIME_TIME_OFF).
#define CHIME_OFF               0x00      // hourly chime is OFF.
#define CHIME_ON                0x01      // hourly chime is ON.
#define CRC16_POLYNOM           0x1021    // different polynom values are used by different authorities. (0x8005, 0x1021, 0x1DCF, 0x755B, 0x5935, 0x3D65, 0x8BB7, 0x0589, 0xC867, 0xA02B, 0x2F15, 0x6815, 0xC599, 0x202D, 0x0805, 0x1CF5)
#define DEFAULT_YEAR_CENTILE    20        // to be used before flash configuration is read.
#define DISPLAY_BUFFER_SIZE     248       // size of framebuffer.
#define FAHRENHEIT              !CELSIUS  // temperature unit to display.
#define FALSE                   0x00
#define FLAG_OFF                0x00      // flag is OFF.
#define FLAG_ON                 0xFF      // flag is ON.
#define FLASH_CONFIG_OFFSET     0x1FF000  // offset in the Pico's 2 MB where to save data. Starting at 2.00MB - 4096 bytes.
#define H12                     FLAG_OFF  // 12-hours time format.
#define H24                     FLAG_ON   // 24-hours time format.
#define MAX_ALARMS              9         // total number of alarms available.
#define MAX_DHT_READINGS        100       // maximum number of level changes while reading DHT22 data stream.
#define MAX_EVENTS              50        // maximum number of "calendar events" that can be programmed in the source code.
#define MAX_READINGS            500       // maximum number of "readings" when receiving data from IR remote control (one reading for each logic level).
#define MAX_SCROLL_QUEUE        75        // maximum number of messages in the scroll buffer queue (big enough for MAX_EVENTS defined for the same day + a few date scrolls).
#define MAX_PASSIVE_SOUND_QUEUE 10000     // maximum number of "sounds" in the passive buzzer sound queue.
#define MAX_ACTIVE_SOUND_QUEUE  10000     // maximum number of "sounds" in the active buzzer sound queue.
#define NIGHT_LIGHT_AUTO        0x03      // night light will turn on when ambient light is low enough
#define NIGHT_LIGHT_NIGHT       0x02      // night light On between NightLightTimeOn and NightLightTimeOff.
#define NIGHT_LIGHT_OFF         0x00      // night light always Off.
#define NIGHT_LIGHT_ON          0x01      // night light always On.
#define TIMER_COUNT_DOWN        0x01      // timer mode is "count down".
#define TIMER_COUNT_UP          0x02      // timer mode is "count up".
#define TIMER_OFF               0x00      // timer is currently OFF.
#define TRUE                    0x01

/* Language and locals used. */
#define LANGUAGE_LO_LIMIT 0x00
#define ENGLISH           0x01
#define FRENCH            0x02
#define SPANISH           0x03           // just a placeholder for now. Spanish is not implemented yet.
#define LANGUAGE_HI_LIMIT 0x04

/* DAYLIGHT_SAVING_TIME choices are as below for now: */
#define DST_LO_LIMIT      0             // to make the logic easier in the firmware.
#define DST_NONE          0             // there is no "Daylight Saving Time" in user's country.
#define DST_NORTH_AMERICA 1             //  02:00:00 second SUN in MAR  -until-  02:00:00 first SUN in NOV
#define DST_EUROPE        2             //  01:00:00Z  last SUN in MAR  -until-  01:00:00Z last SUN in OCT
#define DST_HI_LIMIT      3             // to make the logic easier in the firmware.
/* Add choices here and search for "DST_" in the code to add the specific algorithm. */

/* Clock mode definitions. */
#define MODE_LO_LIMIT     0x00
#define MODE_UNDEFINED    0x00  // mode is currently undefined.
#define MODE_ALARM_SETUP  0x01  // user is setting up an alarm.
#define MODE_CLOCK_SETUP  0x02  // user is setting up clock parameters.
#define MODE_SCROLLING    0x03  // clock is scrolling data on the display.
#define MODE_SHOW_TIME    0x04  // clock is displaying time.
#define MODE_SHOW_VOLTAGE 0x05  // clock is displaying power supply voltage.
#define MODE_TEST         0x06  // clock is in test mode (to disable automatic clock behaviors on the display).
#define MODE_TIMER_SETUP  0x07  // user is setting up a timer.
#define MODE_DISPLAY      0x08  // generic display mode used with IR remote control.
#define MODE_HI_LIMIT     0x09

/* Bitmasks for sections of code to be debugged through UART display (values can be or'ed together). */
#define DEBUG_NONE         0x0000000000000000
#define DEBUG_ALARMS       0x0000000000000001
#define DEBUG_BME280       0x0000000000000002
#define DEBUG_BRIGHTNESS   0x0000000000000004
#define DEBUG_CHIME        0x0000000000000008
#define DEBUG_CRC16        0x0000000000000010
#define DEBUG_FLASH        0x0000000000000020
#define DEBUG_IDLE_MONITOR 0x0000000000000040
#define DEBUG_DHT          0x0000000000000080
#define DEBUG_SOUND_QUEUE  0x0000000000000100
#define DEBUG_IR_COMMAND   0x0000000000000200
// #define DEBUG_?????            0x0000000000000400
// #define DEBUG_?????            0x0000000000000800
// #define DEBUG_?????            0x0000000000001000
// #define DEBUG_?????            0x0000000000002000
// #define DEBUG_?????            0x0000000000004000
// #define DEBUG_?????            0x0000000000008000
// #define DEBUG_?????            0x0000000000010000
// #define DEBUG_?????            0x0000000000020000
// #define DEBUG_?????            0x0000000000040000
// #define DEBUG_?????            0x0000000000080000
// #define DEBUG_?????            0x0000000000100000
// #define DEBUG_?????            0x0000000000200000
// #define DEBUG_?????            0x0000000000400000
// #define DEBUG_?????            0x0000000000800000
// etc...

/* "Display modes" used with remote control. */
#define DISPLAY_LO_LIMIT     0x00
#define DISPLAY_TIME         0x01
#define DISPLAY_DATE         0x02
#define DISPLAY_DST          0x03
#define DISPLAY_BEEP         0x04
#define DISPLAY_SCROLLING    0x05
#define DISPLAY_TEMP_UNIT    0x06
#define DISPLAY_LANGUAGE     0x07
#define DISPLAY_TIME_FORMAT  0x08
#define DISPLAY_HOURLY_CHIME 0x09
#define DISPLAY_NIGHT_LIGHT  0x0A
#define DISPLAY_DIM          0x0B
#define DISPLAY_HI_LIMIT     0x0C

/* Tags that can be used in process_scroll() function. */
#define TAG_ALARM            0xFF   // tag used to display current alarm parameters (for both alarm 0 and alarm 1).
#define TAG_AMBIENT_LIGHT    0xFE   // tag used to scroll current ambient light.
#define TAG_BME280_DEVICE_ID 0xFD   // tag used to scroll the BME280 device id (should be 0x60 for a "real" BME280).
#define TAG_DATE             0xFC   // tag used to scroll current date, temperature and power supply voltage.
#define TAG_DEBUG            0xFB   // tag used to scroll variables for debug purposes, even inside ISR.
#define TAG_DST              0xFA   // tag used to scroll daylight saving time ("DST") status on clock display.
#define TAG_EXT_TEMP         0xF9   // tag used to display outside temperature and relative humidity.
#define TAG_FIRMWARE_VERSION 0xF8   // tag used to display Pico Green Clock firmware version.
#define TAG_IDLE_MONITOR     0xF7
#define TAG_PICO_TEMP        0xF6   // tag used to display Pico internal temperature.
#define TAG_PICO_UNIQUE_ID   0xF5   // tag used to display Pico (flash) unique ID.
#define TAG_QUEUE            0xF4   // tag used to display "Head", "Tail", and "Tag" of currently used scroll queue (for debugging purposes).
#define TAG_TEMPERATURE      0xF3   // tag used to display ambient temperature.
#define TAG_VOLTAGE          0xF2   // tag used to display power supply voltage.

/* DayOfWeek definitions. */
#define ALL 0x00  // All days
#define SUN 0x01  // Sunday
#define MON 0x02  // Monday
#define TUE 0x03  // Tuesday
#define WED 0x04  // Wednesday
#define THU 0x05  // Thursday
#define FRI 0x06  // Friday
#define SAT 0x07  // Saturday

/* Month definitions. */
#define JAN 1  // January
#define FEB 2  // February
#define MAR 3  // March
#define APR 4  // April
#define MAY 5  // May
#define JUN 6  // June
#define JUL 7  // July
#define AUG 8  // August
#define SEP 9  // September
#define OCT 10 // October
#define NOV 11 // November
#define DEC 12 // December

/* NOTE: Clock setup step definitions are kept as variables and can be seen in the variables section below. */

/* Setup source definitions. */
#define SETUP_SOURCE_NONE    0x00
#define SETUP_SOURCE_ALARM   0x01
#define SETUP_SOURCE_CLOCK   0x02
#define SETUP_SOURCE_TIMER   0x03

/* Alarm setup steps definitions. */
#define SETUP_ALARM_LO_LIMIT 0x00
#define SETUP_ALARM_NUMBER   0x01
#define SETUP_ALARM_ON_OFF   0x02
#define SETUP_ALARM_HOUR     0x03
#define SETUP_ALARM_MINUTE   0x04
#define SETUP_ALARM_DAY      0x05
#define SETUP_ALARM_HI_LIMIT 0x06

/* Timer setup steps definitions. */
#define SETUP_TIMER_LO_LIMIT 0x00
#define SETUP_TIMER_UP_DOWN  0x01
#define SETUP_TIMER_MINUTE   0x02
#define SETUP_TIMER_SECOND   0x03
#define SETUP_TIMER_READY    0x04
#define SETUP_TIMER_HI_LIMIT 0x05

/* List or commands available to remote control. */
#ifdef  IR_SUPPORT
#define IR_LO_LIMIT                 0x00
#define IR_BUTTON_TOP_QUICK         0x01
#define IR_BUTTON_TOP_LONG          0x02
#define IR_BUTTON_MIDDLE_QUICK      0x03
#define IR_BUTTON_MIDDLE_LONG       0x04
#define IR_BUTTON_BOTTOM_QUICK      0x05
#define IR_BUTTON_BOTTOM_LONG       0x06
#define IR_DICE_ROLLING             0x07
#define IR_DISPLAY_AMBIENT_LIGHT    0x08
#define IR_DISPLAY_EVENTS_THIS_WEEK 0x09
#define IR_DISPLAY_EVENTS_TODAY     0x0A
#define IR_DISPLAY_GENERIC          0x0B
#define IR_DISPLAY_OUTSIDE_TEMP     0x0C
#define IR_DISPLAY_SECOND           0x0D
#define IR_FULL_TEST                0x0E
#define IR_IDLE_TIME                0x0F
#define IR_SILENCE_PERIOD           0x10
#define IR_HI_LIMIT                 0x11
#endif

#define SILENT 0

/* Music notes definitions. */
#ifdef PASSIVE_PIEZO_SUPPORT
#define DO_a         262
#define DO_DIESE_a   277
#define RE_a         294
#define RE_DIESE_a   311
#define MI_a         330
#define FA_a         349
#define FA_DIESE_a   370
#define SOL_a        392
#define SOL_DIESE_a  415
#define LA_a         440
#define LA_DIESE_a   466
#define SI_a         494
#define DO_b         523
#define DO_DIESE_b   554
#define RE_b         587
#define RE_DIESE_b   622
#define MI_b         659
#define FA_b         699
#define FA_DIESE_b   740
#define SOL_b        784
#define SOL_DIESE_b  831
#define LA_b         880
#define LA_DIESE_b   932
#define SI_b         988
#define DO_c        1047
#define DO_DIESE_c  1109
#define RE_c        1175
#define RE_DIESE_c  1245
#define MI_c        1319
#define FA_c        1397
#define FA_DIESE_c  1480
#define SOL_c       1568
#define SOL_DIESE_c 1661
#define LA_c        1760
#define LA_DIESE_c  1865
#define SI_c        1976

/* Jingle definitions. */
#define JINGLE_LO_LIMIT  0x00
#define JINGLE_BIRTHDAY  0x01
#define JINGLE_ENCOUNTER 0x02
#define JINGLE_FETE      0x03
#define JINGLE_RACING    0x04
#define JINGLE_HI_LIMIT  0x05
#else
/* Jingle definitions. Placeholder only if not using a passive piezo. */
#define JINGLE_LO_LIMIT  0x00
#define JINGLE_BIRTHDAY  0x00
#define JINGLE_ENCOUNTER 0x00
#define JINGLE_FETE      0x00
#define JINGLE_RACING    0x00
#define JINGLE_HI_LIMIT  0x00
#endif

/* Include files. */
#include "bitmap.h"
#include "ctype.h"
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
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "math.h"
#include "pico/platform.h"
#include "pico/sync.h"
#include "pico/unique_id.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

typedef unsigned int  UINT; // processor-optimized.
typedef uint8_t       UINT8;
typedef uint16_t      UINT16;
typedef uint32_t      UINT32;
typedef uint64_t      UINT64;
typedef unsigned char UCHAR;

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
UINT8 SETUP_KEYCLICK =             0x07;
UINT8 SETUP_SCROLLING =            0x08;
UINT8 SETUP_TEMP_UNIT =            0x09;
UINT8 SETUP_LANGUAGE =             0x0A;
UINT8 SETUP_TIME_FORMAT =          0x0B;
UINT8 SETUP_HOURLY_CHIME =         0x0C;
UINT8 SETUP_CHIME_TIME_ON =        0x0D;
UINT8 SETUP_CHIME_TIME_OFF =       0x0E;
UINT8 SETUP_NIGHT_LIGHT =          0x0F;
UINT8 SETUP_NIGHT_LIGHT_TIME_ON =  0x10;
UINT8 SETUP_NIGHT_LIGHT_TIME_OFF = 0x11;
UINT8 SETUP_AUTO_BRIGHT =          0x12;
#define SETUP_CLOCK_HI_LIMIT       0x13

/* $TITLE=Calendar events. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
       Calendar events have been implemented to be configurable
     by the user. However, they must be configured at compile time
         (they can't be configured once the clock is running).
       As a user may intuitively guess, the parameters are:
       1) Day of month (1 to 31).
       2) Month (3-letters: JAN / FEB / MAR / APR / etc...)
       3) Jingle ID (Jingle number to play while scrolling this calendar event, 0 = none)
       4) Text to scroll on clock display (between "double-quotes" in the code)
          (The text is limited to 40 characters. It takes a
          relatively long time to read 40 characters of text on the clock scrolling display !)

   NOTE: Calendar events are verified twice an hour, at xxh14 and xxh44. The text configured will be scrolled on
         the clock display if the clock is in its "normal" time display mode (the text will not scroll if user is in a setup operation).
         Also, the tone will sound (according to TONE_EVENT_COUNT and TONE_EVENT_DURATION) during day hours, as defined between
         CHIME_TIME_ON and CHIME_TIME_OFF. Outside these hours, the text will also scroll on the display, but there will be no sound.
         NOTE: See also the "nighttime workers" information in the revision history above.
         NOTE: No validation is done on the date. If an invalid date is set, the event will never be scrolled.
               Also, if, for example, 29-FEB is set, the event will be scrolled only every 4 years.
\* ------------------------------------------------------------------ */
struct event
{
  UINT8  Day;
  UINT8  Month;
  UINT16 Jingle;
  UCHAR  Description[41];
};

/* Events to scroll on clock display at specific dates. */
#include CALENDAR_FILENAME

struct alarm
{
  UINT8 FlagStatus;
  UINT8 Second;
  UINT8 Minute;
  UINT8 Hour;
  UINT8 Day;
  UCHAR Text[40];
};

struct sound_active
{
  UINT16 MSec;
  UINT16 RepeatCount;
};

struct sound_passive
{
  UINT16 Freq;
  UINT16 MSec;
};

/* Structure containing the Green Clock configuration being saved to flash memory.
   Those variables will be restored after a reboot and / or power failure. */
struct
{
  UCHAR Version[6];         // firmware version number (format: "06.00" - including end-of-string).
  UINT8 CurrentYearCentile; // assume we are in years 20xx (could be changed in clock setup, but will revert to 20 at each power-up).
  UINT8 Language;           // language used for data display (including date scrolling).
  UCHAR DaylightSavingTime; // specifies how to handle the daylight saving time (see clock options above).
  UINT8 TemperatureUnit;    // CELSIUS or FAHRENHEIT default value (see clock options above).
  UINT8 TimeDisplayMode;    // H24 or H12 default value (see clock options above).
  UINT8 ChimeMode;          // chime mode (Off / On / Day).
  UINT8 ChimeTimeOn;        // hourly chime will begin at this hour.
  UINT8 ChimeTimeOff;       // hourly chime will stop after this hour.
  UINT8 NightLightMode;     // night light mode (On / Off / Auto / Night).
  UINT8 NightLightTimeOn;   // default night light time on.
  UINT8 NightLightTimeOff;  // default night light time off.
  UINT8 FlagAutoBrightness; // flag indicating we are in "Auto Brightness" mode.
  UINT8 FlagKeyclick;       // flag for keyclick ("button-press" tone)
  UINT8 FlagScrollEnable;   // flag indicating the clock will scroll the date and temperature at regular intervals on the display.
  UINT8 Reserved1[50];      // reserved for future use.
  struct alarm Alarm[9];    // alarms 1 and 2, then alarms 3 to 9 parameters. Day is a bit mask for alarms 3 to 9.
  UCHAR Reserved2[158];     // reserved for future use.
  UINT16 Crc16;             // crc16 of previous data to validate configuration.
} FlashConfig;

#ifdef BME280_SUPPORT
/* BME280 calibration parameters computed from data written in the device. */
struct bme280_calib_param
{
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
};
#endif

/* $TITLE=Global variables declaration / definition. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
             Global variables declaration / definition.
   NOTE: There was a huge amount of global variables in the original
         code. It is a good practice to limit the use of global
         variable whenever possible. This could be a way to improve
         the code in future releases...
\* ------------------------------------------------------------------ */
UCHAR Level[MAX_READINGS];           // logic levels of remote control signal: 'L' (low), 'H' (high), or 'X' (undefined).

UINT16 AdcCount = 0;                 // cumulative milliseconds before reading power supply voltage.
UINT16 AlarmReachedBitMask = 0;      // assume no alarms are ringing on entry.
UINT16 AlarmRingTime[MAX_ALARMS];    // cumulative number of seconds alarm has been ringing.
UINT8  AlarmNumber = 0;
UINT8  AlarmTargetDay = MON;         // blinking day-of-week to be selected or unselected for current alarm setting.
UINT64 ArchiveIdleMonitor[14][120];  // keep an history of idle monitorfor reference purposes (outside monitor is required).
UINT16 AverageLightLevel = 550;      // relative ambient light value (for LED display auto-brightness clock feature). Assume average level (500) on entry.

UINT8  Bme280CalibData[42];          // calibration data for the specific BME280 used (stored in BME280's non-volatile memory).
UINT64 Bme280Errors = 0;             // cumulative number of errors while trying to read BME280 sensor.
UINT64 Bme280Readings = 0;           // cumulative number of reads from BME280.
UINT16 BottomKeyPressTime = 0;       // keep track of the time the Down ("Bottom") key is pressed.

UINT16 ChannelLevel;                         // PWM-related variable.
UINT8  ChimeTimeOffDisplay = CHIME_TIME_OFF; // variable formatted to display in 12-hours or 24-hours format.
UINT8  ChimeTimeOnDisplay = CHIME_TIME_ON;   // variable formatted to display in 12-hours or 24-hours format.
UINT8  RowScanNumber;
UINT8  CurrentClockMode = MODE_UNDEFINED;    // current clock mode.
UINT8  CurrentDayOfMonth;
UINT8  CurrentDayOfWeek;
UINT8  CurrentHour;
UINT8  CurrentHourSetting = 0;             // hours read and written to the RTC IC.
UINT8  CurrentMinute;
UINT8  CurrentMinuteSetting = 0;           // minutes setting from / to real time clock IC.
UINT8  CurrentMonth;
UINT8  CurrentYearLowPart;                 // lowest two digits of the year (battery backed-up).
UINT8  FirstSunday;                        // Used for DST calculation, first sunday of current month.
UINT8  LastSunday;                         // same as above, but last Sunday

UINT64 DebugBitMask = DEBUG_NONE;          // bitmask of code sections to be debugged through UART display (see definitions of DEBUG sections above).
UINT64 DhtErrors = 0;                      // cumulate number of errors while receiving DHT packets (for statistic purposes).
UINT64 DhtReadings = 0;                    // cumulate total number of reads from DHT22.
UINT8  DhtLevel[MAX_DHT_READINGS];
UINT64 DhtInitialValue[MAX_DHT_READINGS];
UINT64 DhtFinalValue[MAX_DHT_READINGS];
UINT64 DhtResultValue[MAX_DHT_READINGS];
UINT16 DhtStepCount = 0;                   // number of "events" received from IR remote control in current stream.
UCHAR  DisplayBuffer[DISPLAY_BUFFER_SIZE]; // framebuffer containing the bitmap of the string to be displayed / scrolled on clock display.
UINT16 DotBlinkCount;                      // cumulate milliseconds to blink the two "middle dots".

UINT64 IrFinalValue[MAX_READINGS];         // final timer value when receiving edge change from remote control.
UINT8  FlagAdcLightTime       = FLAG_OFF;
UINT8  FlagAlarmBeeping       = FLAG_OFF;                                                                                                                 // flag indicating an alarm is sounding.
UINT8  FlagBlinking[20] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // bitmap to logically "and" with a character for blinking.
UINT8  FlagChimeDone          = FLAG_OFF;                                                                                                                    // indicates that hourly chime has already been triggered for current hour.
UINT8  FlagDayLightSavingTime = FLAG_OFF;                                                                                                           // flag indicating we are in "daylight saving time" mode.
UINT8  FlagIdleCheck          = FLAG_OFF;                                                                                                                    // if ON, we keep track of idle time to eventually declare a time-out.
UINT8  FlagIdleMonitor        = FLAG_OFF;
UINT8  FlagScrollData         = FLAG_OFF;  // time has come to scroll data on the display.
UINT8  FlagScrollStart        = FLAG_OFF;  // flag indicating it is time to start scrolling.
UINT8  FlagSetAlarm           = FLAG_OFF;  // flag indicating we are in alarm setup mode.
UINT8  FlagSetTimer           = FLAG_OFF;  // user pressed the "up" (Middle) button.
UINT8  FlagSetTimerCountDown  = FLAG_OFF;  // flag indicating the count down timer is active.
UINT8  FlagSetTimerCountUp    = FLAG_OFF;  // flag indicating count up timer is active.
UINT8  FlagSetClock           = FLAG_OFF;  // user has pressed the "Set" (top) button to enter clock setup mode.
UINT8  FlagSetupAlarm[SETUP_ALARM_HI_LIMIT];
UINT8  FlagSetupClock[SETUP_CLOCK_HI_LIMIT];
UINT8  FlagSetupTimer[SETUP_TIMER_HI_LIMIT];
UINT8  FlagSetupRTC           = FLAG_OFF;  // flag indicating time must be programmed in the real-time clock IC.
UINT8  FlagTimerCountDownEnd  = FLAG_OFF;  // flag indicating the count down timer reached final count (0m00s).
UINT8  FlagTone               = FLAG_OFF;  // flag indicating there is a tone sounding.
UINT8  FlagToneOn             = FLAG_OFF;  // flag indicating it is time to make a tone.
UINT8  FlagUpdateTime         = FLAG_OFF;  // flag indicating it is time to refresh the time on clock display.
UINT8 *FlashData;                                                       // pointer to an allocated RAM memory space used for flash operations.
UINT8 *FlashMemoryAddress = (UINT8 *)(XIP_BASE + FLASH_CONFIG_OFFSET);  // pointer to flash memory used to store clock configuration (flash base address + offset).
UINT8 *FlashMemoryOffset  = (UINT8 *)FLASH_CONFIG_OFFSET;               // offset from Pico's beginning of flash where data will be stored.
UINT16 FlashingCount = 0;                                               // cumulate number of milliseconds before blinking clock display.

UCHAR GetAddHigh = 0x11;
UCHAR GetAddLow = 0x12;

UINT16 IdleArchivePacket;     // packet number being archived for debugging purposes.
UINT64 IdleMonitor[14];       // evaluate average number of loops performed per second.
UINT8  IdleMonitorPacket;     // idle monitor packet for current 5-seconds period.
UINT8  LastIdleMonitorPacket; // idle monitor packet number processed in the last pass.
UINT8  IdleNumberOfSeconds;   // keep track of the number of seconds the system has been idle.
UINT16 IdleTimeSamples;
UINT64 IrInitialValue[MAX_READINGS]; // initial timer value when receiving edge change from remote control.
UINT16 IrStepCount = 0;              // number of "events" received from IR remote control in current stream.

UINT8 LightSetting = 0;

UINT16 MiddleKeyPressTime = 0;                                                // keep track of the time the Up ("Middle") key is pressed.
UINT8  MonthDays[2][12] = {{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},  // number of days in a month - "leap" year.
                           {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}}; // number of days in a month - "normal" year.

UINT8 NightLightTimeOffDisplay; // to adapt to 12 or 24-hours time format.
UINT8 NightLightTimeOnDisplay;  // to adapt to 12 or 24-hours time format.

UINT16 PwmChannelNumber;
UINT16 PwmSliceNumber;

UINT64 IrResultValue[MAX_READINGS];       // duration of this logic level (Low or High) in the signal received from remote control.

UINT8  ScrollDotCount = 0;              // keep track of "how many dots to the left" remain to be scrolled.
UINT8  ScrollQueue[MAX_SCROLL_QUEUE];   // circular buffer containing the tag of the next messages to be scrolled.
UINT8  ScrollQueueHead;                 // current day indicator for blinking purposes.   // head of circular buffer.
UINT8  ScrollQueueTail;                 // tail of circular buffer.
UINT8  ScrollSecondCounter = 0;         // keep track of number of seconds to reach time-to-scroll.
UINT8  ScrollStartCount = 0;            // count the number of milliseconds before scrolling one more dot position to the left.
UINT8  SecondCounter = 0;               // cumulate the number of seconds before minute change.
UINT8  SetupSource = SETUP_SOURCE_NONE; // indicate the source of current setup activities (alarm, clock or timer).
UINT8  SetupStep = 0;                   // indicate the setup step we are through the clock setup, alarm setup, or timer setup.
UINT16 SilencePeriod = 0;               // temporarily turn off most sounds from the clock.
UINT16 SoundActiveHead;                 // head of sound circular buzzer for passive buzzer.
UINT16 SoundActiveTail;                 // tail of sound circular buffer for passive buzzer.
UINT16 SoundPassiveHead;                // head of sound circular buffer for active buzzer.
UINT16 SoundPassiveTail;                // tail of sound circular buffer for active buzzer.
UINT32 SystemClock;                     // PWM-related variable.

UCHAR  TemperaturePart1;          // current temperature value, decimal part (before the decimal dot)   - Not for the DHT22.
UCHAR  TemperaturePart2;          // current temperature value, fractional part (after the decimal dot) - Not for the DHT22.
UINT8  TimerMinutes = 0;
UINT8  TimerMode = TIMER_OFF;     // timer mode (0 = Off / 1 = Count down / 2 = Count up).
UINT8  TimerSeconds = 0;
UINT8  TimerShowCount = 0;        // in case we want to refresh timer display less often than every second.
UINT8  TimerShowSecond = 0;
UINT8  ToneMSecCounter = 0;       // cumulate number of milliseconds for tone duration.
UINT8  ToneRepeatCount = 0;       // number of "tones" to sound for different events.
UINT8  ToneType;                  // determine the type of "tone" we are sounding.
UINT16 TopKeyPressTime = 0;       // keep track of the time the Set ("Top") key is pressed.

UINT8 UpId = 0;

UINT16 Year4Digits;   // year in 4-digits format.

TIME_RTC Time_RTC;

semaphore_t SemSync; // semaphore used to synchronize minutes and seconds display with double dots blinking.

uart_inst_t *Uart;   // Pico's UART used to serially transfer debug data to a VT100-type monitor or to a PC.

UCHAR MonthName[3][13][10] =
{
        {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}},
        {{}, {"January"}, {"February"}, {"March"}, {"April"}, {"May"}, {"June"}, {"July"}, {"August"}, {"September"}, {"October"}, {"November"}, {"December"}},
        {{}, {"Janvier"}, {"Fevrier"}, {"Mars"}, {"Avril"}, {"Mai"}, {"Juin"}, {"Juillet"}, {"Aout"}, {"Septembre"}, {"Octobre"}, {"Novembre"}, {"Decembre"}}};

UCHAR ShortMonth[3][13][4] =
{
        {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}},
        {{}, {"JAN"}, {"FEB"}, {"MAR"}, {"APR"}, {"MAY"}, {"JUN"}, {"JUL"}, {"AUG"}, {"SEP"}, {"OCT"}, {"NOV"}, {"DEC"}},
        {{}, {"JAN"}, {"FEV"}, {"MAR"}, {"AVR"}, {"MAI"}, {"JUN"}, {"JUL"}, {"AOU"}, {"SEP"}, {"OCT"}, {"NOV"}, {"DEC"}}};

UCHAR DayName[3][8][10] =
{
        {{}, {}, {}, {}, {}, {}, {}, {}},
        {{}, {"Sunday"}, {"Monday"}, {"Tuesday"}, {"Wednesday"}, {"Thursday"}, {"Friday"}, {"Saturday"}},
        {{}, {"Dimanche"}, {"Lundi"}, {"Mardi"}, {"Mercredi"}, {"Jeudi"}, {"Vendredi"}, {"Samedi"}}};

struct sound_passive SoundQueuePassive[MAX_PASSIVE_SOUND_QUEUE];
struct sound_active SoundQueueActive[MAX_ACTIVE_SOUND_QUEUE];
struct repeating_timer TimerSec;
struct repeating_timer Timer50uSec;

#ifdef BME280_SUPPORT
struct bme280_calib_param Bme280CalibParam;
#endif

/* Uncomment and modify value below until you get (or you remove) the error message to get an idea of approximate remaining RAM available. */
/* NOTE: Keep in mind that the "malloc()" is not calculated when doing so and must be added to the total RAM required by the firmware. */
// UINT8 RemainingRam[231930];

/* $TITLE=Function prototypes. */
/* $PAGE */
/* ------------------------------------------------------------------ *\
                         Function prototypes
\* ------------------------------------------------------------------ */
/* Read ambient light from analog-to-digital gpio on Pico. */
void adc_show_count(void);

/* Clear the display framebuffer. */
void clear_framebuffer(UINT8 StartColumn);

/* Convert a 24-hour format time to 12-hour format. */
UINT8 convert_h24_to_h12(UINT8 Hour, UINT8 *AmFlag, UINT8 *PmFlag);

/* Generate a date stamp for debug info. */
void date_stamp(UCHAR *String);

/* Turn On or Off day indicator. */
void day_indicator(UINT8 DayOfWeek, UINT8 Flag);

/* Display data sent in argument through serial port. */
void display_data(UCHAR *Data, UINT32 Size);

/* Display the power supply voltage for a few seconds. */
void display_voltage(void);

/* Evaluate if it is time to blink data on the display (while in setup mode). */
void evaluate_blinking_time(void);

/* Evaluate if it is time to scroll characters on the display. */
void evaluate_scroll_time(void);

/* Fill the virtual framebuffer with the given ASCII character, beginning at the specified column position (using 5 X 7 character bitmap). */
UINT16 fill_display_buffer_5X7(UINT8 Column, UINT8 AsciiCharacter);

/* Fill the virtual framebuffer with the given ASCII character, beginning at the specified column position (using 4 X 7 character bitmap). */
void fill_display_buffer_4X7(UINT8 Column, UINT8 AsciiCharacter);

/* Compare crc16 between flash saved configuration and current active configuration. */
void flash_check_config(void);

/* Display flash content through external monitor. */
void flash_display(UINT32 Offset, UINT32 Length);

/* Display Green Clock configuration from flash memory. */
UINT8 flash_display_config(void);

/* Read Green Clock configuration from flash memory. */
UINT8 flash_read_config(void);

/* Save current Green Clock configuration to flash memory. */
UINT8 flash_save_config(void);

/* Write data to Pico flash memory. */
UINT flash_write(UINT32 FlashMemoryOffset, UINT8 NewData[], UINT16 NewDataSize);

/* Erase data in Pico flash memory. */
void flash_erase(UINT32 FlashMemoryOffset);

/* Read analog-to-digital interface. */
UINT16 get_ads1015(void);

/* Get temperature from ADC. */
void get_ambient_temperature(void);

/* Build the string containing the date to scroll on the clock display. */
void get_date_string(UCHAR *String);

/* Return the number of days in a specific month (while checking if it is a leap year or not for February). */
UINT8 get_month_days(UINT16 CurrentYear, UINT8 MonthNumber);

/* Display Pico temperature. */
void get_pico_temperature(UCHAR *String);

/* Get power supply voltage from analog-to-digital Pico gpio. */
void get_voltage(UCHAR *String);

/* Return the day-of-week, given the day-of-month, month and year. */
UINT8 get_weekday(UINT16 year_cnt, UINT8 month_cnt, UINT8 date_cnt);

/* Initialize all required GPIO ports of the Raspberry Pi Pico. */
int init_gpio(void);

/* Interrupt handler for signal received from IR sensor. */
gpio_irq_callback_t isr_signal_trap(UINT8 gpio, UINT32 Events);

/* Test clock LED matrix, column-by-column, and also all display indicators. */
void matrix_test(void);

/* Make pixel animation for the specified number of seconds. */
void pixel_twinkling(UINT16 Seconds);

/* Play a specific jingle. */
void play_jingle(UINT16 JingleNumber);

/* Handle the tag that has been put in the scroll queue. */
void process_scroll_queue(void);

/* Reverse the bit order of the byte given in argument. */
UINT8 reverse_bits(UINT8 InputByte);

/* Scroll the virtual framebuffer one dot to the left. */
void scroll_one_dot(void);

/* Enqueue the given tag in the scroll queue. */
UINT8 scroll_queue(UINT8 Tag);

/* Put a specific value in the queue so that we can check for a specific event even inside an ISR.
   MUST BE USED ONLY FOR DEBUG PURPOSES SINCE IT INTERFERES WITH (overwrites) CALENDAR EVENTS. */
UINT8 scroll_queue_value(UINT8 Tag, UCHAR *String);

/* Scroll the specified string on the display. */
void scroll_string(UINT8 StartColumn, UCHAR *String);

/* Unqueue next tag from the scroll queue. */
UINT8 scroll_unqueue(void);

/* Turn ON the right weekday indicator on the display and turn OFF all others. */
void select_weekday(UINT8 x);

/* Send data to the matrix controller IC. */
void send_data(UINT8 data);

/* Exit current setup mode step. */
void set_mode_out(void);

/* Exit setup mode because of inactivity (time-out). */
void set_mode_timeout(void);

/* Display current alarm parameters. */
void setup_alarm_frame(void);

/* Enter new alarm parameters. */
void setup_alarm_variables(UINT8 FlagButtonSelect);

/* Display current clock parameters. */
void setup_clock_frame(void);

/* Enter new clock parameters. */
void setup_clock_variables(UINT8 FlagButtonSelect);

/* Display current timer parameters. */
void setup_timer_frame(void);

/* Enter new timer parameters. */
void setup_timer_variables(UINT8 FlagButtonSelect);

/* Read the real-time clock IC and display time. */
void show_time(void);

/* Enqueue the given sound in the active buzzer sound queue. */
UINT16 sound_queue_active(UINT16 MSeconds, UINT16 RepeatCount);

/* Enqueue the given sound in the passive buzzer sound queue. */
UINT16 sound_queue_passive(UINT16 Frequency, UINT16 MSeconds);

/* Unqueue next sound from the active buzzer sound queue. */
UINT8 sound_unqueue_active(UINT16 *MSeconds, UINT16 *RepeatCount);

/* Unqueue next sound from the passive buzzer sound queue. */
UINT8 sound_unqueue_passive(UINT16 *Frequency, UINT16 *MSeconds);

/* Sound callback function (50 milliseconds period). */
bool sound_callback_ms(struct repeating_timer *Timer50MSec);

/* One millisecond period callback function. */
bool timer_callback_ms(struct repeating_timer *TimerMSec);

/* One second period callback function. */
bool timer_callback_s(struct repeating_timer *TimerSec);

/* 50 microseconds period callback function. */
bool timer_callback_us(struct repeating_timer *Timer50uSec);

/* Make a tone for the specified number of milliseconds. */
void tone(UINT16 MilliSeconds);

/* Return total number of "one" bits in the given integer. */
uint8_t total_one_bits(UINT32 Data, UINT8 Size);

/* Send a string to a VT101 monitor or PC screen through Pico's UART (for debugging purposes). */
void uart_send(UINT LineNumber, UCHAR *String);

/* calculate if a date falls withing Daylight Savings Time */
bool time_is_dst(TIME_RTC Time_RTC, UINT8 DST_Type);

/* ------------------------------------------------------------------ *\
                  BME280-related function prototypes
\* ------------------------------------------------------------------ */
#ifdef BME280_SUPPORT
/* Read temperature, humidity and barometric pressure. */
UINT8 bme280_get_temp(void);

/* Compute calibration parameters from calibration data stored in the specific BME280 device used. */
UINT8 bme280_compute_calib_param(void);

/* Soft-reset the BME280. */
UINT8 bme280_init(void);

/* Read BME280 calibration data from the device's non volatile memory. */
UINT8 bme280_read_calib_data(void);

/* Read BME280 configuration. */
UINT8 bme280_read_config(void);

/* Read BME280 device ID. */
UINT8 bme280_read_device_id(void);

/* Read extra BME280 registers. */
UINT8 bme280_read_all_registers();

/* Read all BME280 data registers (temperature, humidity and barometric pressure). */
UINT8 bme280_read_registers(UINT8 *Register);

/* Read BME280 current status. */
UINT8 bme280_read_status(void);

/* Read BME280 unique id ("serial number" of the specific device used). */
UINT32 bme280_read_unique_id(void);
#endif

/* ------------------------------------------------------------------ *\
                   DHT22-related function prototypes
\* ------------------------------------------------------------------ */
#ifdef DHT22_SUPPORT
/* Read the temperature from external DHT22. */
UINT8 read_dht22(float *Temperature, float *Humidity);
#endif

/* ------------------------------------------------------------------ *\
                 IR sensor-related function prototypes
\* ------------------------------------------------------------------ */
#ifdef IR_SUPPORT
/* Decode IR remote control command. */
UINT8 decode_ir_command(UCHAR FlagDebug, UINT8 *IrCommand);

/* Execute the command received from IR remote control. */
void process_ir_command(UINT64 IrCommand);
#endif  // IR_SUPPORT

#ifdef TEST_CODE
/* Perform different tests on Pico Green Clock (to be used for testing and / or debugging). */
void test_zone(UINT8 TestType);
#endif  // TEST_CODE





/* $PAGE */
/* $TITLE=Main program entry point. */
/* ------------------------------------------------------------------ *\
                       Main program entry point.
\* ------------------------------------------------------------------ */
int main(void)
{
  UCHAR String[DISPLAY_BUFFER_SIZE];

  UINT8 Dum1Initial;
  UINT8 Dum1Max;
  UINT8 Dum1UInt8;
  UINT8 IrCommand;     // command received from remote control.
  UINT8 Loop1UInt8;
  UINT8 SundayCounter; // used to find daylight saving time status.

  UINT16 Loop1UInt16;

  UINT32 Bme280UniqueId;
  UINT32 OutsideTemp;

  float ClockDivider;
  float Humidity;
  float Temperature;

  struct repeating_timer TimerMSec;
  struct repeating_timer Timer50MSec;


  /* Add accents to some French characters. */
  MonthName[FRENCH][FEB][1] = (UCHAR)31; // accent aigu sur fevrier.
  MonthName[FRENCH][AUG][2] = (UCHAR)30; // accent circonflexe sur aout.
  MonthName[FRENCH][DEC][1] = (UCHAR)31; // accent aigu sur decembre.

  ShortMonth[FRENCH][FEB][1] = (UCHAR)31; // accent aigu sur FEV.
  ShortMonth[FRENCH][AUG][2] = (UCHAR)30; // accent circonflexe sur AOU.
  ShortMonth[FRENCH][DEC][1] = (UCHAR)31; // accent aigu sur DEC.



  /* ---------------------------------------------------------------- *\
                   Initialize PWM-related variables
                so that we don't need to compute them
           every time we execute the sound callback function.
  \* ---------------------------------------------------------------- */
  /* Get PWM slice number for our passive piezo PWM, depending on which GPIO we installed it. */
  PwmSliceNumber = pwm_gpio_to_slice_num(PPIEZO);

  /* Get channel number for our passive buzzer PWM. */
  PwmChannelNumber = pwm_gpio_to_channel(PPIEZO);

  /* Retrieve system clock (should be 125 Mhz)... */
  SystemClock = clock_get_hz(clk_sys);

  /* ...and find the divider required to slow it down to 1 Mhz (for more flexibility with audio frequencies). */
  ClockDivider = SystemClock / 1000000;

  /* Slow the clock down to 1 MHz. */
  pwm_set_clkdiv(PwmSliceNumber, ClockDivider);

  

  /* ---------------------------------------------------------------- *\
            Seed random number generator (for dice rolling).
  \* ---------------------------------------------------------------- */
  srand(time_us_64());


  
  /* ---------------------------------------------------------------- *\
      Initialize all required GPIO ports of the Raspberry Pi Pico.
  \* ---------------------------------------------------------------- */
  init_gpio();


  
  /* ---------------------------------------------------------------- *\
                      Initialize system idle monitor.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < 14; ++Loop1UInt8)
    IdleMonitor[Loop1UInt8] = 0;

  /* IdleMonitor. Display results every 10 minutes, so reset archive values to get prepared for storage. */
  IdleTimeSamples = 120;
  for (IdleArchivePacket = 0; IdleArchivePacket < IdleTimeSamples; ++IdleArchivePacket)
  {
    for (Loop1UInt8 = 0; Loop1UInt8 < 14; ++Loop1UInt8)
    {
      ArchiveIdleMonitor[Loop1UInt8][IdleArchivePacket] = 0;
    }
  }
  LastIdleMonitorPacket = 0;
  IdleMonitorPacket = 0;
  IdleArchivePacket = 0;


  /* While in development environment, give some time to start the terminal emulator software after the CDC USB device has been recognized. */
  #ifdef DEVELOPER_VERSION
  while (gpio_get(SET_BUTTON) == 1)
  {
    tone(10);
    gpio_put(PICO_LED, 1);
    sleep_ms(1000);
    gpio_put(PICO_LED, 0);
    sleep_ms(1000);
  }
  while (gpio_get(SET_BUTTON) == 0)
    ; // make sure Set button has been released before going on...
  #endif


  /* Read the real-time clock IC. */
  Time_RTC = Read_RTC();

  Time_RTC.seconds    = Time_RTC.seconds & 0x7F;
  Time_RTC.minutes    = Time_RTC.minutes & 0x7F;
  Time_RTC.hour       = Time_RTC.hour & 0x3F;
  Time_RTC.dayofweek  = Time_RTC.dayofweek & 0x07;
  Time_RTC.dayofmonth = Time_RTC.dayofmonth & 0x3F;
  Time_RTC.month      = Time_RTC.month & 0x1F;

  CurrentHourSetting   = bcd_to_byte(Time_RTC.hour);
  CurrentMinute        = bcd_to_byte(Time_RTC.minutes);
  CurrentDayOfMonth    = bcd_to_byte(Time_RTC.dayofmonth);
  CurrentMonth         = bcd_to_byte(Time_RTC.month);
  CurrentYearLowPart   = bcd_to_byte(Time_RTC.year);
  CurrentDayOfWeek     = bcd_to_byte(Time_RTC.dayofweek) + 1;
  if (CurrentDayOfWeek == 8) CurrentDayOfWeek = 1; // Sunday equals 1 with new alarm algorithm.


  /* ---------------------------------------------------------------- *\
    WARNING: Variables below are for debugging purposes
             Be aware that setting On a debug option may have a
             significant impact on normal clock behavior, namely on
             timings !!!
             (Important timings will be out-of-sync).
             Use them with care and when you face a problem, investigate
             first if the debug option is not in cause.

             Turn on bits for debug information we want to display
             on external monitor.
  \* ---------------------------------------------------------------- */
  #ifdef RELEASE_VERSION
  DebugBitMask = DEBUG_NONE;
  #else  // RELEASE_VERSION
  /* NOTE: If DEBUG_FLASH is turned On, it allows logging information about flash memory operations.
           However, logging a flash update during a the callback_s procedure takes too long at 38400 bauds and will generate a crash.
           So, it is possible to keep all logging information and use the debug before starting the callback_s, or it is required to trim down
           the logged information if there is a need to consult the logged information during the callback_s flash configuration update. */
  DebugBitMask = DEBUG_NONE;
  // DebugBitMask += DEBUG_ALARMS;
  // DebugBitMask += DEBUG_BME280;
  // DebugBitMask += DEBUG_BRIGHTNESS;
  // DebugBitMask += DEBUG_CHIME;
  // DebugBitMask += DEBUG_CRC16;
  // DebugBitMask += DEBUG_FLASH;
  // DebugBitMask += DEBUG_IDLE_MONITOR;
  // DebugBitMask += DEBUG_DHT;
  // DebugBitMask += DEBUG_SOUND_QUEUE;
  // DebugBitMask += DEBUG_IR_COMMAND;
  #endif  // RELEASE_VERSION


  /* If user requested any section to be debugged through Pico's UART, send a time stamp to the log screen. */
  if (DebugBitMask)
  {
    for (Loop1UInt8 = 0; Loop1UInt8 < 50; ++Loop1UInt8)
      uart_send(__LINE__, "\r");

    sprintf(String, "cls");
    uart_send(__LINE__, String);

    sprintf(String, "home");
    uart_send(__LINE__, String);

    /* French accents don't show up on external monitor, so temporarily put back non-accent characters. */
    MonthName[FRENCH][FEB][1] = 'e';  // accent aigu sur fevrier.
    MonthName[FRENCH][AUG][2] = 'u';  // accent circonflexe sur aout.
    MonthName[FRENCH][DEC][1] = 'e';  // accent aigu sur decembre.

    /* Assign temporary values until configuration is read from flash. */
    FlashConfig.CurrentYearCentile = DEFAULT_YEAR_CENTILE;
    FlashConfig.Language = DEFAULT_LANGUAGE;

    sprintf(String, "\r\r\r\r- - - - - = = = = = %2.2u-%s-%2.2u%2.2u %2.2u:%2.2u GREEN CLOCK LOG INFO = = = = = - - - - -\r\r", CurrentDayOfMonth, MonthName[FlashConfig.Language][CurrentMonth], FlashConfig.CurrentYearCentile, CurrentYearLowPart, CurrentHourSetting, CurrentMinute);
    uart_send(__LINE__, String);

    sprintf(String, "NOTE: Parts of the time stamps below may be wrong until flash configuration has been read and validated...\r");
    uart_send(__LINE__, String);

    sprintf(String, "      ...and callback to read real-time clock has been triggered.\r\r");
    uart_send(__LINE__, String);

    /* Put back French accents when done. */
    MonthName[FRENCH][FEB][1] = (UCHAR)31; // accent aigu sur fevrier.
    MonthName[FRENCH][AUG][2] = (UCHAR)30; // accent circonflexe sur aout.
    MonthName[FRENCH][DEC][1] = (UCHAR)31; // accent aigu sur decembre.

    uart_send(__LINE__, "- Sections below will be logged for debug: \r\r");

    /* Log all debug sections that are under analyze. */
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
            uart_send(__LINE__, "-> LED matrix auto brightness\r");
          break;

          case DEBUG_CHIME:
            uart_send(__LINE__, "-> Hourly chime algorithm\r");
          break;

          case DEBUG_CRC16:
            uart_send(__LINE__, "-> CRC16 calculation\r");
          break;

          case DEBUG_FLASH:
            uart_send(__LINE__, "-> Flash read / write\r");
          break;

          case DEBUG_IDLE_MONITOR:
            uart_send(__LINE__, "-> System idle monitor\r");
          break;

          case DEBUG_DHT:
            uart_send(__LINE__, "-> DHT22\r");
          break;

          case DEBUG_SOUND_QUEUE:
            uart_send(__LINE__, "-> Sound queue\r");
          break;

          case DEBUG_IR_COMMAND:
            uart_send(__LINE__, "-> IR command\r");
          break;

          default:
            sprintf(String, "-> Section #%u\r", Loop1UInt8);
            uart_send(__LINE__, String);
          break;
        }
      }
    }
    uart_send(__LINE__, "\r\r\r");
  }


  /* ---------------------------------------------------------------- *\
               Reserve RAM space area for flash operations.
  \* ---------------------------------------------------------------- */
  FlashData = (UINT8 *)malloc(FLASH_SECTOR_SIZE);


  /* ---------------------------------------------------------------- *\
          Retrieve Green Clock configuration from flash memory.
  \* ---------------------------------------------------------------- */
  /***
  if (DebugBitMask & DEBUG_FLASH)
    uart_send(__LINE__, "Erasing configuration section of Pico's flash memory to force generating a new configuration.\r");

  flash_erase(0x1FF000);
  ***/

  flash_read_config();

  if (DebugBitMask & DEBUG_FLASH)
    flash_display_config();


  /* ---------------------------------------------------------------- *\
                  Initialize scroll queue on entry.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < MAX_SCROLL_QUEUE; ++Loop1UInt8)
    ScrollQueue[Loop1UInt8] = 0xAA; // let's put 0xAA since 0x00 corresponds to calendar event number 0.
  ScrollQueueHead = 0;
  ScrollQueueTail = 0;


  /* ---------------------------------------------------------------- *\
                Initialize queue for active buzzer on entry.
                (buzzer integrated in the Pico Green Clock)
  \* ---------------------------------------------------------------- */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_ACTIVE_SOUND_QUEUE; ++Loop1UInt16)
  {
    SoundQueueActive[Loop1UInt16].MSec = 0;
    SoundQueueActive[Loop1UInt16].RepeatCount = 0;
  }
  SoundActiveHead = 0;
  SoundActiveTail = 0;


  /* ---------------------------------------------------------------- *\
               Initialize queue for passive buzzer on entry.
                       (must be installed by user)
  \* ---------------------------------------------------------------- */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_PASSIVE_SOUND_QUEUE; ++Loop1UInt16)
  {
    SoundQueuePassive[Loop1UInt16].Freq = 0;
    SoundQueuePassive[Loop1UInt16].MSec = 0;
  }
  SoundPassiveHead = 0;
  SoundPassiveTail = 0;


  /* ---------------------------------------------------------------- *\
                 Initialize FlagSetupXxx[] on entry.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_ALARM_HI_LIMIT; ++Loop1UInt8)
    FlagSetupAlarm[Loop1UInt8] = FLAG_OFF;

  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_CLOCK_HI_LIMIT; ++Loop1UInt8)
    FlagSetupClock[Loop1UInt8] = FLAG_OFF;

  for (Loop1UInt8 = SETUP_NONE; Loop1UInt8 < SETUP_TIMER_HI_LIMIT; ++Loop1UInt8)
    FlagSetupTimer[Loop1UInt8] = FLAG_OFF;


  /* ---------------------------------------------------------------- *\
          Initialize total ringing time for each alarm on entry.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < MAX_ALARMS; ++Loop1UInt8)
    AlarmRingTime[Loop1UInt8] = 0;


  /* ---------------------------------------------------------------- *\
          "Setup order" in French is different than in English.
  \* ---------------------------------------------------------------- */
  if (FlashConfig.Language == FRENCH)
  {
    SETUP_DAY_OF_MONTH = 0x03;
    SETUP_MONTH = 0x04;
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
  /* Initialize callback function for 1 millisecond timer. */
  add_repeating_timer_ms(1, timer_callback_ms, NULL, &TimerMSec);

  /* Initialize callback function for 1 second timer. */
  add_repeating_timer_ms(1000, timer_callback_s, NULL, &TimerSec);

  /* Initialize sound callback function (50 milliseconds period) for both active and passive buzzers. */
  add_repeating_timer_ms(50, sound_callback_ms, NULL, &Timer50MSec);


  #ifdef TEST_CODE
  /* ---------------------------------------------------------------- *\
                     Temporary testing zone below
      NOTE: By putting tests here, the callback function to scroll
            data on the clock display is active, but time display and
            other functions are not started yet, making things easier.
  \* ---------------------------------------------------------------- */
  /* If we are in test mode, jump to the generic test routine and perform tests there. */
  // test_zone(1);   // tests on RTC IC registers.
  // test_zone(2);   // tests for flash read / write / erase.
  // test_zone(3);   // tests for CRC16 function.
  // test_zone(4);   // test ambient light intensity.
  // test_zone(5);   // scrolling test.
  // test_zone(6);   // test variable character width and spacing.
  // test_zone(7);   // display 4 X 7 character map.
  // test_zone(8);   // display all characters of the 5 X 7 bitmap.
  // test_zone(9);   // tests with indicators on the left side of the display.
  // test_zone(10);  // turn on each bit of each byte of the active display matrix.
  // test_zone(11);  // display every 5 X 7 character between a left and right frames.
  // test_zone(12);  // display all 5 X 7 characters, one at a time, using fill_display_buffer_5X7() function.
  // test_zone(13);  // display a few 5 X 7 characters on the display.
  // test_zone(14);  // scroll numbers and capital letters of 4 X 7 characters.
  // test_zone(15);  // display information about system variables for debug  purposes.
  // test_zone(16);  // check each bit of each byte of the display matrix.
  // test_zone(17);  // play with clock display indicators.
  // test_zone(18);  // quick test with both character formats (4 X 7 and 5 X 7).
  /* ---------------------------------------------------------------- *\
                         End of testing zone
  \* ---------------------------------------------------------------- */
  #endif


  #ifndef QUICK_START
  /* ---------------------------------------------------------------- *\
                           Test active buzzer... 
              Four short tones to indicate we are entering
                      the main Green Clock firmware.
   \* ---------------------------------------------------------------- */
  tone(10);
  sleep_ms(100);
  tone(10);
  sleep_ms(100);
  tone(10);
  sleep_ms(100);
  tone(10);
  sleep_ms(300);


  /* ---------------------------------------------------------------- *\
                           Test passive buzzer.
  \* ---------------------------------------------------------------- */
  #ifdef PASSIVE_PIEZO_SUPPORT
  sound_queue_passive(523,  200);
  sound_queue_passive(SILENT, 50);
  sound_queue_passive(659,  200);
  sound_queue_passive(SILENT, 50);
  sound_queue_passive(784,  200);
  sound_queue_passive(SILENT, 50);
  sound_queue_passive(1047, 500);
  sound_queue_passive(SILENT, 50);
  #endif  // PASSIVE_PIEZO_SUPPORT
  

  /* ---------------------------------------------------------------- *\
                      Test clock LED matrix.
  \* ---------------------------------------------------------------- */
  matrix_test();

  /* ---------------------------------------------------------------- *\
           Make some pixel animation on entry (just for fun).
  \* ---------------------------------------------------------------- */
  pixel_twinkling(3);
  #endif // QUICK_START


  /* ---------------------------------------------------------------- *\
              Clear clock LED matrix framebuffer on entry.
  \* ---------------------------------------------------------------- */
  clear_framebuffer(0);


  /* ---------------------------------------------------------------- *\
      Check if "Move On" (scrolling) indicator should be turned on
      (If FlagScrollEnable has been turned On in the clock options
         at compile time). This can also be changed in the clock
                     configuration at run time.
  \* ---------------------------------------------------------------- */
  if (FlashConfig.FlagScrollEnable == FLAG_ON)
    IndicatorScrollOn;


  /* ---------------------------------------------------------------- *\
         Turn on "Alarm On: indicator if at least one alarm is On.
  \* ---------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
  {
    if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_ON)
    {
      IndicatorAlarmOn;
      break; // get out of for loop as soon as alarm indicator is On.
    }
  }


  /* ---------------------------------------------------------------- *\
             Turn on selected temperature indicator on entry.
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


  /* ---------------------------------------------------------------- *\
           Turn on appropriate Hourly chime indicator on entry.
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


  /* ---------------------------------------------------------------- *\
            Turn on auto-brightness indicator if appropriate.
  \* ---------------------------------------------------------------- */
  if (FlashConfig.FlagAutoBrightness == FLAG_ON)
    IndicatorAutoLightOn;


  /* ------------------------------------------------------------------------------------------------------------------------ *\
                                                 DAYLIGHT SAVING TIME SUPPORT
                        When powering up the clock, guess if we are in "Daylight Saving Time" on entry...
         If we are between 2nd Sunday of March and 1st Sunday of November, we assume that we are in Daylight Saving Time.
         If we are between 2h00 and 3h00 on the second Sunday of March, we assume that the hour must be changed since at
           2h00 we're supposed to jump directly at 3h00. So, the hour is adjusted automatically. However, in November,
         between 2h00 and 3h00, there is no way to know if the clock has been adjusted before the clock is restarted. So,
      we will assume that we are passed the clock adjustment. If the guess is wrong and the user sees a discrepancy, he can
      turn off the clock, then turn it back on, after 3h00 during the first Sunday of November. This should fix the problem.
  \* ------------------------------------------------------------------------------------------------------------------------ */
  if (FlashConfig.DaylightSavingTime != DST_NONE)
  {
    FlagDayLightSavingTime = time_is_dst(Time_RTC, FlashConfig.DaylightSavingTime);
  }
  /* ------------------------------------------------------------------------------------------------------------------------ *\
                                                End of daylight saving time section
  \* ------------------------------------------------------------------------------------------------------------------------ */



  /* ---------------------------------------------------------------- *\
                        Scroll firmware version.
       Leave at least one string scrolling on clock display outside
      of QUICK_START to trigger time display (show_time() function).
  \* ---------------------------------------------------------------- */
  sprintf(String, "Pico Green Clock - Firmware Version %s", FIRMWARE_VERSION);
  scroll_string(24, String);

 
  /* ---------------------------------------------------------------- *\
        Special message if sound has been cut-off at compile-time.
  \* ---------------------------------------------------------------- */
  #ifdef NO_SOUND
  /* Special warning if sound has been cut-off in source code. */
  if (FlashConfig.Language == ENGLISH)
    scroll_string(24, "WARNING - SOUND CUT-OFF");

  if (FlashConfig.Language == FRENCH)
    scroll_string(24, "ATTENTION - PAS DE SON");
  #endif  // NO_SOUND


  #ifndef QUICK_START
  /* ---------------------------------------------------------------- *\
           Special message if some debug options are selected.
  \* ---------------------------------------------------------------- */
  if (DebugBitMask)
  {
    if (FlashConfig.Language == ENGLISH)
      scroll_string(24, "WARNING - SOME DEBUG OPTIONS ARE ENABLED");

    if (FlashConfig.Language == FRENCH)
      scroll_string(24, "ATTENTION - DES OPTIONS DEBUG SONT ACTIVES");
  }
  #endif  // QUICK_START


  #ifdef BME280_SUPPORT
  /* ---------------------------------------------------------------- *\
                       Initialize BME280 sensor.
  \* ---------------------------------------------------------------- */
  if (bme280_init())
  {
    scroll_string(24, "Error while trying to initialize BME280");
  }
  else
  {
    /* Read BME280 device ID to make sure it is a "real" BME280, since some companies sell BMP280 while saying they are BME280. */
    /* BME280 device ID is 0x60, while 0x56 and 0x57 are BMP280 samples and 0x58 are BMP280 mass production. */
    Dum1UInt8 = bme280_read_device_id();
    switch (Dum1UInt8)
    {
      case 0x60:
        scroll_string(24, "BME280 device ID: 0x60");
      break;

      case 0x56:
      case 0x57:
        sprintf(String, "BMP280 device ID: 0x%2.2X (sample production units)", Dum1UInt8);
        scroll_string(24, String);
        scroll_string(24, "Your device seems not to be a true BME280");
      break;

      case 0x58:
        scroll_string(24, "BMP280 device ID: 0x58 (mass production units)");
        scroll_string(24, "Your device seems not to be a true BME280");
      break;

      default:
        sprintf(String, "Unrecognized BME280 device ID: 0x%2.2X", Dum1UInt8);
        scroll_string(24, String);
        scroll_string(24, "Your device seems not to be a true BME280");
      break;
    }

    /* Read BME280 unique id ("serial number" of the specific device used). */
    Bme280UniqueId = bme280_read_unique_id();
    sprintf(String, "BME280 Unique ID: %4.4X %4.4X", ((Bme280UniqueId & 0xFFFF0000) >> 16), Bme280UniqueId & 0xFFFF);
    scroll_string(24, String);

    /* Read calibration data of the specific device used (written in BME280's non volatile memory). */
    bme280_read_calib_data();

    /* Compute calibration parameters from calibration data. */
    bme280_compute_calib_param();
  }
  #endif // BME280_SUPPORT


  #ifndef QUICK_START
  /* ---------------------------------------------------------------- *\
                     Scroll Pico unique identifier
            NOTE: Scroll queue will be processed when entering
                      the main program loop below.
  \* ---------------------------------------------------------------- */
  scroll_queue(TAG_PICO_UNIQUE_ID);



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


  /* Initialize interrupt handler to capture remote control infrared commands and DHT22 data stream. */
  /* NOTE: Same interrupt service routine is used for more than both. So let's make sure it is started.
           If no extra device has been installed by user, no interrupt will be generated and the ISR will never be called... */
  /* NOTE: Interrupt-driven algorithm for DHT22 reading was not successful and has been changed back to polling type.
           However, source code has been left in "read_dht22()" function for those who want to expeeriment. */
  IrStepCount = 0;
  /// gpio_set_irq_enabled_with_callback(IR_RX, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, (gpio_irq_callback_t)&isr_signal_trap);
  
  /* See comments about code section below in function read_dht22() */
  /* gpio_set_irq_enabled(DHT22_RX, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true); */



  /* ---------------------------------------------------------------- *\
                  Validate communication with DHT22.
  \* ---------------------------------------------------------------- */
  #ifdef DHT22_SUPPORT
  if (read_dht22(&Temperature, &Humidity))
  {
    scroll_string(24, "Error while trying to communicate with DHT22");
  }
  #endif


  /* Main program loop... will loop forever. */
  while (TRUE)
  {
    /* If user pressed the "Setup" (top) button (detected in the call-back function), proceed with clock setup. */
    if (FlagSetClock == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON; // keep track of inactive (idle) time to eventually declare a time-out.
      setup_clock_frame();     // display clock template and current values.
      FlagSetClock = FLAG_OFF; // reset clock setup mode flag after this pass.
    }

    /* If we just entered the "Alarm" setup mode (detected in the call-back function), proceed with alarm setup. */
    if (FlagSetAlarm == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON; // keep track of inactive (idle) time to eventually declare a time-out.
      setup_alarm_frame();     // display alarm template and current values.
      FlagSetAlarm = FLAG_OFF; // reset alarm setup mode flag after this pass.
    }

    /* If we just entered the "Timer" setup mode (detected in the call-back function), proceed with timer setup. */
    if (FlagSetTimer == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON; // keep track of inactive (idle) time to eventually declare a time-out.
      setup_timer_frame();     // display timer template and current values.
      FlagSetTimer = FLAG_OFF; // reset timer setup mode after this pass.
    }

    /* Check if it is time to update time display on the clock. */
    if ((FlagUpdateTime == FLAG_ON) && (ScrollDotCount == 0))
    {
      show_time();
      FlagUpdateTime = FLAG_OFF;
    }

    /* If a tag has been inserted in the scroll queue, process it. */
    if (ScrollQueueHead != ScrollQueueTail)
    {
      process_scroll_queue();
    }

    #ifdef IR_SUPPORT
    /* If infrared remote control support is enable,
       Check if data has been received by infrared sensor. */
    if (IrStepCount != 0)
    {
      sleep_ms(250); // make sure all IR burst has been received.

      if ((decode_ir_command(FLAG_OFF, &IrCommand) == 0) && (IrCommand != IR_LO_LIMIT) && (IrCommand < IR_HI_LIMIT))
      {
        /* Process IR command only if it has been received / decoded without error. */
        IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.

        process_ir_command(IrCommand);
      }
    }
    #endif

    /* Note:
             IdleMonitor[00] = Cumulative loops from 00 to 05 seconds
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

             IdleMonitor[12] = Count for current 5-seconds period.

             IdleMonitor[13] = Average number of idle loops per second for the last 60-seconds period. */

    IdleMonitorPacket = SecondCounter / 5;

    /* If we just chenged of "5-seconds" packet, make some housekeeping. */
    if (IdleMonitorPacket != LastIdleMonitorPacket)
    {
      IdleMonitor[LastIdleMonitorPacket] = IdleMonitor[12];
      IdleMonitor[12] = 0;

      LastIdleMonitorPacket = IdleMonitorPacket;

      if (DebugBitMask & DEBUG_IDLE_MONITOR)
      {
        sprintf(String, "IdleMonitorPacket = %2u\r", IdleMonitorPacket);
        uart_send(__LINE__, String);
      }
    }

    /* One more loop executed. */
    ++IdleMonitor[12];

    if ((SecondCounter == 35) && (FlagIdleMonitor == FLAG_OFF))
    {
      if (DebugBitMask & DEBUG_IDLE_MONITOR)
      {
        for (Loop1UInt8 = 0; Loop1UInt8 < 14; ++Loop1UInt8)
        {
          ArchiveIdleMonitor[Loop1UInt8][IdleArchivePacket] = IdleMonitor[Loop1UInt8];
        }
        ++IdleArchivePacket;
      }

      /* Calculate average idle monitor for last 60-seconds period. */
      IdleMonitor[13] = 0;
      for (Loop1UInt8 = 0; Loop1UInt8 < 12; ++Loop1UInt8)
      {
        IdleMonitor[13] += IdleMonitor[Loop1UInt8];
      }

      IdleMonitor[13] = IdleMonitor[13] / 60;

      if (DebugBitMask & DEBUG_IDLE_MONITOR)
      {
        for (Loop1UInt8 = 0; Loop1UInt8 < 12; ++Loop1UInt8)
        {
          sprintf(String, "IdleMonitor[%2.2u] = %8llu  (%2.2u to %2.2u))\r", Loop1UInt8, IdleMonitor[Loop1UInt8], (Loop1UInt8 * 5), ((Loop1UInt8 * 5) + 5));
          uart_send(__LINE__, String);
        }

        sprintf(String, "Current period: IdleMonitor[12] = %llu\r", IdleMonitor[12]);
        uart_send(__LINE__, String);

        sprintf(String, "Resulting system idle monitor   = %llu\r\r\r", IdleMonitor[13]);
        uart_send(__LINE__, String);
      }

      FlagIdleMonitor = FLAG_ON; // only one calculation per 5-seconds period.
    }

    /* Prepare IdleMonitor to be displayed for next minute. */
    if ((SecondCounter == 36) && (FlagIdleMonitor == FLAG_ON))
      FlagIdleMonitor = FLAG_OFF;

    if (DebugBitMask & DEBUG_IDLE_MONITOR)
    {
      if (IdleArchivePacket >= IdleTimeSamples)
      {
        for (IdleArchivePacket = 0; IdleArchivePacket < IdleTimeSamples; ++IdleArchivePacket)
        {
          for (Loop1UInt8 = 0; Loop1UInt8 < 12; ++Loop1UInt8)
          {
            sprintf(String, "ArchiveIdleMonitor[%2.2u][%3.3u] = %8llu)\r", Loop1UInt8, IdleArchivePacket, ArchiveIdleMonitor[Loop1UInt8][IdleArchivePacket]);
            uart_send(__LINE__, String);
          }

          uart_send(__LINE__, "\r");
          sprintf(String, "ArchiveIdleMonitor[12][%3.3u] = %8llu (current period)\r\r", IdleArchivePacket, ArchiveIdleMonitor[12][IdleArchivePacket]);
          uart_send(__LINE__, String);

          sprintf(String, "System idle monitor   [%3.3u] = %8llu\r\r\r", IdleArchivePacket, ArchiveIdleMonitor[13][IdleArchivePacket]);
          uart_send(__LINE__, String);
        }
        IdleArchivePacket = 0;
      }
    }
  }
}





/* $PAGE */
/* $TITLE=adc_show() */
/* ------------------------------------------------------------------ *\
        Read ambient light from Pico's analog-to-digital gpio.
     For better performance, average light value in the last minute.
\* ------------------------------------------------------------------ */
void adc_show_count(void)
{
  UCHAR String[128];

  UINT Loop1UInt;

  static UINT16 AmbientLightMSecCounter;
  static UINT64 CumulativeLightLevel;
  static UINT16 LightLevel[12] = {550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550}; // assume average ambient light on entry.
  static UINT NextCell;                                                                        // point to next member of circular buffer to be read.


  /* If the clock has been setup for automatic ambient light detection, get ambient light value from Pico's analog-to-digital converter. */
  if (FlashConfig.FlagAutoBrightness == FLAG_ON)
  {
    ++AmbientLightMSecCounter;
    CumulativeLightLevel += get_ads1015();

    if (AmbientLightMSecCounter >= 5000)
    {
      /* Compute the average ambient light level for the last five seconds (5000 milliseconds). */
      AmbientLightMSecCounter = 0;
      LightLevel[NextCell++] = CumulativeLightLevel / 5000;
      if (NextCell >= 12)
        NextCell = 0;
      CumulativeLightLevel = 0;

      /* Compute average ambient light level for the last minute. */
      for (Loop1UInt = 0; Loop1UInt < 12; ++Loop1UInt)
        CumulativeLightLevel += LightLevel[Loop1UInt];

      AverageLightLevel = (CumulativeLightLevel / 12);
      CumulativeLightLevel = 0;
    }
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
  {
    uart_send(__LINE__, "bme280_compute_calib_param(Entering)\r\r");
  }

  /* Compute calibration parameters for temperature. */
  Bme280CalibParam.DigT1 = (UINT16)(Bme280CalibData[0] + (Bme280CalibData[1] << 8));
  Bme280CalibParam.DigT2 = (int16_t)(Bme280CalibData[2] + (Bme280CalibData[3] << 8));
  Bme280CalibParam.DigT3 = (int16_t)(Bme280CalibData[4] + (Bme280CalibData[5] << 8));

  /* Compute calibration parameters for pressure. */
  Bme280CalibParam.DigP1 = (UINT16)(Bme280CalibData[6] + (Bme280CalibData[7] << 8));
  Bme280CalibParam.DigP2 = (int16_t)((Bme280CalibData[8] + (Bme280CalibData[9] << 8)) & 0xFFFF);

  Dum1UInt16 = (((Bme280CalibData[11] << 8) & 0xFF00) + Bme280CalibData[10]);
  Bme280CalibParam.DigP3 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280CalibData[13] << 8) & 0xFF00) + Bme280CalibData[12]);
  Bme280CalibParam.DigP4 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280CalibData[15] << 8) & 0xFF00) + Bme280CalibData[14]);
  Bme280CalibParam.DigP5 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280CalibData[17] << 8) & 0xFF00) + Bme280CalibData[16]);
  Bme280CalibParam.DigP6 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280CalibData[19] << 8) & 0xFF00) + Bme280CalibData[18]);
  Bme280CalibParam.DigP7 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280CalibData[21] << 8) & 0xFF00) + Bme280CalibData[20]);
  Bme280CalibParam.DigP8 = (int16_t)Dum1UInt16;

  Dum1UInt16 = (((Bme280CalibData[23] << 8) & 0xFF00) + Bme280CalibData[22]);
  Bme280CalibParam.DigP9 = (int16_t)Dum1UInt16;

  /* Compute calibration parameters for humidity. */
  Bme280CalibParam.DigH1 = (UCHAR)Bme280CalibData[25];
  Bme280CalibParam.DigH2 = (int16_t)(Bme280CalibData[26] + (Bme280CalibData[27] << 8));
  Bme280CalibParam.DigH3 = (UCHAR)Bme280CalibData[28];
  Bme280CalibParam.DigH4 = (int16_t)((Bme280CalibData[29] << 4) + (Bme280CalibData[30] & 0x0F));
  Bme280CalibParam.DigH5 = (int16_t)((Bme280CalibData[31] << 4) + ((Bme280CalibData[30] & 0xF0) >> 4));
  Bme280CalibParam.DigH6 = (char)Bme280CalibData[32];

  if (DebugBitMask & DEBUG_BME280)
  {
    uart_send(__LINE__, "Temperature:\r");
    sprintf(String, "DigT1: %4.4X   %7u\r", Bme280CalibParam.DigT1, Bme280CalibParam.DigT1);
    uart_send(__LINE__, String);

    sprintf(String, "DigT2: %4.4X   %7d\r", Bme280CalibParam.DigT2, Bme280CalibParam.DigT2);
    uart_send(__LINE__, String);

    sprintf(String, "DigT3: %4.4X   %7d\r\r\r", Bme280CalibParam.DigT3, Bme280CalibParam.DigT3);
    uart_send(__LINE__, String);

    uart_send(__LINE__, "Pressure:\r");
    sprintf(String, "DigP1: %4.4X   %7u\r", Bme280CalibParam.DigP1, Bme280CalibParam.DigP1);
    uart_send(__LINE__, String);

    sprintf(String, "DigP2: %4.4X   %7d\r", (Bme280CalibParam.DigP2 & 0xFFFF), Bme280CalibParam.DigP2);
    uart_send(__LINE__, String);

    sprintf(String, "DigP3: %4.4X   %7d\r", Bme280CalibParam.DigP3, Bme280CalibParam.DigP3);
    uart_send(__LINE__, String);

    sprintf(String, "DigP4: %4.4X   %7d\r", Bme280CalibParam.DigP4, Bme280CalibParam.DigP4);
    uart_send(__LINE__, String);

    sprintf(String, "DigP5: %4.4X   %7d\r", Bme280CalibParam.DigP5, Bme280CalibParam.DigP5);
    uart_send(__LINE__, String);

    sprintf(String, "DigP6: %4.4X   %7d\r", (Bme280CalibParam.DigP6 & 0xFFFF), Bme280CalibParam.DigP6);
    uart_send(__LINE__, String);

    sprintf(String, "DigP7: %4.4X   %7d\r", Bme280CalibParam.DigP7, Bme280CalibParam.DigP7);
    uart_send(__LINE__, String);

    sprintf(String, "DigP8: %4.4X   %7d\r", (Bme280CalibParam.DigP8 & 0xFFFF), Bme280CalibParam.DigP8);
    uart_send(__LINE__, String);

    sprintf(String, "DigP9: %4.4X   %7d\r\r\r", Bme280CalibParam.DigP9, Bme280CalibParam.DigP9);
    uart_send(__LINE__, String);

    uart_send(__LINE__, "Humidity:\r");
    sprintf(String, "DigH1: 0x%2.2X   %7u\r", Bme280CalibParam.DigH1, Bme280CalibParam.DigH1);
    uart_send(__LINE__, String);

    sprintf(String, "DigH2: %4.4X   %7d\r", Bme280CalibParam.DigH2, Bme280CalibParam.DigH2);
    uart_send(__LINE__, String);

    sprintf(String, "DigH3: 0x%2.2X   %7u\r", Bme280CalibParam.DigH3, Bme280CalibParam.DigH3);
    uart_send(__LINE__, String);

    sprintf(String, "DigH4: %4.4X   %7d\r", Bme280CalibParam.DigH4, Bme280CalibParam.DigH4);
    uart_send(__LINE__, String);

    sprintf(String, "DigH5: %4.4X   %7d\r", Bme280CalibParam.DigH5, Bme280CalibParam.DigH5);
    uart_send(__LINE__, String);

    sprintf(String, "DigH6: 0x%2.2X   %7d\r\r\r\r\r", Bme280CalibParam.DigH6, Bme280CalibParam.DigH6);
    uart_send(__LINE__, String);
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
  ++Bme280Readings;

  if (DebugBitMask & DEBUG_BME280)
  {
    uart_send(__LINE__, "bme280_get_temp(Entering)\r\r");
  }

  /* -------------------- Force a BME280 reading -------------------- */
  /* --- and oversampling for temperature and barometric pressure --- */
  /* Bits 1 and 0: BME280 sensor mode.  00 = sleep mode; 01, 10 = forced mode; 11 = normal mode.
     Bits 4, 3, 2: oversampling for barometric pressure data.
     Bits 7, 6, 5: oversampling for temperature data.
     Note: 0b101 = oversampling X 16; 0b100 = oversampling X 8; 0b011 = oversampling X 4;
           0b010 = oversampling X 2;  0b001 = oversampling X 1; 0b000 = skip oversampling. */
  Buffer[0] = BME280_REGISTER_CTRL_MEAS;
  Buffer[1] = 0b00100000    // temperature oversampling X 1
              + 0b00000100  // pressure oversampling X 1
              + 0b00000001; // BME280 in "force" mode

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
    ++Bme280Errors;

    return 0xFF;
  }

  /* Next, read temperature, humidity and barometric pressure raw data.
     Give some time to the device to complete operation
     (for some reason, the "status" register doesn't seem to indicate a pending operation) . */
  bme280_read_registers(Buffer);

  /* Compute raw values. */
  RawPress = ((Buffer[0] << 12) + (Buffer[1] << 4) + ((Buffer[2] & 0xF0) >> 4));
  RawTemp = ((Buffer[3] << 12) + (Buffer[4] << 4) + ((Buffer[5] & 0xF0) >> 4));
  RawHum = ((Buffer[6] << 8) + Buffer[7]);

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "RawPress: %5.5X      ", RawPress);
    uart_send(__LINE__, String);

    sprintf(String, "- RawTemp:  %5.5X      ", RawTemp);
    uart_send(__LINE__, String);

    sprintf(String, "- RawHum:   %5.4X\r\r\r", RawHum);
    uart_send(__LINE__, String);
  }

  /* Compute actual temperature. */
  Dum1Int32 = ((((RawTemp >> 3) - ((int32_t)Bme280CalibParam.DigT1 << 1))) * ((int32_t)Bme280CalibParam.DigT2)) >> 11;
  Dum2Int32 = (((((RawTemp >> 4) - ((int32_t)Bme280CalibParam.DigT1)) * ((RawTemp >> 4) - ((int32_t)Bme280CalibParam.DigT1))) >> 12) * ((int32_t)Bme280CalibParam.DigT3)) >> 14;
  TempFactor = Dum1Int32 + Dum2Int32;
  Bme280CalibParam.TemperatureC = ((TempFactor * 5 + 128) >> 8) / 100.0;
  Bme280CalibParam.TemperatureF = ((Bme280CalibParam.TemperatureC * 9) / 5.0) + 32;
  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "Temperature:    %3.3f'C   %3.3f'F\r\r\r", Bme280CalibParam.TemperatureC, Bme280CalibParam.TemperatureF);
    uart_send(__LINE__, String);
  }

  /* Compute actual relative humidity. */
  FinalHum = (TempFactor - ((int32_t)76800));
  FinalHum = (((((RawHum << 14) - (((int32_t)Bme280CalibParam.DigH4) << 20) - (((int32_t)Bme280CalibParam.DigH5) * FinalHum)) +
                ((int32_t)16384)) >>
               15) *
              (((((((FinalHum * ((int32_t)Bme280CalibParam.DigH6)) >> 10) *
                   (((FinalHum * ((int32_t)Bme280CalibParam.DigH3)) >> 11) + ((int32_t)32768))) >>
                  10) +
                 ((int32_t)2097152)) *
                    ((int32_t)Bme280CalibParam.DigH2) +
                8192) >>
               14));
  FinalHum = (FinalHum - (((((FinalHum >> 15) * (FinalHum >> 15)) >> 7) * ((int32_t)Bme280CalibParam.DigH1)) >> 4));
  FinalHum = (FinalHum < 0 ? 0 : FinalHum);
  FinalHum = (FinalHum > 419430400 ? 419430400 : FinalHum);
  FinalHum = (int32_t)(FinalHum >> 12);
  Bme280CalibParam.Humidity = FinalHum / 1024.0;
  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "Humidity:       %2.3f%%\r\r\r", Bme280CalibParam.Humidity);
    uart_send(__LINE__, String);
  }

  /* Compute actual barometric pressure. */
  Dum1Int64 = ((int64_t)TempFactor) - 128000;
  Dum2Int64 = Dum1Int64 * Dum1Int64 * (int64_t)Bme280CalibParam.DigP6;
  Dum2Int64 = Dum2Int64 + ((Dum1Int64 * (int64_t)Bme280CalibParam.DigP5) << 17);
  Dum2Int64 = Dum2Int64 + (((int64_t)Bme280CalibParam.DigP4) << 35);
  Dum1Int64 = ((Dum1Int64 * Dum1Int64 * (int64_t)Bme280CalibParam.DigP3) >> 8) + ((Dum1Int64 * (int64_t)Bme280CalibParam.DigP2) << 12);
  Dum1Int64 = (((((int64_t)1) << 47) + Dum1Int64)) * ((int64_t)Bme280CalibParam.DigP1) >> 33;
  if (Dum1Int64 == 0)
    return 0; // to prevent a division by zero.
  FinalPress = 1048576 - RawPress;
  FinalPress = (((FinalPress << 31) - Dum2Int64) * 3125) / Dum1Int64;
  Dum1Int64 = (((int64_t)Bme280CalibParam.DigP9) * (FinalPress >> 13) * (FinalPress >> 13)) >> 25;
  Dum2Int64 = (((int64_t)Bme280CalibParam.DigP8) * FinalPress) >> 19;
  FinalPress = (UINT32)((FinalPress + Dum1Int64 + Dum2Int64) >> 8) + (((int64_t)Bme280CalibParam.DigP7) << 4);
  Bme280CalibParam.Pressure = FinalPress / 25600.0;
  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "Pressure:       %4.3f hPa\r\r\r", Bme280CalibParam.Pressure);
    uart_send(__LINE__, String);
  }

  /* NOTE: Relative altitude may not be an interesting value to calculate since this value changes with
           normal and usual pressure changes due to atmospheric factors. */
  /*** Compute approximate altitude above sea level, based on atmospheric pressure and current temperature.
     NOTE: 1013.25 hPa is the standard pressure at sea level. ***
  Bme280CalibParam.Altitude = 44330.0 * (1.0 - pow(Bme280CalibParam.Pressure / 1013.25, 0.190294957));

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "Altitude:       %4.3f - seems to vary too much with pressure change over time\r\r\r", Bme280CalibParam.Altitude);
    uart_send(__LINE__, String);
  }
  ***/

  /* NOTE: Equation below to calculate the humidex factor is wrong. References about how to calculate this parameter are hard to
             find and not always accurate. This part will need to be reworked when trustful documentation is found on the subject. */
  /* Compute humidex factor, based on current temperature, relative humidity and atmospheric pressure. */
  /* Humidex is not valid if temperature is below 0'C. */
  /*** Part requiring rework... ***
  if (Bme280CalibParam.TemperatureC < 0)
    Bme280CalibParam.Humidex = 0;

  Alpha = ((17.27 * Bme280CalibParam.TemperatureC) / (237.7 + Bme280CalibParam.TemperatureC)) + log(Bme280CalibParam.Humidity / 100);
  DewPoint = (237.7 * Alpha) / (17.27 - Alpha);

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "Dew point:      %4.3f - need to be confirmed\r\r\r", DewPoint);
    uart_send(__LINE__, String);
  }

  Bme280CalibParam.Humidex = (TempC + (0.5555 * (Bme280CalibParam.Pressure - 10.0)));

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "Humidex:        %4.3f - current equation is incorrect\r\r\r\r\r", Bme280CalibParam.Humidex);
    uart_send(__LINE__, String);
  }
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
  {
    uart_send(__LINE__, "bme280_init(Entering)\r\r\r\r\r");
  }

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
  ReturnCode = i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 2, false);
  if (ReturnCode == PICO_ERROR_GENERIC)
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
  {
    uart_send(__LINE__, "bme280_read_all_registers(Entering)    ");
  }

  /* Give adress of the first BME280 register to read (lowest address). */
  Buffer[0] = BME280_REGISTER_CTRL_HUM;

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 1, true);
  ReturnCode = i2c_read_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 13, false);
  if (ReturnCode == PICO_ERROR_GENERIC)
    return 0xFF;

  if (DebugBitMask & DEBUG_BME280)
  {
    /* Validate return code, to make sure register read was successful. */
    sprintf(String, "Read(%u)\r\r", ReturnCode);
    uart_send(__LINE__, String);

    /* Display ctrl_hum register content. */
    sprintf(String, "ctrl_hum:   %2.2X\r", Buffer[0]);
    uart_send(__LINE__, String);

    /* Display status register content. */
    sprintf(String, "status:     %2.2X\r", Buffer[1]);
    uart_send(__LINE__, String);

    /* Display ctrl_meas register content. */
    sprintf(String, "ctrl_meas:  %2.2X\r", Buffer[2]);
    uart_send(__LINE__, String);

    /* Display config register content. */
    sprintf(String, "config:     %2.2X\r", Buffer[3]);
    uart_send(__LINE__, String);

    /* Display unknown register content. */
    sprintf(String, "unknown:    %2.2X\r", Buffer[4]);
    uart_send(__LINE__, String);

    /* Display press_msb register content. */
    sprintf(String, "press_msb:  %2.2X\r", Buffer[5]);
    uart_send(__LINE__, String);

    /* Display press_lsb register content. */
    sprintf(String, "press_lsb:  %2.2X\r", Buffer[6]);
    uart_send(__LINE__, String);

    /* Display press_xlsb register content. */
    sprintf(String, "press_xlsb: %2.2X\r", Buffer[7]);
    uart_send(__LINE__, String);

    /* Display temp_msb register content. */
    sprintf(String, "temp_msb:   %2.2X\r", Buffer[8]);
    uart_send(__LINE__, String);

    /* Display temp_lsb register content. */
    sprintf(String, "temp_lsb:   %2.2X\r", Buffer[9]);
    uart_send(__LINE__, String);

    /* Display temp_xlsb register content. */
    sprintf(String, "temp_xlsb:  %2.2X\r", Buffer[10]);
    uart_send(__LINE__, String);

    /* Display hum_msb register content. */
    sprintf(String, "hum_msb:    %2.2X\r", Buffer[11]);
    uart_send(__LINE__, String);

    /* Display hum_lsb register content. */
    sprintf(String, "hum_lsb:    %2.2X\r\r\r\r\r", Buffer[12]);
    uart_send(__LINE__, String);
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
  {
    uart_send(__LINE__, "bme280_read_calib_data(Entering)\r\r");
  }

  /* Read first chunk of BME280 calibration data. */
  Buffer = BME280_REGISTER_CALIB00;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, Bme280CalibData, 26, false, 10000000);

  /* Read second chunk of BME280 calibration data. */
  Buffer = BME280_REGISTER_CALIB26;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, &Bme280CalibData[26], 16, false, 10000000);

  if (DebugBitMask & DEBUG_BME280)
  {
    /* Display calibration data to optional monitor connected via UART. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 26; ++Loop1UInt8)
    {
      sprintf(String, "Calib[%2.2u]  Address: 0x%2.2X = %2.2X\r", Loop1UInt8, (Loop1UInt8 + 0x88), Bme280CalibData[Loop1UInt8]);
      uart_send(__LINE__, String);
    }

    for (Loop1UInt8 = 0; Loop1UInt8 < 16; ++Loop1UInt8)
    {
      sprintf(String, "Calib[%2.2u]  Address: 0x%2.2X = %2.2X\r", (Loop1UInt8 + 26), (0xE1 + Loop1UInt8), Bme280CalibData[Loop1UInt8 + 26]);
      uart_send(__LINE__, String);
    }

    uart_send(__LINE__, "\r\r\r\r");
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
  {
    uart_send(__LINE__, "bme280_read_config(Entering)           ");
  }

  Buffer = BME280_REGISTER_CONFIG;

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true);
  i2c_read_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, false);

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "Config: 0x%2.2X\r\r\r\r", Buffer);
    uart_send(__LINE__, String);
  }

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
  {
    uart_send(__LINE__, "bme280_read_device_id(Entering)         ");
  }

  Buffer = BME280_REGISTER_ID;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, &Buffer, 1, false, 10000000);

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "- Read(%u)    0x%2.2X\r\r\r\r\r", ReturnCode, Buffer);
    uart_send(__LINE__, String);
  }

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
  {
    sprintf(String, "bme280_read_registers(Entering)  %2.2X\r\r", Buffer[0]);
    uart_send(__LINE__, String);
  }

  Loop1UInt8 = 0;
  while (bme280_read_status() != 0)
  {
    ++Loop1UInt8;
  }

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 1, true);
  i2c_read_blocking(I2C_PORT, BME280_ADDRESS, Buffer, 8, false);

  if (DebugBitMask & DEBUG_BME280)
  {
    /* Display registers read from BME280. */
    sprintf(String, "Registers:       (wait: %4u)          ", Loop1UInt8);
    uart_send(__LINE__, String);
    for (Loop1UInt8 = 0; Loop1UInt8 < 8; ++Loop1UInt8)
    {
      sprintf(String, "- %2.2X ", Buffer[Loop1UInt8]);
      uart_send(__LINE__, String);
    }
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
  {
    uart_send(__LINE__, "bme280_read_status(Entering)           ");
  }

  Buffer = BME280_REGISTER_STATUS;

  i2c_write_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, true);
  i2c_read_blocking(I2C_PORT, BME280_ADDRESS, &Buffer, 1, false);

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "- Status: 0x%2.2X\r\r", Buffer);
    uart_send(__LINE__, String);
  }

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
  {
    uart_send(__LINE__, "bme280_read_unique_id(Entering)         ");
  }

  /* Give adress of the first BME280 register to read (lowest address). */
  Buffer[0] = BME280_REGISTER_UNIQUE_ID;

  i2c_write_timeout_us(I2C_PORT, BME280_ADDRESS, Buffer, 1, true, 10000000);
  ReturnCode = i2c_read_timeout_us(I2C_PORT, BME280_ADDRESS, Buffer, 4, false, 10000000);
  if (ReturnCode == PICO_ERROR_GENERIC)
    return 0xFF;

  UniqueId = ((((uint32_t)Buffer[3] + ((uint32_t)Buffer[2] << 8)) & 0x7FFFF) << 16) +
             (((uint32_t)Buffer[1]) << 8) + (uint32_t)Buffer[0];

  if (DebugBitMask & DEBUG_BME280)
  {
    sprintf(String, "- Read(%u)    Hex: %4.4X %4.4X\r\r\r\r\r", ReturnCode, ((UniqueId & 0xFFFF0000) >> 16), UniqueId & 0xFFFF);
    uart_send(__LINE__, String);
  }

  return UniqueId;
}
#endif





/* $PAGE */
/* $TITLE=clear_framebuffer() */
/* ------------------------------------------------------------------ *\
                 Clear the clock virtual framebuffer
                starting at the given column position.
           If the start column is inside the "real" display,
               then the closk display will be cleared.
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
  if (Data == NULL)
    return 0;

  if (DebugBitMask & DEBUG_CRC16)
  {
    sprintf(String, "Calculating CRC16 of this packet (size: %u):\r", DataSize);
    uart_send(__LINE__, String);

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
  {
    sprintf(String, "CRC16 computed: %X\r\r\r", CrcValue & 0xFFFF);
    uart_send(__LINE__, String);
  }

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


  /* If Language configuration is invalid, force English for debug info. */
  if ((FlashConfig.Language > LANGUAGE_LO_LIMIT) && (FlashConfig.Language < LANGUAGE_HI_LIMIT))
    Language = FlashConfig.Language;
  else
    Language = ENGLISH;

  /* Make sure year centile is valid. */
  if ((FlashConfig.CurrentYearCentile > 17) && (FlashConfig.CurrentYearCentile < 22))
    YearCentile = FlashConfig.CurrentYearCentile;
  else
    YearCentile = 20;

  /* French accents don't show up on external monitor, so temporarily put back non-accent characters. */
  ShortMonth[FRENCH][FEB][1] = 'E'; // accent aigu sur FEV.
  ShortMonth[FRENCH][AUG][2] = 'U'; // accent circonflexe sur AOU.
  ShortMonth[FRENCH][DEC][1] = 'E'; // accent aigu sur DEC.

  sprintf(String, "[%2.2u-%s-%2.2u%2.2u %2.2u:%2.2u:%2.2u] - ", CurrentDayOfMonth, ShortMonth[Language][CurrentMonth], YearCentile, CurrentYearLowPart, CurrentHourSetting, CurrentMinute, SecondCounter);

  /* Put back French accents when done. */
  ShortMonth[FRENCH][FEB][1] = (UCHAR)31; // accent aigu sur FEV.
  ShortMonth[FRENCH][AUG][2] = (UCHAR)30; // accent circonflexe sur AOU.
  ShortMonth[FRENCH][DEC][1] = (UCHAR)31; // accent aigu sur DEC.

  return;
}





/* $PAGE */
/* $TITLE=day_indicator() */
/* ------------------------------------------------------------------ *\
                       Turn On or Off day indicator.
\* ------------------------------------------------------------------ */
void day_indicator(UINT8 DayOfWeek, UINT8 Flag)
{
  if (Flag == FLAG_ON)
  {
    /* Turn On specified day-of-week indicator. */
    switch (DayOfWeek)
    {
      case (SUN):
      // turn ON  both LEDs of Sunday indicator
        DisplayBuffer[16] |= (1 << 5) | (1 << 6);
      break;

      case (MON):
        // turn ON  both LEDs of Monday indicator
        DisplayBuffer[0] |= (1 << 3) | (1 << 4);
      break;

      case (TUE):
        // turn ON  both LEDs of Tuesday indicator
        DisplayBuffer[0] |= (1 << 6) | (1 << 7);
      break;

      case (WED):
        // turn ON  both LEDs of Wednesday indicator
        DisplayBuffer[8] |= (1 << 1) | (1 << 2);
      break;

      case (THU):
        // turn ON  both LEDs of Thursday indicator
        DisplayBuffer[8] |= (1 << 4) | (1 << 5);
      break;

      case (FRI):
        // turn  ON both LEDs of Friday indicator
        DisplayBuffer[8]  |= (1 << 7);
        DisplayBuffer[16] |= (1 << 0);
      break;

      case (SAT):
        // turn ON  both LEDs of Saturday indicator
        DisplayBuffer[16] |= (1 << 2) | (1 << 3);
      break;

      case (ALL):
      default:
        /* For "ALL", or in case of error, turn on all day-of-week indicators. */
        // turn ON  both LEDs of Sunday indicator
        DisplayBuffer[16] |= (1 << 5) | (1 << 6);

        // turn ON  both LEDs of Monday indicator
        DisplayBuffer[0] |= (1 << 3) | (1 << 4);

        // turn ON  both LEDs of Tuesday indicator
        DisplayBuffer[0] |= (1 << 6) | (1 << 7);

        // turn ON  both LEDs of Wednesday indicator
        DisplayBuffer[8] |= (1 << 1) | (1 << 2);

        // turn ON  both LEDs of Thursday indicator
        DisplayBuffer[8] |= (1 << 4) | (1 << 5);

        // turn  ON both LEDs of Friday indicator
        DisplayBuffer[8]  |= (1 << 7);
        DisplayBuffer[16] |= (1 << 0);

        // turn ON  both LEDs of Saturday indicator
        DisplayBuffer[16] |= (1 << 2) | (1 << 3);
      break;
    }
  }
  else
  {
    /* Turn Off specified day-of-week indicator. */
    switch (DayOfWeek)
    {
      case (SUN):
        // turn OFF both LEDs of Sunday indicator
        DisplayBuffer[16] &= ~((1 << 5) | (1 << 6));
      break;

      case (MON):
        // turn OFF both LEDs of Monday indicator
        DisplayBuffer[0] &= ~((1 << 3) | (1 << 4));
      break;

      case (TUE):
        // turn OFF both LEDs of Tuesday indicator
        DisplayBuffer[0] &= ~((1 << 6) | (1 << 7));
      break;

      case (WED):
        // turn OFF both LEDs of Wednesday indicator
        DisplayBuffer[8] &= ~((1 << 1) | (1 << 2));
      break;

      case (THU):
        // turn OFF both LEDs of Thursday indicator
        DisplayBuffer[8] &= ~((1 << 4) | (1 << 5));
      break;

      case (FRI):
        // turn OFF both LEDs of Friday indicator
        DisplayBuffer[8] &= ~(1 << 7);
        DisplayBuffer[16] &= ~(1 << 0);
      break;

      case (SAT):
        // turn OFF both LEDs of Saturday indicator
        DisplayBuffer[16] &= ~((1 << 2) | (1 << 3));
      break;

      case (ALL):
        /* Turn Off all day-of-week indicators. */

        // turn Off both LEDs of Sunday indicator
        DisplayBuffer[16] &= ~((1 << 5) | (1 << 6));
      
        // turn Off both LEDs of Monday indicator
        DisplayBuffer[0] &= ~((1 << 3) | (1 << 4));
      
        // turn Off both LEDs of Tuesday indicator
        DisplayBuffer[0] &= ~((1 << 6) | (1 << 7));
      
        // turn Off both LEDs of Wednesday indicator
        DisplayBuffer[8] &= ~((1 << 1) | (1 << 2));
      
        // turn Off both LEDs of Thursday indicator
        DisplayBuffer[8] &= ~((1 << 4) | (1 << 5));
      
        // turn Off both LEDs of Friday indicator
        DisplayBuffer[8]  &= ~(1 << 7);
        DisplayBuffer[16] &= ~(1 << 0);
      
        // turn OFF both LEDs of Saturday indicator
        DisplayBuffer[16] &= ~((1 << 2) | (1 << 3));
      break;

      default:
        /* In case of error, turn On all indicators. */

        // turn On both LEDs of Sunday indicator
        DisplayBuffer[16] |= (1 << 5) | (1 << 6);
      
        // turn On both LEDs of Monday indicator
        DisplayBuffer[0] |= (1 << 3) | (1 << 4);
      
        // turn On both LEDs of Tuesday indicator
        DisplayBuffer[0] |= (1 << 6) | (1 << 7);
      
        // turn On both LEDs of Wednesday indicator
        DisplayBuffer[8] |= (1 << 1) | (1 << 2);
      
        // turn On both LEDs of Thursday indicator
        DisplayBuffer[8] |= (1 << 4) | (1 << 5);
      
        // turn On both LEDs of Friday indicator
        DisplayBuffer[8] |= (1 << 7);
        DisplayBuffer[16] |= (1 << 0);
      
        // turn On both LEDs of Saturday indicator
        DisplayBuffer[16] |= (1 << 2) | (1 << 3);
      break;
    }
  }
}





/* $PAGE */
/* $TITLE=display_data() */
/* ------------------------------------------------------------------ *\
                     Display data sent in argument.
\* ------------------------------------------------------------------ */
void display_data(UCHAR *Data, UINT32 Size)
{
  UCHAR String[256];

  UINT32 Loop1UInt32;
  UINT32 Loop2UInt32;


  sprintf(String, "Entering display_data(size: %u)\r", Size);
  uart_send(__LINE__, String);

  for (Loop1UInt32 = 0; Loop1UInt32 < Size; Loop1UInt32 += 16)
  {
    sprintf(String, "[%4.4X] ", Loop1UInt32);

    for (Loop2UInt32 = 0; Loop2UInt32 < 16; ++Loop2UInt32)
    {
      if ((Loop1UInt32 + Loop2UInt32) >= Size)
        sprintf(&String[strlen(String)], "   ");
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
      if ((Data[Loop1UInt32 + Loop2UInt32] >= 32) && (Data[Loop1UInt32 + Loop2UInt32] <= 126))
      {
        sprintf(&String[strlen(String)], "%c", Data[Loop1UInt32 + Loop2UInt32]);
      }
      else
      {
        sprintf(&String[strlen(String)], ".");
      }
    }
    sprintf(&String[strlen(String)], "\r");
    uart_send(__LINE__, String);

    // sleep_ms(50);  // let some time to flush UART buffer.
  }

  uart_send(__LINE__, "Exiting display_data()\r\r\r");

  return;
}





/* $PAGE */
/* $TITLE=display_voltage() */
/* ------------------------------------------------------------------ *\
         Display the power supply voltage for a few seconds.
      This function is only used in the test section for now,
    but it has been left in the code for an eventual use later...
\* ------------------------------------------------------------------ */
void display_voltage(void)
{
  UINT16 AdcValue;

  float Volts;


  CurrentClockMode = MODE_SHOW_VOLTAGE;

  /* gpio29 is used as adc to read power supply voltage. */
  adc_select_input(3);

  /* Read ADC converter raw value. */
  AdcValue = adc_read();

  /* Convert raw value to voltage value. */
  Volts = (3 * (AdcValue * 3.25f / 4096));

  UINT8 Single_digit = (int)Volts;
  UINT8 Decile = (int)(Volts * 10) % 10;
  UINT8 Percentile = (int)(Volts * 100) % 10;

  /* Display power supply voltage (2 digits after decimal). */
  fill_display_buffer_4X7(0, Single_digit + '0');
  fill_display_buffer_4X7(5, '.');
  fill_display_buffer_4X7(7, Decile + '0');
  fill_display_buffer_4X7(12, Percentile + '0');
  fill_display_buffer_4X7(17, 'V');

  return;
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
            if (FlashConfig.Alarm[AlarmNumber].Day & (1 << (Loop1UInt8 - 1)))
              day_indicator(Loop1UInt8, FLAG_ON);
            else
              day_indicator(Loop1UInt8, FLAG_OFF);
          }

          /* ...and blink day-of-week indicator for the one hilight for selection. */
          if (FlagBlinking[SetupStep])
          {
            /* Turn on day-of-week-indicator ("blink On"). */
            day_indicator(AlarmTargetDay, FLAG_ON);
          }
          else
          {
            /* Turn off day-of-week-indicator ("blink Off"). */
            day_indicator(AlarmTargetDay, FLAG_OFF);
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

      if (SecondCounter < 20)
      {
        /* From 0 to 19 seconds, blink the top "middle dot". */
        DisplayBuffer[11] &= 0xEF; // slim ":" - top dot only.

        /* Make sure the other dot (bottom dot) is on. */
        DisplayBuffer[13] |= 0x10; // slim ":" - bottom dot only.
      }
      else if (SecondCounter < 40)
      {
        /* From 20 to 39 seconds, blink the bottom "middle dot". */
        DisplayBuffer[13] &= 0xEF; // slim ":" - bottom dot only.

        /* Make sure the other dot (top dot) is on. */
        DisplayBuffer[11] |= 0x10; // slim ":" - top dot only.
      }
      else
      {
        /* From 40 to 59 seconds, blink both "middle dots". */
        DisplayBuffer[11] &= 0xEF; // slim ":" - top dot only.
        DisplayBuffer[13] &= 0xEF; // slim ":" - bottom dot only.
      }
    }

    /* 1000 means "redraw the target dot(s)". */
    if (DotBlinkCount == 1000)
    {
      /* We change the status of blinking double-dots, allow "display seconds" to be updated
         on display at the same time if it is active (to prevent a glitch in display update). */
      sem_release(&SemSync);

      if (SecondCounter < 20)
      {
        /* From 0 to 19 seconds, blink the top "middle dot". */
        DisplayBuffer[11] |= 0x10; // slim ":" - top dot only.
      }
      else if (SecondCounter < 40)
      {
        /* From 20 to 39 seconds, blink the bottom "middle dot". */
        DisplayBuffer[13] |= 0x10; // slim ":" - bottom dot only.
      }
      else
      {
        /* From 40 to 59 seconds, blink both "middle dot". */
        DisplayBuffer[11] |= 0x10; // slim ":" - top dot.
        DisplayBuffer[13] |= 0x10; // slim ":" - bottom dot.
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
  /* Check if there is a scroll going on. */
  if (FlagScrollStart == FLAG_ON)
  {
    /* Scroll one dot to the left every "SCROLL_DOT_TIME" milliseconds (typically from 100 to 150 milli-seconds). */
    ++ScrollStartCount;
  }

  /* Check if we reached the delay before scrolling one more dot to the left. */
  if (ScrollStartCount == SCROLL_DOT_TIME)
  {
    /* Check if there are some more columns to scroll in the display buffer. */
    if (ScrollDotCount > 0)
    {
      /* There are more characters to scroll on the display. */
      scroll_one_dot(); // scroll one more dot to the left on clock display.
      --ScrollDotCount; // one less dot to be scrolled on the display.
    }
    else
    {
      fill_display_buffer_4X7(24, ' '); // blank the first "invisible column" at the right of the display buffer when done.
      FlagScrollStart = FLAG_OFF;       // reset scroll start flag when done.
      FlagUpdateTime = FLAG_ON;         // request a time update on the clock.
    }

    ScrollStartCount = 0; // reset the millisecond counter to wait for next dot scroll to the left.
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
  UINT8 ColumnPosition;
  UINT8 RowNumber;
  UINT8 SectionNumber;

  UINT16 TableOffset;
  UINT16 Width;


  /* If column number passed as an argument is out-of-bound, return DISPLAY_BUFFER_SIZE. */
  if (ColumnPosition > DISPLAY_BUFFER_SIZE)
    return DISPLAY_BUFFER_SIZE;

  /* First 2 columns are reserved for indicators. */
  if (Column < 2)
    Column = 2;

  SectionNumber = Column / 8;
  ColumnInSection = Column % 8;

  /* Special handling of character 0x00 used to blink characters on the display. */
  if (AsciiCharacter == 0x00)
    AsciiCharacter = 32; // blank ("space") character.

  /* Determine the digit bitmap corresponding to the character we want to display. */
  if ((AsciiCharacter >= 0x1E) && (AsciiCharacter <= 0x81))
  {
    TableOffset = (UINT16)((AsciiCharacter - 0x1E) * 7);
    Width = CharWidth[AsciiCharacter - 0x1E];
  }
  else
  {
    /* If ASCII character is out-of-bound, replace it with "?". */
    TableOffset = (UINT16)((0x3F - 0x1E) * 7);
    Width = CharWidth[AsciiCharacter - 0x3F];
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
  {
    sprintf(String, "CRC16 computed for current active clock configuration: 0x%X (%u)\r", Crc16, Crc16);
    uart_send(__LINE__, String);
  }

  /* We know that FlashConfig.Crc16 corresponds to the crc16 that has been read the last time the configuration was restored (or saved) from (to) flash.
     So, check if the current active configuration's CRC16 is still the same as the last one read (or saved)
     (in which case it means that current configuration has not changed). */
  if (FlashConfig.Crc16 == Crc16)
  {
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Current active configuration is the same as the last one restored from flash.\r\r\r");
    }

    return;
  }
  else
  {
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Current active configuration is different than flash.\r");
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
  {
    uart_send(__LINE__, "Entering flash_display()\r");

    sprintf(String, "XIP_BASE: 0x%p   Offset: 0x%6.6X   Length: 0x%X (%u)\r", XIP_BASE, Offset, Length, Length);
    uart_send(__LINE__, String);
  }

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
      if ((FlashBaseAddress[Loop1UInt32 + Loop2UInt32] >= 32) && (FlashBaseAddress[Loop1UInt32 + Loop2UInt32] <= 126))
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
  {
    uart_send(__LINE__, "Exiting flash_display()\r\r\r");
  }

  return;
}





/* $PAGE */
/* $TITLE=flash_display_config() */
/* ------------------------------------------------------------------ *\
          Display Green Clock configuration from flash memory.
\* ------------------------------------------------------------------ */
UINT8 flash_display_config(void)
{
  UCHAR DayMask[16];
  UCHAR String[256];

  UINT8 Dum1UInt8;
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;


  uart_send(__LINE__, "Entering flash_display_config()\r");

  sprintf(String, "NOTE: FLAG_OFF = 0x%2.2X   FLAG_ON = 0x%2.2X\r", FLAG_OFF, FLAG_ON);
  uart_send(__LINE__, String);

  sprintf(String, "NOTE: sizeof(FlashConfig):          0x%3X (%3u)\r", sizeof(FlashConfig), sizeof(FlashConfig));
  uart_send(__LINE__, String);

  sprintf(String, "NOTE: sizeof(FlashConfig.Reserved1): 0x%2X (%3u)\r", sizeof(FlashConfig.Reserved1), sizeof(FlashConfig.Reserved1));
  uart_send(__LINE__, String);

  sprintf(String, "NOTE: sizeof(FlashConfig.Reserved2): 0x%2X (%3u)\r\r", sizeof(FlashConfig.Reserved2), sizeof(FlashConfig.Reserved2));
  uart_send(__LINE__, String);

  sprintf(String, "[%X] Firmware version:         %c%c%c%c\r", &FlashConfig.Version, FlashConfig.Version[0], FlashConfig.Version[1], FlashConfig.Version[2], FlashConfig.Version[3]);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] CurrentYearCentile:        %3u\r", &FlashConfig.CurrentYearCentile, FlashConfig.CurrentYearCentile);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] Language:                  %3u     (01 = Eng   02 = Fre  03 = Spa - not implemented)\r", &FlashConfig.Language, FlashConfig.Language);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] DaylightSavingTime:        %3u     ( 0 = No support       1 = North America)\r", &FlashConfig.DaylightSavingTime, FlashConfig.DaylightSavingTime);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] TemperatureUnit:          0x%2.2X     (00 = Celsius   01 = Fahrenheit)\r", &FlashConfig.TemperatureUnit, FlashConfig.TemperatureUnit);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] TimeDisplayMode:          0x%2.2X     (00 = 12Hours   FF = 24Hours)\r", &FlashConfig.TimeDisplayMode, FlashConfig.TimeDisplayMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeMode:                 %3u     (00 = Off   01 = On   02 = On, Intermittent)\r", &FlashConfig.ChimeMode, FlashConfig.ChimeMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeTimeOn:               %3u\r", &FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOn);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeTimeOff:              %3u\r", &FlashConfig.ChimeTimeOff, FlashConfig.ChimeTimeOff);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightMode:            %3u     (00 = Off   01 = On   02 = Night   03 = Auto)\r", &FlashConfig.NightLightMode, FlashConfig.NightLightMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightTimeOn:          %3u\r", &FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOn);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightTimeOff:         %3u\r", &FlashConfig.NightLightTimeOff, FlashConfig.NightLightTimeOff);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagAutoBrightness:       0x%2.2X     (00 = Off   FF = On)\r", &FlashConfig.FlagAutoBrightness, FlashConfig.FlagAutoBrightness);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagKeyclick:             0x%2.2X     (00 = Off   FF = On)\r", &FlashConfig.FlagKeyclick, FlashConfig.FlagKeyclick);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagScrollEnable:         0x%2.2X     (00 = Off   FF = On)\r", &FlashConfig.FlagScrollEnable, FlashConfig.FlagScrollEnable);
  uart_send(__LINE__, String);

  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved1); ++Loop1UInt16)
  {
    sprintf(String, "[%X] Reserved1[%3.3u]:           0x%2.2X\r", &FlashConfig.Reserved1[Loop1UInt16], Loop1UInt16, FlashConfig.Reserved1[Loop1UInt16]);
    uart_send(__LINE__, String);
  }

  sprintf(String, "Bit mask used for weekday selection:\r");
  uart_send(__LINE__, String);

  /* Display all 7 day-of-week. */
  for (Loop1UInt16 = 1; Loop1UInt16 < 8; ++Loop1UInt16)
  {
    sprintf(String, "                                  ");
    /* Display binary bitmap. */
    for (Loop2UInt16 = 7; Loop2UInt16 > 0; --Loop2UInt16)
    {
      if (Loop1UInt16 == Loop2UInt16)
        sprintf(&String[strlen(String)], "1");
      else
        sprintf(&String[strlen(String)], "0");
    }
    sprintf(&String[strlen(String)], "   %s\r", DayName[FlashConfig.Language][Loop1UInt16]);
    uart_send(__LINE__, String);
  }

  /* Scan all alarms (0 to 8). */
  for (Loop1UInt16 = 0; Loop1UInt16 < 9; ++Loop1UInt16)
  {
    sprintf(String, "[%X] Alarm[%2.2u].Status:         0x%2.2X     (00 = Off   FF = On)\r", &FlashConfig.Alarm[Loop1UInt16].FlagStatus, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].FlagStatus);
    uart_send(__LINE__, String);

    sprintf(String, "[%X] Alarm[%2.2u].Hour:            %3u\r", &FlashConfig.Alarm[Loop1UInt16].Hour, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].Hour);
    uart_send(__LINE__, String);

    sprintf(String, "[%X] Alarm[%2.2u].Minute:          %3u\r", &FlashConfig.Alarm[Loop1UInt16].Minute, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].Minute);
    uart_send(__LINE__, String);

    sprintf(String, "[%X] Alarm[%2.2u].Second:          %3u\r", &FlashConfig.Alarm[Loop1UInt16].Second, Loop1UInt16, FlashConfig.Alarm[Loop1UInt16].Second);
    uart_send(__LINE__, String);

    /* Identify all day-of-week targets for each alarm. */
    DayMask[0] = 0x00; // initialize string as null.
    for (Loop2UInt16 = 7; Loop2UInt16 > 0; --Loop2UInt16)
    {
      if (FlashConfig.Alarm[Loop1UInt16].Day & (1 << (Loop2UInt16 - 1)))
        sprintf(&DayMask[strlen(DayMask)], "1");
      else
        sprintf(&DayMask[strlen(DayMask)], "0");
    }
    sprintf(String, "[%X] Alarm[%2.2u].DayMask:     %s     (%X) ", &FlashConfig.Alarm[Loop1UInt16].Day, Loop1UInt16, DayMask, FlashConfig.Alarm[Loop1UInt16].Day);

    for (Loop2UInt16 = 1; Loop2UInt16 < 8; ++Loop2UInt16)
    {
      if (FlashConfig.Alarm[Loop1UInt16].Day & (1 << (Loop2UInt16 - 1)))
      {
        Dum1UInt8 = strlen(String);
        sprintf(&String[strlen(String)], "%s", DayName[FlashConfig.Language][Loop2UInt16]);
        String[Dum1UInt8 + 3] = 0x20; // space separator.
        String[Dum1UInt8 + 4] = 0x00; // keep first 3 characters of day name.
      }
    }

    sprintf(&String[strlen(String)], "\r");
    uart_send(__LINE__, String);
  }

  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved2); ++Loop1UInt16)
  {
    sprintf(String, "[%X] Reserved2[%3.3u]:           0x%2.2X\r", &FlashConfig.Reserved2[Loop1UInt16], Loop1UInt16, FlashConfig.Reserved2[Loop1UInt16]);
    uart_send(__LINE__, String);
  }

  sprintf(String, "[%X] Crc16:                  0x%4.4X\r\r\r", &FlashConfig.Crc16, FlashConfig.Crc16);
  uart_send(__LINE__, String);

  sprintf(String, "Size of data for Crc16:  %u -  %u = %u\r", &FlashConfig.Crc16, &FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  uart_send(__LINE__, String);

  sprintf(String, "               (in hex: 0x%8.8X - 0x%8.8X = 0x%8.8X)\r\r\r", &FlashConfig.Crc16, &FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  uart_send(__LINE__, String);

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
       will be erased at a time, beginning at the specified offset,
       which must be aligned on a sector boundary (4096).
\* ------------------------------------------------------------------ */
void flash_erase(UINT32 FlashMemoryOffset)
{
  UCHAR String[256];

  UINT32 InterruptMask;
  UINT16 Loop1UInt16;


  if (DebugBitMask & DEBUG_FLASH)
  {
    sprintf(String, "Entering flash_erase(offset: 0x%8.8X)\r", FlashMemoryOffset);
    uart_send(__LINE__, String);
  }

  /* Erase an area of the Pico's flash memory.
     NOTES:
     FlashMemoryOffset is the offset in flash memory (relative to XIP_BASE) and must be aligned on a sector boundary (a multiple of 4096).
     The second parameter must be a multiple of a sector size (a multiple of 4096). */
  if (FlashMemoryOffset % FLASH_SECTOR_SIZE)
  {
    if (DebugBitMask & DEBUG_FLASH)
    {
      sprintf(String, "Offset specified for flash_erase(0x%8.8X) is not aligned on a sector boundary (multiple of 4096)\r", FlashMemoryOffset);
      uart_send(__LINE__, String);
    }

    while (FlashMemoryOffset % FLASH_SECTOR_SIZE)
    {
      ++FlashMemoryOffset;
    }

    if (DebugBitMask & DEBUG_FLASH)
    {
      sprintf(String, "Offset has been shifted to 0x%8.8X\r", FlashMemoryOffset);
      uart_send(__LINE__, String);
    }
  }

  /* Housekeeping before disabling interrupts to prevent garbage on clock display. */
  clear_framebuffer(0);
  cancel_repeating_timer(&Timer50uSec);
  OE_DISABLE;

  /* Keep track of interrupt mask on entry. */
  InterruptMask = save_and_disable_interrupts();

  /* Erase flash area to reprogram. */
  flash_range_erase(FlashMemoryOffset, FLASH_SECTOR_SIZE);

  /* Restore original interrupt mask when done. */
  restore_interrupts(InterruptMask);

  /* ................................................................ *\
        Restore display after flash memory has been programmed.
  \* ................................................................ */
  add_repeating_timer_us(50, timer_callback_us, NULL, &Timer50uSec);

  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Exiting  flash_erase()\r\r\r");
  }

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
    sprintf(String, "Entering flash_read_config()\r");
    uart_send(__LINE__, String);

    sprintf(String, "sizeof(FlashConfig): 0x%X (%u)\r", sizeof(FlashConfig), sizeof(FlashConfig));
    uart_send(__LINE__, String);
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
    uart_send(__LINE__, "Display configuration retrieved from flash memory:\r");
    flash_display_config();
  }

  /* Validate CRC16 read from flash. */
  Dum1UInt16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);

  if (DebugBitMask & DEBUG_FLASH)
  {
    sprintf(String, "CRC16 saved in flash configuration: 0x%X (%u)\r", FlashConfig.Crc16, FlashConfig.Crc16);
    uart_send(__LINE__, String);

    sprintf(String, "CRC16 computed from data read:      0x%X (%u)\r", Dum1UInt16, Dum1UInt16);
    uart_send(__LINE__, String);
  }

  /* If CRC16 read from flash does not correspond to the data read, assign default values and save new configuration. */
  if (FlashConfig.Crc16 == Dum1UInt16)
  {
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "Flash configuration is valid.\r\r\r");
    }

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
  sprintf(FlashConfig.Version, "%s", FIRMWARE_VERSION);  // firmware version number.
  FlashConfig.CurrentYearCentile = 20;                   // assume we are in years 20xx. Green Clock was always reverting to 20.
  FlashConfig.Language = DEFAULT_LANGUAGE;               // hourly chime will begin at this hour.
  FlashConfig.DaylightSavingTime = DAYLIGHT_SAVING_TIME; // specifies how to handle the daylight saving time (see clock options above).
  FlashConfig.TemperatureUnit = TEMPERATURE_DEFAULT;     // CELSIUS or FAHRENHEIT default value (see clock options above).
  FlashConfig.TimeDisplayMode = TIME_DISPLAY_DEFAULT;    // H24 or H12 default value (see clock options above).
  FlashConfig.ChimeMode = CHIME_DEFAULT;                 // chime mode (Off / On / Day).
  FlashConfig.ChimeTimeOn = CHIME_TIME_ON;               // hourly chime will begin at this hour.
  FlashConfig.ChimeTimeOff = CHIME_TIME_OFF;             // hourly chime will begin at this hour.
  FlashConfig.NightLightMode = NIGHT_LIGHT_DEFAULT;      // night light mode (On / Off / Auto / Night).
  FlashConfig.NightLightTimeOn = NIGHT_LIGHT_TIME_ON;    // default night light time on.
  FlashConfig.NightLightTimeOff = NIGHT_LIGHT_TIME_OFF;  // default night light time off.
  FlashConfig.FlagAutoBrightness = FLAG_ON;              // flag indicating we are in "Auto Brightness" mode.
  FlashConfig.FlagKeyclick = FLAG_ON;                    // flag for keyclick ("button-press" tone)
  FlashConfig.FlagScrollEnable = SCROLL_DEFAULT;         // flag indicating the clock will scroll the date and temperature at regular intervals on the display.

  /* Make provosion for future parameters. */
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(FlashConfig.Reserved1); ++Loop1UInt16)
  {
    FlashConfig.Reserved1[Loop1UInt16] = 0xFF;
  }

  /* Default configuration for 9 alarms. Text may be changed for another 40-characters max string. */

  FlashConfig.Alarm[0].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[0].Day = (1 << (MON - 1)) + (1 << (TUE - 1)) + (1 << (WED - 1)) + (1 << (THU - 1)) + (1 << (FRI - 1));
  FlashConfig.Alarm[0].Hour = 8;
  FlashConfig.Alarm[0].Minute = 00;
  FlashConfig.Alarm[0].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[0].Text, "Alarm 1"); // string to be scrolled when alarm 1 is triggered (customize).

  FlashConfig.Alarm[1].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[1].Day = (1 << (SAT - 1)) + (1 << (SUN - 1));
  FlashConfig.Alarm[1].Hour = 14;
  FlashConfig.Alarm[1].Minute = 37;
  FlashConfig.Alarm[1].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[1].Text, "Alarm 2"); // string to be scrolled when alarm 2 is triggered (customize).

  FlashConfig.Alarm[2].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[2].Day = (1 << (MON - 1));
  FlashConfig.Alarm[2].Hour = 14;
  FlashConfig.Alarm[2].Minute = 36;
  FlashConfig.Alarm[2].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[2].Text, "Alarm 3"); // string to be scrolled when alarm 3 is triggered (customize).

  FlashConfig.Alarm[3].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[3].Day = (1 << (TUE - 1));
  FlashConfig.Alarm[3].Hour = 14;
  FlashConfig.Alarm[3].Minute = 35;
  FlashConfig.Alarm[3].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[3].Text, "Alarm 4"); // string to be scrolled when alarm 4 is triggered (customize).

  FlashConfig.Alarm[4].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[4].Day = (1 << (WED - 1));
  FlashConfig.Alarm[4].Hour = 14;
  FlashConfig.Alarm[4].Minute = 34;
  FlashConfig.Alarm[4].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[4].Text, "Alarm 5"); // string to be scrolled when alarm 5 is triggered (customize).

  FlashConfig.Alarm[5].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[5].Day = (1 << (THU - 1));
  FlashConfig.Alarm[5].Hour = 14;
  FlashConfig.Alarm[5].Minute = 33;
  FlashConfig.Alarm[5].Second = 29;
  sprintf(FlashConfig.Alarm[5].Text, "Alarm 6"); // string to be scrolled when alarm 6 is triggered (customize).

  FlashConfig.Alarm[6].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[6].Day = (1 << (FRI - 1));
  FlashConfig.Alarm[6].Hour = 14;
  FlashConfig.Alarm[6].Minute = 32;
  FlashConfig.Alarm[6].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[6].Text, "Alarm 7"); // string to be scrolled when alarm 7 is triggered (customize).

  FlashConfig.Alarm[7].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[7].Day = (1 << (SAT - 1));
  FlashConfig.Alarm[7].Hour = 14;
  FlashConfig.Alarm[7].Minute = 31;
  FlashConfig.Alarm[7].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[7].Text, "Alarm 8"); // string to be scrolled when alarm 8 is triggered (customize).

  FlashConfig.Alarm[8].FlagStatus = FLAG_OFF; // all alarms set to Off in default configuration.
  FlashConfig.Alarm[8].Day = (1 << (SUN - 1));
  FlashConfig.Alarm[8].Hour = 14;
  FlashConfig.Alarm[8].Minute = 30;
  FlashConfig.Alarm[8].Second = 29;              // not used for now... 29 is hardcoded in source code to offload the clock during busy periods.
  sprintf(FlashConfig.Alarm[8].Text, "Alarm 9"); // string to be scrolled when alarm 9 is triggered (customize).

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
    uart_send(__LINE__, "Save current Green Clock configuration to flash.\r");

    sprintf(String, "Pointer to FlashConfig.Version: %X\r", FlashConfig.Version);
    uart_send(__LINE__, String);

    sprintf(String, "Pointer to FlashConfig.Crc16:   %X\r", &FlashConfig.Crc16);
    uart_send(__LINE__, String);

    sprintf(String, "&FlashConfig.Crc16 - &FlashConfig.Version: %X (%u)\r\r\r", (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
    uart_send(__LINE__, String);

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

  UINT8 *FlashBaseAddress;

  UINT16 Loop1UInt16;

  UINT32 SectorOffset;
  UINT32 InterruptMask;


  /* NOTE: DEBUG_FLASH will work fine while writing to flash before starting the callback_s function.
           If flash_write() is called by callback_s while DEBUG_FLASH is On, garbage will show up on the screen and
           firmware will crash. Logging must be kept to a minimum if logging is to be used inside flash_write() while
           called by callback_s. */
  if (DebugBitMask & DEBUG_FLASH)
  {
    sprintf(String, "Entering flash_write()   Data offset: 0x%X   Data size: 0x%X (%u)\r", NewDataOffset, NewDataSize, NewDataSize);
    uart_send(__LINE__, String);

    uart_send(__LINE__, "Display the data to write to flash.\r");
    display_data(NewData, NewDataSize);
  }

  SectorOffset = NewDataOffset; // assume offset specified is on a sector boundary.
  NewDataOffset = 0;            // assume that new data will be at the beginning of sector.
  if (SectorOffset % FLASH_SECTOR_SIZE)
  {
    /* Offset specified is not aligned on a sector boundary. */
    if (DebugBitMask & DEBUG_FLASH)
    {
      sprintf(String, "New data offset specified (0x%6.6X) is not aligned on a sector boundary (multiple of 4096)\r", SectorOffset);
      uart_send(__LINE__, String);

      sprintf(String, "Phased out by 0x%X (%u) bytes.\r", SectorOffset % FLASH_SECTOR_SIZE, SectorOffset % FLASH_SECTOR_SIZE);
      uart_send(__LINE__, String);
    }

    while (SectorOffset % FLASH_SECTOR_SIZE)
    {
      --SectorOffset;
      ++NewDataOffset;
    }

    if (DebugBitMask & DEBUG_FLASH)
    {
      sprintf(String, "Sector offset has been shifted down to 0x%6.6X\r", SectorOffset);
      uart_send(__LINE__, String);

      sprintf(String, "And new data offset has been shifted up to 0x%X (%u)\r", NewDataOffset, NewDataOffset);
      uart_send(__LINE__, String);
    }
  }

  if (NewDataOffset + NewDataSize > 4096)
  {
    if (DebugBitMask & DEBUG_FLASH)
    {
      uart_send(__LINE__, "The arguments given cross a sector boundary which is not allowed...\r");

      sprintf(String, "Sector offset: %X   New data offset: %X  New data size: %X\r\r\r", SectorOffset, NewDataOffset, NewDataSize);
      uart_send(__LINE__, String);
    }

    return 1;
  }

  /* A wear leveling algorithm has not been implemented since the flash usage for configuration parameters doesn't require it.
     However, flash write should not be use for intensive data logging without adding a wear leveling algorithm. */
  FlashBaseAddress = (UINT8 *)(XIP_BASE);

  if (DebugBitMask & DEBUG_FLASH)
  {
    sprintf(String, "FlashBaseAddress: %p   Sector offset: %X   Target offset in sector: %X\r", FlashBaseAddress, SectorOffset, NewDataOffset);
    uart_send(__LINE__, String);
  }

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
    sprintf(String, "Data to be written to flash offset %X:\r", SectorOffset);
    uart_send(__LINE__, String);
    display_data(FlashData, FLASH_SECTOR_SIZE);
  }

  /* Erase flash before reprogramming. */
  flash_erase(SectorOffset);

  /* Housekeeping before disabling interrupts to prevent garbage on clock display. */
  clear_framebuffer(0);
  cancel_repeating_timer(&Timer50uSec);
  OE_DISABLE;

  /* Keep track of interrupt mask on entry and disable interrupts during flash writing. */
  InterruptMask = save_and_disable_interrupts();

  /* Save data to flash memory. */
  flash_range_program(SectorOffset, FlashData, FLASH_SECTOR_SIZE);

  /* Restore original interrupt mask when done. */
  restore_interrupts(InterruptMask);

  /* ................................................................ *\
       Restore clock display after flash memory has been programmed.
  \* ................................................................ */
  add_repeating_timer_us(50, timer_callback_us, NULL, &Timer50uSec);

  if (DebugBitMask & DEBUG_FLASH)
  {
    uart_send(__LINE__, "Exiting flash_write()\r\r\r");
  }

  return 0;
}





/* $PAGE */
/* $TITLE=get_ads1015() */
/* ------------------------------------------------------------------ *\
              Read analog-to-digital gpio for ambient light.
\* ------------------------------------------------------------------ */
UINT16 get_ads1015(void)
{
  UINT16 Value;


  /* Select ADC input 0 (gpio 26) for reading ambient light. */
  adc_select_input(0);

  /* Read gpio 26. */
  Value = adc_read();

  /* The way the photoresistor is installed in the circuit: the higher the number, the darker it is, which is counter-intuitive...
     ADC precision on the Pico is 12 bits (0 to 4095). Let's inverse the value so that an higher value means more light... */
  Value = 4095 - Value;

  return Value;
}





/* $PAGE */
/* $TITLE=get_ambient_temperature() */
/* ------------------------------------------------------------------ *\
      Get temperature from DS3231 RTC IC using I2C communication.
\* ------------------------------------------------------------------ */
void get_ambient_temperature(void)
{
  UCHAR start_tran[2] = {0x0E, 0x20};


  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, start_tran, 2, FALSE);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &GetAddHigh, 1, TRUE);
  i2c_read_blocking(I2C_PORT, DS3231_ADDRESS, &TemperaturePart1, 1, FALSE);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &GetAddLow, 1, TRUE);
  i2c_read_blocking(I2C_PORT, DS3231_ADDRESS, &TemperaturePart2, 1, FALSE);
  TemperaturePart2 = (TemperaturePart2 >> 6) * 25;

  return;
}





/* $PAGE */
/* $TITLE=get_date_string() */
/* ------------------------------------------------------------------ *\
                Build the string containing the date
                  to be scrolled on clock display.
\* ------------------------------------------------------------------ */
void get_date_string(UCHAR *String)
{
  UCHAR Suffix[3];

  UINT8 DumDayOfMonth;
  UINT8 DumMonth;
  UINT8 TemperatureF;

  UINT16 Loop1UInt16;


  /* Wipe string on entry. */
  for (Loop1UInt16 = 0; Loop1UInt16 < sizeof(String); ++Loop1UInt16)
    String[Loop1UInt16] = 0x00;

  /* Read the real-time clock IC DS3231. */
  Time_RTC = Read_RTC();
  CurrentDayOfWeek = bcd_to_byte(Time_RTC.dayofweek) + 1;
  if (CurrentDayOfWeek == 8)
    CurrentDayOfWeek = 1; // Sunday equals 1.
  DumDayOfMonth = bcd_to_byte(Time_RTC.dayofmonth);
  DumMonth = bcd_to_byte(Time_RTC.month);

  if (FlashConfig.Language == ENGLISH)
  {
    /* Day-of-week and month name. */
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

  if (FlashConfig.Language == FRENCH)
  {
    /* Day-of-week name. */
    sprintf(String, "%s ", DayName[FlashConfig.Language][CurrentDayOfWeek]);

    /* Add Day-of-month. */
    if (Time_RTC.dayofmonth == 1)
      sprintf(&String[strlen(String)], "%Xer ", Time_RTC.dayofmonth); // premier du mois.
    else
      sprintf(&String[strlen(String)], "%X ", Time_RTC.dayofmonth); // autres dates.

    /* Add month name. */
    sprintf(&String[strlen(String)], "%s ", MonthName[FlashConfig.Language][DumMonth]);

    /* Add 4-digits year. */
    sprintf(&String[strlen(String)], " %2.2u%2.2u ", FlashConfig.CurrentYearCentile, CurrentYearLowPart);
  }

  return;
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
/* $TITLE=get_pico_temperature() */
/* ------------------------------------------------------------------ *\
         Get temperature from analog-to-digital Pico's gpio.
                  This needs to be investigated.
           There is something wrong with this function.
\* ------------------------------------------------------------------ */
void get_pico_temperature(UCHAR *String)
{
  UINT8 Dum1UInt8;

  UINT16 AdcValue;

  float AdcVolts;


  adc_set_temp_sensor_enabled(1);

  /* Select Pico temperature to be internally connected to ADC. */
  adc_select_input(4);

  /* Get ADC raw value. */
  AdcValue = adc_read();

  /* Convert ADC raw value to volts. Reference voltage (3.28 volts) should be measured if temperature seems erroneous. */
  AdcVolts = (AdcValue * 3.28f) / 4096;

  /* Generate string to be scrolled... while evaluating Pico internal temperature from voltage value.
     NOTE: 27'C gives a voltage of 0.706 volt and each additional 'C reduces the voltage by 0.001721 volt (1.721 mV). */
  if (FlashConfig.TemperatureUnit == CELSIUS)
  {
    /* ...in Celsius. */
    sprintf(String, "Pico temp: %2.2f%cC", (27 - ((AdcVolts - 0.706) / 0.001721)), 0x80);
  }
  else
  {
    /* ...or in Fahrenheit. */
    sprintf(String, "Pico temp: %2.2f%cF", (((27 - ((AdcVolts - 0.706) / 0.001721)) * 9 / 5) + 32), 0x80);
  }

  return;
}





/* $PAGE */
/* $TITLE=get_voltage() */
/* ------------------------------------------------------------------ *\
      Get power supply voltage from analog-to-digital Pico's gpio.
      - VREF reading is 3.22.
      - ADC precision is 12 bits.
      - Divider gives 1/3rd of power supply voltage.
\* ------------------------------------------------------------------ */
void get_voltage(UCHAR *Voltage)
{
  UINT16 AdcValue;

  float Volts;


  /* gpio29 is used as adc to read power supply voltage. */
  adc_select_input(3);

  /* Read ADC converter raw value. */
  AdcValue = adc_read();

  Volts = (3 * (AdcValue * 3.25f / 4096));

  sprintf(Voltage, "%2.2f Volts", Volts);

  return;
}





/* $PAGE */
/* $TITLE=get_weekday() */
/* ------------------------------------------------------------------ *\
                       Return the day-of-week,
               given the day-of-month, month and year
\* ------------------------------------------------------------------ */
UINT8 get_weekday(UINT16 year_cnt, UINT8 month_cnt, UINT8 date_cnt)
{
  UINT8 weekday = 8;


  if (month_cnt == 1 || month_cnt == 2)
  {
    month_cnt += 12;
    --year_cnt;
  }

  weekday = (date_cnt + 1 + 2 * month_cnt + 3 * (month_cnt + 1) / 5 + year_cnt + year_cnt / 4 - year_cnt / 100 + year_cnt / 400) % 7;

  switch (weekday)
  {
    case 0:
      return 7;
    break;
  
    case 1:
      return 1;
    break;
  
    case 2:
      return 2;
    break;
  
    case 3:
      return 3;
    break;
  
    case 4:
      return 4;
    break;
  
    case 5:
      return 5;
    break;
  
    case 6:
      return 6;
    break;
  }
}





/* $PAGE */
/* $TITLE=init_gpio() */
/* ------------------------------------------------------------------ *\
              GPIO ports initialization / configuration

              GPIO  0 - Not used.
              GPIO  1 - Not used.
              GPIO  2 - (In) Top button.
              GPIO  3 - (In) SQW (DS3231 RTC IC).
              GPIO  4 - UART1 - TX.
              GPIO  5 - UART1 - RX.
              GPIO  6 - (I2C) SDA (Temperature reading).
              GPIO  7 - (I2C) SCL (Temperature reading).
              GPIO  8 - (In) DHT22 (must be added by user... not on the original Green Clock).
              GPIO  9 - (In) Infrared phototransistor to receive remote control commands (must be added by user).
              GPIO 10 - (Out) CLK (LED matrix controller).
              GPIO 11 - (Out) SDI (LED matrix controller).
              GPIO 12 - (Out) LE.
              GPIO 13 - (Out) OE.
              GPIO 14 - (Out) Active piezo / buzzer (included in Green clock).
              GPIO 15 - (In) Down button.
              GPIO 16 - (Out) A0 (LED matrix brightness control).
              GPIO 17 - (In) Up button.
              GPIO 18 - (Out) A1 (LED matrix brightness control).
              GPIO 19 - (Out) Passive piezo / buzzer (must be added by user).
              GPIO 20 - Not used.
              GPIO 21 - Not used.
              GPIO 22 - (Out) A2 (LED matrix brightness control).
              GPIO 23 - Used internally for voltage regulation.
              GPIO 24 - Used internally for voltage detection.
              GPIO 25 - On-board Pico LED.
              GPIO 26 - ADC0 (Ambient light reading).
              GPIO 27 - Not used.
              GPIO 28 - Not used.
              GPIO 29 - ADC-Vref (Power supply reading).

\* ------------------------------------------------------------------ */
int init_gpio(void)
{
  stdio_init_all();

  /* Refer to the GPIO table above to know which GPIO is used for what. */
  gpio_init(A0);
  gpio_init(A1);
  gpio_init(A2);

  gpio_init(CLK);
  gpio_init(LE);
  gpio_init(OE);
  gpio_init(SDI);
  gpio_init(SQW);

  gpio_init(IR_RX);

  gpio_init(SET_BUTTON);
  gpio_init(UP_BUTTON);
  gpio_init(DOWN_BUTTON);

  /* Configure the GPIO associated to the three push-buttons of the clock in "input" mode. */
  gpio_set_dir(SET_BUTTON, GPIO_IN);
  gpio_set_dir(UP_BUTTON, GPIO_IN);
  gpio_set_dir(DOWN_BUTTON, GPIO_IN);

  /* Setup a pull-up on those three GPIOs. */
  gpio_pull_up(SET_BUTTON);
  gpio_pull_up(UP_BUTTON);
  gpio_pull_up(DOWN_BUTTON);

  /* Initialize UART1 used to send information to a VT-101 type monitor mainly for debugging purposes. */
  uart_init(uart0, 921600); // 38400

  // gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);  // when using UART0, RX_PIN (pin #2) is used by the Set button.
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

  /* If "NO_SOUND" is defined, absolutely no sound will be made by the clock. */
  #ifndef NO_SOUND
  /* Original Green clock active piezo. */
  gpio_init(BUZZ);
  gpio_set_dir(BUZZ, GPIO_OUT);

  #ifdef PASSIVE_PIEZO_SUPPORT // Optional passive piezo to be provided by user.
  gpio_init(PPIEZO);
  gpio_set_function(PPIEZO, GPIO_FUNC_PWM);
  #endif  // PASSIVE_PPIEZO_SUPPORT
  #endif  // NO_SOUND

  /* If user installed an infrared sensor (VS1838B) and support for a remote control. */
  #ifdef IR_SUPPORT
  /* Initialize infrared sensor gpio. */
  gpio_init(IR_RX);

  /* IR sensor will receive IR command from remote control. */
  gpio_set_dir(IR_RX, GPIO_IN);

  /* Line will remain High level until a signal is received. */
  gpio_pull_up(IR_RX);
  #endif

  /* If user installed a DHT22 external temperature and humidity sensor. */
  #ifdef DHT22_SUPPORT
  gpio_init(DHT22);
  gpio_set_dir(DHT22, GPIO_OUT); // While idle, keep the line as output.
  gpio_put(DHT22, 1);            // Keep signal high while idle.
  #endif

  gpio_set_dir(A0,  GPIO_OUT);
  gpio_set_dir(A1,  GPIO_OUT);
  gpio_set_dir(A2,  GPIO_OUT);
  gpio_set_dir(CLK, GPIO_OUT);
  gpio_set_dir(LE,  GPIO_OUT);
  gpio_set_dir(OE,  GPIO_OUT);
  gpio_set_dir(SDI, GPIO_OUT);
  gpio_set_dir(SQW, GPIO_IN);

  /* Inter integrated circuit (I2C) protocol configuration (for DS3231 and optionally for BME280). Set baud rate to 100 kHz.
     Pico will default to "master". */
  i2c_init(I2C_PORT, 100000);
  gpio_set_function(SDA, GPIO_FUNC_I2C); // data line.
  gpio_set_function(SCL, GPIO_FUNC_I2C); // clock line.

  gpio_pull_up(SDA);
  gpio_pull_up(SCL);

  /* Initialize Pico's Analog-to-digital (ADC) converter. */
  adc_init();

  /* Disable other functions for these gpio's since they will be used as ADC converter. */
  adc_gpio_init(ADC_LIGHT); // ambient light.
  adc_gpio_init(ADC_VCC);   // power supply voltage.

  return 0;
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
    if (Events & GPIO_IRQ_EDGE_RISE)
    {
      /* IR line goes from Low to High. */
      IrFinalValue[IrStepCount]   = time_us_64();                                             // this is the final timer value for current Low level.
      IrResultValue[IrStepCount]  = IrFinalValue[IrStepCount] - IrInitialValue[IrStepCount];  // calculate duration of current Low level.
      Level[IrStepCount] = 'L';                                                               // identify  as Low level.
      ++IrStepCount;                                                                          // start next event.
      IrInitialValue[IrStepCount] = time_us_64();                                             // this is also start timer of next High level.
    }

    if (Events & GPIO_IRQ_EDGE_FALL)
    {
      if (IrStepCount > 0)
      {
        /* IR line goes from High to Low. */
        IrFinalValue[IrStepCount]  =  time_us_64();                                            // this is the final timer value for current High level.
        IrResultValue[IrStepCount] = IrFinalValue[IrStepCount] - IrInitialValue[IrStepCount];  // calculate duration of current High level.
        Level[IrStepCount] = 'H';                                                              // identify as High level.
        ++IrStepCount;                                                                         // start next event.
      }
      IrInitialValue[IrStepCount]  = time_us_64(); // this is also start timer of next Low level.
    }
  }



  // /* See comments about the code section below in function "read_dht22()". */
  // if (gpio == DHT22_RX)
  // {
  //   if (Events & GPIO_IRQ_EDGE_RISE)
  //   {
  //     // DHT22 line goes from Low to High.
  //     DhtFinalValue[DhtStepCount] = time_us_64();                                                  // this is the final timer value for current Low level.
  //     DhtResultValue[DhtStepCount] = DhtFinalValue[DhtStepCount] - DhtInitialValue[DhtStepCount];  // calculate duration of current Low level.
  //     Level[DhtStepCount] = 'L';                                                                   // identify  as Low level.
  //     ++DhtStepCount;                                                                              // start next event.
  //     DhtInitialValue[DhtStepCount] = time_us_64();                                                // this is also start timer of next High level.
  //   }
  // 
  //   if (Events & GPIO_IRQ_EDGE_FALL)
  //   {
  //     if (DhtStepCount > 0)
  //     {
  //       // DHT22 line goes from High to Low.
  //       DhtFinalValue[DhtStepCount] = time_us_64();                                                  // this is the final timer value for current High level.
  //       DhtResultValue[DhtStepCount] = DhtFinalValue[DhtStepCount] - DhtInitialValue[DhtStepCount];  // calculate duration of current High level.
  //       Level[DhtStepCount] = 'H';                                                                   // identify as High level.
  //       ++DhtStepCount;                                                                              // start next event.
  //     }
  //     DhtInitialValue[DhtStepCount] = time_us_64(); // this is also start timer of next Low level.
  //   }
  // }
}





/* $PAGE */
/* $TITLE=matrix_test() */
/* ------------------------------------------------------------------ *\
          Test Green Clock LED matrix, column-by-column,
                 and also all display indicators.
\* ------------------------------------------------------------------ */
void matrix_test(void)
{
  UINT8 Column;
  UINT8 Loop1UInt8;
  UINT8 Row;
  UINT8 Section;

  UINT64 StartTime;


  /* Wipe framebuffer on entry. */
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
      sleep_ms(500);
    }
  }

  clear_framebuffer(0);

  /* ----------------------- Indicators test ------------------------ */
  /* -------------------- Horizontal indicators --------------------- */
  DisplayBuffer[16] |= 1 << 6;
  sleep_ms(2000); // first delay longer so that user can watch indicators.
  DisplayBuffer[16] |= 1 << 5;
  sleep_ms(650);
  DisplayBuffer[16] |= 1 << 3;
  sleep_ms(650);
  DisplayBuffer[16] |= 1 << 2;
  sleep_ms(650);
  DisplayBuffer[16] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[8] |= 1 << 7;
  sleep_ms(650);
  DisplayBuffer[8] |= 1 << 5;
  sleep_ms(650);
  DisplayBuffer[8] |= 1 << 4;
  sleep_ms(650);
  DisplayBuffer[8] |= 1 << 2;
  sleep_ms(650);
  DisplayBuffer[8] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[0] |= 1 << 7;
  sleep_ms(650);
  DisplayBuffer[0] |= 1 << 6;
  sleep_ms(650);
  DisplayBuffer[0] |= 1 << 4;
  sleep_ms(650);
  DisplayBuffer[0] |= 1 << 3;
  sleep_ms(650);

  /* --------------------- Vertical indicators ---------------------- */
  DisplayBuffer[0] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[0] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[1] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[1] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[2] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[2] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[3] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[3] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[4] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[4] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[5] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[5] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[6] |= 1 << 0;
  sleep_ms(650);
  DisplayBuffer[6] |= 1 << 1;
  sleep_ms(650);
  DisplayBuffer[7] |= 1 << 0;
  sleep_ms(650);
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

  sleep_ms(400);

  /* Wipe framebuffer when done. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    DisplayBuffer[Loop1UInt8] = 0x00;

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


  /* All pixels are Off on entry. Set their status accordingly. */
  for (DumFrame = 0; DumFrame < 24; ++DumFrame)
    for (DumBit = 0; DumBit < 8; ++DumBit)
      Status[DumFrame][DumBit] = 0;



  /* Make sure sounds do not begin before pixel twinkling. */
  #ifdef PASSIVE_PIEZO_SUPPORT
  sound_queue_passive(SILENT, 300);

  /* Make random sounds while playing with pixels (50 msec per sound - one call-back duration). */
  for (Loop1UInt16 = 1; Loop1UInt16 < (Seconds * 1000 / 95); ++Loop1UInt16)
  {
    Frequency = (rand() % 7000) + 300;

    /* Add a new quick sound in sound queue. If sound queue is full, stop adding sounds. */
    if (sound_queue_passive(Frequency, 50) == MAX_PASSIVE_SOUND_QUEUE)
      break;
  }
  #endif  // PASSIVE_PIEZO_SUPPORT



  StartTime = time_us_64();
  while (time_us_64() < (StartTime + (1000000 * Seconds)))
  {
    /* Generate a FrameBuffer, excluding 8 and 16 that are reserved. */
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
    if (Status[DumFrame][DumBit] == 0)
    {
      /* This bit is turned Off, turn it On. */
      DisplayBuffer[DumFrame] |= (1 << DumBit);
      Status[DumFrame][DumBit] = 1;
    }
    else
    {
      /* This bit is turned On, turn it Off. */
      DisplayBuffer[DumFrame] &= ~(1 << DumBit);
      Status[DumFrame][DumBit] = 0;
    }

    /* Short pause before next pixel action. */
    sleep_ms(2);
  }

  return;
}





#ifdef PASSIVE_PIEZO_SUPPORT
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
  if (JingleNumber == JINGLE_BIRTHDAY)
  {
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(LA_b, 400);
    sound_queue_passive(SOL_b, 400);
    sound_queue_passive(DO_c, 400);
    sound_queue_passive(SI_b, 800);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(LA_b, 400);
    sound_queue_passive(SOL_b, 400);
    sound_queue_passive(RE_c, 400);
    sound_queue_passive(DO_c, 800);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_c, 400);
    sound_queue_passive(MI_c, 400);
    sound_queue_passive(DO_c, 400);
    sound_queue_passive(SI_b, 400);
    sound_queue_passive(LA_b, 800);
    sound_queue_passive(FA_c, 200);
    sound_queue_passive(FA_c, 200);
    sound_queue_passive(MI_c, 400);
    sound_queue_passive(DO_c, 400);
    sound_queue_passive(RE_c, 400);
    sound_queue_passive(DO_c, 1000);
  }

  if (JingleNumber == JINGLE_ENCOUNTER)
  {
    /* Close encounter of the third kind. */
    sound_queue_passive(SOL_b, 600);
    sound_queue_passive(LA_b, 600);
    sound_queue_passive(FA_b, 600);
    sound_queue_passive(FA_a, 600);
    sound_queue_passive(DO_b, 800);
  }

  if (JingleNumber == JINGLE_FETE)
  {
    /* "Gens du pays". */
    sound_queue_passive(LA_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(FA_b, 200);
    sound_queue_passive(DO_c, 800);
    sound_queue_passive(LA_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(FA_b, 200);
    sound_queue_passive(RE_c, 800);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(LA_DIESE_b, 200);
    sound_queue_passive(RE_c, 200);
    sound_queue_passive(DO_c, 600);
    sound_queue_passive(FA_b, 200);
    sound_queue_passive(LA_b, 600);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(FA_b, 1600);
  }

  if (JingleNumber == JINGLE_RACING)
  {
    /* "Horse racing" / "Call to post". */
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(DO_c, 200);
    sound_queue_passive(MI_c, 200);
    sound_queue_passive(SOL_c, 300);
    sound_queue_passive(SOL_c, 100);
    sound_queue_passive(SOL_c, 200);
    sound_queue_passive(MI_c, 300);
    sound_queue_passive(MI_c, 100);
    sound_queue_passive(MI_c, 200);
    sound_queue_passive(DO_c, 200);
    sound_queue_passive(MI_c, 200);
    sound_queue_passive(DO_c, 200);
    sound_queue_passive(SOL_b, 800);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(DO_c, 200);
    sound_queue_passive(MI_c, 200);
    sound_queue_passive(SOL_c, 300);
    sound_queue_passive(SOL_c, 100);
    sound_queue_passive(SOL_c, 200);
    sound_queue_passive(MI_c, 300);
    sound_queue_passive(MI_c, 100);
    sound_queue_passive(MI_c, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(SOL_b, 200);
    sound_queue_passive(DO_c, 1400);
  }
}
#endif





#ifdef IR_SUPPORT
/* $PAGE */
/* $TITLE=process_ir_command() */
/* ------------------------------------------------------------------ *\
       Function to execute a command received from remte control
     NOTE: This function is independant from "decode_ir_command()",
           so that it can be used, modified and improved with any
           brand of remote control. Only the "decode_ir_command()"
           needs to be changed (by adding another "#include" module
           and adapted to the brand of remote control used.
\* ------------------------------------------------------------------ */
void process_ir_command(UINT64 IrCommand)
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


  /* Check if IrCommand is out-of-bound. */
  if ((IrCommand == IR_LO_LIMIT) || (IrCommand >= IR_HI_LIMIT))
  {
    // scroll_string(24, "Out-of-bound IR command");

    return;
  }

  if (IrCommand == IR_BUTTON_TOP_QUICK)
  {
    // scroll_string(24, "Button 'Set': Top quick");

    /* If one or more alarms are ringing, the "Set" button is used to shut them Off. */
    if (AlarmReachedBitMask)
    {
      AlarmReachedBitMask = 0;
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
          FlagSetClock = FLAG_ON;
          SetupSource = SETUP_SOURCE_CLOCK;
          CurrentClockMode = MODE_CLOCK_SETUP;
          IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
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
    SetupSource = SETUP_SOURCE_ALARM;
    ++SetupStep; // process through all alarm setup steps.
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
        scroll_queue(TAG_EXT_TEMP);
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
          SetupSource = SETUP_SOURCE_TIMER;
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

    /* Leave both digits On for a short while. */
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

    /* Blank display before displaying dice values. */
    clear_framebuffer(0);

    /* Roll dice number 1. */
    Dum1UInt8 = rand() % 6 + 1;
    fill_display_buffer_4X7(4, 0x30 + Dum1UInt8);

    /* Roll dice number 2. */
    Dum1UInt8 = rand() % 6 + 1;
    fill_display_buffer_4X7(13, 0x30 + Dum1UInt8);
  }

  if (IrCommand == IR_DISPLAY_OUTSIDE_TEMP)
  {
    // scroll_string(24, "Button 'CD door': Outside temperature");

    scroll_queue(TAG_EXT_TEMP);
    scroll_queue(TAG_TEMPERATURE);
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
    sound_queue_passive(SILENT, 1000); // quick silence before beginning jingles.

    play_jingle(JINGLE_ENCOUNTER);
    sound_queue_passive(SILENT, 1500); // quick silence between jingles.

    play_jingle(JINGLE_BIRTHDAY);
    sound_queue_passive(SILENT, 1500); // quick silence between jingles.

    play_jingle(JINGLE_FETE);
    sound_queue_passive(SILENT, 1500); // quick silence between jingles.

    play_jingle(JINGLE_RACING);
    #endif

    /* LED matrix column test. */
    matrix_test();

    /* Pixel twinkling on clock display. */
    pixel_twinkling(10);

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
    scroll_queue(TAG_PICO_UNIQUE_ID);   // Pico microcontroller unique ID ("serial number").
    scroll_queue(TAG_TEMPERATURE);      // display ambient temperature.
    scroll_queue(TAG_EXT_TEMP);         // outside parameters (if a DHT22 or a BME280 has been installed by user).
    scroll_queue(TAG_PICO_TEMP);        // Pico microcontroller internal temperature.
    scroll_queue(TAG_DST);              // daylight saving time mode / status.
    scroll_queue(TAG_AMBIENT_LIGHT);    // current ambient light.
    scroll_queue(TAG_VOLTAGE);          // power supply voltage.
    scroll_queue(TAG_BME280_DEVICE_ID); // BME280 device ID if one has been installed by user.
    scroll_queue(TAG_IDLE_MONITOR);     // system idle time monitor.

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

    if (FlashConfig.Language == ENGLISH)
    {
      sprintf(String, "Silence period: %u minutes", (int)(SilencePeriod / 60));
    }
    else if (FlashConfig.Language == FRENCH)
    {
      sprintf(String, "Periode de silence: %u minutes", (int)(SilencePeriod / 60));
      String[1] = (UINT8)31; // e accent aigu.
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
      // SETUP_KEYCLICK             0x07
      // SETUP_SCROLLING            0x08
      // SETUP_TEMP_UNIT            0x09
      // SETUP_LANGUAGE             0x0A
      // SETUP_TIME_FORMAT          0x0B
      // SETUP_HOURLY_CHIME         0x0C
      // SETUP_CHIME_TIME_ON        0x0D
      // SETUP_CHIME_TIME_OFF       0x0E
      // SETUP_NIGHT_LIGHT          0x0F
      // SETUP_NIGHT_LIGHT_TIME_ON  0x10
      // SETUP_NIGHT_LIGHT_TIME_OFF 0x11
      // SETUP_AUTO_BRIGHT          0x12
      // SETUP_CLOCK_HI_LIMIT       0x13
      switch (SetupStep)
      {
        case (0x01): // SETUP_HOUR
          /* Hour, minute, seconds, day-of-month, month and year display. */
          if (FlashConfig.TimeDisplayMode == H12)
          {
            Dum1UInt8 = convert_h24_to_h12(CurrentHour, &AmFlag, &PmFlag);
            sprintf(String, "%u:%2.2u:%2.2u %s", Dum1UInt8, CurrentMinute, SecondCounter, AmFlag ? "AM" : "PM");
            scroll_string(24, String);
            get_date_string(String);
            scroll_string(24, String);
          }
          else
          {
            sprintf(String, "%2.2u:%2.2u:%2.2u", CurrentHour, CurrentMinute, SecondCounter);
            scroll_string(24, String);
            get_date_string(String);
            scroll_string(24, String);
          }
        break;

        case (0x06): // SETUP_DST
          /* Daylight saving time status. */
          scroll_queue(TAG_DST);
        break;

        case (0x07): // SETUP_KEYCLICK
          /* Beep (keyclick). */
          if (FlashConfig.Language == ENGLISH)
          {
            if (FlashConfig.FlagKeyclick == FLAG_ON)
            {
              sprintf(String, "Keyclick On - Duration: %u ms Repeat1: %u Repeat2: %u", TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1, TONE_KEYCLICK_REPEAT2);
              scroll_string(24, String);
            }
            else
            {
              scroll_string(24, "Keyclick Off");
            }
          }

          if (FlashConfig.Language == FRENCH)
          {
            if (FlashConfig.FlagKeyclick == FLAG_ON)
            {
              sprintf(String, "Keyclick On - Duree %u ms Repeat1: %u Repeat2: %u", TONE_KEYCLICK_DURATION, TONE_KEYCLICK_REPEAT1, TONE_KEYCLICK_REPEAT2);
              String[17] = (UINT8)31; // e accent aigu.
              scroll_string(24, String);
            }
            else
            {
              scroll_string(24, "Keyclick Off");
            }
          }
        break;

        case (0x08): // SETUP_SCROLLING
          /* Display scroll. */
          if (FlashConfig.Language == ENGLISH)
          {
            if (FlashConfig.FlagScrollEnable == FLAG_ON)
            {
              sprintf(String, "Scrolling On - Frequency: %u minutes Dot speed: %u msec.", SCROLL_PERIOD_MINUTE, SCROLL_DOT_TIME);
              scroll_string(24, String);
            }
            else
            {
              scroll_string(24, "Scrolling Off.");
            }
          }

          if (FlashConfig.Language == FRENCH)
          {
            if (FlashConfig.FlagScrollEnable == FLAG_ON)
            {
              sprintf(String, "Scrolling On - Periode: %u minutes Vitesse des dots: %u msec.", SCROLL_PERIOD_MINUTE, SCROLL_DOT_TIME);
              String[16] = (UINT8)31; // e accent aigu.
              scroll_string(24, String);
            }
            else
            {
              scroll_string(24, "Scrolling OFF.");
            }
          }
        break;

        case (0x09): // SETUP_TEMP_UNIT
          /* Temperature unit. */
          if (FlashConfig.Language == ENGLISH)
          {
            if (FlashConfig.TemperatureUnit == CELSIUS)
            {
              scroll_string(24, "Temperature unit is Celsius");
            }
            else
            {
              scroll_string(24, "Temperature unit is Fahrenheit");
            }
          }

          if (FlashConfig.Language == FRENCH)
          {
            if (FlashConfig.TemperatureUnit == CELSIUS)
            {
              sprintf(String, "Unite de temperature: Celsius");
              String[4] = (UINT8)31;  // e accent aigu.
              String[13] = (UINT8)31; // e accent aigu.
              scroll_string(24, String);
            }
            else
            {
              sprintf(String, "Unite de temperature: Fahrenheit");
              String[4] = (UINT8)31;  // e accent aigu.
              String[13] = (UINT8)31; // e accent aigu.
              scroll_string(24, String);
            }
          }
          scroll_queue(TAG_TEMPERATURE);
          scroll_queue(TAG_EXT_TEMP);
        break;

        case (0x0A): // SETUP_LANGUAGE
          /* Language. */
          if (FlashConfig.Language == ENGLISH)
            scroll_string(24, "Language is English");

          if (FlashConfig.Language == FRENCH)
            scroll_string(24, "La langue est le francais");
        break;

        case (0x0B): // SETUP_TIME_FORMAT
          /* Time display format. */
          if (FlashConfig.Language == ENGLISH)
          {
            if (FlashConfig.TimeDisplayMode == H12)
            {
              scroll_string(24, "Time display format: 12-hours");
            }
            else
            {
              scroll_string(24, "Time display format: 24-hours");
            }
          }

          if (FlashConfig.Language == FRENCH)
          {
            if (FlashConfig.TimeDisplayMode == H12)
            {
              scroll_string(24, "Format d'affichage de l'heure: 12 heures");
            }
            else
            {
              scroll_string(24, "Format d'affichage de l'heure: 24 heures");
            }
          }
        break;

        case (0x0C): // SETUP_HOURLY_CHIME
          /* Hourly chime. */
          if (FlashConfig.Language == ENGLISH)
          {
            if (FlashConfig.ChimeMode == CHIME_OFF)
            {
              scroll_string(24, "Hourly chime is Off");
            }
            else if (FlashConfig.ChimeMode == CHIME_ON)
            {
              scroll_string(24, "Hourly chime is On");
            }
            else if (FlashConfig.ChimeMode == CHIME_DAY)
            {
              sprintf(String, "Hourly chime is intermittent, from %u:00 to %u:00", FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff);
              scroll_string(24, String);
            }
          }

          if (FlashConfig.Language == FRENCH)
          {
            if (FlashConfig.ChimeMode == CHIME_OFF)
            {
              scroll_string(24, "Le signal horaire est a Off");
            }
            else if (FlashConfig.ChimeMode == CHIME_ON)
            {
              scroll_string(24, "Le signal horaire est a On");
            }
            else if (FlashConfig.ChimeMode == CHIME_DAY)
            {
              sprintf(String, "Le signal horaire est intermittent, de %u:00 a %u:00", FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff);
              scroll_string(24, String);
            }
          }
        break;

        case (0x0F): // SETUP_NIGHT_LIGHT
          /* Night light. */
          if (FlashConfig.Language == ENGLISH)
          {
            if (FlashConfig.NightLightMode == NIGHT_LIGHT_OFF)
            {
              scroll_string(24, "Night light is Off");
            }
            else if (FlashConfig.NightLightMode == NIGHT_LIGHT_ON)
            {
              scroll_string(24, "Night light is On");
            }
            else if (FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT)
            {
              sprintf(String, "Night light is intermittent, from %u:00 to %u:00", FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOff);
              scroll_string(24, String);
            }
            else if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
            {
              sprintf(String, "Night light is automatic");
              scroll_string(24, String);
            }
          }

          if (FlashConfig.Language == FRENCH)
          {
            if (FlashConfig.NightLightMode == NIGHT_LIGHT_OFF)
            {
              scroll_string(24, "La veilleuse de nuit est a Off");
            }
            else if (FlashConfig.NightLightMode == NIGHT_LIGHT_ON)
            {
              scroll_string(24, "La veilleuse de nuit est a On");
            }
            else if (FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT)
            {
              sprintf(String, "La veilleuse de nuit est intermittente, de %u:00 a %u:00", FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOff);
              scroll_string(24, String);
            }
            else if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
            {
              sprintf(String, "La veilleuse de nuit est automatique");
              scroll_string(24, String);
            }
          }
        break;

        case (0x12): // SETUP_AUTO_BRIGHT
          /* Light level. */
          if (FlashConfig.Language == ENGLISH)
          {
            if (FlashConfig.FlagAutoBrightness == FLAG_ON)
            {
              sprintf(String, "Auto brightness is On. Light level: %u. Hysteresis: %u", get_ads1015(), AverageLightLevel);
              scroll_string(24, String);
            }
            else
            {
              scroll_string(24, "Auto brightness is Off");
            }
          }

          if (FlashConfig.Language == FRENCH)
          {
            if (FlashConfig.FlagAutoBrightness == FLAG_ON)
            {
              sprintf(String, "Intensite automatique est a On. Luminosite: %u. Hysteresis: %u", get_ads1015(), AverageLightLevel);
              String[8] = (UINT8)31;  // e accent aigu.
              String[41] = (UINT8)31; // e accent aigu.
              scroll_string(24, String);
            }
            else
            {
              sprintf(String, "Intensite automatique est a Off");
              String[8] = (UINT8)31; // e accent aigu.
              scroll_string(24, String);
            }
          }
        break;
      }
    }
  }

  if (IrCommand == IR_DISPLAY_EVENTS_TODAY)
  {
    // scroll_string(24, "Button 'Over': Today's events");

    Dum1UInt8 = 0;
    for (Loop1UInt8 = 0; Loop1UInt8 < MAX_EVENTS; ++Loop1UInt8)
    {
      if ((CalendarEvent[Loop1UInt8].Month == CurrentMonth) && (CalendarEvent[Loop1UInt8].Day == CurrentDayOfMonth))
      {
        ++Dum1UInt8;

        if (FlashConfig.Language == ENGLISH)
          sprintf(String, "%s %u: %s", MonthName[FlashConfig.Language][CurrentMonth], CurrentDayOfMonth, CalendarEvent[Loop1UInt8].Description);

        if (FlashConfig.Language == FRENCH)
          sprintf(String, "%u %s: %s", CurrentDayOfMonth, MonthName[FlashConfig.Language][CurrentMonth], CalendarEvent[Loop1UInt8].Description);

        scroll_string(24, String);
      }
    }

    if (FlashConfig.Language == ENGLISH)
    {
      if (Dum1UInt8 == 0)
        scroll_string(24, "No event for today");
      else if (Dum1UInt8 == 1)
        scroll_string(24, "1 event today");
      else
      {
        sprintf(String, "%u events today", Dum1UInt8);
        scroll_string(24, String);
      }
    }

    if (FlashConfig.Language == FRENCH)
    {
      if (Dum1UInt8 == 0)
      {
        sprintf(String, "Aucun evenement aujourd'hui");
        String[6] = (UCHAR)31; // e accent aigu.
        String[8] = (UCHAR)31; // e accent aigu
        scroll_string(24, String);
      }
      else
      {
        if (Dum1UInt8 == 1)
        {
          sprintf(String, "1 evenement aujourd'hui");
          String[2] = (UCHAR)31; // e accent aigu.
          String[4] = (UCHAR)31; // e accent aigu
          scroll_string(24, String);
        }
        else
        {
          sprintf(String, "%u evenements aujourd'hui", Dum1UInt8);
          String[strlen(String) - 20] = (UCHAR)31; // e accent aigu.
          String[strlen(String) - 22] = (UCHAR)31; // e accent aigu
          scroll_string(24, String);
        }
      }
    }
  }

  if (IrCommand == IR_DISPLAY_EVENTS_THIS_WEEK)
  {
    // scroll_string(24, "Button 'Mute': This week's events");

    /* Retrieve beginning of week (previous Sunday). */
    Dum1UInt8 = get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth);

    if (Dum1UInt8 != SUN)
    {
      DumDayOfMonth = CurrentDayOfMonth;
      DumMonth = CurrentMonth;
      DumYear = Year4Digits;

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

        Dum1UInt8 = get_weekday(DumYear, DumMonth, DumDayOfMonth);
      } while (Dum1UInt8 != SUN);
    }

    /* For debugging purposes - Display date of first day of the week (Sunday) that has been found. */
    // sprintf(String, "Week beginning:%u %s %u", DumDayOfMonth, MonthName[FlashConfig.Language][DumMonth], DumYear);
    // scroll_string(24, String);

    /* And display all events for each of the next seven days, starting with this Sunday. */
    Dum1UInt8 = 0;

    /* Scan each day of current week. */
    for (Loop1UInt8 = 0; Loop1UInt8 < 7; ++Loop1UInt8)
    {
      /* Check every calendar event to find a match with date under evaluation. */
      for (Loop2UInt8 = 0; Loop2UInt8 < MAX_EVENTS; ++Loop2UInt8)
      {
        if ((CalendarEvent[Loop2UInt8].Month == DumMonth) && (CalendarEvent[Loop2UInt8].Day == DumDayOfMonth))
        {
          /* We found a match, display calendar event. */
          ++Dum1UInt8;

          if (FlashConfig.Language == ENGLISH)
            sprintf(String, "%s %u: %s", MonthName[FlashConfig.Language][DumMonth], DumDayOfMonth, CalendarEvent[Loop2UInt8].Description);

          if (FlashConfig.Language == FRENCH)
            sprintf(String, "%u %s: %s", DumDayOfMonth, MonthName[FlashConfig.Language][DumMonth], CalendarEvent[Loop2UInt8].Description);

          scroll_string(24, String);
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

    if (FlashConfig.Language == ENGLISH)
    {
      if (Dum1UInt8 == 0)
        scroll_string(24, "No event this week");
      else
      {
        if (Dum1UInt8 == 1)
        {
          scroll_string(24, "1 event this week");
        }
        else
        {
          sprintf(String, "%u events this week", Dum1UInt8);
          scroll_string(24, String);
        }
      }
    }

    if (FlashConfig.Language == FRENCH)
    {
      if (Dum1UInt8 == 0)
      {
        sprintf(String, "Aucun evenement cette semaine");
        String[6] = (UCHAR)31; // e accent aigu.
        String[8] = (UCHAR)31; // e accent aigu
        scroll_string(24, String);
      }
      else
      {
        if (Dum1UInt8 == 1)
        {
          sprintf(String, "1 evenement cette semaine");
          String[2] = (UCHAR)31; // e accent aigu.
          String[4] = (UCHAR)31; // e accent aigu
          scroll_string(24, String);
        }
        else
        {
          sprintf(String, "%u evenements cette semaine", Dum1UInt8);
          String[strlen(String) - 22] = (UCHAR)31; // e accent aigu.
          String[strlen(String) - 24] = (UCHAR)31; // e accent aigu
          scroll_string(24, String);
        }
      }
    }
  }

  if (IrCommand == IR_DISPLAY_SECOND)
  {
    // scroll_string(24, "Button 'Time': Display second");

    /* Wait till eventual current scrolling has completed. */
    while (ScrollDotCount)
    {
    };

    /* Allow two middle dots blinking during minutes and seconds display. */
    CurrentClockMode = MODE_SHOW_TIME;

    /* Reset the number of seconds the system has been idle. */
    IdleNumberOfSeconds = 0;

    /* Display seconds during: 10 times 500 msec (5 seconds). */
    for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
    {
      /* Synchronize seconds update on clock display with middle dots blinking to prevent a glitch while updating.
         A semaphore is used since the compiler used with the Geany IDE seems to optimize the code even when using the "volatile" keyword... */
      if (sem_acquire_timeout_ms(&SemSync, 1000) == true)
      {
        Time_RTC = Read_RTC();

        Time_RTC.minutes = Time_RTC.minutes & 0x7F;
        Time_RTC.seconds = Time_RTC.seconds & 0x7F;

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
  UCHAR String[DISPLAY_BUFFER_SIZE];
  UCHAR Voltage[12];

  UINT8 DhtDisplay[4];
  UINT8 Dum1UInt8;
  UINT8 Head;
  UINT8 Loop1UInt8;
  UINT8 Tag;
  UINT8 Tail;

  int Dum1Int;

  float Humidity;
  float Temperature;
  float TemperatureF;

  pico_unique_board_id_t board_id;


  /* Check if for any reason, the function has been called while the queue is empty. */
  if (ScrollQueueHead == ScrollQueueTail)
    return;

  while (ScrollQueueTail != ScrollQueueHead)
  {
    /* For debugging purposes - Keep track of scroll queue head and tail before dequeuing. */
    // Head = ScrollQueueHead;
    // Tail = ScrollQueueTail;

    Tag = scroll_unqueue();

    /* NOTE: A special Calendar Event (with Description = "Debug") may be inserted in the bundle of calendar events.
             This specific case may be handled (see below) to display whatever information we want. If no Calendar Event
             exists with a description "Debug", this code will not interfere with normal clock usage. */
    if (strcmp(CalendarEvent[Tag].Description, "Debug") == 0)
      Tag = TAG_ALARM;

    if (Tag < MAX_EVENTS)
    {
      /* We must scroll a calendar event. */
      scroll_string(24, CalendarEvent[Tag].Description);
      while (ScrollDotCount)
        sleep_ms(100); // wait until scrolling is over.
    }
    else
    {
      /* Special cases when tag number is at the end of UINT8 (near 0xFF). */
      switch (Tag)
      {
        case (TAG_ALARM):
          /* For debug purposes.
             Display current alarm parameters. */
          byte_data(); // read alarm parameters.

          /* Display current parameters for alarm 0 and alarm 1. */
          sprintf(String, "Alarm 0: %s Day %u  Hour %2.2u  Minute %2.2u  Second %2.2u   Alarm 1: %s Day %u  Hour %2.2u  Minute %2.2um",
                  FlashConfig.Alarm[0].FlagStatus ? "ON" : "OFF", (ByteData[DS3231_REG_A1D] & 0xFF), ByteData[DS3231_REG_A1H], ByteData[DS3231_REG_A1M], ByteData[DS3231_REG_A1S],
                  FlashConfig.Alarm[1].FlagStatus ? "ON" : "OFF", (ByteData[DS3231_REG_A2D] & 0xFF), ByteData[DS3231_REG_A2H], ByteData[DS3231_REG_A2M]);
          scroll_string(24, String);
        break;

        case (TAG_AMBIENT_LIGHT):
          /* If we are not currently in setup mode, display ambient light. */
          switch (FlashConfig.Language)
          {
            case (FRENCH):
              sprintf(String, "Luminosite: %u   Hysteresis: %u", get_ads1015(), AverageLightLevel);
              String[9] = (UINT8)31; // e accent aigu.
              scroll_string(24, String);
            break;

            case (ENGLISH):
              sprintf(String, "Ambient light: %u   Hysteresis: %u", get_ads1015(), AverageLightLevel);
              scroll_string(24, String);
            break;
          }
        break;

        #ifdef BME280_SUPPORT // if a BME280 outside temperature, humidity and barometric pressure sensor has been installed by user.
        case (TAG_BME280_DEVICE_ID):
          Dum1UInt8 = bme280_read_device_id();
          sprintf(String, "BME280 device ID: 0x%2.2X", Dum1UInt8);
          scroll_string(24, String);
        break;
        #endif

        case (TAG_DATE):
          /* Used to scroll the date on clock display. */
          get_date_string(String);
          scroll_string(24, String);
        break;

        case (TAG_DEBUG):
          /* For debug purposes. Display a few variables while implementing DST support. */
          sprintf(String, "L: %2.2X  FSTDoM: %2.2X  FSTM: %2.2X", FlashConfig.Language, FlagSetupClock[SETUP_DAY_OF_MONTH], FlagSetupClock[SETUP_MONTH]);
          scroll_string(15, String);
        break;

        case (TAG_DST):
          /* Display current daylight saving time setting.
             No support = No DST support at all.
             Inactive   = DST is supported, but we are not during DST period of the year.
             Active     = DST is supported and we are during DST period of the year. */
          switch (FlashConfig.Language)
          {
            case (ENGLISH):
              if (DAYLIGHT_SAVING_TIME == DST_NONE)
              {
                scroll_string(24, "No support for daylight saving time");
              }
              else
              {
                /* Announce if Daily Saving Time ("DST") is currently active or inactive, depending of current date. */
                if (FlagDayLightSavingTime == FLAG_ON)
                  scroll_string(24, "Daylight saving time active");
                else
                  scroll_string(24, "Daylight saving time inactive");
              }
            break;

            case (FRENCH):
              if (DAYLIGHT_SAVING_TIME == DST_NONE)
              {
                sprintf(String, "Heure avancee non supportee");
                String[11] = (UINT8)31; // e accent aigu.
                String[25] = (UINT8)31; // e accent aigu.
                scroll_string(24, String);
              }
              else
              {
                /* Announce if Daily Saving Time ("DST") is currently active or inactive, depending of current date. */
                if (FlagDayLightSavingTime == FLAG_ON)
                {
                  sprintf(String, "Heure avancee active");
                  String[11] = (UINT8)31; // e accent aigu.
                  scroll_string(24, String);
                }
                else
                {
                  sprintf(String, "Heure avancee inactive");
                  String[11] = (UINT8)31; // e accent aigu.
                  scroll_string(24, String);
                }
              }
            break;
          }
        break;

        case (TAG_EXT_TEMP):
          /* User must decide if external temp should be read from the DHT22 that he installed, or from the BME280 that he installed.
             It may be useful to read the user guide about DHT22. It seems not to be reliable if the cable is more that a few inches long. */

          /* -------------------------------------------------------------------- *\
                  Read outdoor temperature, humidity and barometric pressure
                               from BME280 installed by user
               and scroll them on clock display only if no error while reading.
          \* -------------------------------------------------------------------- */
          #ifdef BME280_SUPPORT // if a BME280 outdoor temperature, humidity and barometric pressure sensor has been installed by user.
          if (bme280_get_temp() == 0)
          {
            // Build-up the string to be scrolled, first with temperature...
            if (FlashConfig.TemperatureUnit == CELSIUS)
            {
              // ...in Celsius.
              switch (FlashConfig.Language)
              {
                case (ENGLISH):
                  sprintf(String, "Out: %2.2f%cC  Hum: %2.2f%%  Pressure: %4.2f hPa", Bme280CalibParam.TemperatureC, 0x80, Bme280CalibParam.Humidity, Bme280CalibParam.Pressure);
                break;

                case (FRENCH):
                  sprintf(String, "Ext: %2.2f%cC  Hum: %2.2f%%  Pression: %4.2f hPa", Bme280CalibParam.TemperatureC, 0x80, Bme280CalibParam.Humidity, Bme280CalibParam.Pressure);
                break;
              }
            }
            else
            {
              // ...or in Fahrenheit.
              switch (FlashConfig.Language)
              {
                case (ENGLISH):
                  sprintf(String, "Out: %2.2f%cF  Hum: %2.2f%%  Pressure: %4.2f hPa", Bme280CalibParam.TemperatureF, 0x80, Bme280CalibParam.Humidity, Bme280CalibParam.Pressure);
                break;

                case (FRENCH):
                  sprintf(String, "Ext: %2.2f%cF  Hum: %2.2f%%  Pression: %4.2f hPa", Bme280CalibParam.TemperatureF, 0x80, Bme280CalibParam.Humidity, Bme280CalibParam.Pressure);
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
          sprintf(String, " [%llu/%llu]", Bme280Errors, Bme280Readings);
          if (DebugBitMask & DEBUG_BME280)
          {
            uart_send(__LINE__, String);
            uart_send(__LINE__, "\r\r");
          }
          scroll_string(24, String);
          /* ------------------------- END OF BLOCK ---------------------------- */
          #endif

          /* ------------------------------------------------------------------- *\
                    Read outdoor relative humidity and temperature
                    from DHT22 installed by user and display them
                    only if no error in the communication process.
          \* ------------------------------------------------------------------- */
          /***
          #ifdef DHT22_SUPPORT
          if (read_dht22(&Temperature, &Humidity) == 0)
          {
            // Build-up the string to be scrolled, first with temperature...
            if (FlashConfig.TemperatureUnit == CELSIUS)
            {
              // ...in Celsius.
              switch (FlashConfig.Language)
              {
                case (ENGLISH):
                  sprintf(String, "Out: %2.2f%cC  Hum: %2.2f%%", Temperature, 0x80, Humidity);
                break;

                case (FRENCH):
                  sprintf(String, "Ext: %2.2f%cC  Hum: %2.2f%%", Temperature, 0x80, Humidity);
                break;
              }
            }
            else
            {
              // ...or in Fahrenheit.
              TemperatureF = ((Temperature * 9) / 5) + 32;  // convert from Celsius to Fahrenheit.
              switch (FlashConfig.Language)
              {
                case (ENGLISH):
                  sprintf(String, "Out: %2.2f%cF  Hum: %2.2f%%", TemperatureF, 0x80, Humidity);
                break;

                case (FRENCH):
                  sprintf(String, "Ext: %2.2f%cF  Hum: %2.2f%%", TemperatureF, 0x80, Humidity);
                break;
              }
            }
            scroll_string(24, String);
          }


          // For statistic purposes, display the cumulative number of errors while reading DHT data.
          sprintf(String, " (%llu/%llu)", DhtErrors, DhtReadings);
          scroll_string(24, String);
          // ------------------------- END OF BLOCK ----------------------------
          #endif  // DHT22_SUPPORT
          ***/
        break;

        case (TAG_FIRMWARE_VERSION):
          sprintf(String, "Pico Green Clock - Firmware Version %s", FIRMWARE_VERSION);
          scroll_string(24, String);
        break;

        case (TAG_IDLE_MONITOR):
          sprintf(String, "System idle monitor: %llu", IdleMonitor[13]);
          scroll_string(24, String);
        break;

        case (TAG_PICO_TEMP):
          get_pico_temperature(String);
          scroll_string(24, String);
        break;

        case (TAG_PICO_UNIQUE_ID):
          pico_get_unique_board_id(&board_id);

          sprintf(String, "Pico ID: ");
          for (Loop1UInt8 = 0; Loop1UInt8 < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++Loop1UInt8)
          {
            sprintf(&String[strlen(String)], "%2.2X", board_id.id[Loop1UInt8]);
            if (Loop1UInt8 % 2)
              sprintf(&String[strlen(String)], " ");
          }

          scroll_string(24, String);
        break;

        case (TAG_QUEUE):
          /* For debug purposes. Display all allocated scroll queue slots. */
          if (ScrollQueueHead == ScrollQueueTail)
          {
            sprintf(String, "H%2.2u = T%2.2u - No element allocated...", ScrollQueueHead, ScrollQueueTail);
            scroll_string(24, String);
          }
          else
          {
            Dum1UInt8 = ScrollQueueTail;
            while (TRUE)
            {
              sprintf(String, "(H%2.2u) T%2.2u E%2.2u", ScrollQueueHead, Dum1UInt8, ScrollQueue[Dum1UInt8]);
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

        case (TAG_TEMPERATURE):
          /* Read ambient temperature. */
          #ifndef DHT22_SUPPORT
          /* ------------------------------------------------------------------- *\
                     THIS BLOCK OF CODE TO READ AMBIENT TEMPERATURE
                             FROM DS3231 REAL-TIME CLOCK IC
                  Read ambient temperature from DS3231 real-time clock ic
                           integrated in the Pico Green Clock
          \* ------------------------------------------------------------------ */
          get_ambient_temperature(); // read ambient temperature from DS3231.

          /* Write temperature to the string to be displayed... */
          if (FlashConfig.TemperatureUnit == CELSIUS)
          {
            /* ...in Celsius. */
            sprintf(String, "%u.%u%cC", TemperaturePart1, TemperaturePart2, 0x80);
          }
          else
          {
            /* ...or in Fahrenheit. */
            TemperatureF = TemperaturePart1 * 9 / 5 + 32; // convert from Celsius to Fahrenheit.
            sprintf(String, " %2.2f%cF", TemperatureF, 0x80);
          }
          scroll_string(24, String);
          /* ------------------------ END OF BLOCK ---------------------------- */
          #endif

          /* ------------------------------------------------------------------- *\
               THIS BLOCK OF CODE TO READ AMBIENT TEMPERATURE FROM DHT22
                    Read ambient relative humidity and temperature
                    from DHT22 installed by user and display them
                       only if no error in the reading process.
          \* ------------------------------------------------------------------ */
          #ifdef DHT22_SUPPORT // if a DHT22 outside temperature and humidity sensor has been installed by user.
          if (read_dht22(&Temperature, &Humidity) == 0)
          {
            /* Build-up the string to be scrolled, first with temperature... */
            if (FlashConfig.TemperatureUnit == CELSIUS)
            {
              /* ...in Celsius. */
              sprintf(String, "%2.2f%cC  Hum: %2.2f%%", Temperature, 0x80, Humidity);
            }
            else
            {
              /* ...or in Fahrenheit. */
              TemperatureF = ((Temperature * 9) / 5) + 32; // convert from Celsius to Fahrenheit.
              sprintf(String, "%2.2f%cF  Hum: %2.2f%%", TemperatureF, 0x80, Humidity);
            }
            scroll_string(24, String);
          }

          /* For statistic purposes, display the cumulative number of errors while reading DHT data. */
          sprintf(String, " (%llu/%llu)", DhtErrors, DhtReadings);
          scroll_string(24, String);
          /* ------------------------- END OF BLOCK ---------------------------- */
          #endif  // DHT22_SUPPORT
        break;

        case (TAG_VOLTAGE):
          get_voltage(Voltage);
          scroll_string(24, Voltage);
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





#ifdef DHT22_SUPPORT
/* $PAGE */
/* $TITLE=read_dht22() */
/* ------------------------------------------------------------------ *\
              Read the temperature from external DHT22.
\* ------------------------------------------------------------------ */
UINT8 read_dht22(float *Temperature, float *Humidity)
{
  UCHAR Dum1UChar[41];
  UCHAR Dum2UChar[41];
  UCHAR LogicLevel[MAX_READINGS];
  UCHAR String[256];

  UINT8 Byte[4];
  UINT8 Checksum;
  UINT8 CurrentBit;

  UINT16 ComputedCheck;
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;
  UINT16 Value;

  UINT32 DataBuffer;
  UINT32 InterruptMask;

  UINT64 InitialTime;
  UINT64 FinalTime;
  UINT64 TimerValue[MAX_READINGS];


  /* Initializations. */
  DhtStepCount = 0;

  /* Reset variables before receiving new data. */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_READINGS; ++Loop1UInt16)
  {
    TimerValue[Loop1UInt16] = 0;
    LogicLevel[Loop1UInt16] = 'X';
  }

  

  /* So far, line has been kept to a high level by the internal pull-up resistor. */
  gpio_set_dir(DHT22, GPIO_OUT);

  
  /* Send the Low level "request" to DHT22 to trigger a read cycle. */
  gpio_put(DHT22, 0);

  
  /* Keep "Request" signal Low for a while. */
  sleep_ms(20);

  
  /* Make sure DHT22 receives all the attention from microcontroller so that precision is good enough for an errorless reading.
     NOTE: Trial and error has shown that DHT timing is critical and the current load on Green Clock may prevent such a precision... */
  clear_framebuffer(0);
  cancel_repeating_timer(&Timer50uSec);
  OE_DISABLE;

  /* Keep track of interrupt mask on entry. */
  InterruptMask = save_and_disable_interrupts();


  /* Then, wait for the response from DHT22 (20 to 40 microseconds delay after the request signal). */
  gpio_set_dir(DHT22, GPIO_IN);

  
  /* Put a pull-up resistor on the line. */
  gpio_pull_up(DHT22);


  /* ---------------------------------------------------------------- *\
  NOTE: Inside every "while" loop below, there is a time-out algorithm.
        As long as everything goes fine, this time-out is useless.
       (except the last one, while receiving the "second half-bit")
        However, it allows the clock to continue running fine in case
        of problem with the DHT22 (broken part, cable disconnect, etc...)
  \* ---------------------------------------------------------------- */
  /* ---------------------------------------------------------------- *\
              Wait for a while until DHT22 begins to respond.
           The line is idle (High level) and we expect the DHT22
             to begin responding within 20 to 40 microseconds.
  \* ---------------------------------------------------------------- */
  DhtInitialValue[DhtStepCount] = time_us_64();
  while (gpio_get(DHT22) == 1)
  {
    DhtFinalValue[DhtStepCount] = time_us_64();  // overwrite same variable until we get out of the while loop.
    if ((DhtFinalValue[DhtStepCount] - DhtInitialValue[DhtStepCount]) > 100) break;  // time out.
  }
  LogicLevel[DhtStepCount] = 'H';             // we will calculate below the length of this High level.
  ++DhtStepCount;



  /* ---------------------------------------------------------------- *\
            First, read the two "get ready" signal from DHT22
                 (one Low level then, one Hign level).
               Then, read the 40 bits (5 bytes X 8 bits).
                   (4 data bytes + 1 checksum byte).

             One logic level while waiting for DHT22 to answer,
                then 2 "get ready" signals (or "levels"),
      followed by 40 bits (2 logic levels for wach) = 83 steps total.
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
    DhtInitialValue[DhtStepCount] = time_us_64();
    while (gpio_get(DHT22) == 0)
    {
      DhtFinalValue[DhtStepCount] = time_us_64();  // overwrite same variable until we get out of "while" loop.
      if ((DhtFinalValue[DhtStepCount] - DhtInitialValue[DhtStepCount]) > 80) break;  // time out.
    }
    LogicLevel[DhtStepCount] = 'L';             // we will calculate below the length of this Low level.

    /* One more "logic level" change occurs. */
    ++DhtStepCount;



    /* --------------------------------------------------------- *\
                   Receiving the second half-bit.
        Wait as long as the line is High, which is the last part
                  of the bit ("second half-bit").
           We keep track of the duration and this time, it is
          important, since this is what determines if this is
            a "0" bit (around 26 microseconds), or a "1" bit
                       (around 70 microseconds).
    \* --------------------------------------------------------- */
    DhtInitialValue[DhtStepCount] = time_us_64();
    while (gpio_get(DHT22) == 1)
    {
      DhtFinalValue[DhtStepCount] = time_us_64();  // overwrite same variable until we get out of "while" loop.
      if ((DhtFinalValue[DhtStepCount] - DhtInitialValue[DhtStepCount]) > 80) break;
    }
    LogicLevel[DhtStepCount] = 'H';             // we will calculate below the length of this High level.

    /* One more "logic level" change occurs. */
    ++DhtStepCount;



    /* In case we got out of previous while loop with a timeout,
       wait to return to a Low level before going back to the "for" loop to proceed with next bit. */
    InitialTime = time_us_64();
    while (gpio_get(DHT22) == 1)
    {
      FinalTime = time_us_64();
      if ((FinalTime - InitialTime) > 45) break;
    }
  }



  /* Restore original interrupt mask as soon as DHT22 reading is completed. */
  restore_interrupts(InterruptMask);

  OE_ENABLE;

  /* Restore clock display after DHT22 data stream has been received. */
  add_repeating_timer_us(50, timer_callback_us, NULL, &Timer50uSec);



  /* If debug is On, display number of steps (logic levels) received. */
  if (DebugBitMask & DEBUG_DHT)
  {
    sprintf(String, "DHT step count: %u\r", DhtStepCount);
    uart_send(__LINE__, String);
  }



  /* Analyze and process data stream received from DHT22. */
  Checksum   = 0;
  CurrentBit = 0;
  DataBuffer = 0;
  sprintf(Dum2UChar, "00000000");                         // Checksum in binary (8 bits).
  sprintf(Dum1UChar, "00000000000000000000000000000000"); // DataBuffer (result) in binary (32 bits);


  /* Now that all bits have been received, decode the stream sent by DHT22. */
  for (Loop1UInt16 = 0; Loop1UInt16 < DhtStepCount; ++Loop1UInt16)
  {
    /* Compute the length of this logic level. */
    DhtResultValue[Loop1UInt16] = (DhtFinalValue[Loop1UInt16] - DhtInitialValue[Loop1UInt16]);

    if (DebugBitMask & DEBUG_DHT)
    {
      if (Loop1UInt16 == 0)
      {
        sprintf(String, "Step: %2.2u    Level: %c    ResultValue: %2llu   (Wait for the DHT22 to respond to the clock request - 20 to 40 usec)\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);

        continue;
      }


      /* Skip the 1st "Get ready" level received from DHT22 (first: Low level, then High level, both 80 microseconds). */
      if (Loop1UInt16 == 1)
      {
        sprintf(String, "Step: %2.2u    Level: %c    ResultValue: %2llu   (1st <get ready> received from DHT22 - around 70 usec)\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);

        continue;
      }


      /* Skip the 2nd "Get ready" level received from DHT22 (first: Low level, then High level, both 80 microseconds). */
      if (Loop1UInt16 == 2)
      {
        sprintf(String, "Step: %2.2u    Level: %c    ResultValue: %2llu   (2nd <get ready> received from DHT22 - around 70 usec)\r\r\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);

        continue;
      }
    }



    /* --------------------------------------------------------- *\
                      Evaluate each bit received.
    \* --------------------------------------------------------- */
    if (LogicLevel[Loop1UInt16] == 'L')
    {
      /* Low level, it is a first half-bit. */
      if (DebugBitMask & DEBUG_DHT)
      {
        sprintf(String, "Current bit: %u\r", CurrentBit);
        uart_send(__LINE__, String);

        sprintf(String, "Step: %2u   Level: %c   (should be around 50 usec)   ResultValue: %2llu\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);
      }
    }
    else
    {
      /* Let's assume on entry that we received a 0-bit. Left-shift all bits already received.
         A new 0-bit is automatically added to the right by default. */
      if (CurrentBit < 32)
      {
        /* We're still receiving the first 32 data bits. */
        DataBuffer <<= 1;
      }
      else
      {
        /* We already received all 32 data bits, we're receiving the checksum bits. */
        Checksum <<= 1;
      }


      /* A 0-bit is around 26 usec long whereas a 1-bit is around 70 usec long.
         if the new bit is a 1-bit), replace the 0-bit just added above by a 1-bit. */
      if (DhtResultValue[Loop1UInt16] > 30)
        if (CurrentBit < 32)
          ++DataBuffer;
        else
          ++Checksum;



      if (DebugBitMask & DEBUG_DHT)
      {
         /* Generate DataBuffer in binary. */
        for (Loop2UInt16 = 0; Loop2UInt16 < 32; ++Loop2UInt16)
        {
          if (DataBuffer & (1 << Loop2UInt16))
            Dum1UChar[31 - Loop2UInt16] = '1'; // if this is a one bit.
          else
            Dum1UChar[31 - Loop2UInt16] = '0'; // if this is a zero bit.
        }
          
 
        /* Generate Checksum in binary. */
        for (Loop2UInt16 = 0; Loop2UInt16 < 8; ++Loop2UInt16)
        {
          if (Checksum & (1 << Loop2UInt16))
            Dum2UChar[7 - Loop2UInt16] = '1'; // if this is a one bit.
          else
            Dum2UChar[7 - Loop2UInt16] = '0'; // if this is a zero bit.
        }

        sprintf(String, "Step: %2u   Level: %c   (26 usec = 0  70 usec = 1)   ResultValue: %2llu  \r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);

        sprintf(String, "DataBuffer in binary: %s\r", Dum1UChar);
        uart_send(__LINE__, String);

        sprintf(String, "DataBuffer:           0x%8.8X\r", DataBuffer);
        uart_send(__LINE__, String);

        sprintf(String, "Checksum   in binary: %s\r", Dum2UChar);
        uart_send(__LINE__, String);

        sprintf(String, "Checksum:             0x%2.2X\r\r", Checksum);
        uart_send(__LINE__, String);
      }

      ++CurrentBit;
    }
  }


  /* Checksum validation. */
  ComputedCheck = 0;
  ++DhtReadings; // one more reading from DHT22.
  uart_send(__LINE__, "\r");



  /* Compute the checksum of data bytes received. */
  for (Loop1UInt16 = 0; Loop1UInt16 < 4; ++Loop1UInt16)
  {
    Value = ((DataBuffer >> (24 - (8 * Loop1UInt16))) & 0xFF);

    /* Compose each part of the checksum in binary. */
    sprintf(Dum2UChar, "00000000");
    for (Loop2UInt16 = 0; Loop2UInt16 < 8; ++Loop2UInt16)
    {
      if (Value & (1 << Loop2UInt16))
        Dum2UChar[7 - Loop2UInt16] = '1'; // if this is a one bit.
      else
        Dum2UChar[7 - Loop2UInt16] = '0'; // if this is a zero bit.
    }

    if (DebugBitMask & DEBUG_DHT)
    {
      sprintf(String, "Calculate Checksum (part %u): 0x%2.2X     %s\r", Loop1UInt16 + 1, Value, Dum2UChar);
      uart_send(__LINE__, String);
    }
    
    ComputedCheck += Value;
    ComputedCheck &= 0xFF; // when adding, may become more than 8 bits.
  }
  uart_send(__LINE__, "\r");



  /* Compare the computed checksum with the one received from DHT22 and return an error code in case of error. */
  if (ComputedCheck != Checksum)
  {
    if (DebugBitMask & DEBUG_DHT)
    {
      sprintf(String, "=== ERROR === Checksum calculated: 0x%2.2X   Checksum received: 0x%2.2X\r\r\r\r", ComputedCheck, Checksum);
      uart_send(__LINE__, String);
    }

    ++DhtErrors;

    return 0xFF;
  }


  if (DebugBitMask & DEBUG_DHT)
  {
    sprintf(String, "=== SUCCESS === ComputedCheck: 0x%2.2X   Checksum: 0x%2.2X\r\r", ComputedCheck, Checksum);
    uart_send(__LINE__, String);
  }



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

  sprintf(String, "Temperature: %2.3f C\r", *Temperature);
  uart_send(__LINE__, String);

  sprintf(String, "Humidity:    %2.3f %c\r\r\r", *Humidity, 0x25);
  uart_send(__LINE__, String);

  return 0x00;



















  /* ================================================================== *\
     NOTE: Given the high error ratio that I got with the "polling"
           version of DHT22 communication, I tried to convert the
           algorithm to an interrupt-driven one. However, I realized
           that it ended-up not being better. After some thinking,
           I thought that I would revert to the polling version, but
           this time disabling interrupts during DHT22 reading so that
           the Pico could spend all its time to take care of the DHT22.

           I decided that I would leave the interrupt-driven code here
           for those who want to experiment with the interrupt version.
           I believe it should work fine in another environment with
           less callback's / interrupt service routines.
  \* ================================================================== */
  /* === Code below is for DHT22 reading using interrupt-driven algorithm. === */
  /* Initializations. */
  Checksum      = 0;
  DhtStepCount  = 0;
  DataBuffer    = 0;

  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_DHT_READINGS; ++Loop1UInt16)
  {
    DhtInitialValue[Loop1UInt16] = 0;
    DhtFinalValue[Loop1UInt16]   = 0;
    DhtResultValue[Loop1UInt16]  = 0;
    LogicLevel[Loop1UInt16]      = 'X';  // "undefined".
  }


  /* So far, DHT22 GPIO line has been kept to a High level by the internal pull-up resistor. */
  gpio_set_dir(DHT22, GPIO_OUT);

  /* Send the Low level "request" to DHT22. */
  gpio_put(DHT22, 0);

  /* Keep "Request" signal Low for a while. */
  sleep_ms(20);

  /* Then wait for the response from DHT22 (20 to 40 microseconds delay after the request signal). */
  gpio_set_dir(DHT22, GPIO_IN);

  /* Set a pull-up resistor on the line. */
  gpio_pull_up(DHT22);


  /* At this point, DHT22 should respond and its data will be received through interrupt handler...
     Wait to receive all data stream from DHT22. */
  InitialTime = time_us_64();
  while (DhtStepCount < 83)
  {
    FinalTime = time_us_64();  // evaluate time-out period.
    if ((FinalTime - InitialTime) > 4200) break; // time out.
  }


  /* If debug is On, display number of steps received. */
  if (DebugBitMask & DEBUG_DHT)
  {
    sprintf(String, "DHT step count: %u\r", DhtStepCount);
    uart_send(__LINE__, String);
  }



  /* Analyse and process each step change in DHT22 data burst. */
  CurrentBit = 0;
  sprintf(Dum1UChar, "00000000000000000000000000000000"); // DataBuffer (result) in binary (32 bits);
  sprintf(Dum2UChar, "00000000");  // Checksum in binary (8 bits).

  for (Loop1UInt16 = 0; Loop1UInt16 < DhtStepCount; ++Loop1UInt16)
  {
    if (DebugBitMask & DEBUG_DHT)
    {
      if (Loop1UInt16 == 0)
      {
        sprintf(String, "Step: %2.2u  Level: %c  ResultValue: %6u   (Clock sends a read request to DHT22)\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);
      }


      if (Loop1UInt16 == 1)
      {
        sprintf(String, "Step: %2.2u  Level: %c  ResultValue: %6u   (Waiting for DHT22 to answer back)\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);
      }
     
      if (Loop1UInt16 == 2)
      {
        sprintf(String, "Step: %2.2u  Level: %c  ResultValue: %6u   (1st <get ready> received from DHT22 - around 70 usec)\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);
      }

      if (Loop1UInt16 == 3)
      {
        sprintf(String, "Step: %2.2u  Level: %c  ResultValue: %6u   (2nd <get ready> received from DHT22 - around 70 usec)\r\r\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
        uart_send(__LINE__, String);
      }
    }



    /* Skip the two "Get ready" levels received from DHT22 (first: Low level, then High level, both 80 microseconds). */
    if (Loop1UInt16 > 3)
    {
      if (Level[Loop1UInt16] == 'L')
      {
        /* Low level, it is a first half-bit. */
        if (DebugBitMask & DEBUG_DHT)
        {
          sprintf(String, "Current bit: %u\r", CurrentBit);
          uart_send(__LINE__, String);

          sprintf(String, "Step: %2u   Level: %c   (should be around 50 usec)   ResultValue: %2llu\r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
          uart_send(__LINE__, String);
        }
      }
      else
      {
        /* High level, it is a second half-bit... assume it is a "zero" bit on entry. */
        if (CurrentBit < 32)
          DataBuffer <<= 1;
        else
          Checksum <<= 1;


        /* Now check if our assumption was correct. 0-bit should be around 26 usecond. */
        if (DhtResultValue[Loop1UInt16] > 32)
        {
          /* It was a "one" bit. DataBuffer has already been shifted left above, simply add 1 for current "one" bit. */
          if (CurrentBit < 32)
            ++DataBuffer;
          else
            ++Checksum;
        }


        if (DebugBitMask & DEBUG_DHT)
        {
          /* Compose DataBuffer in binary. */
          for (Loop2UInt16 = 0; Loop2UInt16 < 32; ++Loop2UInt16)
          {
            if (DataBuffer & (1 << Loop2UInt16))
              Dum1UChar[31 - Loop2UInt16] = '1'; // if this is a one bit.
            else
              Dum1UChar[31 - Loop2UInt16] = '0'; // if this is a zero bit.
          }
          
          /* Compose Checksum in binary. */
          for (Loop2UInt16 = 0; Loop2UInt16 < 8; ++Loop2UInt16)
          {
            if (Checksum & (1 << Loop2UInt16))
              Dum2UChar[7 - Loop2UInt16] = '1'; // if this is a one bit.
            else
              Dum2UChar[7 - Loop2UInt16] = '0'; // if this is a zero bit.
          }

          sprintf(String, "Step: %2u   Level: %c   (26 usec = 0  70 usec = 1)   ResultValue: %2llu  \r", Loop1UInt16, LogicLevel[Loop1UInt16], DhtResultValue[Loop1UInt16]);
          uart_send(__LINE__, String);

          sprintf(String, "DataBuffer in binary: %s\r", Dum1UChar);
          uart_send(__LINE__, String);

          sprintf(String, "DataBuffer:           0x%8.8X\r", DataBuffer);
          uart_send(__LINE__, String);

          sprintf(String, "Checksum in binary:   %s\r", Dum2UChar);
          uart_send(__LINE__, String);

          sprintf(String, "Checksum:             0x%2.2X\r\r", Checksum);
          uart_send(__LINE__, String);
        }

        ++CurrentBit;
      }
    }
  }



  /* Checksum validation. */
  ++DhtReadings; // one more reading from DHT22.
  ComputedCheck = 0;

  uart_send(__LINE__, "\r");

  /* Compute the checksum of data bytes received. */
  for (Loop1UInt16 = 0; Loop1UInt16 < 4; ++Loop1UInt16)
  {
    Value = ((DataBuffer >> (24 - (8 * Loop1UInt16))) & 0xFF);

    /* Compose each part of the checksum in binary. */
    sprintf(Dum2UChar, "00000000");
    for (Loop2UInt16 = 0; Loop2UInt16 < 8; ++Loop2UInt16)
    {
      if (Value & (1 << Loop2UInt16))
        Dum2UChar[7 - Loop2UInt16] = '1'; // if this is a one bit.
      else
        Dum2UChar[7 - Loop2UInt16] = '0'; // if this is a zero bit.
    }

    if (DebugBitMask & DEBUG_DHT)
    {
      sprintf(String, "Computing Checksum (part %u): 0x%2.2X     %s\r", Loop1UInt16 + 1, Value, Dum2UChar);
      uart_send(__LINE__, String);
    }
    
    ComputedCheck += Value;
    ComputedCheck &= 0xFF; // when adding, may become more than 8 bits.
  }
  uart_send(__LINE__, "\r");



  /* Compare the computed checksum with the one received from DHT22 and return an error code in case of error. */
  if (ComputedCheck != Checksum)
  {
    if (DebugBitMask & DEBUG_DHT)
    {
      sprintf(String, "=== ERROR === ComputedCheck: 0x%2.2X   Checksum: 0x%2.2X\r\r", ComputedCheck, Checksum);
      uart_send(__LINE__, String);
    }

    ++DhtErrors;

    return 0xFF;
  }



  /* If no error, compute the temperature and relative humidity from received data. */
  Byte[3] =  (DataBuffer & 0xFF);
  Byte[2] = ((DataBuffer >>  8) & 0xFF);
  Byte[1] = ((DataBuffer >> 16) & 0xFF);
  Byte[0] = ((DataBuffer >> 24) & 0xFF);

  /* Decode temperature. */
  *Temperature = (float)((((Byte[2] & 0x7F) << 8) | Byte[3]) / 10.0);
  if (Byte[2] & 0x80) *Temperature *= -1;

  /* Decode relative humidity. */
  *Humidity = (float)((((Byte[0] & 0x7F) << 8) | Byte[1]) / 10.0);

  if (DebugBitMask & DEBUG_DHT)
  {
    sprintf(String, "=== SUCCESS === ComputedCheck: 0x%2.2X   Checksum: 0x%2.2X\r\r", ComputedCheck, Checksum);
    uart_send(__LINE__, String);

    sprintf(String, "DataBuffer:       %8.8X\r", DataBuffer);
    uart_send(__LINE__, String);

    sprintf(String, "DataBuffer >>  8: %8.8X\r", (DataBuffer >> 8));
    uart_send(__LINE__, String);

    sprintf(String, "DataBuffer >> 16: %8.8X\r", (DataBuffer >> 16));
    uart_send(__LINE__, String);

    sprintf(String, "DataBuffer >> 24: %8.8X\r", (DataBuffer >> 24));
    uart_send(__LINE__, String);

    for (Loop1UInt16 = 0; Loop1UInt16 < 4; ++Loop1UInt16)
    {
      sprintf(String, "Byte[%u]: %2.2X\r", Loop1UInt16, Byte[Loop1UInt16]);
      uart_send(__LINE__, String);
    }

    sprintf(String, "Temperature: %f 'C\r", *Temperature);
    uart_send(__LINE__, String);

    sprintf(String, "Humidity:    %f %c\r\r", *Humidity, 0x25);
    uart_send(__LINE__, String);
  }

  return 0;
}
#endif  // DHT22_SUPPORT





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
  UINT8 CharacterBuffer; // buffer to temporarily hold a character (to keep track of "indicators" status).
  UINT8 RowNumber;       // row number in the framebuffer.
  UINT8 SectionNumber;   // section number in the framebuffer.


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
    DisplayBuffer[RowNumber] = (DisplayBuffer[RowNumber] & (~0x03)) | CharacterBuffer; // restore the "indicators" status.
  }

  return;
}





/* $PAGE */
/* $TITLE=scroll_queue() */
/* ------------------------------------------------------------------ *\
               Queue the given tag in the scroll queue.
            Use the queue algorithm where one slot is lost.
              This prevents the use of a lock mechanism.
\* ------------------------------------------------------------------ */
UINT8 scroll_queue(UINT8 Tag)
{
  UINT8 Dum1UChar;


  /* Check if the circular buffer is full. */
  if (((ScrollQueueTail > 0) && (ScrollQueueHead == ScrollQueueTail - 1)) || ((ScrollQueueTail == 0) && (ScrollQueueHead == MAX_SCROLL_QUEUE - 1)))
  {
    /* Scroll queue is full, return error code. */
    return MAX_SCROLL_QUEUE;
  }

  /* If there is at least one slot available in the queue, insert the tag for that string to be scrolled. */
  ScrollQueue[ScrollQueueHead] = Tag;
  Dum1UChar = ScrollQueueHead;
  ++ScrollQueueHead;

  if (ScrollQueueHead >= MAX_SCROLL_QUEUE)
    ScrollQueueHead = 0; // revert to zero when reaching out-of-bound.

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
void scroll_string(UINT8 StartColumn, UCHAR *String)
{
  UINT16 Loop1UInt;


  if (CurrentClockMode != MODE_SCROLLING)
  {
    /* Nothing scrolling on LED display... Clear framebuffer on entry. */
    clear_framebuffer(0);

    /* If no string is scrolling, use the start column specified. (The first two columns are used by LED indicators). */
    if (StartColumn == 0)
      ScrollDotCount = 1; // will be auto-incremented in the fill_display_buffer_5X7() function call.
    else
      ScrollDotCount = StartColumn;
  }
  else
  {
    /* There is something currently scrolling on LED display. Append 2 spaces to previous text, then the new string. */
    for (Loop1UInt = 0; Loop1UInt < 2; ++Loop1UInt)
    {
      if (ScrollDotCount >= (DISPLAY_BUFFER_SIZE - 9)) // make sure we have enough space to add spaces.
      {
        /* We're approaching the end of the virtual framebuffer... wait a few seconds for the display buffer to be partly cleared. */
        sleep_ms(SCROLL_DOT_TIME * 20);
      }

      /* Add two "space separators" before concatenating next string. */
      ScrollDotCount = fill_display_buffer_5X7(ScrollDotCount, ' ');
      ScrollDotCount = fill_display_buffer_5X7(ScrollDotCount, ' ');
    }
  }

  /* Insert the new string in the framebuffer. */
  for (Loop1UInt = 0; Loop1UInt < strlen(String); ++Loop1UInt)
  {
    while (ScrollDotCount >= (DISPLAY_BUFFER_SIZE - 6))
    {
      /* We are approaching the end of the framebuffer... wait a few seconds for the buffer to be partly cleared. */
      sleep_ms(SCROLL_DOT_TIME * 6 * 10); // wait for about 10 characters to be scrolled.
    }
    ScrollDotCount = fill_display_buffer_5X7(++ScrollDotCount, String[Loop1UInt]);

    CurrentClockMode = MODE_SCROLLING;
    FlagScrollStart = FLAG_ON;
  }

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


  /* Check if the scroll queue is empty. */
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
/* $TITLE=select_weekday() */
/* ------------------------------------------------------------------ *\
           Turn on clock indicator for selected day-of-week,
                       and turn OFF all others.
             (10 is a special case and turn them all OFF).
\* ------------------------------------------------------------------ */
void select_weekday(UINT8 DayNumber)
{
  switch (DayNumber)
  {
    case 0:
      IndicatorMondayOn;
      IndicatorTuesdayOff;
      IndicatorWednesdayOff;
      IndicatorThursdayOff;
      IndicatorFridayOff;
      IndicatorSaturdayOff;
      IndicatorSundayOff;
    break;
  
    case 1:
      IndicatorMondayOff;
      IndicatorTuesdayOn;
      IndicatorWednesdayOff;
      IndicatorThursdayOff;
      IndicatorFridayOff;
      IndicatorSaturdayOff;
      IndicatorSundayOff;
    break;
  
    case 2:
      IndicatorMondayOff;
      IndicatorTuesdayOff;
      IndicatorWednesdayOn;
      IndicatorThursdayOff;
      IndicatorFridayOff;
      IndicatorSaturdayOff;
      IndicatorSundayOff;
    break;
  
    case 3:
      IndicatorMondayOff;
      IndicatorTuesdayOff;
      IndicatorWednesdayOff;
      IndicatorThursdayOn;
      IndicatorFridayOff;
      IndicatorSaturdayOff;
      IndicatorSundayOff;
    break;
    
    case 4:
      IndicatorMondayOff;
      IndicatorTuesdayOff;
      IndicatorWednesdayOff;
      IndicatorThursdayOff;
      IndicatorFridayOn;
      IndicatorSaturdayOff;
      IndicatorSundayOff;
    break;
  
    case 5:
      IndicatorMondayOff;
      IndicatorTuesdayOff;
      IndicatorWednesdayOff;
      IndicatorThursdayOff;
      IndicatorFridayOff;
      IndicatorSaturdayOn;
      IndicatorSundayOff;
    break;
    
    case 6:
      IndicatorMondayOff;
      IndicatorTuesdayOff;
      IndicatorWednesdayOff;
      IndicatorThursdayOff;
      IndicatorFridayOff;
      IndicatorSaturdayOff;
      IndicatorSundayOn;
    break;
  
    case 10:
      IndicatorMondayOff;
      IndicatorTuesdayOff;
      IndicatorWednesdayOff;
      IndicatorThursdayOff;
      IndicatorFridayOff;
      IndicatorSaturdayOff;
      IndicatorSundayOff;
    break;
  }

  return;
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
/* $TITLE=set_mode_out() */
/* ------------------------------------------------------------------ *\
        Program potentially modified clock parameters to RTC IC.
\* ------------------------------------------------------------------ */
void set_mode_out(void)
{
  UINT8 Loop1UInt8;


  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_HOUR] == FLAG_ON))
    set_hour(CurrentHourSetting);

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_MINUTE] == FLAG_ON))
    set_minute(CurrentMinuteSetting);

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_MONTH] == FLAG_ON))
  {
    set_month(CurrentMonth);
    set_day_of_week(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth));
    select_weekday(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) - 1);
  }

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_DAY_OF_MONTH] == FLAG_ON))
  {
    set_day_of_month(CurrentDayOfMonth);
    set_day_of_week(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth));
    select_weekday(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) - 1);
  }

  if ((FlagSetupRTC == FLAG_ON) && (FlagSetupClock[SETUP_YEAR] == FLAG_ON))
  {
    set_year(CurrentYearLowPart);
    set_day_of_week(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth));
    select_weekday(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) - 1);
  }

  FlagBlinking[SetupStep] = 0xFF;

  /*** Check the purpose of this code chunk. ***/
  if (FlagSetupAlarm[SETUP_ALARM_MINUTE] == FLAG_ON)
  {
    fill_display_buffer_4X7(13, (FlashConfig.Alarm[AlarmNumber].Minute / 10 + '0') & FlagBlinking[4]);
    fill_display_buffer_4X7(18, (FlashConfig.Alarm[AlarmNumber].Minute % 10 + '0') & FlagBlinking[4]);
  }
  /***/

  /*** Check the purpose of this code chunk. ***
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
          Exit setup mode because of inactivity (time-out).
\* ------------------------------------------------------------------ */
void set_mode_timeout(void)
{
  ScrollSecondCounter = 0; // reset number of seconds to reach time-to-scroll.
  SetupSource = SETUP_SOURCE_NONE;
  FlagIdleCheck = FLAG_OFF;
  FlagSetAlarm = FLAG_OFF;  // reset alarm setup mode flag when timed-out.
  FlagSetClock = FLAG_OFF;  // reset clock setup mode flag when timed-out.
  FlagSetTimer = FLAG_OFF;  // reset timer setup mode flag when timed-out.
  FlagUpdateTime = FLAG_ON; // let's display current time on the clock, now.

  return;
}





/* $PAGE */
/* $TITLE=setup_alarm_frame() */
/* ------------------------------------------------------------------ *\
                   Display current alarm parameters.
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
  if (SetupStep == SETUP_ALARM_NUMBER || SetupStep == SETUP_ALARM_ON_OFF)
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
    /* Note: day-of-week indicator for alarm setting will blink in the funtion "evaluate_blinking_time()". */
  }

  /* Check if we are done with all alarm setting. */
  if (SetupStep >= SETUP_ALARM_HI_LIMIT)
  {
    /* If alarm #1 is turned on, write it to the RTC IC. */
    if ((AlarmNumber == 0) && (FlashConfig.Alarm[0].FlagStatus == FLAG_ON))
    {
      set_alarm1_clock(ALARM_MODE_HOUR_MIN_SEC_MATCHED, 00, FlashConfig.Alarm[0].Minute, FlashConfig.Alarm[0].Hour, FlashConfig.Alarm[0].Day);
    }

    /* If alarm #2 is turned on, write it to the RTC IC. */
    if ((AlarmNumber == 1) && (FlashConfig.Alarm[1].FlagStatus == FLAG_ON))
    {
      set_alarm2_clock(FlashConfig.Alarm[1].Minute, FlashConfig.Alarm[1].Hour, FlashConfig.Alarm[1].Day);
    }

    /* Reset all related flags and variables when alarm setup is done. */
    FlagSetAlarm = FLAG_OFF;  // reset alarm setup flag when done.
    FlagIdleCheck = FLAG_OFF; // no more checking for a timeout.
    FlagTone = FLAG_OFF;      // reset flag tone.
    FlagUpdateTime = FLAG_ON; // it's now time to display time on clock display.
    IdleNumberOfSeconds = 0;  // reset the number of seconds the system has been idle.
    ScrollSecondCounter = 0;  // reset number of seconds to reach time-to-scroll (obsolete).
    SetupSource = SETUP_SOURCE_NONE;
    SetupStep = SETUP_NONE;

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
      if (AlarmNumber > 8)
        AlarmNumber = 0;
    }
    else
    {
      --AlarmNumber;
      if (AlarmNumber > 8)
        AlarmNumber = 8;
    }
  }

  /* Toggle alarm On / Off. */
  if (FlagSetupAlarm[SETUP_ALARM_ON_OFF] == FLAG_ON)
  {
    if (FlashConfig.Alarm[AlarmNumber].FlagStatus == FLAG_ON)
      FlashConfig.Alarm[AlarmNumber].FlagStatus = FLAG_OFF;
    else
      FlashConfig.Alarm[AlarmNumber].FlagStatus = FLAG_ON;

    /* We now have 9 alarms available... Check if at least one alarm is On and if ever the case, turn On both alarm indocators. */
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
        FlashConfig.Alarm[AlarmNumber].Hour = 0; // reset to zero when reaching out-of-bound.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Alarm Hour" set mode. */
      --FlashConfig.Alarm[AlarmNumber].Hour;
      if (FlashConfig.Alarm[AlarmNumber].Hour == 255)
        FlashConfig.Alarm[AlarmNumber].Hour = 23; // reset to 23 when reaching out-of-bound.
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
        FlashConfig.Alarm[AlarmNumber].Minute = 0; // reset to zero when reaching out-of-bound.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Alarm Minute" set mode. */
      --FlashConfig.Alarm[AlarmNumber].Minute;
      if (FlashConfig.Alarm[AlarmNumber].Minute == 255)
        FlashConfig.Alarm[AlarmNumber].Minute = 59; // reset to 59 when reaching out-of-bound.
    }
  }

  /* Check if we are setting up "Alarm Day". */
  if (FlagSetupAlarm[SETUP_ALARM_DAY] == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      if (DebugBitMask & DEBUG_ALARMS)
      {
        uart_send(__LINE__, "Received FLAG_UP\r");
      }

      /* User pressed the "Up" (middle) button, increment potential alarm target day-of-week. */
      ++AlarmTargetDay;
      if (AlarmTargetDay >= 8)
        AlarmTargetDay = 1; // when reaching out-of-bound, revert to 1.
    }

    if (FlagButtonSelect == FLAG_DOWN)
    {
      if (DebugBitMask & DEBUG_ALARMS)
      {
        uart_send(__LINE__, "Received FLAG_DOWN\r");
      }

      /* User presser the "Down" (bottom) button, proceed through all day-of-week. */
      --AlarmTargetDay;
      if ((AlarmTargetDay == 0) || (AlarmTargetDay >= 8))
        AlarmTargetDay = 7; // when reaching out-of-bound, revert to 1.
    }

    if (FlagButtonSelect == FLAG_LONG_UP)
    {
      FlashConfig.Alarm[AlarmNumber].Day |= (1 << (AlarmTargetDay - 1));

      if (DebugBitMask & DEBUG_ALARMS)
      {
        uart_send(__LINE__, "Received FLAG_LONG_UP\r");
      }
    }

    if (FlagButtonSelect == FLAG_LONG_DOWN)
    {
      FlashConfig.Alarm[AlarmNumber].Day &= (~(1 << (AlarmTargetDay - 1)));

      if (DebugBitMask & DEBUG_ALARMS)
      {
        uart_send(__LINE__, "Received FLAG_LONG_DOWN\r");
      }
    }
  }

  if (DebugBitMask & DEBUG_ALARMS)
  {
    sprintf(String, "AlarmTargetDay = %u   (%s)\r", AlarmTargetDay, DayName[FlashConfig.Language][AlarmTargetDay]);
    uart_send(__LINE__, String);

    /* Identify all day-of-week targets for current alarm. */
    DayMask[0] = 0x00; // initialize bitmask.
    for (Loop1UInt8 = 7; Loop1UInt8 > 0; --Loop1UInt8)
    {
      if (FlashConfig.Alarm[AlarmNumber].Day & (1 << (Loop1UInt8 - 1)))
        sprintf(&DayMask[strlen(DayMask)], "1");
      else
        sprintf(&DayMask[strlen(DayMask)], "0");
    }
    sprintf(String, "[%X] Alarm[%2.2u].DayMask:     %s     (%X) ", &FlashConfig.Alarm[AlarmNumber].Day, AlarmNumber, DayMask, FlashConfig.Alarm[AlarmNumber].Day);

    for (Loop1UInt8 = 1; Loop1UInt8 < 8; ++Loop1UInt8)
    {
      if (FlashConfig.Alarm[AlarmNumber].Day & (1 << (Loop1UInt8 - 1)))
      {
        Dum1UInt8 = strlen(String);
        sprintf(&String[strlen(String)], "%s", DayName[FlashConfig.Language][Loop1UInt8]);
        String[Dum1UInt8 + 3] = 0x20; // space separator.
        String[Dum1UInt8 + 4] = 0x00; // keep first 3 characters of day name.
      }
    }

    sprintf(&String[strlen(String)], "\r");
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
  UINT8 Loop1UInt8;


  /* Make sure the function has not been called by mistake. */
  if (SetupStep == SETUP_NONE)
    return;

  CurrentClockMode = MODE_CLOCK_SETUP;

  /* ---------------- Read the real-time clock IC. ------------------ */
  Time_RTC = Read_RTC();

  Time_RTC.seconds = Time_RTC.seconds & 0x7F;
  Time_RTC.minutes = Time_RTC.minutes & 0x7F;
  Time_RTC.hour = Time_RTC.hour & 0x3F;
  Time_RTC.dayofweek = Time_RTC.dayofweek & 0x07;
  Time_RTC.dayofmonth = Time_RTC.dayofmonth & 0x3F;
  Time_RTC.month = Time_RTC.month & 0x1F;
  CurrentDayOfWeek = bcd_to_byte(Time_RTC.dayofweek) + 1;
  if (CurrentDayOfWeek == 8)
    CurrentDayOfWeek = 1; // Sunday equals 1.

  /* ------------------- Setup Hour and Minute ---------------------- */
  /* Setup "Hours" and "Minutes". */
  if ((SetupStep == SETUP_HOUR) || (SetupStep == SETUP_MINUTE))
  {
    if (SetupStep == SETUP_HOUR)
    {
      /* Setup "hours". */
      if (FlagSetupClock[SETUP_HOUR] == FLAG_OFF)
      {
        /* Decode current hour. */
        CurrentHourSetting = bcd_to_byte(Time_RTC.hour);
      }
      FlagSetupClock[SETUP_HOUR] = FLAG_ON;
    }

    if (SetupStep == SETUP_MINUTE)
    {
      /* Setup "minutes". */
      if (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF)
      {
        /* Decode current minute. */
        CurrentMinuteSetting = bcd_to_byte(Time_RTC.minutes);
      }
      FlagSetupClock[SETUP_MINUTE] = FLAG_ON;
    }

    if (FlashConfig.TimeDisplayMode == H12)
    {
      CurrentHour = convert_h24_to_h12(CurrentHourSetting, &AmFlag, &PmFlag);
      (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
      (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
    }
    else
    {
      /* We are in "24-hours" display mode. */
      CurrentHour = CurrentHourSetting;
    }

    /* When in 12-hour time display format, first digit is not displayed if it is zero. */
    if ((FlashConfig.TimeDisplayMode == H12) && (CurrentHour < 10))
    {
      fill_display_buffer_4X7(0, ' ' & FlagBlinking[SETUP_HOUR]);
    }
    else
      fill_display_buffer_4X7(0, (CurrentHour / 10 + '0') & FlagBlinking[SETUP_HOUR]);

    fill_display_buffer_4X7(5, (CurrentHour % 10 + '0') & FlagBlinking[SETUP_HOUR]);
    fill_display_buffer_4X7(10, 0x3A); // slim ":"
    fill_display_buffer_4X7(12, (CurrentMinute / 10 + '0') & FlagBlinking[SETUP_MINUTE]);
    fill_display_buffer_4X7(17, (CurrentMinute % 10 + '0') & FlagBlinking[SETUP_MINUTE]);
  }

  /* --------------- Setup Month and Day-of-month ------------------- */
  if ((SetupStep == SETUP_DAY_OF_MONTH) || (SetupStep == SETUP_MONTH))
  {
    /* There are conditions where Day-of-month can be higher than allowed. Re-adjust if it is the case. */
    /* For example, we set March 31st, then we change March to April while leaving 31 as the day. */
    Year4Digits = FlashConfig.CurrentYearCentile * 100 + CurrentYearLowPart;
    Dum1UChar = get_month_days(Year4Digits, CurrentMonth);
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

    if (FlashConfig.Language == FRENCH)
    {
      /* In French, display and set day-of-month before month.
         Make a trick in the flashing below. */
      fill_display_buffer_4X7(0, ((CurrentDayOfMonth / 10 + 0x30) & FlagBlinking[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(5, ((CurrentDayOfMonth % 10 + 0x30) & FlagBlinking[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(10, '-');
      fill_display_buffer_4X7(13, ((CurrentMonth / 10 + 0x30) & FlagBlinking[SETUP_MONTH]));
      fill_display_buffer_4X7(18, ((CurrentMonth % 10 + 0x30) & FlagBlinking[SETUP_MONTH]));
    }
    else
    {
      /* For English and other languages. */
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
    Year4Digits = FlashConfig.CurrentYearCentile * 100 + CurrentYearLowPart;

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
    fill_display_buffer_5X7(2, 'D');
    fill_display_buffer_5X7(8, 'S');
    fill_display_buffer_5X7(14, 'T');

    switch (FlashConfig.DaylightSavingTime)
    {
      default:
      case (DST_NONE):
        fill_display_buffer_4X7(18, ('0' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_NORTH_AMERICA):
        fill_display_buffer_4X7(18, ('1' & FlagBlinking[SETUP_DST]));
      break;

      case (DST_EUROPE):
        fill_display_buffer_4X7(18, ('2' & FlagBlinking[SETUP_DST]));
      break;
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
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
    fill_display_buffer_4X7(11, ':');

    switch (FlashConfig.Language)
    {
      default:
      case (ENGLISH):
        fill_display_buffer_5X7(15, ('E' & FlagBlinking[SETUP_LANGUAGE]));
      break;

      case (FRENCH):
        fill_display_buffer_5X7(15, ('F' & FlagBlinking[SETUP_LANGUAGE]));
      break;

      case (SPANISH):
        /* To be implemented. */
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
      CurrentHourSetting = bcd_to_byte(Time_RTC.hour);
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
    IdleNumberOfSeconds = 0;         // reset the number of seconds the system has been idle.
    SetupSource = SETUP_SOURCE_NONE; // reset setup source when done.
    SetupStep = SETUP_NONE;          // reset SetupStep when done.
    FlagIdleCheck = FLAG_OFF;        // getting out of setup mode, no more watching for potential time-out.
    FlagSetClock = FLAG_OFF;         // getting out of clock setup mode.
    FlagTone = FLAG_OFF;             // reset flag tone.
    FlagUpdateTime = FLAG_ON;        // it's now time to update time display on the clock.

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
      if (CurrentHourSetting == 24)
        CurrentHourSetting = 0; // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Hour" setup mode. */
      --CurrentHourSetting;

      /* If we go below zero (out-of-bound), set hour to 23. */
      if (CurrentHourSetting == 255)
        CurrentHourSetting = 23;
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
      if (CurrentMinuteSetting == 60)
        CurrentMinuteSetting = 0;
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Minute" setup mode. */
      --CurrentMinuteSetting;

      /* If we go below zero (out-of-bound), set minutes to 59. */
      if (CurrentMinuteSetting == 255)
        CurrentMinuteSetting = 59;
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
       sure Year4Digits is correct before determining DayOfMonth (for leap years). */
    Year4Digits = (FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart;

    if (CurrentDayOfMonth > get_month_days(Year4Digits, CurrentMonth))
    {
      /* If we are over this month's maximum day, revert to last day of this month. */
      CurrentDayOfMonth = get_month_days(Year4Digits, CurrentMonth);
    }
  }

  /* ------------------- Day-of-month setting ----------------------- */
  if (FlagSetupClock[SETUP_DAY_OF_MONTH] == FLAG_ON)
  {
    /* We are setting up day-of-month. */
    FlagSetupRTC = FLAG_ON; // indicate we must program the RTC IC.

    /* Make sure Year4Digits is correct before determining maximum day-of-month (for leap years). */
    Year4Digits = (FlashConfig.CurrentYearCentile * 100) + CurrentYearLowPart;

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "day-of-month" setup mode. */
      ++CurrentDayOfMonth;

      /* If we pass the maximum day-of-month for current month (out-of-bound), roll-back to 1. */
      if (CurrentDayOfMonth > get_month_days(Year4Digits, CurrentMonth))
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
        CurrentDayOfMonth = get_month_days(Year4Digits, CurrentMonth);
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
      ++FlashConfig.DaylightSavingTime;
      if (FlashConfig.DaylightSavingTime >= DST_HI_LIMIT)
        FlashConfig.DaylightSavingTime = DST_NONE;
    }
    else
    {
      /* Must be "Button down". */
      --FlashConfig.DaylightSavingTime;
      if (FlashConfig.DaylightSavingTime == 255)
        FlashConfig.DaylightSavingTime = DST_HI_LIMIT - 1;
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
       go through different language options (French / English). */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++FlashConfig.Language;
      if (FlashConfig.Language > FRENCH)
        FlashConfig.Language = ENGLISH;
    }
    else
    {
      /* Must be "Button down". */
      --FlashConfig.Language;
      if (FlashConfig.Language < ENGLISH)
        FlashConfig.Language = FRENCH;
    }

    /* Setup order in French is different than in English. */
    if (FlashConfig.Language == FRENCH)
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
        CurrentHour = convert_h24_to_h12(CurrentHourSetting, &AmFlag, &PmFlag);
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
    IdleNumberOfSeconds = 0;         // reset the number of seconds the system has been idle.
    FlagSetTimerCountDown = FLAG_ON; // make count down timer active.
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

    IdleNumberOfSeconds = 0; // reset the number of seconds the system has been idle.
    UpId = 0;
    ScrollSecondCounter = 0;         // reset number of seconds to reach time-to-scroll.
    SetupStep = SETUP_NONE;          // reset SetupStep for next pass.
    SetupSource = SETUP_SOURCE_NONE; // exit timer setup mode.
    FlagSetTimer = FLAG_OFF;         // reset "up" button.
    FlagIdleCheck = FLAG_OFF;        // no more checking for an idle time-out.
    FlagTone = FLAG_OFF;             // reset flag tone.
    FlagUpdateTime = FLAG_ON;        // setup is over, it becomes time to display time.

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


  /* Read the real-time clock IC. */
  Time_RTC = Read_RTC();

  Time_RTC.seconds = Time_RTC.seconds & 0x7F;
  Time_RTC.minutes = Time_RTC.minutes & 0x7F;
  Time_RTC.hour = Time_RTC.hour & 0x3F;
  Time_RTC.dayofweek = Time_RTC.dayofweek & 0x07;
  Time_RTC.dayofmonth = Time_RTC.dayofmonth & 0x3F;
  Time_RTC.month = Time_RTC.month & 0x1F;

  CurrentHourSetting = bcd_to_byte(Time_RTC.hour);
  CurrentMinute = bcd_to_byte(Time_RTC.minutes);
  CurrentDayOfMonth = bcd_to_byte(Time_RTC.dayofmonth);
  CurrentMonth = bcd_to_byte(Time_RTC.month);
  CurrentYearLowPart = bcd_to_byte(Time_RTC.year);
  CurrentDayOfWeek = bcd_to_byte(Time_RTC.dayofweek) + 1;
  if (CurrentDayOfWeek == 8)
    CurrentDayOfWeek = 1; // Sunday equals 1.

  /* Check if we are in 12-hours or 24-hours time format. */
  if (FlashConfig.TimeDisplayMode == H12)
  {
    CurrentHour = convert_h24_to_h12(CurrentHourSetting, &AmFlag, &PmFlag);
    (AmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 0)) : (DisplayBuffer[4] &= ~(1 << 0));
    (PmFlag == FLAG_ON) ? (DisplayBuffer[4] |= (1 << 1)) : (DisplayBuffer[4] &= ~(1 << 1));
  }
  else
  {
    /* We are in "24-hours" display mode, nothing to do to what we read from the real-time clock. */
    CurrentHour = CurrentHourSetting;
  }

  /* When in 12-hour time display format, first digit is not displayed if it is zero. */
  if ((FlashConfig.TimeDisplayMode == H12) && (CurrentHour < 10))
  {
    TimeBuffer[0] = (' '); // hours first digit.
  }
  else
    TimeBuffer[0] = ((CurrentHour / 10) + '0'); // hours first digit.

  TimeBuffer[1] = ((CurrentHour % 10) + '0');      // hours second digit.
  TimeBuffer[2] = ((Time_RTC.minutes / 16) + '0'); // minutes first digit.
  TimeBuffer[3] = ((Time_RTC.minutes % 16) + '0'); // minutes second digit.
  SecondCounter = ((float)Time_RTC.seconds) / 1.5;

  /* Display "time of day" if we are not scrolling some data. */
  if (FlagScrollStart == FLAG_OFF)
  {
    CurrentClockMode = MODE_SHOW_TIME;

    fill_display_buffer_4X7(0, TimeBuffer[0]);
    fill_display_buffer_4X7(5, TimeBuffer[1]);
    fill_display_buffer_4X7(10, 0x3A); // slim ":"
    fill_display_buffer_4X7(12, TimeBuffer[2]);
    fill_display_buffer_4X7(17, TimeBuffer[3]);
  }

  /* Turn ON the weekday indicator. */
  select_weekday(Time_RTC.dayofweek - 1);

  return;
}





/* $PAGE */
/* $TITLE=sound_callback_ms() */
/* ------------------------------------------------------------------ *\
          Sound callback function (50 millisecond period).
        When called, will check if there are sounds to play.
           If not, wait for next call in 50 milliseconds.
         If there are sound to play, manage sound timing.
\* ------------------------------------------------------------------ */
bool sound_callback_ms(struct repeating_timer *Timer50MSec)
{
  UCHAR String[128];

  UINT32 CounterHiLimit;

  static UINT16 ActiveMSeconds;
  static UINT16 ActiveMSecCounter;
  static UINT16 ActiveRepeatCount;
  static UINT16 CurrentRepeat;
  static UINT16 DutyCycle = 50; // initialize PWM (sound wave) duty cycle.
  static UINT8 FlagActiveSound;
  static UINT8 FlagPassiveSound;
  static UINT16 Frequency;
  static UINT16 PassiveMSeconds;
  static UINT16 PassiveMSecCounter;


  /* ========================================================= *\
                     Handling of active buzzer
              (The one integrated in the Green Clock)
  \* ========================================================= */
  /* If there is a sound on-going on the avtive buzzer, check if it is completed. */
  if (FlagActiveSound == FLAG_ON)
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      if (ActiveRepeatCount == SILENT)
        uart_send(__LINE__, "- A Silence\r");
      else
        uart_send(__LINE__, "- A Sounding\r");
    }

    ActiveMSecCounter += 50; // 50 milliseconds more since last callback.

    if (ActiveMSecCounter >= ActiveMSeconds)
    {
      if (DebugBitMask & DEBUG_SOUND_QUEUE)
      {
        uart_send(__LINE__, "- A Shutoff\r");
      }

      /* Current sound is over on active buzzer. Turn sound off to make a cut with next sound. */
      gpio_put(BUZZ, 0);

      /* One more repeat count has been done. */
      ++CurrentRepeat;
      if (CurrentRepeat > ActiveRepeatCount)
      {
        CurrentRepeat = 0;
      }

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
      if (ActiveRepeatCount != SILENT)
      {
        gpio_put(BUZZ, 1);
      }

      FlagActiveSound = FLAG_ON;
      ActiveMSecCounter = 0;
    }
    else
    {
      /* Check if there are more sounds to play on active buzzer. Request duration and repeat count for next sound. */
      if (sound_unqueue_active(&ActiveMSeconds, &ActiveRepeatCount) == 0xFF)
      {
        /* Either there is no more sound, either there was an error while trying to unqueue next sound (corrupted sound queue).
           NOTE: Queue head and queue tail are returned in case of error. */
        /* If sound_unqueue_active() failed, make sure audio is turned off. */
        gpio_put(BUZZ, 0);
      }
      else
      {
        if (DebugBitMask & DEBUG_SOUND_QUEUE)
        {
          sprintf(String, "- A Unqueuing %4u   %4u\r", ActiveMSeconds, ActiveRepeatCount);
          uart_send(__LINE__, String);
        }

        /* If RepeatCount is 0 ("SILENT"), we wanted to wait for specified duration without any sound. */
        if (ActiveRepeatCount != SILENT)
        {
          gpio_put(BUZZ, 1);
        }

        FlagActiveSound = FLAG_ON;
        CurrentRepeat = 1;
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
        uart_send(__LINE__, "- P Silence\r");
      else
        uart_send(__LINE__, "- P Sounding\r");
    }

    /* There is a sound on-going on the passive buzzer, check if it is completed. */
    PassiveMSecCounter += 50; // 50 milliseconds more since last callback.

    if (PassiveMSecCounter >= PassiveMSeconds)
    {
      if (DebugBitMask & DEBUG_SOUND_QUEUE)
      {
        uart_send(__LINE__, "- P Shutoff\r");
      }

      /* Current sound on passive buzzer is over. Turn sound Off to make a cut with next sound. */
      pwm_set_enabled(PwmSliceNumber, FALSE);

      /* No active sound for now. */
      FlagPassiveSound = FLAG_OFF;

      /* Reset sound elapsed time for now. */
      PassiveMSecCounter = 0;
    }
  }
  else
  {
    /* Check if there are more sounds to play on passive buzzer. Request frequency and duration for next sound. */
    if (sound_unqueue_passive(&Frequency, &PassiveMSeconds) == 0xFF)
    {
      /* Either there is no more sound for passive buzzer, either there was an error while trying to unqueue next sound (corrupted sound queue).
         NOTE: Queue head and queue tail are returned in case of error. */

      /* If sound_unqueue_passive() failed, make sure audio PWM is turned off. */
      pwm_set_enabled(PwmSliceNumber, FALSE);
    }
    else
    {
      if (DebugBitMask & DEBUG_SOUND_QUEUE)
      {
        sprintf(String, "- P Unqueuing %4u   %4u\r", Frequency, PassiveMSeconds);
        uart_send(__LINE__, String);
      }

      /* If frequency is 0 ("SILENT"), we wanted to wait for specified duration without any sound. */
      if (Frequency != SILENT)
      {
        /* Find CounterHiLimit (wrap value) required so that counter wraps to produce the target frequency / sound... */
        CounterHiLimit = (1000000 / Frequency) - 1;

        // ...and program it.
        pwm_set_wrap(PwmSliceNumber, CounterHiLimit);

        /* Calculate channel level from given duty cycle in %... */
        ChannelLevel = (CounterHiLimit * DutyCycle / 100);

        /* ...and program it. */
        pwm_set_chan_level(PwmSliceNumber, PwmChannelNumber, ChannelLevel);

        // Start PWM.
        pwm_set_enabled(PwmSliceNumber, TRUE);
      }

      FlagPassiveSound   = FLAG_ON;
      PassiveMSecCounter = 0;
    }
  }
  #endif

  return TRUE;
}





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


  /* Check if the sound circular buffer (sound queue) is full. */
  if (((SoundPassiveTail > 0) && (SoundPassiveHead == SoundPassiveTail - 1)) || ((SoundPassiveTail == 0) && (SoundPassiveHead == MAX_PASSIVE_SOUND_QUEUE - 1)))
  {
    /* Sound queue is full, return error code. */
    return MAX_PASSIVE_SOUND_QUEUE;
  }

  /* If there is at least one slot available in the queue, insert the sound to be played. */
  SoundQueuePassive[SoundPassiveHead].Freq = Frequency;
  SoundQueuePassive[SoundPassiveHead].MSec = MSeconds;
  ++SoundPassiveHead;

  /* Revert to zero when reaching out-of-bound. */
  if (SoundPassiveHead >= MAX_PASSIVE_SOUND_QUEUE)
    SoundPassiveHead = 0;

  if (DebugBitMask & DEBUG_SOUND_QUEUE)
  {
    if (SoundPassiveHead > SoundPassiveTail)
    {
      sprintf(String, "Passive queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_PASSIVE_SOUND_QUEUE - SoundPassiveHead + SoundPassiveTail - 1), SoundPassiveHead, SoundPassiveTail);
      uart_send(__LINE__, String);
    }

    if (SoundPassiveHead == SoundPassiveTail)
    {
      sprintf(String, "Passive queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_PASSIVE_SOUND_QUEUE - 1), SoundPassiveHead, SoundPassiveTail);
      uart_send(__LINE__, String);
    }

    if (SoundPassiveHead < SoundPassiveTail)
    {
      sprintf(String, "Passive queue remaining space: %5u  Head: %3u  Tail: %3u\r", (SoundPassiveTail - SoundPassiveHead - 1), SoundPassiveHead, SoundPassiveTail);
      uart_send(__LINE__, String);
    }
  }

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

  UINT32 CurrentTail;


  /* Check if the sound queue is empty. */
  if (SoundPassiveHead == SoundPassiveTail)
  {
    /* In case of empty queue or queue error, return queue head and queue tail instead. */
    *Frequency = SoundPassiveHead;
    *MSeconds = SoundPassiveTail;

    return 0xFF;
  }


  /* Extract data for next sound to play. */
  *Frequency = SoundQueuePassive[SoundPassiveTail].Freq;
  *MSeconds  = SoundQueuePassive[SoundPassiveTail].MSec;


  /* And reset this slot in the sound queue. */
  SoundQueuePassive[SoundPassiveTail].Freq = 0;
  SoundQueuePassive[SoundPassiveTail].MSec = 0;

  if (DebugBitMask & DEBUG_SOUND_QUEUE)
  {
    sprintf(String, "Passive unqueue (Tail): %5u\r", SoundPassiveTail);
    uart_send(__LINE__, String);
  }



  if (*MSeconds != 0)
  {
    /* The sound found in this slot is valid. */
    ++SoundPassiveTail;

    /* If reaching out-of-bound, revert to zero. */
    if (SoundPassiveTail >= MAX_PASSIVE_SOUND_QUEUE)
      SoundPassiveTail = 0;

    if  (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      sprintf(String, "Adjust Tail to:         %5u\r", SoundPassiveTail);
      uart_send(__LINE__, String);
    }

    return 0;
  }
  else
  {
    if  (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      sprintf(String, "Passive sound queue - housekeeping\r");
      uart_send(__LINE__, String);
    }

    /* If this slot was corrupted, make some housekeeping. */
    CurrentTail = SoundPassiveTail; // keep track of starting position in the queue.

    /* In case of empty queue or queue error, return queue head and queue tail instead of frequency and duration. */
    *Frequency = SoundPassiveHead;
    *MSeconds = SoundPassiveTail;

    do
    {
      SoundQueuePassive[SoundPassiveTail].Freq == 0;
      ++SoundPassiveTail;

      /* If reaching out-of-bound, revert to zero. */
      if (SoundPassiveTail >= MAX_PASSIVE_SOUND_QUEUE)
        SoundPassiveTail = 0;

      /* If we come back to where we started from, sound queue must be empty. */
      if (SoundPassiveTail == CurrentTail)
      {
        SoundPassiveHead = SoundPassiveTail;
        break; // get out of "do" loop.
      }
    } while (SoundQueuePassive[SoundPassiveTail].MSec == 0);

    return 0xFF;
  }
}
#endif





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


  /* Check if the sound circular buffer (sound queue) is full. */
  if (((SoundActiveTail > 0) && (SoundActiveHead == SoundActiveTail - 1)) || ((SoundActiveTail == 0) && (SoundActiveHead == MAX_ACTIVE_SOUND_QUEUE - 1)))
  {
    /* Sound queue is full, return error code. */
    return MAX_ACTIVE_SOUND_QUEUE;
  }

  /* If there is at least one slot available in the queue, insert the sound to be played. */
  SoundQueueActive[SoundActiveHead].MSec = MSeconds;
  SoundQueueActive[SoundActiveHead].RepeatCount = RepeatCount;
  ++SoundActiveHead;

  /* Revert to zero when reaching out-of-bound. */
  if (SoundActiveHead >= MAX_ACTIVE_SOUND_QUEUE)
    SoundActiveHead = 0;

  if (DebugBitMask & DEBUG_SOUND_QUEUE)
  {
    if (SoundActiveHead > SoundActiveTail)
    {
      sprintf(String, "Active queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_ACTIVE_SOUND_QUEUE - SoundActiveHead + SoundActiveTail - 1), SoundActiveHead, SoundActiveTail);
      uart_send(__LINE__, String);
    }

    if (SoundActiveHead == SoundActiveTail)
    {
      sprintf(String, "Active queue remaining space: %5u  Head: %3u  Tail: %3u\r", (MAX_ACTIVE_SOUND_QUEUE - 1), SoundActiveHead, SoundActiveTail);
      uart_send(__LINE__, String);
    }

    if (SoundActiveHead < SoundActiveTail)
    {
      sprintf(String, "Active queue remaining space: %5u  Head: %3u  Tail: %3u\r", (SoundActiveTail - SoundActiveHead - 1), SoundActiveHead, SoundActiveTail);
      uart_send(__LINE__, String);
    }
  }
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

  UINT32 CurrentTail;


  /* Check if the sound queue is empty. */
  if (SoundActiveHead == SoundActiveTail)
  {
    /* In case of empty queue or queue error, return queue head and queue tail instead. */
    *MSeconds = SoundActiveHead;
    *RepeatCount = SoundActiveTail;

    return 0xFF;
  }

  /* Extract data for next sound to play. */
  *MSeconds = SoundQueueActive[SoundActiveTail].MSec;
  *RepeatCount = SoundQueueActive[SoundActiveTail].RepeatCount;

  /* And reset this slot in the sound queue. */
  SoundQueueActive[SoundActiveTail].MSec = 0;
  SoundQueueActive[SoundActiveTail].RepeatCount = 0;

  if (DebugBitMask & DEBUG_SOUND_QUEUE)
  {
    sprintf(String, "Unqueuing MSec: %u   Repeat: %u\r", *MSeconds, *RepeatCount);
    uart_send(__LINE__, String);
  }

  if (*MSeconds != 0)
  {
    /* The sound found in this slot is valid. */
    ++SoundActiveTail;

    /* If reaching out-of-bound, revert to zero. */
    if (SoundActiveTail >= MAX_ACTIVE_SOUND_QUEUE)
      SoundActiveTail = 0;

    return 0;
  }
  else
  {
    if (DebugBitMask & DEBUG_SOUND_QUEUE)
    {
      sprintf(String, "Housekeeping from %u\r", CurrentTail);
      uart_send(__LINE__, String);
    }

    /* If this slot was corrupted, make some housekeeping. */
    CurrentTail = SoundActiveTail; // keep track of starting position in the queue.

    /* In case of empty queue or queue error, return queue head and queue tail instead of frequency and duration. */
    *MSeconds = SoundActiveHead;
    *RepeatCount = SoundActiveTail;

    do
    {
      SoundQueueActive[SoundActiveTail].MSec == 0;
      ++SoundActiveTail;

      /* If reaching out-of-bound, revert to zero. */
      if (SoundActiveTail >= MAX_ACTIVE_SOUND_QUEUE)
        SoundActiveTail = 0;

      /* If we come back to where we started from, sound queue must be empty. */
      if (SoundActiveTail == CurrentTail)
      {
        SoundActiveHead = SoundActiveTail;
        break; // get out of "do" loop.
      }
    } while (SoundQueueActive[SoundActiveTail].MSec == 0);

    return 0xFF;
  }
}





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
void test_zone(UINT8 TestType)
{
  UCHAR CharacterBuffer;             // buffer to temporarily hold a character.
  UCHAR CrcString[64];               // buffer to test crc16() function.
  UCHAR String[DISPLAY_BUFFER_SIZE]; // string to scroll on clock display.

  UINT Status;
  UINT TrigLevel;

  UINT8 DisplayType;
  UINT8 Dum1UInt8;
  UINT8 *FlashBaseAddress;
  UINT8 FlashSector[4096];
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 Loop3UInt8;
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


  /* Configure the GPIO associated to the three push-buttons of the clock in "input" mode to use them while testing. */
  /* Tests are usually performed before gpio initialization. */
  gpio_set_dir(SET_BUTTON, GPIO_IN);
  gpio_set_dir(UP_BUTTON, GPIO_IN);
  gpio_set_dir(DOWN_BUTTON, GPIO_IN);

  /* Setup a pull-up on those three GPIOs. */
  gpio_pull_up(SET_BUTTON);
  gpio_pull_up(UP_BUTTON);
  gpio_pull_up(DOWN_BUTTON);

  /* NOTE: Display power supply voltage and clock, so that we
           know we are entering the test function. */
  /* ---------------------------------------------------------------- *\
                     Display power supply voltage
              and update it a few times on the display.
  \* ---------------------------------------------------------------- */
  /* Single tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  for (Loop1UInt8 = 0; Loop1UInt8 < 5; ++Loop1UInt8)
  {
    display_voltage();
    sleep_ms(200);
  }
  sleep_ms(1000);
  /* ---------------------------------------------------------------- *\
                 END - Display power supply voltage
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
      Display time (clock) for a few seconds before beginning tests.
  \* ---------------------------------------------------------------- */
  /* Single tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  show_time();
  sleep_ms(1000);
  /* ---------------------------------------------------------------- *\
                      END - Display time clock
  \* ---------------------------------------------------------------- */

  /* ---------------------------------------------------------------- *\
       Jump to the specific test number if one has been specified.
              (0 means to execute all tests in sequence).
  \* ---------------------------------------------------------------- */
  if (TestType != 0)
  {
    switch (TestType)
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
  sprintf(String, "Maximum value for  8 bits unsigned:               0x%X                    %u\r", Dum1UInt8, Dum1UInt8);
  uart_send(__LINE__, String);

  Dum1UInt16 = 0;
  --Dum1UInt16;
  sprintf(String, "Maximum value for 16 bits unsigned:             0x%X                  %u\r", Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  Dum1UInt32 = 0;
  --Dum1UInt32;
  sprintf(String, "Maximum value for 32 bits unsigned:         0x%lX             %lu\r", Dum1UInt32, Dum1UInt32);
  uart_send(__LINE__, String);

  Dum1UInt64 = 0;
  --Dum1UInt64;
  sprintf(String, "Maximum value for 64 bits unsigned: 0x%llX   %llu\r", Dum1UInt64, Dum1UInt64);
  uart_send(__LINE__, String);

  /* Display other useful information. */
  sprintf(String, "Address of malloc() pointer returned (for variable FlashData): %p\r", FlashData);
  uart_send(__LINE__, String);

  sprintf(String, "FLASH_PAGE_SIZE: %u\r", FLASH_PAGE_SIZE);
  uart_send(__LINE__, String);

  sprintf(String, "FLASH_SECTOR_SIZE: %u\r", FLASH_SECTOR_SIZE);
  uart_send(__LINE__, String);

  sprintf(String, "Address of beginning of flash (XIP_BASE): %X\r\r", XIP_BASE);
  uart_send(__LINE__, String);

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
  FlashSector[0] = 0x50;
  FlashSector[1] = 0x51;
  FlashSector[2] = 0x52;
  FlashSector[3] = 0x53;
  FlashSector[4] = 0x54;
  FlashSector[5] = 0x55;
  FlashSector[6] = 0x56;
  FlashSector[7] = 0x57;
  FlashSector[8] = 0x58;
  FlashSector[9] = 0x59;
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
      sprintf(String, "Address: %8.8X   Content: %2.2X\r", (XIP_BASE + Loop1UInt32), FlashBaseAddress[Loop1UInt32]);
      uart_send(__LINE__, String);
    }
  }
  */
  uart_send(__LINE__, "End of flash tests...\r\r\r");

  /*
  // Display the complete Pico's 2MB flash address space.
  sprintf(String, "Flash content:\r\r");
  uart_send(__LINE__, String);

  flash_display(0, 0x1FFFFF);

  sprintf(String, "End of flash content\r\r");
  uart_send(__LINE__, String);
  */

  /* Tests to save Green clock configuration to flash. */
  sprintf(String, "sizeof(FlashConfig): 0x%X (%u)\r", sizeof(FlashConfig), sizeof(FlashConfig));
  uart_send(__LINE__, String);

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
  sprintf(String, "NOTE: FLAG_OFF: %u     FLAG_ON: %u\r", FLAG_OFF, FLAG_ON);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] Firmware version:  %s\r", &FlashConfig.Version, FlashConfig.Version);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] CurrentYearCentile: %3u\r", &FlashConfig.CurrentYearCentile, FlashConfig.CurrentYearCentile);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] DaylightSavingTime: %3u\r", &FlashConfig.DaylightSavingTime, FlashConfig.DaylightSavingTime);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeTimeOn:        %3u\r", &FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOn);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeTimeOff:       %3u\r", &FlashConfig.ChimeTimeOff, FlashConfig.ChimeTimeOff);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagAutoBrightness: %3u\r", &FlashConfig.FlagAutoBrightness, FlashConfig.FlagAutoBrightness);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagKeyclick:       %3u\r", &FlashConfig.FlagKeyclick, FlashConfig.FlagKeyclick);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagScrollEnable:   %3u\r", &FlashConfig.FlagScrollEnable, FlashConfig.FlagScrollEnable);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeMode:          %3u\r", &FlashConfig.ChimeMode, FlashConfig.ChimeMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] Language:           %3u\r", &FlashConfig.Language, FlashConfig.Language);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightMode:     %3u\r", &FlashConfig.NightLightMode, FlashConfig.NightLightMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightTimeOn:   %3u\r", &FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOn);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightTimeOff:  %3u\r", &FlashConfig.NightLightTimeOff, FlashConfig.NightLightTimeOff);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] TemperatureUnit:    %3u\r", &FlashConfig.TemperatureUnit, FlashConfig.TemperatureUnit);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] TimeDisplayMode:    %3u\r\r\r", &FlashConfig.TimeDisplayMode, FlashConfig.TimeDisplayMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] CRC16 read from flash          0x%X (%u)\r", &FlashConfig.Crc16, FlashConfig.Crc16, FlashConfig.Crc16);
  uart_send(__LINE__, String);

  /* Validate CRC16 read from flash. */
  Dum1UInt16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  sprintf(String, "CRC16 computed from data read: 0x%X (%u)\r", Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  /* If CRC16 read from flash does not correspond to the data read, assign default values and save new configuration. */
  if (FlashConfig.Crc16 == Dum1UInt16)
  {
    uart_send(__LINE__, "Flash configuration is valid. Flash memory tests completed.\r\r\r");
  }
  else
  {
    uart_send(__LINE__, "Flash configuration is corrupted. Save a new configuration to flash.\r\r\r");
  }

  /* If CRC16 computed from retrieved data is different from CRC16 read from flash...
     Assign default values and save a new configuration to flash. */
  sprintf(FlashConfig.Version, FIRMWARE_VERSION);        // firmware version number.
  FlashConfig.CurrentYearCentile = 20;                   // assume we are in years 20xx (could be changed in clock setup, but will revert to 20 at each power-up).
  FlashConfig.ChimeTimeOff = CHIME_TIME_OFF;             // hourly chime will begin at this hour.
  FlashConfig.ChimeTimeOn = CHIME_TIME_ON;               // hourly chime will begin at this hour.
  FlashConfig.DaylightSavingTime = DAYLIGHT_SAVING_TIME; // specifies how to handle the daylight saving time (see clock options above).
  FlashConfig.FlagAutoBrightness = FLAG_ON;              // flag indicating we are in "Auto Brightness" mode.
  FlashConfig.FlagKeyclick = FLAG_OFF;                   // flag for keyclick ("button-press" tone)
  FlashConfig.FlagScrollEnable = SCROLL_DEFAULT;         // flag indicating the clock will scroll the date and temperature at regular intervals on the display.
  FlashConfig.ChimeMode = CHIME_DEFAULT;                 // chime mode (Off / On / Day).
  FlashConfig.Language = DEFAULT_LANGUAGE;               // language used for data display (including date scrolling).
  FlashConfig.NightLightMode = NIGHT_LIGHT_DEFAULT;      // night light mode (On / Off / Auto / Night).
  FlashConfig.NightLightTimeOff = NIGHT_LIGHT_TIME_OFF;  // default night light time off.
  FlashConfig.NightLightTimeOn = NIGHT_LIGHT_TIME_ON;    // default night light time on.
  FlashConfig.TemperatureUnit = TEMPERATURE_DEFAULT;     // CELSIUS or FAHRENHEIT default value (see clock options above).
  FlashConfig.TimeDisplayMode = TIME_DISPLAY_DEFAULT;    // H24 or H12 default value (see clock options above).

  /* Write Green Clock configuration to flash. */
  uart_send(__LINE__, "Write Green Clock configuration to flash.\r");

  sprintf(String, "Pointer to FlashConfig.Version: %X\r", FlashConfig.Version);
  uart_send(__LINE__, String);

  sprintf(String, "Pointer to FlashConfig.Crc16:   %X\r", &FlashConfig.Crc16);
  uart_send(__LINE__, String);

  sprintf(String, "&FlashConfig.Crc16 - &FlashConfig.Version: %X (%u)\r\r\r", (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  uart_send(__LINE__, String);

  /* Display configuration being saved. */
  sprintf(String, "NOTE: FLAG_OFF: %u     FLAG_ON: %u\r", FLAG_OFF, FLAG_ON);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] Firmware version:  %s\r", &FlashConfig.Version, FlashConfig.Version);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] CurrentYearCentile: %3u\r", &FlashConfig.CurrentYearCentile, FlashConfig.CurrentYearCentile);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] DaylightSavingTime: %3u\r", &FlashConfig.DaylightSavingTime, FlashConfig.DaylightSavingTime);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeTimeOn:        %3u\r", &FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOn);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeTimeOff:       %3u\r", &FlashConfig.ChimeTimeOff, FlashConfig.ChimeTimeOff);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagAutoBrightness: %3u\r", &FlashConfig.FlagAutoBrightness, FlashConfig.FlagAutoBrightness);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagKeyclick:       %3u\r", &FlashConfig.FlagKeyclick, FlashConfig.FlagKeyclick);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] FlagScrollEnable:   %3u\r", &FlashConfig.FlagScrollEnable, FlashConfig.FlagScrollEnable);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] ChimeMode:          %3u\r", &FlashConfig.ChimeMode, FlashConfig.ChimeMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] Language:           %3u\r", &FlashConfig.Language, FlashConfig.Language);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightMode:     %3u\r", &FlashConfig.NightLightMode, FlashConfig.NightLightMode);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightTimeOn:   %3u\r", &FlashConfig.NightLightTimeOn, FlashConfig.NightLightTimeOn);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] NightLightTimeOff:  %3u\r", &FlashConfig.NightLightTimeOff, FlashConfig.NightLightTimeOff);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] TemperatureUnit:    %3u\r", &FlashConfig.TemperatureUnit, FlashConfig.TemperatureUnit);
  uart_send(__LINE__, String);

  sprintf(String, "[%X] TimeDisplayMode:    %3u\r\r\r", &FlashConfig.TimeDisplayMode, FlashConfig.TimeDisplayMode);
  uart_send(__LINE__, String);

  uart_send(__LINE__, "Calculating CRC16\r");

  FlashConfig.Crc16 = crc16((UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version);
  sprintf(String, "Pointer: %X   Size: %u   CRC16: %X (%u)\r\r\r", (UINT8 *)&FlashConfig, (UINT32)&FlashConfig.Crc16 - (UINT32)&FlashConfig.Version, FlashConfig.Crc16, FlashConfig.Crc16);
  uart_send(__LINE__, String);

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
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  sprintf(CrcString, "FF101A8B");
  Dum1UInt16 = crc16(CrcString, 8);
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  sprintf(CrcString, "FF101B0B");
  Dum1UInt16 = crc16(CrcString, 8);
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  sprintf(CrcString, "FF101B8B");
  Dum1UInt16 = crc16(CrcString, 8);
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  sprintf(CrcString, "FF101C0B");
  Dum1UInt16 = crc16(CrcString, 8);
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  sprintf(CrcString, "FF101C8B");
  Dum1UInt16 = crc16(CrcString, 8);
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  sprintf(CrcString, "FF101D0B");
  Dum1UInt16 = crc16(CrcString, 8);
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

  sprintf(CrcString, "FF101D8B");
  Dum1UInt16 = crc16(CrcString, 8);
  sprintf(String, "String: %s   CRC16: %X (%u)\r", CrcString, Dum1UInt16, Dum1UInt16);
  uart_send(__LINE__, String);

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

  add_repeating_timer_us(50, timer_callback_us, NULL, &Timer50uSec);

  while (1)
  {
    Dum1UInt16 = get_ads1015();
    sprintf(String, "%4.4u", AverageLightLevel);

    fill_display_buffer_4X7(1, String[0]);
    fill_display_buffer_4X7(6, String[1]);
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
    sleep_ms(100); // let the time to complete scrolling.

  /* Scan all ASCII characters that are defined in the bitmap table. */
  for (Loop1UInt8 = 0x1E; Loop1UInt8 <= 0x7F; ++Loop1UInt8)
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
  String[0] = 0x1E;
  String[1] = 0x1F;
  String[2] = 0x20;
  String[3] = 0x21;
  String[4] = 0x22;
  String[5] = 0x23;
  String[6] = 0x24;
  String[7] = 0x25;
  String[8] = 0x26;
  String[9] = 0x27;
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
  String[0] = 0x3A;
  String[1] = 0x3B;
  String[2] = 0x3C;
  String[3] = 0x3D;
  String[4] = 0x3E;
  String[5] = 0x3F;
  String[6] = 0x40;
  String[7] = 0x41;
  String[8] = 0x42;
  String[9] = 0x43;
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

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);

  /* Announce test number. */
  scroll_string(24, "Test #9 - Sound queues");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Passive sound queue tests. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(800, 50);
    sound_queue_passive(SILENT, 40);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(1200, 50);
    sound_queue_passive(SILENT, 40);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(2500, 50);
    sound_queue_passive(SILENT, 40);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(4000, 50);
    sound_queue_passive(SILENT, 40);
  }

  sound_queue_passive(SILENT, 700);

  /* Passive sound queue tests. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(800, 50);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(1200, 50);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(2500, 50);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 15; ++Loop1UInt8)
  {
    sound_queue_passive(4000, 50);
  }

  sound_queue_passive(SILENT, 700);

  return;

  for (Loop1UInt8 = 0; Loop1UInt8 < 5; ++Loop1UInt8)
  {
    sound_queue_passive(1000, 50);
    sound_queue_passive(SILENT, 40);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 5; ++Loop1UInt8)
  {
    sound_queue_passive(1200, 50);
    sound_queue_passive(SILENT, 40);
  }

  sound_queue_passive(SILENT, 700);

  for (Loop1UInt8 = 0; Loop1UInt8 < 5; ++Loop1UInt8)
  {
    sound_queue_passive(1500, 50);
    sound_queue_passive(SILENT, 40);
  }

  sound_queue_passive(SILENT, 700);

  sleep_ms(15000);

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
                 END - Test 9 - Tests with indicators.
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

  // Give information to user.
  scroll_string(24, "Variable character width...");
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
    fill_display_buffer_4X7(0, Loop1UInt8 / 10 + '0');
    fill_display_buffer_4X7(5, Loop1UInt8 % 10 + '0');

    for (Loop2UInt8 = 9; Loop2UInt8 < 16; ++Loop2UInt8)
      DisplayBuffer[Loop2UInt8] &= 0x7F;

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
  CurrentClockMode = MODE_SCROLLING;

  /* Announce test number. */
  scroll_string(4, "Test #14 - 4 X 7 characters scrolling");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  fill_display_buffer_5X7(5, 'A');
  fill_display_buffer_5X7(11, 'B');
  fill_display_buffer_5X7(17, 'C');
  fill_display_buffer_5X7(23, 'D');
  fill_display_buffer_5X7(29, 'E');
  fill_display_buffer_5X7(35, 'F');
  fill_display_buffer_5X7(41, 'G');
  fill_display_buffer_5X7(47, 'H');
  fill_display_buffer_5X7(53, 'I');
  fill_display_buffer_5X7(59, 'J');
  fill_display_buffer_5X7(65, 'K');
  fill_display_buffer_5X7(71, 'L');
  fill_display_buffer_5X7(77, 'M');
  fill_display_buffer_5X7(83, 'N');
  fill_display_buffer_5X7(90, 'O');
  fill_display_buffer_5X7(96, 'P');
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
  CurrentClockMode = MODE_SCROLLING;

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Test the fill_display_buffer_4X7() function for 4 X 7 characters...
     ... first in the "physical" section of the display memory. */
  fill_display_buffer_4X7(0, '0');
  sleep_ms(500);
  fill_display_buffer_4X7(5, '1');
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
  CurrentClockMode = MODE_SCROLLING;

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
  CurrentClockMode = MODE_SCROLLING;

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

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

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

  DisplayBuffer[8] |= (1 << 7); // Friday
  DisplayBuffer[16] |= (1 << 0);
  sleep_ms(500);
  DisplayBuffer[8] &= ~(1 << 7);
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
       Test 18 - Quick display test with both character formats.
\* ------------------------------------------------------------------ */
Test18:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);

  /* Tone to announce entering a new test. */
  tone(10);

  /* Announce test number. */
  scroll_string(24, "Test #18 - Test with both character bitmaps");
  while (ScrollDotCount)
    sleep_ms(100); // let the time to complete scrolling.

  /* Quick display test with 4 X 6 character bitmap. */
  fill_display_buffer_4X7(1, '1');
  fill_display_buffer_4X7(6, '2');
  fill_display_buffer_4X7(11, '3');
  fill_display_buffer_4X7(16, '4');
  sleep_ms(2000);

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);
  sleep_ms(200);

  /* Quick display test with 5 X 7 character bitmap. */
  ColumnPosition = fill_display_buffer_5X7(4, 'A');
  ColumnPosition = fill_display_buffer_5X7(++ColumnPosition, 'B');
  ColumnPosition = fill_display_buffer_5X7(++ColumnPosition, 'C');
  // ColumnPosition = fill_display_buffer_5X7(++ColumnPosition, (UINT8)0x99);  // out-of-bound test.
  // ColumnPosition = fill_display_buffer_5X7(++ColumnPosition, (UINT8)0x04);  // out-of-bound test.
  sleep_ms(2000);

  /* Clear framebuffer when done. */
  clear_framebuffer(0);

  return;
  /* ------------------------------------------------------------------ *\
      END - Test 18 - Quick display test with both character formats.
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
  UINT8 Dum1UInt8;
  UINT8 Loop1UInt8;


  Dum1UInt8 = 0;

  adc_show_count();         // read ambient light from Pico ADC port every 5000 milliseconds (i.e every 5 seconds).
  evaluate_blinking_time(); // evaluate if it is time to toggle blinking some data on clock display.
  evaluate_scroll_time();   // evaluate if it is time to scroll one dot to the left on clock display.

  /* ................................................................ *\
                        Manage "Set" (top) button.
  \* ................................................................ */
  /* Check if the "Set" (top) button is pressed. */
  if (gpio_get(SET_BUTTON) == 0)
  {
    /* The "Set" (top) button is pressed... cumulate the number of milliseconds it is pressed. */
    ++TopKeyPressTime;
  }
  else
  {
    /* If 'Set' key has been pressed while one or more of the alarms 3 to 9 was ringing, reset all alarms and continue.
       When one or more of the alarms 3 to 9 are ringing, 'Set' key is used simply to reset alarms. */
    if (TopKeyPressTime)
    {
      if (AlarmReachedBitMask)
      {
        AlarmReachedBitMask = 0;

        TopKeyPressTime = 0;
      }
    }

    /* If the "Set" (top) button is not pressed, but cumulative time is not zero, it means that the
       "Set" button has just been released and it is a "short press" (between 50 and 300 milliseconds). */
    if ((TopKeyPressTime > 50) && (TopKeyPressTime < 300))
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
          /* If not already in a setup mode, "Set" button triggers entering in clock setup mode. */
          FlagSetClock = FLAG_ON;
          SetupSource = SETUP_SOURCE_CLOCK;
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

      /* If we are not already in setup mode, enter alarm setup mode. */
      FlagSetAlarm = FLAG_ON; // enter alarm setup mode.
      SetupSource = SETUP_SOURCE_ALARM;
      ++SetupStep; // process through all alarm setup steps.
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
          scroll_queue(TAG_EXT_TEMP);
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
        SetupSource = SETUP_SOURCE_TIMER;
        ++SetupStep;
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
  }
  else
  {
    /* If the "Down" (bottom) button is not pressed, but cumulative time is not zero, it means that the
       "Down" button has just been released and it is a "short press" (between 50 and 300 milliseconds). */
    if ((BottomKeyPressTime > 0) && (BottomKeyPressTime < 300))
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
          /* If we are not in setup mode, toggle the "auto-brightness" On / Off. */
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

    /* Reset bottom button cumulative press time. */
    BottomKeyPressTime = 0;
  }

  /* ................................................................ *\
                    Increment LED matrix scanned row.
  \* ................................................................ */
  /* Increment row number to scan on clock display (Row0 to Row7). */
  ++RowScanNumber;
  if (RowScanNumber > 7)
    RowScanNumber = 0;

  /* ................................................................ *\
            Disable matrix scan while changing column data.
  \* ................................................................ */
  cancel_repeating_timer(&Timer50uSec);
  OE_DISABLE;

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

  /* ................................................................ *\
         Restore display after LED matrix data has been changed.
  \* ................................................................ */
  add_repeating_timer_us(50, timer_callback_us, NULL, &Timer50uSec);

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

  UINT16 BeepLength;
  UINT8 Dum1UInt8;
  UINT16 LightLevel;
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 Period; // 5-second period completed.
  UINT8 TotalBeeps;
  UINT8 TotalOneBits;


  /* ................................................................ *\
            Cumulate 60 seconds before updating minute count.
  \* ................................................................ */
  ++SecondCounter;



  /* ................................................................ *\
      If user requested a silence period, decrement its second count.
  \* ................................................................ */
  if (SilencePeriod != 0)
    --SilencePeriod;



  /* ................................................................ *\
         Check for a change in current active clock configuration.
  \* ................................................................ */
  /* Make sure we're not currently in mode setup when updating clock configuration.
     It's better to wait until we completed the configuration process before updating flash configuration.
     Perform the update at xx:xx:35 seconds to prevent the peak operations performed at xx:xx:00.
     Since we need to temporarily disable interrupts during flash write, perform the check near the minute change.
     This we, we can turn Off clock display and it will refresh soon after, at minute change. */
  if ((SecondCounter == 59) && (CurrentClockMode == MODE_SHOW_TIME))
    flash_check_config();



  /* ................................................................ *\
                             Feed alarm ringers.
  \* ................................................................ */
  if ((SecondCounter % ALARM_PERIOD) == 0)
  {
    if (AlarmReachedBitMask)
    {
      if (DebugBitMask & DEBUG_ALARMS)
      {
        uart_send(__LINE__, "AL ON\r");
      }

      /* Calculate the total number of beeps to decide on the length of each beep. */
      TotalBeeps = 0;
      for (Loop1UInt8 = 0; Loop1UInt8 < MAX_ALARMS; ++Loop1UInt8)
      {
        if (AlarmReachedBitMask & (1 << Loop1UInt8))
        {
          TotalBeeps += (Loop1UInt8 + 1);
        }
      }

      /* Assign beep length. */
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
      {
        sprintf(String, "-TotalBeeps: %u  BeepLength: %u\r", TotalBeeps, BeepLength);
        uart_send(__LINE__, String);
      }

      /* Trigger ringer for each alarm condition reached. */
      for (Loop1UInt8 = 0; Loop1UInt8 < MAX_ALARMS; ++Loop1UInt8)
      {
        if (AlarmReachedBitMask & (1 << Loop1UInt8))
        {
          if (DebugBitMask & DEBUG_ALARMS)
          {
            sprintf(String, "-AL %u ON   RingTime: %u\r", Loop1UInt8 + 1, AlarmRingTime[Loop1UInt8]);
            uart_send(__LINE__, String);
          }

          /* Check if this alarm has been ringing for one hour. */
          AlarmRingTime[Loop1UInt8] += ALARM_PERIOD;
          if (AlarmRingTime[Loop1UInt8] >= MAX_ALARM_RING_TIME)
          {
            /* This alarm has been sounding for one hour now... Turn it Off. */
            AlarmReachedBitMask &= ~(1 << Loop1UInt8);

            /* Reset total ringing time. */
            AlarmRingTime[Loop1UInt8] = 0;

            if (DebugBitMask & DEBUG_ALARMS)
            {
              sprintf(String, "-Bitmask: %8.8X\r", AlarmReachedBitMask);
              uart_send(__LINE__, String);
            }

            break; // skip to check next alarm.
          }

          TotalOneBits = total_one_bits((UINT32)AlarmReachedBitMask, 9);

          #ifdef PASSIVE_PIEZO_SUPPORT
          for (Loop2UInt8 = 0; Loop2UInt8 < (Loop1UInt8 + 1); ++Loop2UInt8)
          {
            sound_queue_passive(600 + (Loop1UInt8 * 150), BeepLength);
            sound_queue_passive(SILENT, 40); // separate each beep in the train of beeps.
          }
          sound_queue_passive(SILENT, 200); // separate the train of beeps of an alarm from each other.
          #else
          for (Loop2UInt8 = 0; Loop2UInt8 < (Loop1UInt8 + 1); ++Loop2UInt8)
          {
            sound_queue_active(BeepLength, 1);
            sound_queue_active(50, SILENT); // separate each beep in the train of beeps.
          }
          sound_queue_active(200, SILENT); // separate the train of beeps of an alarm from each other.
          #endif
        }
      }
    }
  }



  /* ................................................................ *\
        Check alarms and turn On corresponding bit in the bitmask
                     when reaching all alarm conditions.
  \* ................................................................ */
  /* Scan all alarms. The check is done during an off-peak second (not 00, 14, 44). */
  if (SecondCounter == 29)
  {
    for (Loop1UInt8 = 0; Loop1UInt8 < 9; ++Loop1UInt8)
    {
      /* If this alarm is Off, skip it. */
      if (FlashConfig.Alarm[Loop1UInt8].FlagStatus == FLAG_OFF)
      {
        if (DebugBitMask & DEBUG_ALARMS)
        {
          sprintf(String, "-[%2.2u]: Off\r", Loop1UInt8);
          uart_send(__LINE__, String);
        }
        continue;
      }

      /* This alarm is On, check if current day-of-week is defined in the "day-mask". */
      if ((FlashConfig.Alarm[Loop1UInt8].Day & (1 << Time_RTC.dayofweek)) == 0)
      {
        if (DebugBitMask & DEBUG_ALARMS)
        {
          sprintf(String, "-[%2.2u]: Wrong day    (%2.2X VS %2.2X)\r", Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Day, (1 << Time_RTC.dayofweek));
          uart_send(__LINE__, String);
        }
        continue;
      }

      /* Current day-of-week is defined in the "day-mask", check if current hour is the "alarm-hour" . */
      if (FlashConfig.Alarm[Loop1UInt8].Hour != CurrentHour)
      {
        if (DebugBitMask & DEBUG_ALARMS)
        {
          sprintf(String, "-[%2.2u]: Wrong hour   (%2.2u VS %2.2u)\r", Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Hour, CurrentHour);
          uart_send(__LINE__, String);
        }
        continue;
      }

      /* Current hour is the "alarm hour", check if current minute is the "alarm-minute". */
      if (FlashConfig.Alarm[Loop1UInt8].Minute != CurrentMinute)
      {
        if (DebugBitMask & DEBUG_ALARMS)
        {
          sprintf(String, "-[%2.2u]: Wrong minute (%2.2u VS %2.2u)\r", Loop1UInt8, FlashConfig.Alarm[Loop1UInt8].Minute, CurrentMinute);
          uart_send(__LINE__, String);
        }
        continue;
      }

      /* We comply with all alarm conditions. */
      if (DebugBitMask & DEBUG_ALARMS)
      {
        sprintf(String, "-[%2.2u]: **Reached**\r", Loop1UInt8);
        uart_send(__LINE__, String);
      }

      /* Set the bit corresponding to this alarm in the alarm bit mask. */
      AlarmReachedBitMask |= 1 << Loop1UInt8;

      if (DebugBitMask & DEBUG_ALARMS)
      {
        sprintf(String, "-BitMask: 0x%4.4X\r\r", AlarmReachedBitMask);
        uart_send(__LINE__, String);
      }

      /* If there a string associated with this alarm, scroll it on clock display. */
      if (FlashConfig.Alarm[Loop1UInt8].Text[0] != 0x00)
        scroll_string(24, FlashConfig.Alarm[Loop1UInt8].Text);
    }
  }



  /* ................................................................ *\
                        Check for minute change.
  \* ................................................................ */
  if (SecondCounter == 60)
  {
    ++CurrentMinute;
    SecondCounter = 0; // reset second counter.

    if (CurrentMinute == 60)
    {
      CurrentMinute = 0; // reset minute counter.
      ++CurrentHourSetting;

      if (CurrentHourSetting == 24)
        CurrentHourSetting = 0; // reset hour counter.

      /* Reset flag so that next hourly chime will sound. */
      FlagChimeDone = FLAG_OFF;
      if (DebugBitMask & DEBUG_CHIME)
      {
        sprintf(String, "SecondCounter: %u  Minute: %u  Hour: %u  FlagChimeDone: %u\r", SecondCounter, CurrentMinute, CurrentHourSetting, FlagChimeDone);
        uart_send(__LINE__, String);
      }
    }

    /* .............................................................. *\
                 Request a "time" update on clock display.
    \* .............................................................. */
    if ((FlagIdleCheck == FLAG_OFF) && (ScrollStartCount == 0))
    {
      /* Request a "time" update (on clock display) if we are not in some setup mode and not scrolling data. */
      FlagUpdateTime = FLAG_ON;
    }
  }



  /* ................................................................ *\
                              Hourly chime.
  \* ................................................................ */
  /* Hourly chime will never sound if set to Off and will always sound if set to On (except if user requested a silence period).
     However, if Hourly chime is set to Day "OI" ("ON, Intermittent" in clock setup), here is what happens:

     "Daytime workers":
     Hourly chime "normal behavior": When "Chime time on" is smaller than "Chime time off".
     For example: "Chime time on" = 9h00 and "Chime time off" is 21h00. Chime will sound between (and including) 9h00 and 21h00 as we would expect.

     "Nighttime workers":
     Hourly chime "special behavior": When "Chime time on" is greater than "Chime time off".
     For example: "Chime time on" = 21h00 and "Chime time off" is 9h00. Chime will sound AFTER 21h00 and BEFORE 9h00 and will not sound during daytime.
     (that is, will not sound between 9h00 and 21h00). */
  if (DebugBitMask & DEBUG_CHIME)
  {
    sprintf(String, "SecondCounter: %u  Minute: %u  Hour: %u  FlagChimeDone: %u\r", SecondCounter, CurrentMinute, CurrentHourSetting, FlagChimeDone);
    uart_send(__LINE__, String);
    sprintf(String, "ChimeMode: %u  ChimeTimeOn: %u  ChimeTimeOff: %u  FlagSetupClock[SETUP_MINUTE]: %u\r", FlashConfig.ChimeMode, FlashConfig.ChimeTimeOn, FlashConfig.ChimeTimeOff, FlagSetupClock[SETUP_MINUTE]);
    uart_send(__LINE__, String);
  }

  if ((SilencePeriod == 0) &&
      ((FlashConfig.ChimeMode == CHIME_ON) ||                                                                                                                                                                    // always On
       ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOn < FlashConfig.ChimeTimeOff) && (CurrentHourSetting >= FlashConfig.ChimeTimeOn) && (CurrentHourSetting <= FlashConfig.ChimeTimeOff)) ||  // "normal behavior"  for daytime workers.
       ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOff < FlashConfig.ChimeTimeOn) && ((CurrentHourSetting >= FlashConfig.ChimeTimeOff) || (CurrentHourSetting <= FlashConfig.ChimeTimeOn))))) // "special behavior" for nighttime workers.
  {
    if ((CurrentMinute == 0) && (FlagChimeDone == FLAG_OFF) && (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF))
    {
      /* Use active sound queue for hourly chime. */
      for (Loop1UInt8 = 0; Loop1UInt8 < TONE_CHIME_REPEAT2; ++Loop1UInt8)
      {
        sound_queue_active(TONE_CHIME_DURATION, TONE_CHIME_REPEAT1);
        sound_queue_active(50, SILENT);
      }
      sound_queue_active(500, SILENT);
      FlagChimeDone = FLAG_ON; // hourly chime has been done for current hour change.

      #ifdef PASSIVE_PIEZO_SUPPORT
      // Let the time to complete the active buzzer hourly chime.
      sound_queue_passive(SILENT, 1300);

      // Close encounter of the third kind.
      play_jingle(JINGLE_ENCOUNTER);
      #endif
    }
  }



  /* ................................................................ *\
                 Date, temperature and voltage scrolling.
  \* ................................................................ */
  /* Scroll information on clock display every "integer number of times" the SCROLL_PERIOD_MINUTE minutes,
     except if we are currently in some setup mode. For example, if SCROLL_PERIOD_MINUTE is set to 5, data
     will scroll at xh00, xh05, xh10, etc...
     When changing to exact hour (from xh59 to yh00), scrolling will begin 5 seconds later to give time to user
     to look at the clock when he hears the hourly chime. */
  if (((SecondCounter == 0) && ((CurrentMinute % SCROLL_PERIOD_MINUTE) == 0) && (CurrentMinute != 0)) ||
      ((SecondCounter == 5) && (CurrentMinute == 0)))
  {
    if ((FlashConfig.FlagScrollEnable == FLAG_ON) && (CurrentClockMode == MODE_SHOW_TIME))
    {
      /* We reach the time to trigger scrolling of the date and we are in time display mode. */
      scroll_queue(TAG_DATE);

      /* If a DHT22 or BME280 has been installed by user, display outside parameters. */
      scroll_queue(TAG_EXT_TEMP);

      /* Then scroll ambient temperature. */
      scroll_queue(TAG_TEMPERATURE);

      /* Scroll Pico temperature. */
      // scroll_queue(TAG_PICO_TEMP);

      /* Scroll power supply voltage. */
      // scroll_queue(TAG_VOLTAGE);
    }
  }



  /* ................................................................ *\
                              Idle time.
  \* ................................................................ */
  /* If we are in some setup mode (clock, timer or alarm), check if it is time to declare a time-out. */
  if (FlagIdleCheck == FLAG_ON)
  {
    ++IdleNumberOfSeconds; // one more second added while the system has been idle.

    if (IdleNumberOfSeconds == TIME_OUT_PERIOD)
    {
      set_mode_timeout();     // get out of setup mode because of inaction (time-out).
      set_mode_out();         // proceed with what needs to be set and then cleanup as we get out of setup mode.
      SetupStep = SETUP_NONE; // reset setup step.

      /* If we timed out while setting up an alarm, keep what has been defined so far if alarm status is "Alarm On". */
      if ((AlarmNumber == 0) && (FlashConfig.Alarm[AlarmNumber].FlagStatus == FLAG_ON))
      {
        /* Alarm 1 setup. */
        set_alarm1_clock(ALARM_MODE_HOUR_MIN_SEC_MATCHED, 00, FlashConfig.Alarm[AlarmNumber].Minute, FlashConfig.Alarm[AlarmNumber].Hour, FlashConfig.Alarm[AlarmNumber].Day);
      }

      /* If we timed out while setting up an alarm, keep what has been defined so far if alarm status is "Alarm On". */
      if ((AlarmNumber != 0) && (FlashConfig.Alarm[AlarmNumber].FlagStatus == FLAG_ON))
      {
        /* Alarm 2 setup. */
        set_alarm2_clock(FlashConfig.Alarm[AlarmNumber].Minute, FlashConfig.Alarm[AlarmNumber].Hour, FlashConfig.Alarm[AlarmNumber].Day);
      }
    }
  }



  /* ................................................................ *\
                            Count-down timer.
  \* ................................................................ */
  if (FlagSetTimerCountDown == FLAG_ON)
  {
    /* If a count-down timer has been set and reaches zero, we always sound the piezo, no matter what is the Chime time on / off setting. */
    if ((TimerSeconds == 0) && (TimerMinutes == 0))
    {
      for (Loop1UInt8 = 0; Loop1UInt8 < TONE_TIMER_REPEAT2; ++Loop1UInt8)
      {
        sound_queue_active(TONE_TIMER_DURATION, TONE_TIMER_REPEAT1);
        sound_queue_active(1000, SILENT);
      }

      SetupSource = SETUP_SOURCE_NONE;
      FlagSetTimerCountDown = FLAG_OFF; // reset count down timer indicator.
      FlagTimerCountDownEnd = FLAG_ON;  // flag indicating we reached final count of count down timer (0m00).
      IndicatorCountDownOff;            // turn off "count-down" indicator when done.
    }
    else
    {
      ++TimerShowCount;

      /* Decrease the "second" part of the count-down timer. */
      --TimerSeconds;

      /* We let the timer to go one second below zero so that the last second is accounted for in the laps period. */
      if (TimerSeconds == 255)
        TimerSeconds = 0; // if out-of-bound, revert to zero.

      /* If there are more minutes to the count-down. */
      if ((TimerSeconds == 0) && (TimerMinutes != 0))
      {
        --TimerMinutes;
        TimerSeconds = 59;
        TimerShowSecond = 0; // for display purpose
      }

      /* Update timer on clock display every so many seconds. */
      if ((TimerShowCount % 1) == 0)
      {
        TimerShowCount = 0;
        TimerShowSecond = TimerSeconds;
      }
    }
  }



  /* ................................................................ *\
                             Count-up timer.
  \* ................................................................ */
  if (FlagSetTimerCountUp == FLAG_ON)
  {
    ++TimerSeconds; // one more second for Up timer.
    ++TimerShowCount;

    /* Add one minute when we reach 60 seconds to the timer. */
    if (TimerSeconds == 60)
    {
      TimerSeconds = 0;
      ++TimerMinutes;

      /* If reaching out-of-bound, revert to zero. */
      if (TimerMinutes == 60)
        TimerMinutes = 0;
    }

    /* Update timer on the display every so many seconds. */
    if ((TimerShowCount % 1) == 0)
    {
      TimerShowCount = 0;
      TimerShowSecond = TimerSeconds;
    }
  }



  /* ................................................................ *\
                             Calendar events.
              Check if we should scroll a calendar event.
           Check is done twice an hour, at XXh14 and XXh44.
  \* ................................................................ */
  if ((SecondCounter == 0) && ((CurrentMinute == 14) || (CurrentMinute == 44)))
  {
    for (Loop1UInt8 = 0; Loop1UInt8 < MAX_EVENTS; ++Loop1UInt8)
    {
      if ((CalendarEvent[Loop1UInt8].Day == CurrentDayOfMonth) && (CalendarEvent[Loop1UInt8].Month == CurrentMonth))
      {
        /* Scroll text corresponding to this event on clock display. */
        scroll_queue(Loop1UInt8);

        /* Sounds "event tones" only if we are during "day time" as defined between CHIME_TIME_ON and CHIME_TIME_OFF. */
        if ((SilencePeriod == 0) &&
            ((FlashConfig.ChimeMode == CHIME_ON) ||                                                                                                                                                                    // always On
             ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOn < FlashConfig.ChimeTimeOff) && (CurrentHourSetting >= FlashConfig.ChimeTimeOn) && (CurrentHourSetting <= FlashConfig.ChimeTimeOff)) ||  // "normal behavior"  for daytime workers.
             ((FlashConfig.ChimeMode == CHIME_DAY) && (FlashConfig.ChimeTimeOff < FlashConfig.ChimeTimeOn) && ((CurrentHourSetting >= FlashConfig.ChimeTimeOff) || (CurrentHourSetting <= FlashConfig.ChimeTimeOn))))) // "special behavior" for nighttime workers.
        {
          for (Loop2UInt8 = 0; Loop2UInt8 < TONE_EVENT_REPEAT2; ++Loop2UInt8)
          {
            sound_queue_active(TONE_EVENT_DURATION, TONE_EVENT_REPEAT1);
            sound_queue_active(100, SILENT);
          }
          sound_queue_active(400, SILENT);

          #ifdef PASSIVE_PIEZO_SUPPORT
          if (CalendarEvent[Loop1UInt8].Jingle != 0)
          {
            sound_queue_passive(SILENT, 2500); // jingle should begin only after active buzzer.

            /* If there is a jingle defined, play it. */
            play_jingle(CalendarEvent[Loop1UInt8].Jingle);
          }
          #endif
        }
      }
    }
  }



  /* ................................................................ *\
                           Daylight Saving Time
  \* ................................................................ */
  /* Check if it is time to change for "daylight saving time" or "normal time". */
  if ((SecondCounter == 0) && (CurrentMinute == 0) && (FlashConfig.DaylightSavingTime != DST_NONE))
  {
    
    /* Determine if it is time to adjust for "Daylight Saving Time". */
    if ((FlagDayLightSavingTime == FLAG_OFF) && time_is_dst(Time_RTC, FlashConfig.DaylightSavingTime)){
      CurrentHourSetting++;
      set_hour(CurrentHourSetting);
      FlagDayLightSavingTime = FLAG_ON;
    }else if ((FlagDayLightSavingTime == FLAG_ON) && !time_is_dst(Time_RTC, FlashConfig.DaylightSavingTime)){
      CurrentHourSetting--;
      set_hour(CurrentHourSetting);
      FlagDayLightSavingTime = FLAG_OFF;
    }
  }



  /* ................................................................ *\
                               Night light.
  \* ................................................................ */
  /* Night light (the two white LEDs beside clock's three buttons) will
     turn On at NightLightTimeOn and turn Off at NightLightTimeOff if setting
     is set to NIGHT_LIGHT_NIGHT ("OI" when setting up the clock). */

  /* Check if it is time to turn On night light. */
  /* NOTE: Make a check every minute in case we just powered On the clock and / or we recently adjusted the time. */
  if ((FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT) && ((CurrentHourSetting < FlashConfig.NightLightTimeOff) || (CurrentHourSetting >= FlashConfig.NightLightTimeOn)))
  {
    if ((SecondCounter == 0) && (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF))
    {
      IndicatorButtonLightsOn;
    }
  }

  /* Check if it is time to turn Off night light. */
  /* Make a check every minute in case we recently adjusted the time. */
  if ((FlashConfig.NightLightMode == NIGHT_LIGHT_NIGHT) && ((CurrentHourSetting >= FlashConfig.NightLightTimeOff) && (CurrentHourSetting < FlashConfig.NightLightTimeOn)))
  {
    if ((SecondCounter == 0) && (FlagSetupClock[SETUP_MINUTE] == FLAG_OFF))
    {
      IndicatorButtonLightsOff;
    }
  }

  /* If night light is in automatic mode, check ambient light and set night light accordingly.
     A gap is left between "On" and "Off" values to prevent a twilight period of On/Off blinking. */
  if (FlashConfig.NightLightMode == NIGHT_LIGHT_AUTO)
  {
    LightLevel = get_ads1015();

    if (LightLevel < 295)
      IndicatorButtonLightsOn;

    if (LightLevel > 400)
      IndicatorButtonLightsOff;
  }

  return TRUE;
}





/* $PAGE */
/* $TITLE=timer_callback_us() */
/* ------------------------------------------------------------------ *\
           Timer callback function (50 microseconds period)
    Note: Valid values for TrigLevel are as follow:
        0, 2, 4, 6, 8, 10, 14, 16, 18, 26, 30, 32, 34, 40, 54
\* ------------------------------------------------------------------ */
bool timer_callback_us(struct repeating_timer *Timer50uSec)
{
  UCHAR String[128];

  static UINT8 PreviousSecond;
  UINT8 TrigLevel;


  ++LightSetting;

  /* Number of "50 microseconds" period that will be skipped before triggering On
     the display matrix for a 50 microseconds period. */
  TrigLevel = 14;  // assume lowest intensity on entry...
  if (AverageLightLevel >= 175)
    TrigLevel = 8; // ...then increase it...
  if (AverageLightLevel >= 250)
    TrigLevel = 4; // ... as much as required...
  if (AverageLightLevel >= 400)
    TrigLevel = 2; // ... given the average light level.
  if (AverageLightLevel >= 500)
    TrigLevel = 0; // highest intensity.


  if (DebugBitMask & DEBUG_BRIGHTNESS)
  {
    if (PreviousSecond != SecondCounter)
    {
      PreviousSecond = SecondCounter;

      /* Display current TrigLevel on external monitor after power-up sequence is over. */
      if ((CurrentClockMode == MODE_SHOW_TIME) || (CurrentClockMode == MODE_TEST))
      {
        sprintf(String, "TrigLevel: %2.2u", TrigLevel);
        uart_send(__LINE__, String);
      }
    }
  }

  if (LightSetting > TrigLevel)
  {
    OE_ENABLE;

    LightSetting = 0;
  }
  else
  {
    OE_DISABLE;
  }

  return true;
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
void uart_send(UINT LineNumber, UCHAR *String)
{
  UCHAR LineString[15];
  UCHAR TimeStamp[128];

  UINT Loop1UInt;
  UINT StartChar;


  /* Trap special control code for <HOME>. Replace "home" by appropriate control characters for "Home" on a VT101. */
  if (strcmp(String, "home") == 0)
  {
    String[0] = 0x1B; // ESC code
    String[1] = '[';
    String[2] = 'H';
    String[3] = 0x00;
  }

  /* Trap special control code for <CLS>. Replace "cls" by appropriate control characters for "Clear screen" on a VT101. */
  if (strcmp(String, "cls") == 0)
  {
    String[0] = 0x1B;
    String[1] = '[';
    String[2] = '2';
    String[3] = 'J';
    String[4] = 0x00;
  }

  /* Time stamp will not be printed if first character is a '-' (for title line when starting debug, for example),
     or if first character is a line feed '\r' when we simply want add line spacing in the debug log,
     or if first character is the beginning of a control stream (for example 'Home' or "Clear screen'). */
  if ((String[0] != '-') && (String[0] != '\r') && (String[0] != 0x1B) && (String[0] != '|'))
  {
    /* Send line number through UART. */
    printf("[%7u]", LineNumber);

    /* Retrieve current time stamp. */
    date_stamp(TimeStamp);

    /* Send time stamp through UART. */
    printf(TimeStamp);
  }

  /* Send string through UART. */
  printf(String);

  return;
}

/* $PAGE */
/* $TITLE=time_is_dst() */
/* ------------------------------------------------------------------ *\
        Check if a timestamp falls within Daylight Savings Time.
    Currently supports North America or European Union DST logic.
\* ------------------------------------------------------------------ */
bool time_is_dst(TIME_RTC Time_RTC, UINT8 DST_Flag){
  // DST disabled: Don't even bother calculating anything.
  if(DST_Flag==DST_NONE) return false;

  CurrentHour       = bcd_to_byte(Time_RTC.hour);
  CurrentMinute     = bcd_to_byte(Time_RTC.minutes);
  CurrentDayOfMonth = bcd_to_byte(Time_RTC.dayofmonth);
  CurrentMonth      = bcd_to_byte(Time_RTC.month);
  CurrentDayOfWeek  = bcd_to_byte(Time_RTC.dayofweek)%7+1; // SUN = 1, but faster
  Year4Digits = 2000 + bcd_to_byte(Time_RTC.year);

  // Calculate first and last Sundays. You are not expected to understand this.
  FirstSunday       = (CurrentDayOfMonth - CurrentDayOfWeek + 7) %7 + 1;
  LastSunday        = FirstSunday+28;
  if(LastSunday>get_month_days(Year4Digits, CurrentMonth)){
    LastSunday       -= 7;
  }

  // Else, it depends on where we are.
  switch(DST_Flag){
    case DST_NORTH_AMERICA:   
      // 02:00:00 second SUN in MAR  -until-  02:00:00 first SUN in NOV
      // Before MAR or after NOV: we are not in DST
      if (CurrentMonth < MAR || CurrentMonth > NOV) return false;
      // Between APR and OCT: we are always in DST
      if (CurrentMonth > MAR && CurrentMonth < NOV) return true;
      if (CurrentMonth == MAR){
        // Check if we are before or after second SUN
        if(CurrentDayOfMonth<FirstSunday+7) return false;
        if(CurrentDayOfMonth>FirstSunday+7) return true;
        // we are not before or after second SUN and therefore directly on it - Check the hour.
        return(CurrentHour>=2);
      }else{ // CurrentMonth == NOV, by process of elimination
        // Check for first SUN - same as above, but reversed
        if(CurrentDayOfMonth>FirstSunday) return false;
        if(CurrentDayOfMonth<FirstSunday) return true;
        return(CurrentHour<2);
      }
      break; /* Not strictly needed, because every because every possible 
                 if/else path ends in a break or return. 
                The compiler will detect this and exclude this line from 
                 the final program.*/
    case DST_EUROPE:          
      // 01:00:00Z  last SUN in MAR  -until-  01:00:00Z last SUN in OCT
      // see the NORTH_AMERICA section for explanation
      if (CurrentMonth < MAR || CurrentMonth > OCT) return false;
      if (CurrentMonth > MAR && CurrentMonth < OCT) return true;
      if (CurrentMonth == MAR){
        if(CurrentDayOfMonth<LastSunday) return false;
        if(CurrentDayOfMonth>LastSunday) return true;
        /* Time switches at 01:00 GMT, which is 02:00 local for most EU countries.
            Sorry, Ireland and Portugal.*/
        return(CurrentHour>=2);
      }else{ // CurrentMonth == OCT, by process of elimination
        if(CurrentDayOfMonth>LastSunday) return false;
        if(CurrentDayOfMonth<LastSunday) return true;
        // Time switches at 01:00 GMT, which is now 03:00 local time due to DST.
        return(CurrentHour<3);
      }
      break;
    default: 
      break;
  }
  return false;
}