/* ======================================================================== *\
   Pico-Clock-Green.c
   St-Louys Andre - February 2022
   astlouys@gmail.com
   Revision 31-MAR-2022
   Langage: Linux gcc
   Version 3.00

   Raspberry Pi Pico firmware to drive the Waveshare Pico Green Clock.
   From an original software version by Waveshare
   2020 Raspberry Pi (Trading) Ltd. SPDX-License-Identifier: BSD-3-Clause

   NOTE:
   THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
   WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
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
                      ...and assuming the characters used are standard Ascii characters, similar to English).
                    - Implement 5 X 7 character set with variable character width.
                    - Implement a reverse_bits() function allowing the bitmap of the 5 X 7 characters to be more intuitively defined.
                    - Add a generic "scroll_string()" function and a fill_display_buffer_5X7() function to easily handle 5 X 7 characters.
                    - Change the name of many functions to make them more representative of what they do.
                    - Make the scroll_string() function so that it can accept a string longer than what can be handled by the
                      framebuffer. The function will wait until the framebuffer get some free space to transfer next chunk of the string.
                    - Add a specific section at the top of the source code to select many default clock options at compile time.
                    - Implement different tone types with different number of tones and duration for different events.
                      (Note: Tone frequency can't be changed since the oscillator is integrated in the clock piezo).
                    - Clock display will blink the two center dots according to the number of seconds passed since the last minute change.
                    - Add automatic handling of "Daylight Saving Time" for most northern hemisphere countries.
                      (Provision has been made to add different schemes of daylight saving time for other areas of the world).
                    - Add a new option for hourly chime. It may be On, Off, or "Day" (OI for "On, Intermittent"). If set to "Day",
                      chime will sound only during day hours, as defined between CHIME_TIME_ON and CHIME_TIME_OFF set during clock setup.
                      (They are 9h00 and 21h00 by default).
                    - Add handling of "Calendar events" that can be added by user at compile time. During the specified day of the year,
                      the string defined by the user will scroll on the screen twice an hour, at xxh05 and xxh35. Moreover, a special sound
                      will be heard when the message start scrolling during daytime as defined between CHIME_TIME_ON and CHIME_TIME_OFF.
                    - Add an option in the clock setup to configure the Chime Time On and Chime Time Off.
   24-MAR-2022 3.00 - More code rework and optimisation.
                    - When powering up the clock, replace DST = 0xFF (hex value) by "DST = On" (and replace "DST = 0x00" by "DST = Off").
                    - Add logic for 2 different "repeat counts" for sounds, to add versatility and more possibilities for different sound codes.
                    - Day-of-month will change automatically if it becomes out-of-bound while setting up current month
                      (for example, day-of-month will change automatically from 31 to 28 while we change month from 3 (March) to 2 (February).
                    - Add suffix to day-of-month when scrolling the date in English (now "...March 31st...").
                    - Setting Chime time on and Chime time off now comply with 12 or 24-hours time format current setting.
                    - Fix a bug allowing Hourly Chime sound to be heard one hour later than the Chime time off.
                    - Fix a bug allowing Hourly Chime to sound sometimes while doing clock setup.
                    - Setting alarms now comply with 12 or 24-hours time format current setting.
                    - To help knowing which alarm is On (0 or 1), when Alarm 0 is On, Left Alarm LED is On in the
                      alarm indicator and when Alarm 1 is On, Right Alarm LED is On in the indicator.
                      NOTE: On power up, both alarms are set to OFF (this is also true in case of power failure), since there
                            is no backed-up RAM available in the RTC IC to save such variables in case of power failure.
                    - Change the logic so that each alarm (0 and 1) are now checked individually in the RTC IC.
                    - Add the logic for both alarms (0 and 1) to have a distinct (different) alarm sound.
                    - When setting up alarms, current alarm parameters saved in RTC IC are now proposed to user as default values.
                      Note: As mentioned above, alarm status (On or Off) is lost on power failure (and restart as Off), but other alarm
                            parameters (Hour, Minute, Day-of-week) are kept in RTC IC with battery back-up.
                    - Now blink the day-of-week while setting up day-of-week in alarm setup.
                    - Language selection is now a run-time option. English and French are the available languages for now.
                    - Hourly chime: add logic for "nighttime workers". If Chime time on is greater than Chime time off
                      (as opposed to what we would normally expect), we assume that we want sounds to be heard during
                      nighttime and not heard during daytime. Hourly chime will then sounds after Chime time on (in the evening)
                      and before Chime time off (in the morning).
                    - Add support for DHT22 (humidity and temperature sensor - compile time option). To implement this option, it must
                      be "#define" in the code, and a 3-wire cable must be installed between GPIO 8 VSYS and GND, and a DHT22
                      can be installed outdoor.
                    - Add Daily saving time as run-time parameter. User can now select 0 (no support for DST) or 1 (North-America-like DST support).
                    - Add some of the changes already made by David Ruck to fine tune auto brightness.
                    - When time display format is H12 (12-hours format), do not print a leading 0 to comply with the standard.
\* ======================================================================== */



/* ======================================================================== *\
        NOTE: Your comments - if they are constructive - are welcome:
                           astlouys@gmail.com
\* ======================================================================== */

/* ======================================================================== *\
    Other features / options that could be implemented in future releases:
   ========================================================================
    - Make more compile-time options available at run-time by making
      the list of clock setup steps longer. Some options currently
      "compile-time" only could be added to the list of "run-time" options
      (for example: sound ON/OFF, scroll period, etc...).
    - Add a Wi-Fi interface to the Pico, so that the clock can set its
      time by reading it from a time reference source on the Internet.
      (RTC IC is said to be very accurate, but I haven't tested it over
      a long period of time. Not sure what is the long-term precision).
    - More alarms could be processed directly through the Pico. As an
      inconvenient, these extra alarms would not be battery backed-up
      like the two current ones. However, they could provide more
      flexibility (for example, select any number of days for a specific
      alarm, different alarm times for different days of the week, etc...)
    - If a new version / update of the hardware is ever made, here are a
      few suggestions for improvements:
         > circuit to allow changing the frequency of the sound (I guess
           the Pico would be powerful enough to drive the piezo while
           performing its other functions (using 2nd core or PWM ?).
         > use an RTC IC with a few extra bytes of RAM integrated, so that
           some run-time options may be saved in case of a power failure
           (like Chime time on; Chime time off; Hourly chime mode; Hour
           display mode; Display scrolling on or off; Temperature unit;
           etc...)
         > replace the two unused white LEDs near the clock buttons by
           infrared LEDs (installed through "holes" in the plastic case),
           so that they can be used by software to emulate a "remote control"
           to turn on or off a radio, television, or other equipment at a
           specific time (and / or date). Infrared protocol support would
           need to be done "on the side", so that the protocol can be
           reproduced on the Pico Green Clock
\* ======================================================================== */



/* $TITLE=Clock configuration or options. */
/* $PAGE */
/* ======================================================================== *\
              ***** CLOCK CONFIGURATION OR OPTIONS *****
     SOME OF THESE ITEMS ARE ADJUSTABLE AT RUNTIME, OTHERS ARE NOT.
     NOTE: Structure "CalendarEvent" should also be initialized
           according to user needs. It can be found below,
           just above the "Global variables declaration / definition".
\* ======================================================================== */
/* Select the default language for date display. */
#define DEFAULT_LANGUAGE  ENGLISH   /// choices for now are FRENCH and ENGLISH.

/* If NO_SOUND is defined (uncommented), it allows turning OFF **ALL SOUNDS**.
   (For example, during development phase, if your wife is sleeping while
   you work late as was my case - smile). */
// #define NO_SOUND

/* Hourly chime default values. */
#define CHIME_DEFAULT   CHIME_DAY   // choices are CHIME_OFF / CHIME_ON / CHIME_DAY
#define CHIME_TIME_ON           9   // if "Chime day", "hourly chime" and "calendar event" will sound beginning at this time (in the morning).
#define CHIME_TIME_OFF         21   // if "Chime day", "hourly chime" and "calendar event" will sound for the last time at this time (in the evening).
/* NOTE: See also revision history above about "night time workers" support for hourly chime. */

/* Support of an optional (external) temperature and humidity sensor (DHT22) to read outside temperature.
   this part must be bought and installed by the user. It is not included with the Pico Green Clock. */
#define DHT_SUPPORT                 /// if a DHT22 has been added by user.

/* Determine if date scrolling will be enable by default when the clock start. */
#define SCROLL_DEFAULT    FLAG_ON    // choices are FLAG_ON / FLAG_OFF

/* Scroll one dot to the left every such milliseconds (100 is a good start point (lower = faster). */
#define SCROLL_DOT_TIME       100        // this is a UINT8 (must be between 0 and 255).

/* Date, temperature and power supply voltage will scroll at this frequency
   (in minutes - we must leave enough time for the previous scrolling to complete). */
#define SCROLL_PERIOD_MINUTE    5

/* Default temperature unit to display on the clock. */
#define TEMPERATURE_DEFAULT CELSIUS       // choices are CELSIUS and FAHRENHEIT.

/* Flag to include test code in the executable (conditional compile).
   Tests can be run before falling in the normal clock mode.
   Commenting out this #define will exclude test code and make the executable smaller. */
/// #define TEST_MODE

/* Time will be displayed in 24-hours format by default. */
#define TIME_DISPLAY_DEFAULT    H24  // choices are H12 and H24.

/* Exit setup mode after this period of inactivity (in seconds). So, this is a "time-out" value (in seconds).
   (for "clock setup", "alarm setup" or "timer setup"). */
#define TIME_OUT_PERIOD          20

/* Number of "tones" for each "sound pack" (first level of repetition). */
#define TONE_ALARM0_REPEAT_1     10
#define TONE_ALARM1_REPEAT_1      5
#define TONE_CHIME_REPEAT_1       2
#define TONE_EVENT_REPEAT_1       5
#define TONE_KEYCLICK_REPEAT_1    1
#define TONE_TIMER_REPEAT_1       8

/* Number of times the "sound pack" above will repeat itself for each tone type (second level of repetition).
   for example, if TONE_CHIME_REPEAT_1 is set to 3 and TONE_CHIME_REPEAT_2 is set to 2, we will hear:
   beep-beep-beep..........beep-beep-beep (three beeps, twice). */
#define TONE_ALARM0_REPEAT_2      2
#define TONE_ALARM1_REPEAT_2      3
#define TONE_CHIME_REPEAT_2       3
#define TONE_EVENT_REPEAT_2       3
#define TONE_KEYCLICK_REPEAT_2    1
#define TONE_TIMER_REPEAT_2       2

/* Time duration for different tone types (in milliseconds). This is the time between sounds inside one "sound pack".
   The time between consecutive one packs is 4 times this value (the ".........." in the example above). */
#define TONE_ALARM0_DURATION     40  // when sounding an alarm 0.
#define TONE_ALARM1_DURATION     60  // when sounding an alarm 1.
#define TONE_CHIME_DURATION      50  // when sounding an hourly chime.
#define TONE_EVENT_DURATION     100  // when scrolling a calendar event.
#define TONE_KEYCLICK_DURATION    7  // when pressing a button ("keyclick").
#define TONE_TIMER_DURATION      60  // when count-down timer reaches 00m00s.

/* Flag to handle automatically the daylight saving time.
   Since the moment to adjust time may be different depending in which country we are,
   choices should be added without disrupting algorithms already implemented. */
#define DAYLIGHT_SAVING_TIME   DST_NORTH_AMERICA  // choices for now are DST_NONE and DST_NORTH_AMERICA.

/* ======================================================================== *\
             ***** END OF CLOCK CONFIGURATION OR OPTIONS *****
\* ======================================================================== */





/* $TITLE=Definitions and include files. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                    Definitions and include files
\* ----------------------------------------------------------------- */
#define CELSIUS                0x00
#define CHIME_OFF              0x00         // hourly chime is OFF.
#define CHIME_ON               0x01         // hourly chime is ON.
#define CHIME_DAY              0x02         // hourly chime is ON during defined daily hours (between CHIME_TIME_ON and CHIME_TIME_OFF).
#define DISPLAY_BUFFER_SIZE    248          // size of framebuffer.
#define FAHRENHEIT             !CELSIUS     // temperature unit to display.
#define FLAG_OFF               0x00         // flag is OFF.
#define FLAG_ON                0xFF         // flag is ON.
#define H24                    FLAG_ON      // 24-hours time format.
#define H12                    FLAG_OFF     // 12-hours time format.
#define MAX_EVENTS             50           // maximum number of "calendar events" that can be programmed in flash memory of the Pico.
#define MAX_READINGS           100          // maximum number of "readings" when receiving data from DHT22 (one reading for each logic level).
#define MAX_SCROLL_QUEUE       75           // maximum number of messages in the scroll buffer queue (big enough for MAX_EVENTS defined for the same day + a few date scrolls).
#define TIMER_OFF              0x00         // timer is currently OFF.
#define TIMER_COUNT_DOWN       0x01         // timer mode is "count down".
#define TIMER_COUNT_UP         0x02         // timer mode is "count up".
#define TRUE                   0x01
#define FALSE                  0x00


/* Language and locals used. */
#define NO_LANGUAGE            0x00
#define ENGLISH                0x01
#define FRENCH                 0x02
#define SPANISH                0x03        // just a placeholder for now. Spanish is not implemented yet.


/* DAYLIGHT_SAVING_TIME choices are as below for now: */
#define DST_LO_LIMIT           0  // to make the logic easier in the firmware.
#define DST_NONE               0  // there is no "Daylight Saving Time" in user's country.
#define DST_NORTH_AMERICA      1  // daylight saving time in user's country is similar to what it is in North America.
#define DST_HI_LIMIT           2  // to make the logic easier in the firmware.
/* Add choices here and search for "DST_" in the code to add the specific algorithm. */


/* Status required when decoding data received from DHT22. */
#define STATE_FIRST_HALF       0x01
#define STATE_SECOND_HALF      0x02


/* Tone types definitions. */
#define TONE_ALARM0_TYPE       0x01         // for clock "alarm 0" (when an alarm is set).
#define TONE_ALARM1_TYPE       0x02         // for clock "alarm 1" (when an alarm is set).
#define TONE_CHIME_TYPE        0x03         // for clock "hourly chime" (when option is turned on).
#define TONE_EVENT_TYPE        0x04         // for "calendar event".
#define TONE_KEYCLICK_TYPE     0x05         // for "keyclick" event (when pressing a clock button).
#define TONE_TIMER_TYPE        0x06         // for "count-down" timer (no alarm on the count-up timer).


/* Clock mode definitions. */
#define MODE_UNDEFINED         0x00         // mode is currently undefined.
#define MODE_ALARM_SETUP       0x01         // user is setting up an alarm.
#define MODE_CLOCK_SETUP       0x02         // user is setting up clock parameters.
#define MODE_SCROLLING         0x03         // clock is scrolling data on the display.
#define MODE_SHOW_TIME         0x04         // clock is displaying time.
#define MODE_SHOW_VOLTAGE      0x05         // clock is displaying power supply voltage.
#define MODE_TEST              0x06         // clock is in test mode (to disable automatic clock behaviors on the display).
#define MODE_TIMER_SETUP       0x07         // user is setting up a timer.


/* Tags that can be used in process_scroll() function. */
#define TAG_DATE               0xFF         // tag used to scroll current date, temperature and power supply voltage.
#define TAG_DEBUG              0xFE         // tag used to scroll variables for debug purposes, even inside ISR.
#define TAG_DST                0xFD         // tag used to scroll a few program variables on the display to implement new Daylight Saving Time algorithm.
#define TAG_QUEUE              0xFC         // tag used to display "Head", "Tail", and "Tag" of currently used scroll queue (for debugging purposes).
#define TAG_ALARM              0xFB         // tag used to display current alarm parameters (for both alarm 0 and alarm 1).


/* DayOfWeek definitions. */
#define MON                    0x01         // Monday
#define TUE                    0x02         // Tuesday
#define WED                    0x03         // Wednesday
#define THU                    0x04         // Thursday
#define FRI                    0x05         // Friday
#define SAT                    0x06         // Saturday
#define SUN                    0x07         // Sunday


/* Month definitions. */
#define JAN                     1         // January
#define FEB                     2         // February
#define MAR                     3         // March
#define APR                     4         // April
#define MAY                     5         // May
#define JUN                     6         // June
#define JUL                     7         // July
#define AUG                     8         // August
#define SEP                     9         // September
#define OCT                    10         // October
#define NOV                    11         // November
#define DEC                    12         // December



/* NOTE: Clock setup step definitions:
   For technical reasons they are kept as variables and can be seen in the variables section below. */

/* Alarm setup steps definitions. */
#define SETUP_ALARM_LO_LIMIT   0x00
#define SETUP_ALARM_NUMBER     0x01
#define SETUP_ALARM_ON_OFF     0x02
#define SETUP_ALARM_HOUR       0x03
#define SETUP_ALARM_MINUTE     0x04
#define SETUP_ALARM_DAY        0x05
#define SETUP_ALARM_HI_LIMIT   0x06


/* Timer setup steps definitions. */
#define SETUP_TIMER_LO_LIMIT   0x00
#define SETUP_TIMER_UP_DOWN    0x01
#define SETUP_TIMER_MINUTE     0x02
#define SETUP_TIMER_SECOND     0x03
#define SETUP_TIMER_READY      0x04
#define SETUP_TIMER_HI_LIMIT   0x05



/* Include files. */
#include "stdint.h"
#include "pico/platform.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "Ds3231.h"
#include "bitmap.h"



typedef uint8_t  UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef unsigned char UCHAR;





/* $TITLE=Calendar events. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
       Calendar events have been implemented to be configurable
     by the user. However, they must be configured at compile time
         (they can't be configured once the clock is running).
       As a user may intuitively guess, the three parameters are:
       1) Day of month (1 to 31).
       2) Month (3-letters: JAN / FEB / MAR / APR / etc...)
       3) Text to scroll on the display, between "double-quotes"
          (The text is limited to 40 characters. It takes a
          relatively long time to read 40 characters of text on a
          scrolling display !)

   NOTE: Calendar events are verified twice an hour,
         at xxh05 and xxh35. The text configured will be scrolled on
         the clock display if the clock is in its "normal time display"
         mode (the text will not scroll if user is in a setup operation).
         Also, the tone will sound (according to TONE_EVENT_COUNT and
         TONE_EVENT_DURATION) during day hours, as defined between
         CHIME_TIME_ON and CHIME_TIME_OFF. Outside these hours, the text
         will also scroll on the display, but there will be no sound.
         NOTE: See also the "nighttime workers" information in the
               revision history above.
\* ----------------------------------------------------------------- */
struct Event
{
  UINT8 Day;
  UINT8 Month;
  UCHAR Description[41];
};


/* Events to scroll on clock display at specific dates. */
/* Must be customized by user. */
struct Event CalendarEvent[MAX_EVENTS] =
{
  { 0, JAN, "Debug"},  // if a valid date is put here, it will allow displaying useful debugging variables in "process_scroll_queue()" function.
  { 1, JAN, "Calendar event January 1st"},
  { 1, FEB, "Calendar event February 1st"},
  { 1, MAR, "Calendar event March 1st"},
  { 1, APR, "Calendar event April 1st"},
  { 1, MAY, "Calendar event May 1st"},
  { 1, JUN, "Calendar event June 1st"},
  { 1, MAR, "Calendar event July 1st"},
  { 1, MAR, "Calendar event August 1st"},
  { 1, MAR, "Calendar event September 1st"},
  { 1, MAR, "Calendar event October 1st"},
  { 1, MAR, "Calendar event November 1st"},
  { 1, MAR, "Calendar event December 1st"},

  { 1, JAN, "Happy New Year !!"},
  { 1, MAR, "John's Birthday !!!"},
  {14, FEB, "Valentine's Day."},
  {25, DEC, "Merry Christmas !!"},
};





/* $TITLE=Global variables declaration / definition. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
             Global variables declaration / definition.
\* ----------------------------------------------------------------- */
/* Clock setup step definitions. */
/* They are not initialized as #define since there is an exception to handle
   when setting the day-of-month and month in French and it appears to be the
   easiest way to handle this exception. */
UINT8 SETUP_NONE           = 0x00;
UINT8 SETUP_HOUR           = 0x01;
UINT8 SETUP_MINUTE         = 0x02;
UINT8 SETUP_MONTH          = 0x03;
UINT8 SETUP_DAY_OF_MONTH   = 0x04;
UINT8 SETUP_YEAR           = 0x05;
UINT8 SETUP_DST            = 0x06;
UINT8 SETUP_KEYCLICK       = 0x07;
UINT8 SETUP_SCROLLING      = 0x08;
UINT8 SETUP_LANGUAGE       = 0x09;
UINT8 SETUP_DISPLAY_MODE   = 0x0A;
UINT8 SETUP_HOURLY_CHIME   = 0x0B;
UINT8 SETUP_CHIME_TIME_ON  = 0x0C;
UINT8 SETUP_CHIME_TIME_OFF = 0x0D;
UINT8 SETUP_CLOCK_HI_LIMIT = 0x0E;



UINT8 AlarmId                  = 0;
UINT8 AlarmNumber              = 0;

UINT8 ChimeTimeOff             = CHIME_TIME_OFF;  // hourly chime will be heard at this hour for the last time in the evening.
UINT8 ChimeTimeOffDisplay      = CHIME_TIME_OFF;  // variable formatted to display in 12-hours or 24-hours format.
UINT8 ChimeTimeOn              = CHIME_TIME_ON;   // hourly chime will begin at this hour.
UINT8 ChimeTimeOnDisplay       = CHIME_TIME_ON;   // variable formatted to display in 12-hours or 24-hours format.
UINT8 CsCounter;
UINT8 CurrentClockMode         = MODE_UNDEFINED;  // current clock mode.
UINT8 CurrentDayOfMonth;
UINT8 CurrentHour;
UINT8 CurrentHourSetting       = 0;         // hours read and written to the RTC IC.
UINT8 CurrentMinute;
UINT8 CurrentMinuteSetting     = 0;         // minutes setting from / to real time clock IC.
UINT8 CurrentMonth;
UINT8 CurrentYearCentile       = 20;        // assume we are in years 20xx (could be changed in clock setup, but will revert to 20 at each power-up).
UINT8 CurrentYearLowPart;                   // lowest two digits of the year (battery backed-up).

UCHAR DaylightSavingTime = DAYLIGHT_SAVING_TIME;  // specifies how to handle the daylight saving time.
UCHAR DisplayBuffer[DISPLAY_BUFFER_SIZE];   // framebuffer containing the bitmap of the string to be displayed / scrolled on clock display.

UINT8 FlagAdcLightTime         = FLAG_OFF;
UINT8 FlagAlarmBeeping         = FLAG_OFF;  // flag indicating an alarm is sounding.
UINT8 FlagAlarmDaySet          = FLAG_OFF;  // flag indicating we are setting up Alarm Day.
UINT8 FlagAlarmEnable          = FLAG_OFF;  // flag indicating alarm is ON (either alarm 1 or alarm 2).
UINT8 FlagAlarmHourSet         = FLAG_OFF;  // flag indicating we are setting up Alarm Hour.
UINT8 FlagAlarmMinuteSet       = FLAG_OFF;  // flag indicating we are setting up Alarm Minute.
UINT8 FlagAlarmNumberSet       = FLAG_OFF;  // flag indicating we are in "Alarm Number" setting mode (there are 2 alarms available).
UINT8 FlagAlarmSetup           = FLAG_OFF;  // flag indicating we are in alarm setup mode.
UINT8 FlagAlarmToggleOnOff     = FLAG_OFF;  // flag indicating we are toggling Alarm On / Off.
UINT8 FlagAutoBrightness       = FLAG_OFF;  // flag indicating we are in "Auto Brightness" mode.
UINT8 FlagButtonUp             = FLAG_OFF;  // user pressed the "up" (Middle) button.
UINT8 FlagChimeDone            = FLAG_OFF;  // indicates that hourly chime has already been triggered for current hour.
UINT8 FlagDayLightSavingTime   = FLAG_OFF;  // flag indicating we are in "daylight saving time" mode.
UINT8 FlagFlashing[13]         = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // bitmap to logically "and" with a character.
UINT8 FlagIdleCheck            = FLAG_OFF;  // if ON, we keep track of idle time to eventually declare a time-out.
UINT8 FlagKeyclick             = FLAG_OFF;  // flag for keyclick ("button-press" tone)
UINT8 FlagScrollData           = FLAG_OFF;  // time has come to scroll data on the display.
UINT8 FlagScrollEnable         = SCROLL_DEFAULT;  // flag indicating the clock will scroll the date and temperature at regular intervals on the display.
UINT8 FlagScrollStart          = FLAG_OFF;  // flag indicating it is time to start scrolling.
UINT8 FlagSetChime             = FLAG_OFF;  // flag indicating we are setting up Hourly chime.
UINT8 FlagSetChimeTimeOff      = FLAG_OFF;  // flag indicating we are setting up the Chime time off .
UINT8 FlagSetChimeTimeOn       = FLAG_OFF;  // flag indicating we are setting up the Chime time on.
UINT8 FlagSetDisplayMode       = FLAG_OFF;  // flag indicating we are in time display mode setup (12 or 24-hours format).
UINT8 FlagSetDst               = FLAG_OFF;  // flag indicating we are setting up Daylight saving time.
UINT8 FlagSetKeyclick          = FLAG_OFF;  // flag indicating we are setting up Keyclick sound.
UINT8 FlagSetLanguage          = FLAG_OFF;  // flag indicating we are setting up the clock Language.
UINT8 FlagSetScroll            = FLAG_OFF;  // flag indicating we are setting up Scrolling mode ON / OFF.
UINT8 FlagSetTimeDayOfMonth    = FLAG_OFF;  // flag indicating we are setting up the time "DayOfMonth".
UINT8 FlagSetTimeHour          = FLAG_OFF;  // flag indicating we are setting up the time "Hour".
UINT8 FlagSetTimeMinute        = FLAG_OFF;  // flag indicating we are setting up the time "Minute".
UINT8 FlagSetTimeMonth         = FLAG_OFF;  // flag indicating we are setting up the time "Month".
UINT8 FlagSetTimeYear          = FLAG_OFF;  // flag indicating we are setting up the time "Year".
UINT8 FlagSetTimerCountDown    = FLAG_OFF;  // flag indicating the count down timer is active.
UINT8 FlagSetTimerCountUp      = FLAG_OFF;  // flag indicating count up timer is active.
UINT8 FlagSetTimerMinute       = FLAG_OFF;  // flag indicating we are in timer "Minutes" parameter setup.
UINT8 FlagSetTimerMode         = FLAG_OFF;  // flag indicating that we are selecting the timer mode (Off / Up / Down).
UINT8 FlagSetTimerSecond       = FLAG_OFF;  // flag indicating we are in timer "Seconds" parameter setup.
UINT8 FlagSetUp                = FLAG_OFF;  // user has pressed the "setup" (Top) button to enter setup mode.
UINT8 FlagSetupTime            = FLAG_OFF;  // flag indicating time need to be programmed in the real-time clock.
UINT8 FlagTimerCountDownEnd    = FLAG_OFF;  // flag indicating the count down timer reached final count (0m00s).
UINT8 FlagTone                 = FLAG_OFF;  // flag indicating there is a tone sounding.
UINT8 FlagToneOn               = FLAG_OFF;  // flag indicating it is time to make a tone.
UINT8 FlagUpdateTime           = FLAG_OFF;  // flag indicating it is time to refresh the time on the clock display.

UCHAR GetAddHigh               = 0x11;
UCHAR GetAddLow                = 0x12;

UINT8 HourlyChimeMode          = CHIME_DEFAULT;         // chime mode (Off / On / Day).

UINT8 IdleNumberOfSeconds;                              // keep track of the number of seconds the system has been idle.

UINT8 Language                 = DEFAULT_LANGUAGE;      // language used for date scrolling.
UINT8 LevelChange[MAX_READINGS];                        // logic level when communicating with DHT22.
UINT8 LightSetting             = 0;

UINT8 MonthDays[2][12]         = {{31,29,31,30,31,30,31,31,30,31,30,31},   // number of days in a month - "leap" year.
                                  {31,28,31,30,31,30,31,31,30,31,30,31}};  // number of days in a month - "normal" year.

UINT8 ScrollDotCount           = 0;         // keep track of "how many dots to the left" remain to be scrolled.
UINT8 ScrollQueue[MAX_SCROLL_QUEUE];        // circular buffer containing the tag of the next messages to be scrolled.
UINT8 ScrollQueueHead;                      // head of circular buffer.
UINT8 ScrollQueueTail;                      // tail of circular buffer.
UINT8 ScrollSecondCounter      = 0;         // keep track of number of seconds to reach time-to-scroll.
UINT8 ScrollStartCount         = 0;         // count the number of milli-seconds before scrolling one more dot position to the left.
UINT8 SecondCounter            = 0;         // cumulate the number of seconds for each minute.
UINT8 SetupStep                = 0;         // indicate the setup step we are through the clock setup, alarm setup, or timer setup.

UCHAR TemperaturePart1;                     // current temperature value, decimal part (before the decimal dot)   - Not for the DHT22.
UCHAR TemperaturePart2;                     // current temperature value, fractional part (after the decimal dot) - Not for the DHT22.
UINT8 TemperatureUnit          = TEMPERATURE_DEFAULT;   // CELSIUS or FAHRENHEIT default value (see above).
UINT8 TimeDisplayMode          = TIME_DISPLAY_DEFAULT;  // H24 or H12 default value (see above).
UINT8 TimerMinutes             = 0;
UINT8 TimerMode                = TIMER_OFF; // timer mode (0 = Off / 1 = Count down / 2 = Count up).
UINT8 TimerSeconds             = 0;
UINT8 TimerShowCount           = 0;         // in case we want to display timer less often than every second.
UINT8 TimerShowSecond          = 0;
UINT8 ToneMSecCounter          = 0;         // cumulate number of milliseconds for tone duration.
UINT8 ToneRepeatCount          = 0;         // number of "tones" to sound for different events.
UINT8 ToneType;                             // determine the type of "tone" we are sounding.

UINT8 UpId                     = 0;



UINT16 AdcCount                = 0;         // cumulative milli-seconds before reading power supply voltage.
UINT16 AdcLightValue;                       // relative ambient light value (for auto-brightness clock feature).
UINT16 AdcLightCount           = 0;         // cumulate milli-seconds before reading relative ambient light value.

UINT16 BottomKeyPressTime      = 0;         // keep track of the time the Down ("Bottom") key is pressed.
UINT16 Flashing_count          = 0;

UINT16 MiddleKeyPressTime      = 0;         // keep track of the time the Up ("Middle") key is pressed.

UINT16 TopKeyPressTime         = 0;         // keep track of the time the Set ("Top") key is pressed.

UINT16 Year4Digits;                         // year in 4-digits format.



UINT64 DhtErrors               = 0;         // cumulate errors while receiving DHT packets (for statistic purposes).
UINT64 DhtReading              = 0;         // cumulate total temperature requests with DHT22.

UINT64 TimerValue[MAX_READINGS];            // time stamp used while decoding DHT22 data.



TIME_RTC Time_RTC;



/* Current settings for alarm 0 and alarm 1. */
struct
{
  UINT8 FlagStatus;
  UINT8 Second;
  UINT8 Minute;
  UINT8 Hour;
  UINT8 Day;
} Alarm[2];





/* $TITLE=Function and global variable declarations. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
              Function and global variable declarations
\* ----------------------------------------------------------------- */
/* Read ambient light and power supply voltage from analog-to-digital gpio. */
void adc_show_count(void);

/* Clear the display framebuffer. */
void clear_framebuffer(UINT8 StartColumn);

/* Display the power supply voltage for a few seconds. */
void display_voltage(void);

/* Evaluate if it is time to blink data on the display. */
void evaluate_blinking_time(void);

/* Evaluate if it is time to start a keyclick ("button-press tone"). */
void evaluate_keyclick_start_time(void);

/* Evaluate if it is time to scroll characters on the display. */
void evaluate_scroll_time(void);

/* Evaluate if it is time to stop a "tone". */
void evaluate_sound_stop_time(void);

/* Fill the virtual framebuffer with the given ASCII character, beginning at the specified column position (using 5 X 7 character bitmap). */
UINT16 fill_display_buffer_5X7(UINT8 Column, UINT8 AsciiCharacter);

/* Fill the virtual framebuffer with the given ASCII character, beginning at the specified column position (using 4 X 7 character bitmap). */
void fill_display_buffer_4X7(UINT8 Column, UINT8 AsciiCharacter);

/* Read analog-to-digital interface. */
UINT16 get_ads1015(void);

/* Build the string containing the date to scroll on the clock display. */
void get_date_string(UCHAR *String);

/* Return the number of days in a specific month (while checking if it is a leap year or not for February). */
UINT8 get_month_days(UINT16 CurrentYear, UINT8 MonthNumber);

/* Get temperature from ADC IC. */
void get_temperature(void);

/* Get power supply voltage from analog-to-digital Pico gpio. */
void get_voltage(UCHAR *String);

/* Weekday = (day + 2 * mon + 3 * (mon + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7 */
UINT8 get_weekday(UINT16 year, UINT8 month_cnt, UINT8 date_cnt);

/* Initialize all required GPIOs. */
int init_gpio(void);

/* Handle the tag that has been put in scroll queue. */
void process_scroll_queue();

/* Read the temperature from external DHT22. */
UINT8 read_dht22(float *Temperature, float *Humidity);

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

/* One millisecond period callback function. */
bool timer_callback_ms(struct repeating_timer *t);

/* One second period callback function. */
bool timer_callback_s(struct repeating_timer *t);

/* 150 milliseconds period callback function. */
bool timer_callback_us(struct repeating_timer *t);

/* Make a tone for the specified number of milliseconds. */
void tone(UINT16 MilliSeconds);


#ifdef TEST_MODE
/* Perform different tests on the Pico Green Clock (to be used for testing and / or debugging). */
void test_mode(UINT8 TestType);
#endif


struct repeating_timer timer2;





/* $TITLE=Main program entry point. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                       Main program entry point
\* ----------------------------------------------------------------- */
int main(void)
{
  UCHAR String[DISPLAY_BUFFER_SIZE];

  UINT8 Loop1UInt8;
  UINT8 SundayCounter;

  UINT32 OutsideTemp;

  struct repeating_timer timer;
  struct repeating_timer timer1;



  /* --------------------------------------------------------------- *\
      Initialize all required GPIO ports of the Raspberry Pi Pico.
  \* --------------------------------------------------------------- */
  init_gpio();



  /* --------------------------------------------------------------- *\
                   Turn On white LEDs on entry.
  \* --------------------------------------------------------------- */
  IndicatorButtonLightsOn;



  /* --------------------------------------------------------------- *\
                  Initialize scroll queue on entry.
  \* --------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < MAX_SCROLL_QUEUE; ++Loop1UInt8)
    ScrollQueue[Loop1UInt8] = 0xAA;  // let's put 0xAA since 0x00 corresponds to calendar event number 0.
  ScrollQueueHead = 0;
  ScrollQueueTail = 0;



 /* --------------------------------------------------------------- *\
           Setup order in French is different than in English.
  \* --------------------------------------------------------------- */
  if (Language == FRENCH)
  {
    SETUP_DAY_OF_MONTH   = 0x03;
    SETUP_MONTH          = 0x04;
  }



  /* --------------------------------------------------------------- *\
         Retrieve current alarm parameters from RTC IC on entry.
  \* --------------------------------------------------------------- */
  /* Read current alarm parameters. */
  byte_data();

  /* Assign current parameters as default values for alarm 0... */
  Alarm[0].FlagStatus = FLAG_OFF;  // set alarm OFF on entry.
  Alarm[0].Day        = ByteData[DS3231_REG_A1D] & 0x0F;
  Alarm[0].Hour       = ByteData[DS3231_REG_A1H];
  Alarm[0].Minute     = ByteData[DS3231_REG_A1M];
  Alarm[0].Second     = ByteData[DS3231_REG_A1S];

  /* ... and also for alarm 1. */
  Alarm[0].FlagStatus = FLAG_OFF;  // set alarm OFF on entry.
  Alarm[1].Day        = ByteData[DS3231_REG_A2D] & 0x0F;
  Alarm[1].Hour       = ByteData[DS3231_REG_A2H];
  Alarm[1].Minute     = ByteData[DS3231_REG_A2M];



  /* --------------------------------------------------------------- *\
           Turn on the right temperature indicator on entry.
  \* --------------------------------------------------------------- */
  if (TemperatureUnit == FAHRENHEIT)
  {
    IndicatorFrnhtOn;
    IndicatorCelsiusOff;
  }
  else
  {
    IndicatorCelsiusOn;
    IndicatorFrnhtOff;
  }



  /* --------------------------------------------------------------- *\
          Turn on the right Hourly chime indicator on entry.
  \* --------------------------------------------------------------- */
  switch (HourlyChimeMode)
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



  /* --------------------------------------------------------------- *\
      Check if "Move On" (scrolling) indicator should be turned on
      (If FlagScrollEnable has been turned on in the clock options
         at compile time). This can also be changed in the clock
                     configuration at run time.
  \* --------------------------------------------------------------- */
  if (FlagScrollEnable == FLAG_ON)
    IndicatorScrollOn;



  /* Initialize callback function for 1 millisecond timer. */
  add_repeating_timer_ms(1, timer_callback_ms, NULL, &timer);

  /* Initialize callback function for 1 second timer. */
  add_repeating_timer_ms(1000, timer_callback_s, NULL, &timer1);





#ifdef TEST_MODE
  /* --------------------------------------------------------------- *\
                     Temporary testing zone below
  \* --------------------------------------------------------------- */
  /* If we are in test mode, jump to the generic test routine and perform tests there. */
  // test_mode(1);   // tests on RTC IC registers.
  // test_mode(2);   // not used for now.
  // test_mode(3);   // not used for now.
  // test_mode(4);   // temporarily used to display variables for debugging purposes.
  // test_mode(5);   // scrolling test.
  // test_mode(6);   // test variable character width and spacing.
  // test_mode(7);   // display 4 X 7 character map.
  // test_mode(8);   // display all characters of the 5 X 7 bitmap.
  // test_mode(9);   // tests with indicators on the left side of the display.
  // test_mode(10);  // turn on each bit of each byte of the active display matrix.
  // test_mode(11);  // display every 5 X 7 character between a left and right frames.
  // test_mode(12);  // display all 5 X 7 characters, one at a time, using fill_display_buffer_5X7() function.
  // test_mode(13);  // display a few 5 X 7 characters on the display.
  // test_mode(14);  // scroll numbers and capital letters of 4 X 7 characters.
  // test_mode(15);  // display information about system variables for debug  purposes.
  // test_mode(16);  // check each bit of each byte of the display matrix.
  // test_mode(17);  // play with clock display indicators.
  // test_mode(18);  // quick test with both character formats (4 X 7 and 5 X 7).
  /* --------------------------------------------------------------- *\
                         End of testing zone
  \* --------------------------------------------------------------- */
#endif





  /* Four short tones to indicate we are entering the main clock program. */
  tone(10);
  sleep_ms(100);
  tone(10);
  sleep_ms(100);
  tone(10);
  sleep_ms(100);
  tone(10);
  sleep_ms(100);



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
  if (DaylightSavingTime == DST_NORTH_AMERICA)
  {
    /* Assume we are not during "Daylight Saving Time" on entry... */
    FlagDayLightSavingTime = FLAG_OFF;


    /* ...then, check if our assumption is correct... */
    /* Read the real-time clock IC DS3231. */
    Time_RTC = Read_RTC();


    /* Convert to binary values. */
    CurrentHour         = bcd_to_byte(Time_RTC.hour);
    CurrentMinute       = bcd_to_byte(Time_RTC.minutes);
    CurrentDayOfMonth   = bcd_to_byte(Time_RTC.dayofmonth);
    CurrentMonth        = bcd_to_byte(Time_RTC.month);
    Year4Digits         = 2000 + bcd_to_byte(Time_RTC.year);


    /* Between (and including) April and October, we must be in "Daylight Saving Time". */
    if ((CurrentMonth >= APR) && (CurrentMonth <= OCT))
      FlagDayLightSavingTime = FLAG_ON;



    /* ------------------------------------------------------------- *\
                   If we are in March or November,
         it could be "Daylight Saving Time" or not... let's check.
         First, check the case if we are near the "March" boundary.
    \* ------------------------------------------------------------- */
    if (CurrentMonth == MAR)
    {
      /* Check if we are "at" or "passed" the second Sunday of March. */
      SundayCounter = 0;
      for (Loop1UInt8 = CurrentDayOfMonth; Loop1UInt8 > 0; --Loop1UInt8)
      {
        /* Check how many Sunday's have already passed since beginning of March. */
        if (get_weekday(Year4Digits, CurrentMonth, Loop1UInt8) == SUN)
        {
          ++SundayCounter;
        }
      }


      /* If we are "passed" the second Sunday of March, then we must be in Daylight Saving Time. */
      if ((SundayCounter > 2) || ((SundayCounter == 2) && (get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) != SUN)))
        FlagDayLightSavingTime = FLAG_ON;


      /* If we are "at" the second Sunday of March, then it depends what time it is... */
      if ((SundayCounter == 2) && (get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) == SUN) && (CurrentHour > 2))
        FlagDayLightSavingTime = FLAG_ON;
    }



    /* ------------------------------------------------------------- *\
        Then, check the case if we are near the "November" boundary.
    \* ------------------------------------------------------------- */
    if (CurrentMonth == NOV)
    {
      /* Check if we are "at" or "before" the first Sunday of November. */
      SundayCounter = 0;
      for (Loop1UInt8 = CurrentDayOfMonth; Loop1UInt8 > 0; --Loop1UInt8)
      {
        if (get_weekday(Year4Digits, CurrentMonth, Loop1UInt8) == SUN)
          ++SundayCounter;
      }

      /* If we are before the first Sunday of November, then we must be in Daylight Saving Time. */
      if (SundayCounter == 0)
        FlagDayLightSavingTime = FLAG_ON;


      /* If we are "at" the first Sunday of November, it depends what time it is... */
      if ((SundayCounter == 1) && (get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) == SUN))
        if (CurrentHour < 2)
          FlagDayLightSavingTime = FLAG_ON;
    }
  }



  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  if (DAYLIGHT_SAVING_TIME == DST_NONE)
  {
    if (Language == FRENCH)
      scroll_string(24, "DST = Non");

    if (Language == ENGLISH)
      scroll_string(24, "DST = None");

  }
  else
  {
    /* Announce if Daily Saving Time ("DST") is currently On or Off, depending of current date. */
    if (FlagDayLightSavingTime == FLAG_ON)
      scroll_string(24, "DST = On");
    else
      scroll_string(24, "DST = Off");
  }


  /* Also display power supply voltage on entry. */
  get_voltage(String);
  scroll_string(24, String);


#ifdef NO_SOUND
  /* Special warning if sound has been cut-off in source code. */
  if (Language == ENGLISH)
    scroll_string(24, "WARNING - SOUND CUT-OFF");

  if (Language == FRENCH)
    scroll_string(24, "ATTENTION - PAS DE SON");
#endif





  /* Configure the GPIO associated to the three push-buttons of the clock in "input" mode. */
  gpio_set_dir(SET_BUTTON,  GPIO_IN);
  gpio_set_dir(UP_BUTTON,   GPIO_IN);
  gpio_set_dir(DOWN_BUTTON, GPIO_IN);

  /* Setup a pull-up on those three GPIOs. */
  gpio_pull_up(SET_BUTTON);
  gpio_pull_up(UP_BUTTON);
  gpio_pull_up(DOWN_BUTTON);



  /* Turn Off white LEDs when data stopped scrolling and before entering main program loop. */
  while (ScrollDotCount)
    sleep_ms(100);
  IndicatorButtonLightsOff;



  /* Main program loop... will loop forever. */
  while (TRUE)
  {
    /* If user pressed the "Setup" (top) button (detected in the call-back function), proceed with clock setup. */
    if (FlagSetUp == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON;   // keep track of inactive (idle) time to eventually declare a time-out.
      setup_clock_frame();       // enter clock setup mode.
      FlagSetUp     = FLAG_OFF;  // reset clock setup mode flag when done.
    }



    /* If we just entered the "Alarm" setup mode (detected in the call-back function), proceed with alarm setup. */
    if (FlagAlarmSetup == FLAG_ON)
    {
      FlagIdleCheck  = FLAG_ON;   // keep track of inactive (idle) time to eventually declare a time-out.
      setup_alarm_frame();        // enter alarm setup mode.
      FlagAlarmSetup = FLAG_OFF;  // reset alarm setup mode flag when done.
    }



    /* If we just entered the "Timer" setup mode (detected in the call-back function), proceed with timer setup. */
    if (FlagButtonUp == FLAG_ON)
    {
      FlagIdleCheck = FLAG_ON;   // keep track of inactive (idle) time to eventually declare a time-out.
      setup_timer_frame();       // enter timer setup mode.
      FlagButtonUp  = FLAG_OFF;  // reset timer setup mode when done.
    }



    /* Check if it is time to update time display on the clock. */
    if (FlagUpdateTime == FLAG_ON)
    {
      show_time();
      FlagUpdateTime = FLAG_OFF;
    }



    /* If a tag has been inserted in the scroll queue, process it. */
    if (ScrollQueueHead != ScrollQueueTail)
    {
      process_scroll_queue();
    }
  }

  return 0;
}





/* $TITLE=adc_show() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
        Read ambient light from Pico's analog-to-digital gpio.
\* ----------------------------------------------------------------- */
void adc_show_count(void)
{
  /* If the clock has been setup for automatic ambient light detection,
     get ambient light value once every second from analog-to-digital converter. */
  if (FlagAutoBrightness == FLAG_ON)
  {
    ++AdcLightCount;

    if (AdcLightCount == 1000)
    {
      /* Take a reading every second (1000 milliseconds). */
      AdcLightCount = 0;
      AdcLightValue = get_ads1015();
    }
  }

  return;
}





/* $TITLE=clear_framebuffer() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
               Clear the clock virtual framebuffer
              starting at the given column position.
         If the start column is inside the "real" display,
             then the closk display will be cleared.
\* ----------------------------------------------------------------- */
void clear_framebuffer(UINT8 StartColumn)
{
  do
  {
    fill_display_buffer_4X7(StartColumn, ' ');
    StartColumn += 8;
  }while (StartColumn < sizeof(DisplayBuffer));

  return;
}





/* $TITLE=display_voltage() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
         Display the power supply voltage for a few seconds.
      This function is only used in the test section for now,
    but it has been left in the code for an eventual use later...
\* ----------------------------------------------------------------- */
void display_voltage(void)
{
  CurrentClockMode = MODE_SHOW_VOLTAGE;


  const float conversion_factor = 3.3f / (1 << 12);

  UINT16 result = adc_read();


  float voltage = 3 * result * conversion_factor;

  UINT8 Single_digit = (int) voltage;
  UINT8 Decile       = (int)(voltage * 10)  % 10;
  UINT8 Percentile   = (int)(voltage * 100) % 10;



  /* Display power supply voltage (2 digits after decimal). */
  fill_display_buffer_4X7(0,  Single_digit + '0');
  fill_display_buffer_4X7(5, '.');
  fill_display_buffer_4X7(7,  Decile + '0');
  fill_display_buffer_4X7(12, Percentile + '0');
  fill_display_buffer_4X7(17, 'V');

  return;
}





/* $TITLE=evaluate_blinking_time() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
         Evaluate if it is time to blink data on the display.
\* ----------------------------------------------------------------- */
void evaluate_blinking_time(void)
{
  UCHAR String[256];

  static UINT16 DotBlinkCount;



  /* Check if we are in setup mode (we usually need to blink while in this mode). */
  if (SetupStep != SETUP_NONE)
  {
    ++Flashing_count;

    if (Flashing_count == 500)
    {
      /* Toggle blink status every 500 milliseconds (half a second). */
      Flashing_count = 0;  // reset flashing count for next pass.
      FlagFlashing[SetupStep] = ~FlagFlashing[SetupStep];  // toggle from 0x00 to 0xFF and vice-versa

      if (AlarmId == 1)
        FlagAlarmSetup = FLAG_ON;
      else if (UpId == 1)
        FlagButtonUp = FLAG_ON;
      else
        FlagSetUp = FLAG_ON;


      /* If we are setting up alarm day-of-week, blink current selection. */
      if (CurrentClockMode == MODE_ALARM_SETUP)
      {
        if (SetupStep == SETUP_ALARM_DAY)
        {
          if (FlagFlashing[SetupStep])
          {
            /* Turn on day-of-week-indicator ("blink On"). */
            select_weekday((Alarm[AlarmNumber].Day & 0x0F) - 1);
          }
          else
          {
            /* Turn off day-of-week-indicator ("blink Off"). */
            select_weekday(10);  // 10 is a special case for "turn them off all".
         }
        }
      }
    }
  }



  /* While the clock is displayed, blink the top "middle dot" during the first 20 seconds,
     then the bottom "middle dot" during 20 to 40 seconds, and finally both "middle dots"
     between 40 and 59 seconds. */
  if (CurrentClockMode == MODE_SHOW_TIME)
  {
    ++DotBlinkCount;

    /* 500 means "erase the target dot(s)". */
    if (DotBlinkCount == 500)
    {
      if (SecondCounter < 20)
      {
        /* From 0 to 19 seconds, blink the top "middle dot". */
        DisplayBuffer[11] &= 0xEF;  // slim ":" - top dot only.

        /* Make sure the previous dot (bottom dot) is back on. */
        DisplayBuffer[13] |= 0x10;  // slim ":" - bottom dot only.
      }
      else if (SecondCounter < 40)
      {
        /* From 20 to 39 seconds, blink the bottom "middle dot". */
        DisplayBuffer[13] &= 0xEF;  // slim ":" - bottom dot only.

        /* Make sure the previous dot (top dot) is back on. */
        DisplayBuffer[11] |= 0x10;  // slim ":" - top dot only.
      }
      else
      {
        /* From 40 to 59 seconds, blink both "middle dot". */
        DisplayBuffer[11] &= 0xEF;  // slim ":" - top dot only.
        DisplayBuffer[13] &= 0xEF;  // slim ":" - bottom dot only.
      }
    }


    /* 1000 means "redraw the target dot(s)". */
    if (DotBlinkCount == 1000)
    {
      if (SecondCounter < 20)
      {
        /* From 0 to 19 seconds, blink the top "middle dot". */
        DisplayBuffer[11] |= 0x10;  // slim ":" - top dot only.
      }
      else if (SecondCounter < 40)
      {
        /* From 20 to 39 seconds, blink the bottom "middle dot". */
        DisplayBuffer[13] |= 0x10;  // slim ":" - bottom dot only.
      }
      else
      {
        /* From 40 to 59 seconds, blink both "middle dot". */
        DisplayBuffer[11] |= 0x10;  // slim ":" - top dot.
        DisplayBuffer[13] |= 0x10;  // slim ":" - bottom dot.
      }

      DotBlinkCount = 0;  // reset DotBlinkCount
    }
  }

  return;
}





/* $TITLE=evaluate_keyclick_start_time() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
          Evaluate if it is time to start a "keyclick tone".
\* ----------------------------------------------------------------- */
void evaluate_keyclick_start_time(void)
{
  if (FlagKeyclick == FLAG_ON)
  {
    gpio_put(BUZZ, 1);
    FlagToneOn      = FLAG_ON;
    ToneType        = TONE_KEYCLICK_TYPE;
    ToneRepeatCount = 0;  // initialize repeat count when starting.
  }

  return;
}





/* $TITLE=evaluate_sound_stop_time() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
               Evaluate if it is time to stop a "tone".
\* ----------------------------------------------------------------- */
void evaluate_sound_stop_time(void)
{
  static UINT8 FlagPause;
  static UINT8 ToneRepeatCount2;

  static UINT16 CountDownPause;



  /* If we're waiting for a level 2 pause, do not waste time. */
  if (CountDownPause != 0)
  {
    --CountDownPause;
    return;
  }



  if (FlagToneOn == FLAG_ON)
  {
    ++ToneMSecCounter;

    /* Check if we reached the time duration for the sound (or the silence) for a TONE_ALARM0_TYPE. */
    if ((ToneType == TONE_ALARM0_TYPE) && (ToneMSecCounter >= TONE_ALARM0_DURATION))  // number of milliseconds for alarm 0.
    {
      if (FlagPause == FLAG_ON)
      {
        /* Tone has already been paused, but there are more tones to play. */
        gpio_put(BUZZ, 1);
        FlagPause = FLAG_OFF;
        ToneMSecCounter = 0;  // reset ToneMSecCounter.
      }
      else
      {
        /* We reach the specified duration for this tone type, turn sound OFF. */
        gpio_put(BUZZ, 0);
        FlagPause = FLAG_ON;
        ++ToneRepeatCount;    // one more "tone" played for this event type.
        ToneMSecCounter = 0;  // reset ToneMSecCounter.

        if (ToneRepeatCount >= TONE_ALARM0_REPEAT_1)
        {
          /* We played all tones for this "sound pack". */
          ToneRepeatCount = 0;         // reset ToneRepeatCount.
          FlagToneOn      = FLAG_OFF;  // no more tone going on.
          FlagPause       = FLAG_OFF;  // no more considered a pause.


          /* Check if there are more "sound packs" to generate. */
          if (TONE_ALARM0_REPEAT_2 != 0)
          {
            ++ToneRepeatCount2;
            FlagToneOn = FLAG_ON;
            FlagPause  = FLAG_ON;
            if (ToneRepeatCount2 >= TONE_ALARM0_REPEAT_2)
            {
              /* We played all "sound packs". */
              ToneRepeatCount  = 0;         // reset ToneRepeatCount.
              ToneRepeatCount2 = 0;
              FlagToneOn       = FLAG_OFF;  // no more tone going on.
              FlagPause        = FLAG_OFF;  // no more considered a pause.
            }
            else
            {
              /* More "sound packs" to come. */
              CountDownPause = TONE_ALARM0_DURATION * 4;
            }
          }
        }
        else
        {
          /* There are more tones to play for this event. */
        }
      }
    }



    /* Check if we reached the time duration for the sound (or the silence) for a TONE_ALARM1_TYPE. */
    if ((ToneType == TONE_ALARM1_TYPE) && (ToneMSecCounter >= TONE_ALARM1_DURATION))  // number of milliseconds for alarm 1.
    {
      if (FlagPause == FLAG_ON)
      {
        /* Tone has already been paused, but there are more tones to play. */
        gpio_put(BUZZ, 1);
        FlagPause = FLAG_OFF;
        ToneMSecCounter = 0;  // reset ToneMSecCounter.
      }
      else
      {
        /* We reach the specified duration for this tone type, turn sound OFF. */
        gpio_put(BUZZ, 0);
        FlagPause = FLAG_ON;
        ++ToneRepeatCount;    // one more "tone" played for this event type.
        ToneMSecCounter = 0;  // reset ToneMSecCounter.

        if (ToneRepeatCount >= TONE_ALARM1_REPEAT_1)
        {
          /* We played all tones for this "sound pack". */
          ToneRepeatCount = 0;         // reset ToneRepeatCount.
          FlagToneOn      = FLAG_OFF;  // no more tone going on.
          FlagPause       = FLAG_OFF;  // no more considered a pause.


          /* Check if there are more "sound packs" to generate. */
          if (TONE_ALARM1_REPEAT_2 != 0)
          {
            ++ToneRepeatCount2;
            FlagToneOn = FLAG_ON;
            FlagPause  = FLAG_ON;
            if (ToneRepeatCount2 >= TONE_ALARM1_REPEAT_2)
            {
              /* We played all "sound packs". */
              ToneRepeatCount  = 0;         // reset ToneRepeatCount.
              ToneRepeatCount2 = 0;
              FlagToneOn       = FLAG_OFF;  // no more tone going on.
              FlagPause        = FLAG_OFF;  // no more considered a pause.
            }
            else
            {
              /* More "sound packs" to come. */
              CountDownPause   = TONE_ALARM1_DURATION * 4;
            }
          }
        }
        else
        {
          /* There are more tones to play for this event. */
        }
      }
    }



    /* Check if we reached the time duration for the sound (or the silence) for a TONE_CHIME. */
    if ((ToneType == TONE_CHIME_TYPE) && (ToneMSecCounter >= TONE_CHIME_DURATION))  // number of milliseconds for chime.
    {
      if (FlagPause == FLAG_ON)
      {
        /* Tone has already been paused, but there are more tones to play. */
        gpio_put(BUZZ, 1);
        FlagPause = FLAG_OFF;
        ToneMSecCounter = 0;  // reset ToneMSecCounter.
      }
      else
      {
        /* We reach the specified duration for this tone type, turn sound OFF. */
        gpio_put(BUZZ, 0);
        FlagPause = FLAG_ON;
        ++ToneRepeatCount;    // one more "tone" played for this event type.
        ToneMSecCounter = 0;  // reset ToneMSecCounter.

        if (ToneRepeatCount >= TONE_CHIME_REPEAT_1)
        {
          /* We played all tones for this "sound pack". */
          ToneRepeatCount = 0;         // reset ToneRepeatCount.
          FlagToneOn      = FLAG_OFF;  // no more tone going on.
          FlagPause       = FLAG_OFF;  // no more considered a pause.

          /* Check if there are more "sound packs" to generate. */
          if (TONE_CHIME_REPEAT_2 != 0)
          {
            ++ToneRepeatCount2;
            FlagToneOn = FLAG_ON;
            FlagPause  = FLAG_ON;
            if (ToneRepeatCount2 >= TONE_CHIME_REPEAT_2)
            {
              /* We played all "sound packs". */
              ToneRepeatCount  = 0;         // reset ToneRepeatCount.
              ToneRepeatCount2 = 0;
              FlagToneOn       = FLAG_OFF;  // no more tone going on.
              FlagPause        = FLAG_OFF;  // no more considered a pause.
            }
            else
            {
              /* More "sound packs" to come. */
              CountDownPause   = TONE_CHIME_DURATION * 4;
            }
          }
        }
        else
        {
          /* There are more tones to play for this event. */
        }
      }
    }



    /* Check if we reached the time duration for the sound (or the silence) for a TONE_EVENT. */
    if ((ToneType == TONE_EVENT_TYPE) && (ToneMSecCounter >= TONE_EVENT_DURATION))  // number of milliseconds for a calendar event.
    {
      if (FlagPause == FLAG_ON)
      {
        /* Tone has already been paused, but there are more tones to play. */
        gpio_put(BUZZ, 1);
        FlagPause = FLAG_OFF;
        ToneMSecCounter = 0;  // reset ToneMSecCounter.
      }
      else
      {
        /* We reach the specified duration for this tone type, turn sound OFF. */
        gpio_put(BUZZ, 0);
        FlagPause = FLAG_ON;
        ++ToneRepeatCount;    // one more "tone" played for this event type.
        ToneMSecCounter = 0;  // reset ToneMSecCounter.

        if (ToneRepeatCount >= TONE_EVENT_REPEAT_1)
        {
          /* We played all tones for this "sound pack". */
          ToneRepeatCount = 0;         // reset ToneRepeatCount.
          FlagToneOn      = FLAG_OFF;  // no more tone going on.
          FlagPause       = FLAG_OFF;  // no more considered a pause.


          /* Check if there are more "sound packs" to generate. */
          if (TONE_EVENT_REPEAT_2 != 0)
          {
            ++ToneRepeatCount2;
            FlagToneOn = FLAG_ON;
            FlagPause  = FLAG_ON;
            if (ToneRepeatCount2 >= TONE_EVENT_REPEAT_2)
            {
              /* We played all "sound packs". */
              ToneRepeatCount  = 0;         // reset ToneRepeatCount.
              ToneRepeatCount2 = 0;
              FlagToneOn       = FLAG_OFF;  // no more tone going on.
              FlagPause        = FLAG_OFF;  // no more considered a pause.
            }
            else
            {
              /* More "sound packs" to come. */
              CountDownPause   = TONE_EVENT_DURATION * 4;
            }
          }
        }
        else
        {
          /* There are more tones to play for this event. */
        }
      }
    }



    /* Check if we reached the time duration for the sound (or the silence) for a TONE_KEYCLICK. */
    if ((ToneType == TONE_KEYCLICK_TYPE) && (ToneMSecCounter >= TONE_KEYCLICK_DURATION))  // number of milliseconds for keyclick.
    {
      if (FlagPause == FLAG_ON)
      {
        /* Tone has already been paused previously, but there are more tones to play. */
        gpio_put(BUZZ, 1);
        FlagPause = FLAG_OFF;
        ToneMSecCounter = 0;  // reset ToneMSecCounter.
      }
      else
      {
        /* We reach the specified duration for this tone type, turn sound OFF. */
        gpio_put(BUZZ, 0);
        FlagPause = FLAG_ON;
        ++ToneRepeatCount;    // one more "tone" played for this event type.
        ToneMSecCounter = 0;  // reset ToneMSecCounter.

        if (ToneRepeatCount >= TONE_KEYCLICK_REPEAT_1)
        {
          /* We played all tones for this "sound pack". */
          ToneRepeatCount = 0;         // reset ToneRepeatCount.
          FlagToneOn      = FLAG_OFF;  // no more tone going on.
          FlagPause       = FLAG_OFF;  // no more considered a pause.


          /* Check if there are more "sound packs" to generate. */
          if (TONE_KEYCLICK_REPEAT_2 != 0)
          {
            ++ToneRepeatCount2;
            FlagToneOn = FLAG_ON;
            FlagPause  = FLAG_ON;
            if (ToneRepeatCount2 >= TONE_KEYCLICK_REPEAT_2)
            {
              /* We played all "sound packs". */
              ToneRepeatCount  = 0;         // reset ToneRepeatCount.
              ToneRepeatCount2 = 0;
              FlagToneOn       = FLAG_OFF;  // no more tone going on.
              FlagPause        = FLAG_OFF;  // no more considered a pause.
            }
            else
            {
              /* More "sound packs" to come. */
              CountDownPause   = TONE_KEYCLICK_DURATION * 4;
            }
          }
        }
        else
        {
          /* There are more tones to play for this event. */
        }
      }
    }



    /* Check if we reached the time duration for the sound (or the silence) for a TONE_TIMER. */
    if ((ToneType == TONE_TIMER_TYPE) && (ToneMSecCounter >= TONE_TIMER_DURATION))  // number of milliseconds for timer.
    {
      if (FlagPause == FLAG_ON)
      {
        /* Tone has already been paused previously, but there are more tones to play. */
        gpio_put(BUZZ, 1);
        FlagPause = FLAG_OFF;
        ToneMSecCounter = 0;  // reset ToneMSecCounter.
      }
      else
      {
        /* We reach the specified duration for this tone type, turn sound OFF. */
        gpio_put(BUZZ, 0);
        FlagPause = FLAG_ON;
        ++ToneRepeatCount;    // one more "tone" played for this event type.
        ToneMSecCounter = 0;  // reset ToneMSecCounter.

        if (ToneRepeatCount >= TONE_TIMER_REPEAT_1)
        {
          /* We played all tones for this "sound pack". */
          ToneRepeatCount = 0;         // reset ToneRepeatCount.
          FlagToneOn      = FLAG_OFF;  // no more tone going on.
          FlagPause       = FLAG_OFF;  // no more considered a pause.


          /* Check if there are more "sound packs" to generate. */
          if (TONE_TIMER_REPEAT_2 != 0)
          {
            ++ToneRepeatCount2;
            FlagToneOn = FLAG_ON;
            FlagPause  = FLAG_ON;
            if (ToneRepeatCount2 >= TONE_TIMER_REPEAT_2)
            {
              /* We played all "sound packs". */
              ToneRepeatCount  = 0;         // reset ToneRepeatCount.
              ToneRepeatCount2 = 0;
              FlagToneOn       = FLAG_OFF;  // no more tone going on.
              FlagPause        = FLAG_OFF;  // no more considered a pause.
            }
            else
            {
              /* More "sound packs" to come. */
              CountDownPause   = TONE_TIMER_DURATION * 4;
            }
          }
        }
        else
        {
          /* There are more tones to play for this event. */
        }
      }
    }
  }

  return;
}





/* $TITLE=evaluate_scroll_time() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
              Evaluate if it is time to scroll characters
                 one dot to the left on clock display.
\* ----------------------------------------------------------------- */
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
      scroll_one_dot();  // scroll one more dot to the left on clock display.
      --ScrollDotCount;  // one less dot to be scrolled on the display.
    }
    else
    {
      fill_display_buffer_4X7(24, ' '); // blank the first "invisible column" at the right of the display buffer when done.
      FlagScrollStart = FLAG_OFF;       // reset scroll start flag when done.
      FlagUpdateTime  = FLAG_ON;        // request a time update on the clock.
    }

    ScrollStartCount = 0;  // reset the millisecond counter to wait for next dot scroll to the left.
  }

  return;
}





/* $TITLE=fill_display_buffer_5X7() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
      Set the display framebuffer with the given ASCII character
              beginning at the given column position.
          Character width is also passed as an argument.
      NOTE: left-most column visible on clock display is column 2
            columns 0 and 1 are used by indicators.
      This function returns the position of the next free column.
\* ----------------------------------------------------------------- */
UINT16 fill_display_buffer_5X7(UINT8 Column, UINT8 AsciiCharacter)
{
  UINT8 ColumnInSection;
  UINT8 ColumnPosition;
  UINT8 RowNumber;
  UINT8 SectionNumber;

  UINT16 TableOffset;
  UINT16 Width;



  /* If column number passed as an argument is out-of-bound, return DISPLAY_BUFFER_SIZE. */
  if (ColumnPosition > (UINT16)DISPLAY_BUFFER_SIZE) return DISPLAY_BUFFER_SIZE;


  /* First 2 columns are reserved for indicators. */
  if (Column < 2) Column = 2;


  SectionNumber   = Column / 8;
  ColumnInSection = Column % 8;



  /* Special handling of character 0x00 used to blink characters on the display. */
  if (AsciiCharacter == 0x00) AsciiCharacter = 32;  // blank ("space") character.


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





/* $TITLE=fill_display_buffer_4X7() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
         Fill the framebuffer with the 4 X 7 character bitmap
              corresponding to the given ASCII character,
   beginning at the specified column position of the display memory.
   If the selected columns are located in the "physical" section of
      the display memory, the character will show up immediately.
    If the selected columns are located in the "virtual" section of
   the display memory, successive calls to scroll_one_dot() function
   are required for the characters to be scrolled inside the visible
                    part of the clock display.
\* ----------------------------------------------------------------- */
void fill_display_buffer_4X7(UINT8 Column, UINT8 AsciiCharacter)
{
  UINT8 BitmapCharacter;
  UINT8 ColumnInSection;
  UINT8 RowNumber;
  UINT8 SectionNumber;



  Column         += DisplayOffset;  // first 2 columns are used by "indicators".
  SectionNumber   = Column / 8;     // display is divided in sections of 8 columns.
  ColumnInSection = Column % 8;     // determine the relative column number is the specific section.


  /* Determine the character in the bitmap table corresponding to the ASCII character we want to display. */
  if ((AsciiCharacter >= 0x2D) && (AsciiCharacter <= 0x2D + 83))
    BitmapCharacter = AsciiCharacter - 0x2D;
  else switch(AsciiCharacter)
  {
    case 0:                                   // ASCII value 0x00
      /* Special blank character when blinking. */
      BitmapCharacter = 46;
    break;

    case 0x16:                                // ASCII value 0x16
      /* Slim ":" */
      BitmapCharacter = 13;
    break;

    case ' ':                                 // ASCII value 0x20
      /* Blank " " ("space") character. */
      BitmapCharacter = 46;
    break;

    default:
      /* Not a valid character, display symbol "?" */
      BitmapCharacter = 49;
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





/* $TITLE=get_ads() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                     Read analog-to-digital gpio.
\* ----------------------------------------------------------------- */
UINT16 get_ads1015(void)
{
  adc_select_input(0);
  UINT16 value = adc_read();

  return value;
}





/* $TITLE=get_date_string() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                Build the string containing the date
                  to be scrolled on clock display.
\* ----------------------------------------------------------------- */
void get_date_string(UCHAR *String)
{
  UCHAR Suffix[3];

  UINT8 Dum1UChar;
  UINT8 TemperatureF;

  UINT16 Loop1UInt16;



  /* Wipe string on entry. */
  for (Loop1UInt16 = 0; Loop1UInt16 < DISPLAY_BUFFER_SIZE; ++Loop1UInt16)
    String[Loop1UInt16] = 0x00;


  /* Read the real-time clock IC DS3231. */
  Time_RTC = Read_RTC();


  if (Language == ENGLISH)
  {
    /* Display time and date in English format, using 5 X 7 characters. */
    switch (Time_RTC.dayofweek)
    {
      case (SUN):
        sprintf(String, "Sunday ");
      break;

      case (MON):
        sprintf(String, "Monday ");
      break;

      case (TUE):
        sprintf(String, "Tuesday ");
      break;

      case (WED):
        sprintf(String, "Wednesday ");
      break;

      case (THU):
        sprintf(String, "Thursday ");
      break;

      case (FRI):
        sprintf(String, "Friday ");
      break;

      case (SAT):
        sprintf(String, "Saturday ");
      break;
    }


    /* Add month name. */
    switch (Time_RTC.month)
    {
      case (JAN):
        sprintf(&String[strlen(String)], "January");
      break;

      case (FEB):
        sprintf(&String[strlen(String)], "February");
      break;

      case (MAR):
        sprintf(&String[strlen(String)], "March");
      break;

      case (APR):
        sprintf(&String[strlen(String)], "April");
      break;

      case (MAY):
        sprintf(&String[strlen(String)], "May");
      break;

      case (JUN):
        sprintf(&String[strlen(String)], "June");
      break;

      case (JUL):
        sprintf(&String[strlen(String)], "July");
      break;

      case (AUG):
        sprintf(&String[strlen(String)], "August");
      break;

      case (SEP):
        sprintf(&String[strlen(String)], "September");
      break;

      case (OCT):
        sprintf(&String[strlen(String)], "October");
      break;

      case (NOV):
        sprintf(&String[strlen(String)], "November");
      break;

      case (DEC):
        sprintf(&String[strlen(String)], "December");
      break;
    }


    /* For English, add suffix to day-of-month. */
    Dum1UChar = bcd_to_byte(Time_RTC.dayofmonth);
    switch (Dum1UChar)
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


    /* Add 4-digits year. */
    sprintf(&String[strlen(String)], " %X%s %2.2u%2.2u ", Time_RTC.dayofmonth, Suffix, CurrentYearCentile, CurrentYearLowPart);
  }


  if (Language == FRENCH)
  {
    /* Display time and date in French format, using 5 X 7 characters. */
    switch (Time_RTC.dayofweek)
    {
      case (SUN):
        sprintf(String, "Dimanche");
      break;

      case (MON):
        sprintf(String, "Lundi");
      break;

      case (TUE):
        sprintf(String, "Mardi");
      break;

      case (WED):
        sprintf(String, "Mercredi");
      break;

      case (THU):
        sprintf(String, "Jeudi");
      break;

      case (FRI):
        sprintf(String, "Vendredi");
      break;

      case (SAT):
        sprintf(String, "Samedi");
      break;
    }


    /* Add DayOfMonth. */
    if (Time_RTC.dayofmonth == 1)
      sprintf(&String[strlen(String)], " %Xer ", Time_RTC.dayofmonth);  // premier du mois.
    else
      sprintf(&String[strlen(String)], " %X ", Time_RTC.dayofmonth);    // autres dates.


    /* Add month name. */
    switch (Time_RTC.month)
    {
      case (JAN):
        sprintf(&String[strlen(String)], "janvier");
      break;

      case (FEB):
        sprintf(&String[strlen(String)], "fevrier");
        String[strlen(String) - 6] = (UINT8)31;  // e accent aigu.
      break;

      case (MAR):
        sprintf(&String[strlen(String)], "mars");
      break;

      case (APR):
        sprintf(&String[strlen(String)], "avril");
      break;

      case (MAY):
        sprintf(&String[strlen(String)], "mai");
      break;

      case (JUN):
        sprintf(&String[strlen(String)], "juin");
      break;

      case (JUL):
        sprintf(&String[strlen(String)], "juillet");
      break;

      case (AUG):
        sprintf(&String[strlen(String)], "aout");
        String[strlen(String) - 2] = (UINT8)30;  // u accent circonflexe.
      break;

      case (SEP):
        sprintf(&String[strlen(String)], "septembre");
      break;

      case (OCT):
        sprintf(&String[strlen(String)], "octobre");
      break;

      case (NOV):
        sprintf(&String[strlen(String)], "novembre");
      break;

      case (DEC):
        sprintf(&String[strlen(String)], "decembre");
        String[strlen(String) - 7] = (UINT8)31;  // e accent aigu.
      break;
    }


    /* Add 4-digits year. */
    sprintf(&String[strlen(String)], " %2.2u%2.2u ", CurrentYearCentile, CurrentYearLowPart);
  }


  /* Get temperature from Pico's adc gpio. */
  get_temperature();


  /* Add temperature to the string to be displayed... */
  if (TemperatureUnit == CELSIUS)
  {
    /* ...in Celsius. */
    sprintf(&String[strlen(String)], " %u.%u", TemperaturePart1, TemperaturePart2);
    Dum1UChar = strlen(String);
    String[Dum1UChar]     = (UCHAR)0x80;  // degree Celsius symbol.
    String[Dum1UChar + 1] = (UCHAR)0x00;  // end-of-string.
  }
  else
  {
    /* ...or in Fahrenheit. */
    TemperatureF = TemperaturePart1 * 9 / 5 + 32;  // convert from Celsius to Fahrenheit.
    sprintf(&String[strlen(String)], " %u.%u", TemperatureF, TemperaturePart2);
    Dum1UChar = strlen(String);
    String[Dum1UChar]     = (UCHAR)0x81;  // degree Fahrenheit symbol.
    String[Dum1UChar + 1] = (UCHAR)0x00;  // end-of-string.
  }

  return;
}





/* $TITLE=get_month_days() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
            Return the number of days of a specific month,
    given the specified year (to know if it is a leap year or not).
\* ----------------------------------------------------------------- */
UINT8 get_month_days(UINT16 CurrentYear, UINT8 MonthNumber)
{
  if (((CurrentYear % 4 == 0) && (CurrentYear % 100 != 0)) || (CurrentYear % 400 == 0))
  {
    switch(MonthNumber)
    {
      case  1:return MonthDays[0][0]; break;
      case  2:return MonthDays[0][1]; break;
      case  3:return MonthDays[0][2]; break;
      case  4:return MonthDays[0][3]; break;
      case  5:return MonthDays[0][4]; break;
      case  6:return MonthDays[0][5]; break;
      case  7:return MonthDays[0][6]; break;
      case  8:return MonthDays[0][7]; break;
      case  9:return MonthDays[0][8]; break;
      case 10:return MonthDays[0][9]; break;
      case 11:return MonthDays[0][10];break;
      case 12:return MonthDays[0][11];break;
    }
  }
  else
  {
    switch(MonthNumber)
    {
      case  1:return MonthDays[1][0]; break;
      case  2:return MonthDays[1][1]; break;
      case  3:return MonthDays[1][2]; break;
      case  4:return MonthDays[1][3]; break;
      case  5:return MonthDays[1][4]; break;
      case  6:return MonthDays[1][5]; break;
      case  7:return MonthDays[1][6]; break;
      case  8:return MonthDays[1][7]; break;
      case  9:return MonthDays[1][8]; break;
      case 10:return MonthDays[1][9]; break;
      case 11:return MonthDays[1][10];break;
      case 12:return MonthDays[1][11];break;
    }
  }
}





/* $TITLE=get_temperature() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
         Get temperature from analog-to-digital Pico's gpio.
\* ----------------------------------------------------------------- */
void get_temperature(void)
{
  UCHAR start_tran[2] = {0x0E, 0x20};



  i2c_write_blocking(I2C_PORT, Address, start_tran,        2, FALSE);
  i2c_write_blocking(I2C_PORT, Address, &GetAddHigh,       1, TRUE);
  i2c_read_blocking (I2C_PORT, Address, &TemperaturePart1, 1, FALSE);
  i2c_write_blocking(I2C_PORT, Address, &GetAddLow,        1, TRUE);
  i2c_read_blocking (I2C_PORT, Address, &TemperaturePart2, 1, FALSE);
  TemperaturePart2 = (TemperaturePart2 >> 6) * 25;

  return;
}





/* $TITLE=get_voltage() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
      Get power supply voltage from analog-to-digital Pico's gpio.
\* ----------------------------------------------------------------- */
void get_voltage(UCHAR *Voltage)
{
  /***
  UINT16 AdcReading;

  float Volts;

  AdcReading = adc_read();

  Volts = 3 * AdcReading * (3.3f / (1 << 12));

  sprintf(Voltage, "%1.2f Volts", Volts);
  ***/



  const float conversion_factor = 3.3f / (1 << 12);

  uint16_t result = adc_read();

  float voltage = 3 * result * conversion_factor;

  sprintf(Voltage, "%1.2f Volts", voltage);

  return;
}





/* $TITLE=get_weekday() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                       Return the day-of-week,
               given the day-of-month, month and year
\* ----------------------------------------------------------------- */
UINT8 get_weekday(UINT16 year_cnt, UINT8 month_cnt, UINT8 date_cnt)
{
  UINT8 weekday = 8;



  if (month_cnt == 1 || month_cnt == 2)
  {
    month_cnt += 12;
    --year_cnt;
  }

  weekday = (date_cnt + 1 + 2 * month_cnt + 3 * (month_cnt + 1) / 5 + year_cnt + year_cnt / 4 - year_cnt / 100 + year_cnt / 400) % 7;

  switch(weekday)
  {
    case 0 : return 7; break;
    case 1 : return 1; break;
    case 2 : return 2; break;
    case 3 : return 3; break;
    case 4 : return 4; break;
    case 5 : return 5; break;
    case 6 : return 6; break;
  }
}





/* $TITLE=init_gpio() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
              GPIO ports initialization / configuration

              GPIO  2 - (In) Top button
              GPIO  3 - (In) SQW (DS3231 RTC IC)
              GPIO  4 - Not used
              GPIO  5 - Not used
              GPIO  6 - (I2C) SDA (Temperature reading)
              GPIO  7 - (I2C) SCL (Temperature reading)
              GPIO  8 - DHT22 (option added to the Green Clock)
              GPIO  9 - Not used
              GPIO 10 - (Out) CLK (LED matrix controler)
              GPIO 11 - (Out) SDI (LED matrix controler)
              GPIO 12 - (Out) LE
              GPIO 13 - (Out) OE
              GPIO 14 - (Out) Piezo
              GPIO 15 - (In) Down button
              GPIO 16 - (Out) A0 (LED matrix brightness control)
              GPIO 17 - (In) Up button
              GPIO 18 - (Out) A1 (LED matrix brightness control)
              GPIO 19 - Not used
              GPIO 20 - Not used
              GPIO 21 - Not used
              GPIO 22 - (Out) A2 (LED matrix brightness control)
              GPIO 23 - Used internally for voltage regulation
              GPIO 24 - Used internally for voltage detection
              GPIO 25 - On-board Pico LED
              GPIO 26 - ADC0 (Ambient light reading)
              GPIO 27 - Not used
              GPIO 28 - Not used
              GPIO 29 - ADC-Vref (Power supply reading)

\* ----------------------------------------------------------------- */
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

  gpio_init(SET_BUTTON);
  gpio_init(UP_BUTTON);
  gpio_init(DOWN_BUTTON);



/* If "NO_SOUND" is defined, absolutely no sound will be made by the clock. */
#ifndef NO_SOUND
  gpio_init(BUZZ);
  gpio_set_dir(BUZZ,GPIO_OUT);
#endif

/* If user installed an external humidity and temperature sensor. */
#ifdef DHT_SUPPORT
  gpio_init(DHT22);
  gpio_set_dir(DHT22, GPIO_OUT);  // While idle, keep the line as output.
  gpio_put(DHT22, 1);             // Keep signal high while idle.
#endif



  gpio_set_dir(A0,    GPIO_OUT);
  gpio_set_dir(A1,    GPIO_OUT);
  gpio_set_dir(A2,    GPIO_OUT);
  gpio_set_dir(CLK,   GPIO_OUT);
  gpio_set_dir(LE,    GPIO_OUT);
  gpio_set_dir(OE,    GPIO_OUT);
  gpio_set_dir(SDI,   GPIO_OUT);
  gpio_set_dir(SQW,   GPIO_IN);


  /* Inter integrated circuit (I2C) protocol configuration (for DS3231). */
  i2c_init(I2C_PORT, 100000);
  gpio_set_function(SDA, GPIO_FUNC_I2C);
  gpio_set_function(SCL, GPIO_FUNC_I2C);

  gpio_pull_up(SDA);
  gpio_pull_up(SCL);



  /* Initialize Analog-to-digital converter gpio. */
  adc_init();


  /* Make sure adc gpio is high-impedance, no pullups etc. */
  adc_gpio_init(ADC_Light);  // ambient light.
  adc_gpio_init(ADC_VCC);    // power supply voltage.


  /* Select ADC input 0 (gpio 26) for reading ambient light. */
  adc_select_input(3);

  return 0;
}





/* $TITLE=process_scroll_queue() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
          Handle the tag that has been put in scroll queue.
\* ----------------------------------------------------------------- */
void process_scroll_queue(void)
{
  UCHAR String[DISPLAY_BUFFER_SIZE];
  UCHAR Voltage[12];

  UINT8 DhtDisplay[4];
  UINT8 Dum1UChar;
  UINT8 Head;
  UINT8 Loop1UChar;
  UINT8 Tag;
  UINT8 Tail;

  float Humidity;
  float Temperature;
  float TemperatureF;



  /*** For debug purposes.
  CurrentClockMode = MODE_TEST;  // prevent dots blinking.
  clear_framebuffer(0);
  ***/


  /* Check if for any reason, the function has been called while the queue is empty. */
  if (ScrollQueueHead == ScrollQueueTail) return;


  while (ScrollQueueTail != ScrollQueueHead)
  {
    /*** For debug purposes - Keep track of head and tail before dequeuing.
    Head = ScrollQueueHead;
    Tail = ScrollQueueTail;
    ***/

    Tag  = scroll_unqueue();

    /* NOTE: A special Calendar Event (with Description = "Debug") may be inserted in the bundle of calendar events.
             This specific case may be handled (see below) to display whatever information we want. If no Calendar Event
             exists with a description "Debug", this code will not interfere with normal clock usage. */
    if (strcmp(CalendarEvent[Tag].Description, "Debug") == 0) Tag = TAG_ALARM;



    if (Tag < MAX_EVENTS)
    {
      /* We must scroll a calendar event. */
      scroll_string(24, CalendarEvent[Tag].Description);
      while (ScrollDotCount)
        sleep_ms(100);  // waste time to let the time to complete scrolling.
    }
    else
    {
      /* Special cases. */
      switch (Tag)
      {
        case (TAG_DATE):
          /* This one is "not so special"... Used to scroll the date on clock display, along with temperature and power supply voltage. */
          get_date_string(String);
          scroll_string(24, String);

          get_voltage(Voltage);
          scroll_string(24, Voltage);

#ifdef DHT_SUPPORT
          /* Read humidity and temperature from DHT22 and display them only if no error in the reading process. */
          if (read_dht22(&Temperature, &Humidity) == 0)
          {
            if (Language == ENGLISH)
            {
              sprintf(String, "Out Hum %2.2f%%  Out Temp ", Humidity);
            }

            if (Language == FRENCH)
            {
              sprintf(String, "Hum ext %2.2f%%  Temp ext ", Humidity);
            }


            /* Add temperature to the string to be displayed... */
            if (TemperatureUnit == CELSIUS)
            {
              /* ...in Celsius. */
              sprintf(&String[strlen(String)], "%2.2f", Temperature);
              Dum1UChar = strlen(String);
              String[Dum1UChar]     = (UCHAR)0x80;  // degree Celsius symbol.
              String[Dum1UChar + 1] = (UCHAR)0x00;  // add end-of-string.
            }
            else
            {
              /* ...or in Fahrenheit. */
              TemperatureF = ((Temperature * 9) / 5) + 32;  // convert from Celsius to Fahrenheit.
              sprintf(&String[strlen(String)], "%2.2f", TemperatureF);
              Dum1UChar = strlen(String);
              String[Dum1UChar]     = (UCHAR)0x81;  // degree Fahrenheit symbol.
              String[Dum1UChar + 1] = (UCHAR)0x00;  // add end-of-string.
            }
            scroll_string(24, String);
          }


          /* For statistic purposes, display the cumulative number of errors while reading DHT data. */
          sprintf(String, " (%llu/%llu)", DhtErrors, DhtReading);
          scroll_string(24, String);
#endif
        break;

        case (TAG_ALARM):
          /* For debug purposes.
             Display current alarm parameters. */
          byte_data();  // read alarm parameters.

          /* Display current parameters for alarm 0 and alarm 1. */
          sprintf(String, "Alarm 0: %s Day %u  Hour %2.2u  Minute %2.2u  Second %2.2u   Alarm 1: %s Day %u  Hour %2.2u  Minute %2.2um",
                  Alarm[0].FlagStatus ? "ON" : "OFF", (ByteData[DS3231_REG_A1D] & 0xFF), ByteData[DS3231_REG_A1H], ByteData[DS3231_REG_A1M], ByteData[DS3231_REG_A1S],
                  Alarm[1].FlagStatus ? "ON" : "OFF", (ByteData[DS3231_REG_A2D] & 0xFF), ByteData[DS3231_REG_A2H], ByteData[DS3231_REG_A2M]);
          scroll_string(24, String);
        break;

        case (TAG_DEBUG):
          /* For debug purposes.
             Display a few variables while implementing DST support. */
          sprintf(String, "L: %2.2X  FSTDoM: %2.2X  FSTM: %2.2X", Language, FlagSetTimeDayOfMonth, FlagSetTimeMonth);
          scroll_string(15, String);
        break;

        case (TAG_DST):
          /* For debug purposes.
             Display a few variables while implementing DST support. */
          sprintf(String, "HourSetting= %u Hour= %u Minute= %u", CurrentHourSetting, CurrentHour, CurrentMinute);
          scroll_string(22, String);
          sprintf(String, "Month= %u DOW= %u DOM = %u DST= 0x%2.2X", CurrentMonth, get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth), CurrentDayOfMonth, FlagDayLightSavingTime);
          scroll_string(22, String);
        break;

        case (TAG_QUEUE):
          /* For debug purposes.
             Display all allocated scroll queue slots. */
          if (ScrollQueueHead == ScrollQueueTail)
          {
            sprintf(String, "H%2.2u = T%2.2u - No element allocated...", ScrollQueueHead, ScrollQueueTail);
            scroll_string(24, String);
          }
          else
          {
            Loop1UChar = ScrollQueueTail;
            while (TRUE)
            {
              sprintf(String, "(H%2.2u) T%2.2u E%2.2u", ScrollQueueHead, Loop1UChar, ScrollQueue[Loop1UChar]);
              scroll_string(24, String);
              ++Loop1UChar;

              /* If we reach out-of-bound, revert to 0. */
              if (Loop1UChar > (MAX_SCROLL_QUEUE - 1))
                Loop1UChar = 0;

              /* If we scrolled all allocated slots. */
              if (Loop1UChar == ScrollQueueHead)
                break;
            }
          }
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





#ifdef DHT_SUPPORT
/* $TITLE=read_dht22() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
              Read the temperature from external DHT22.
\* ----------------------------------------------------------------- */
UINT8 read_dht22(float *Temperature, float *Humidity)
{
  UCHAR Dum1UChar[15];
  UCHAR String[256];

  UINT8 Byte[4];
  UINT8 Checksum;
  UINT8 Level[100];
  UINT8 StepCount;

  UINT16 ComputedCheck;
  UINT16 Loop1UInt16;
  UINT16 Value;

  UINT32 DataBuffer;
  UINT32 Dum1UInt32;

  UINT64 InitialValue[100];
  UINT64 FinalValue[100];
  UINT64 ResultValue[100];
  UINT64 InitialTime;
  UINT64 FinalTime;



  /* Initializations. */
  StepCount  = 0;
  DataBuffer = 0;


  /* So far, line has been kept to a High level by the pull-up resistor. */
  gpio_set_dir(DHT22, GPIO_OUT);

  /* Send the low level "request" to DHT22
     (I've seen different values for the length required for the request to DHT22.
     Let's remain on the safe side and make it a little more than 18 milli-seconds). */
  gpio_put(DHT22, 0);

  /* Reset variables before grabbing new data. */
  for (Loop1UInt16 = 0; Loop1UInt16 < MAX_READINGS; ++Loop1UInt16)
  {
    TimerValue[Loop1UInt16]  = 0;
    LevelChange[Loop1UInt16] = 'X';
  }

  /* Keep "Request" signal Low for a while. */
  sleep_ms(20);

  /* Then wait for the response from DHT22.
     (20 to 40 micro-seconds delay after the request signal). */
  gpio_set_dir(DHT22, GPIO_IN);

  /* Put a pull-up resistor on the line. */
  gpio_pull_up(DHT22);



  /* --------------------------------------------------------------- *\
  NOTE: Inside every "while" loop below, there is a time-out algorithm.
        As long as everything goes fine, this time-out is useless.
       (except the last one, while receiving the "second half-bit")
        However, it allows the clock to continue running fine in case
        of problem with the DHT22 (broken part, cable disconnect, etc...)
  \* --------------------------------------------------------------- */
  /* --------------------------------------------------------------- *\
              Wait for a while until DHT22 begins to respond.
           The line is idle (High level) and we expect the DHT22
            to begin responding between 20 to 40 micro-seconds.
  \* --------------------------------------------------------------- */
  InitialValue[StepCount] = time_us_64();
  while (gpio_get(DHT22) == 1)
  {
    FinalValue[StepCount] = time_us_64();  // overwrite same variable until we get out of the while loop.
    if ((FinalValue[StepCount] - InitialValue[StepCount]) > 100) break;  // time out.
  }
  Level[StepCount] = 1;                    // we will calculate below the length of this High level.
  /* We get out of while loop when DHT22 sends the first "Get ready" signal (Low level) or time-out. */


  /* If we never entered in the while loop above, we were already at a Low level. */
  if (FinalValue[StepCount] == 0)
    FinalValue[StepCount] = time_us_64();  // to get an idea of the real duration of the delay for DHT22 to begin responding.

  ResultValue[StepCount] = (FinalValue[StepCount] - InitialValue[StepCount]);
  ++StepCount;





  /* --------------------------------------------------------------- *\
       Skip the first 80 micro-seconds "Get ready" signal received
         from DHT22. We keep track of the duration, but only for
               information since it is not that important.
  \* --------------------------------------------------------------- */
  /* Wait for answer from DHT22...
     First, DHT22 will pull the line Low for 80 micro-seconds. */
  InitialValue[StepCount] = time_us_64();
  while (gpio_get(DHT22) == 0)
  {
    FinalValue[StepCount] = time_us_64();  // overwrite same variable until we get out of the while loop.
    if ((FinalValue[StepCount] - InitialValue[StepCount]) > 120) break;  // time out.
  }
  Level[StepCount] = 0;                    // we will calculate below the length of this High level.
  /* We get out of while loop when DHT22 sends the second "Get ready" signal (High level) or time-out. */


  /* If we never entered in the while loop above, we were already at a High level. */
  if (FinalValue[StepCount] == 0)
    FinalValue[StepCount] = time_us_64();  // to get an idea of the real duration of this 80 micro-seconds 1st "Get ready" signal.

  ResultValue[StepCount] = (FinalValue[StepCount] - InitialValue[StepCount]);
  ++StepCount;





  /* --------------------------------------------------------------- *\
      Skip the second 80 micro-seconds "Get ready" signal received
        from DHT22. We keep track of the duration, but only for
               information since it is not that important.
  \* --------------------------------------------------------------- */
  /* Then, DHT22 will pull the line High for another 80 micro-seconds. */
  InitialValue[StepCount] = time_us_64();
  while (gpio_get(DHT22) == 1)
  {
    FinalValue[StepCount] = time_us_64();  // overwrite same variable until we get out of the while loop.
    if ((FinalValue[StepCount] - InitialValue[StepCount]) > 120) break;  // time out.
  }
  Level[StepCount] = 1;                    // we will calculate below the length of this Low level.
  /* We get out of while loop when DHT22 begins sending first data bit (Low level) or time-out. */


  /* If we never entered in the while loop above, we were already at a Low level. */
  if (FinalValue[StepCount] == 0)
    FinalValue[StepCount] = time_us_64();  // to get an idea of the real duration of this 80 micro-seconds 2nd "Get ready" signal.

  ResultValue[StepCount] = (FinalValue[StepCount] - InitialValue[StepCount]);
  ++StepCount;





  /* --------------------------------------------------------------- *\
                Now read the 40 bits (5 bytes X 8 bits).
                   (4 data bytes + 1 checksum byte).
  \* --------------------------------------------------------------- */
  for (Loop1UInt16 = 0; Loop1UInt16 < 40; ++Loop1UInt16)
  {
    /* --------------------------------------------------------- *\
                    Receiving the first half bit.
        Wait as long as the line is Low, which is the beginning
                 of the next bit ("first half bit").
        We keep track of the duration, but only for information
          since the first half bit is always the same length
      (50 micro-seconds) and it is it meaningless in data decoding.
    \* --------------------------------------------------------- */
    InitialValue[StepCount] = time_us_64();
    while (gpio_get(DHT22) == 0)
    {
      FinalValue[StepCount] = time_us_64();  // overwrite same variable until we get out of "while" loop.
      if ((FinalValue[StepCount] - InitialValue[StepCount]) > 80) break;  // time out.
    }
    Level[StepCount] = 0;                // we will calculate below the length of this High level.
    /* We get out of while loop when DHT22 begins sending second data bit (High level) or time-out. */


    /* If we never entered in the while loop above, we were already at a High level.
       If ever the case, we probably face an error. Checksum validation below will trap this situation. */
    if (FinalValue[StepCount] == 0)
      FinalValue[StepCount] = time_us_64();


    ResultValue[StepCount] = (FinalValue[StepCount] - InitialValue[StepCount]);
    ++StepCount;





    /* --------------------------------------------------------- *\
                   Receiving the second half bit.
        Wait as long as the line is High, which is the last part
                 of the next bit ("second half bit").
           We keep track of the duration and this time, it is
         important, since this is what determines if this is a
           a "0" bit (around 26 micro-seconds), or a "1" bit
                       (around 70 micro-seconds).
    \* --------------------------------------------------------- */
    InitialValue[StepCount] = time_us_64();
    while (gpio_get(DHT22) == 1)
    {
      FinalValue[StepCount] = time_us_64();  // overwrite same variable until we get out of "while" loop.
      if ((FinalValue[StepCount] - InitialValue[StepCount]) > 35) break;  // time out after just later than "0" bit length.
    }
    Level[StepCount] = 1;                    // we will calculate below the length of this High level.
    /* NOTE: We get out of while loop either because:
             1) DHT22 is done sending second half of next bit for a "0" bit (26 micro-seconds) or
             2) DHT22 is sending the second half of a "1" bit (70 micro-seconds) and the loop timed-out. */


    /* If we never entered in the while loop above, we were already at a Low level.
       If it is the case, we probably face an error. Checksum validation below will trap this situation. */
    if (FinalValue[StepCount] == 0)
      FinalValue[StepCount] = time_us_64();

    ResultValue[StepCount] = (FinalValue[StepCount] - InitialValue[StepCount]);
    ++StepCount;



    /* --------------------------------------------------------- *\
                   Evaluate the bit just received.
    \* --------------------------------------------------------- */
    /* Let's assume on entry that we received a "0" bit.
       Left-shift all bits already received.
       A new "0" bit is automatically added by default. */
    if (Loop1UInt16 < 32)
    {
      /* We're still receiving the first 32 data bits. */
      DataBuffer <<= 1;
    }
    else
    {
      /* We already received all 32 data bits, we're receiving the checksum bits. */
      Checksum <<= 1;
    }

    /* If we are still at a High level after a time-out of 35 micro-seconds
       (meaning the new bit is a "1"), replace the "0" bit just added by a "1". */
    if (gpio_get(DHT22) == 1)
      if (Loop1UInt16 < 32)
        ++DataBuffer;
      else
        ++Checksum;

    /* Wait to return to a Low level before going back to the "for" loop to proceed with next bit. */
    InitialTime = time_us_64();
    while (gpio_get(DHT22) == 1)
    {
      FinalTime = time_us_64();
      if ((FinalTime - InitialTime) > 45) break;
    }
  }



  /* Checksum validation. */
  ++DhtReading;  // one more reading from DHT22.
  ComputedCheck = 0;


  /* Compute the checksum of data bytes received. */
  for (Loop1UInt16 = 0; Loop1UInt16 < 4; ++Loop1UInt16)
  {
    Value = ((DataBuffer >> (8 * Loop1UInt16)) & 0xFF);
    ComputedCheck += Value;
    ComputedCheck &= 0xFF;  // when adding, may become more than 8 bits.
  }



  /* Compare the computed checksum with the one received from DHT22 and return an error code in case of error. */
  if (ComputedCheck != Checksum)
  {
    /***
    sprintf(String, "Total steps=%u", StepCount);
    scroll_string(24, String);
    sprintf(String, "Data=%8.8X", DataBuffer);
    scroll_string(24, String);
    sprintf(String, "Checksum= %2.2X VS %2.2X", ComputedCheck, Checksum);
    scroll_string(24, String);
    ***/



    /*** Display some communication information. ***
    Dum1UInt32 = 0;
    for (Loop1UInt16 = 0; Loop1UInt16 < StepCount; ++Loop1UInt16)
    {
      if (Loop1UInt16 == 0)
      {
        sprintf(String, "Delay=%u", ResultValue[Loop1UInt16]);
        scroll_string(24, String);
        continue;
      }

      if (Loop1UInt16 == 1)
      {
        sprintf(String, "1st Get ready=%u", ResultValue[Loop1UInt16]);
        scroll_string(24, String);
        continue;
      }

      if (Loop1UInt16 == 2)
      {
        sprintf(String, "2nd Get ready=%u", ResultValue[Loop1UInt16]);
        scroll_string(24, String);
        continue;
      }

      if ((Loop1UInt16 % 2) == 0)  // display only second half bit which determines bit type.
      {
        // Display only second half bit which the important value to analyze.
        sprintf(String, "Level[%u]=%u", Loop1UInt16, Level[Loop1UInt16]);
        scroll_string(24, String);

        sprintf(Dum1UChar, "%u", InitialValue[Loop1UInt16]);
        sprintf(String, "I=%s", &Dum1UChar[strlen(Dum1UChar) - 4]);
        scroll_string(24, String);

        sprintf(Dum1UChar, "%u", FinalValue[Loop1UInt16]);
        sprintf(String, "F=%s", &Dum1UChar[strlen(Dum1UChar) - 4]);
        scroll_string(24, String);

        sprintf(String, "R=%u", ResultValue[Loop1UInt16]);
        scroll_string(24, String);

        Dum1UInt32 <<= 1;
        if (ResultValue[Loop1UInt16] > 35) ++Dum1UInt32;
        sprintf(String, "DB=%X", Dum1UInt32);
        scroll_string(24, String);
      }
    }
    ***/


    ++DhtErrors;
    return 0xFF;
  }


  /* If no error, compute the temperature and relative humidity from received data. */
  Byte[3] =  (DataBuffer        & 0xFF);
  Byte[2] = ((DataBuffer >> 8)  & 0xFF);
  Byte[1] = ((DataBuffer >> 16) & 0xFF);
  Byte[0] = ((DataBuffer >> 24) & 0xFF);


  /* Decode temperature. */
  *Temperature = (float)((((Byte[2] & 0x7F) * 256) + Byte[3]) / 10.0);
  if (Byte[2] & 0x80) *Temperature *= -1;

  /* Decode relative humidity. */
  *Humidity = (float)((((Byte[0] & 0x7F) * 256) + Byte[1]) / 10.0);


  /***
  sprintf(String, "Hum= %2.2f%% Temp= %2.2f", *Humidity, *Temperature);
  scroll_string(24, String);
  ***/

  return 0x00;
}
#endif





/* $TITLE=reverse_bits() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
         Reverse the bit order of the byte given in argument.
\* ----------------------------------------------------------------- */
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





/* $TITLE=scroll_one_dot() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
        Scroll the data in the framebuffer one dot to the left.
\* ----------------------------------------------------------------- */
void scroll_one_dot(void)
{
  UINT8 CharacterBuffer;  // buffer to temporarily hold a character (to keep track of "indicators" status).
  UINT8 RowNumber;        // row number in the framebuffer.
  UINT8 SectionNumber;    // section number in the framebuffer.



  /* --------------------------------------------------------------- *\
               Scroll the framebuffer one dot to the left.
  \* --------------------------------------------------------------- */
  /* Scan the 7 rows of data. Row 0 is reserved for "indicators". */
  for (RowNumber = 1; RowNumber < 8; ++RowNumber)
  {
    CharacterBuffer = DisplayBuffer[RowNumber] & 0x03;  // keep track of "indicators" status (first 2 bits).

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





/* $TITLE=scroll_queue() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
               Queue the given tag in the scroll queue.
            Use the queue algorithm where one slot is lost.
              This prevents the use of a lock mechanism.
\* ----------------------------------------------------------------- */
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

  if (ScrollQueueHead >= MAX_SCROLL_QUEUE) ScrollQueueHead = 0;  // revert to zero when reaching out-of-bound.

  return 0x00;
}





/* $TITLE=scroll_queue_value() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
           Put a specific value in the queue so that we can
            check for a specific event even inside an ISR.
   MUST BE USED ONLY FOR DEBUG PURPOSES SINCE IT INTERFERES WITH
                        CALENDAR EVENTS.
\* ----------------------------------------------------------------- */
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
  if (EventNumber > MAX_EVENTS) EventNumber = 0;

  return EventNumber;
}





/* $TITLE=scroll_string() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
            Scroll the specified string on clock display.
\* ----------------------------------------------------------------- */
void scroll_string(UINT8 StartColumn, UCHAR *String)
{
  UINT16 Loop1UInt;



  if (CurrentClockMode != MODE_SCROLLING)
  {
    /* Nothing scrolling on the screen... Clear framebuffer on entry. */
    clear_framebuffer(0);

    /* If no string is scrolling, use the start column specified. (The first two columns are used by LED indicators). */
    if (StartColumn == 0)
      ScrollDotCount = 1;  // will be auto-incremented in the fill_display_buffer_5X7() function call.
    else
      ScrollDotCount = StartColumn;
  }
  else
  {
    /* There is something currently scrolling. Append 2 spaces, then the new string. */
    for (Loop1UInt = 0; Loop1UInt < 2; ++Loop1UInt)
    {
      if (ScrollDotCount >= (DISPLAY_BUFFER_SIZE - 9))  // make sure we have enough space to add spaces.
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
      sleep_ms(SCROLL_DOT_TIME * 6 * 10);  // wait for about 10 characters to be scrolled.
    }
    ScrollDotCount = fill_display_buffer_5X7(++ScrollDotCount, String[Loop1UInt]);

    CurrentClockMode = MODE_SCROLLING;
    FlagScrollStart  = FLAG_ON;
  }

  return;
}





/* $TITLE=scroll_unqueue() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                 Unqueue next tag from scroll queue.
\* ----------------------------------------------------------------- */
UINT8 scroll_unqueue(void)
{
  UINT8 Tag;



  /* Check if the scroll queue is empty. */
  if (ScrollQueueHead == ScrollQueueTail) return MAX_EVENTS;

  Tag = ScrollQueue[ScrollQueueTail];
  ScrollQueue[ScrollQueueTail] = 0xAA;
  ++ScrollQueueTail;

  /* If reaching out-of-bound, revert to zero. */
  if (ScrollQueueTail >= MAX_SCROLL_QUEUE) ScrollQueueTail = 0;

  return Tag;
}





/* ----------------------------------------------------------------- *\
           Turn on clock indicator for selected day-of-week,
                       and turn OFF all others.
             (10 is a special case and turn them all OFF).
\* ----------------------------------------------------------------- */
void select_weekday(UINT8 DayNumber)
{
  switch(DayNumber)
  {
    case  0:IndicatorMondayOn; IndicatorTuesdayOff;IndicatorWednesdayOff;IndicatorThursdayOff;IndicatorFridayOff;IndicatorSaturdayOff;IndicatorSundayOff; break;
    case  1:IndicatorMondayOff;IndicatorTuesdayOn; IndicatorWednesdayOff;IndicatorThursdayOff;IndicatorFridayOff;IndicatorSaturdayOff;IndicatorSundayOff; break;
    case  2:IndicatorMondayOff;IndicatorTuesdayOff;IndicatorWednesdayOn; IndicatorThursdayOff;IndicatorFridayOff;IndicatorSaturdayOff;IndicatorSundayOff; break;
    case  3:IndicatorMondayOff;IndicatorTuesdayOff;IndicatorWednesdayOff;IndicatorThursdayOn; IndicatorFridayOff;IndicatorSaturdayOff;IndicatorSundayOff; break;
    case  4:IndicatorMondayOff;IndicatorTuesdayOff;IndicatorWednesdayOff;IndicatorThursdayOff;IndicatorFridayOn; IndicatorSaturdayOff;IndicatorSundayOff; break;
    case  5:IndicatorMondayOff;IndicatorTuesdayOff;IndicatorWednesdayOff;IndicatorThursdayOff;IndicatorFridayOff;IndicatorSaturdayOn; IndicatorSundayOff; break;
    case  6:IndicatorMondayOff;IndicatorTuesdayOff;IndicatorWednesdayOff;IndicatorThursdayOff;IndicatorFridayOff;IndicatorSaturdayOff;IndicatorSundayOn;  break;
    case 10:IndicatorMondayOff;IndicatorTuesdayOff;IndicatorWednesdayOff;IndicatorThursdayOff;IndicatorFridayOff;IndicatorSaturdayOff;IndicatorSundayOff; break;
  }

  return;
}





/* $TITLE=send_data() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                Send data to the matrix controller IC.
\* ----------------------------------------------------------------- */
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





/* $TITLE=set_mode_out() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
          Program some of the real time IC clock parameters.
\* ----------------------------------------------------------------- */
void set_mode_out(void)
{
  if ((FlagSetupTime == FLAG_ON) && (FlagSetTimeHour == FLAG_ON))
  {
    /// scroll_queue_value(TAG_DEBUG, "1st write");
    set_hour(CurrentHourSetting);
  }



  if ((FlagSetupTime == FLAG_ON) && (FlagSetTimeMinute == FLAG_ON))
  {
    /// scroll_queue_value(TAG_DEBUG, "2nd write");
    set_minute(CurrentMinuteSetting);
  }



  if ((FlagSetupTime == FLAG_ON) && (FlagSetTimeMonth == FLAG_ON))
  {
    /// scroll_queue_value(TAG_DEBUG, "3rd write");
    set_month(CurrentMonth);
    set_day_of_week(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth));
    select_weekday(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) - 1);
  }



  if ((FlagSetupTime == FLAG_ON) && (FlagSetTimeDayOfMonth == FLAG_ON))
  {
    /// scroll_queue_value(TAG_DEBUG, "4th write");
    set_day_of_month(CurrentDayOfMonth);
    set_day_of_week(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth));
    select_weekday(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) - 1);
  }



  if ((FlagSetupTime == FLAG_ON) && (FlagSetTimeYear == FLAG_ON))
  {
    /// scroll_queue_value(TAG_DEBUG, "5th write");
    set_year(CurrentYearLowPart);
    set_day_of_week(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth));
    select_weekday(get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) - 1);
  }



  FlagFlashing[SetupStep] = 0xFF;



  if (FlagAlarmMinuteSet == FLAG_ON)
  {
    fill_display_buffer_4X7(13, (Alarm[AlarmNumber].Minute / 10 + '0') & FlagFlashing[4]);
    fill_display_buffer_4X7(18, (Alarm[AlarmNumber].Minute % 10 + '0') & FlagFlashing[4]);
  }



  if ((FlagSetTimerMode == FLAG_ON) && (TimerMode == TIMER_OFF))
  {
    /* Timer is currently OFF. */
    fill_display_buffer_4X7(13, '0' & FlagFlashing[1]);
    fill_display_buffer_4X7(18, 'F' & FlagFlashing[1]);
  }



  IdleNumberOfSeconds   = 0;         // reset number of seconds the system has been idle.
  FlagSetScroll         = FLAG_OFF;  // reset flag indicating we are setting up scrolling ON / OFF.
  FlagTone              = FLAG_OFF;  // reset flag tone.
  FlagAlarmNumberSet    = FLAG_OFF;  // reset alarm number setup mode.
  FlagAlarmToggleOnOff  = FLAG_OFF;  // reset flag indicating we are toggling alarm On / Off.
  FlagAlarmHourSet      = FLAG_OFF;  // reset alarm hour setup mode when done with setup.
  FlagAlarmMinuteSet    = FLAG_OFF;  // reset alarm minute setup mode when done with setup.
  FlagSetTimerMode      = FLAG_OFF;  // reset flag to set the type of timer (Up / Down / Off).
  FlagSetTimerMinute    = FLAG_OFF;  // reset flag to set the timer "Minutes" parameter.
  FlagSetTimerSecond    = FLAG_OFF;  // reset flag to set the timer "Seconds" parameter.
  FlagSetDisplayMode    = FLAG_OFF;  // reset flag to set the time display mode (12 or 24-hours format).
  FlagSetChime          = FLAG_OFF;  // reset flag to set the hourly chime.
  FlagSetChimeTimeOff   = FLAG_OFF;  // reset flag to set the chime time off.
  FlagSetChimeTimeOn    = FLAG_OFF;  // reset flag to set the chime time on.
  FlagSetDst            = FLAG_OFF;  // reset flag to set daylight saving time.
  FlagSetKeyclick       = FLAG_OFF;  // reset flag to set keyclick sound.
  FlagSetLanguage       = FLAG_OFF;  // reset flag to set language.
  FlagSetTimeHour       = FLAG_OFF;  // reset flag to set the time "Hour".
  FlagSetTimeMinute     = FLAG_OFF;  // reset flag to set the time "Minute".
  FlagSetTimeMonth      = FLAG_OFF;  // reset flag to set the time "Month".
  FlagSetTimeYear       = FLAG_OFF;  // reset flag to set the time "Year".
  FlagSetTimeDayOfMonth = FLAG_OFF;  // reset flag to set the time "DayOfMonth".
  FlagSetupTime         = FLAG_OFF;  // reset flag indicating time settings have changed.

  return;
}





/* $TITLE=set_mode_timeout() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
          Exit setup mode because of inactivity (time-out).
\* ----------------------------------------------------------------- */
void set_mode_timeout(void)
{
  AlarmId              = 0;
  ScrollSecondCounter  = 0;             // reset number of seconds to reach time-to-scroll.
  UpId                 = 0;
  FlagIdleCheck        = FLAG_OFF;
  FlagSetUp            = FLAG_OFF;      // reset setup mode flag when timed-out.
  FlagAlarmSetup       = FLAG_OFF;      // reset alarm setup flag when timed-out.
  FlagButtonUp         = FLAG_OFF;      // reset "up" button flag when timed-out.
  FlagUpdateTime       = FLAG_ON;       // let's display current time on the clock, now.

  return;
}





/* $TITLE=setup_alarm_frame() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                   Display current alarm parameters.
\* ----------------------------------------------------------------- */
void setup_alarm_frame(void)
{
  UCHAR String[256];

  UINT8 AlarmHourDisplay;  // to display alarm hour in 12 or 24-hours format.



  CurrentClockMode = MODE_ALARM_SETUP;



  /* Check if current alarm setup step is either alarm number or alarm ON / OFF toggle. */
  if (SetupStep == SETUP_ALARM_NUMBER || SetupStep == SETUP_ALARM_ON_OFF)
  {
    /* Check if current alarm setup step is alarm number select. */
    if (SetupStep == SETUP_ALARM_NUMBER)
    {
      FlagAlarmNumberSet = FLAG_ON;
    }


    /* Check if current alarm setup step is alarm ON / OFF toggling. */
    if (SetupStep == SETUP_ALARM_ON_OFF)
    {
      FlagAlarmToggleOnOff = FLAG_ON;
    }


    /* Display first digit on clock display ("A" for "Alarm"). */
    fill_display_buffer_4X7(0, 'A');


    if (AlarmNumber == 0)
    {
      /* If we are setting up alarm 1 ("0" for the computer), display "A1" on clock display. */
      fill_display_buffer_4X7(5, '1' & FlagFlashing[1]);
    }
    else
    {
      /* If we are setting up alarm 2 ("1" for the computer), display "A2" on clock display. */
      fill_display_buffer_4X7(5, '2' & FlagFlashing[1]);
    }


    /* Display double dots as display separator. */
    fill_display_buffer_4X7(10, ':');


    fill_display_buffer_4X7(13, '0' & FlagFlashing[2]);
    if (Alarm[AlarmNumber].FlagStatus == FLAG_ON)
    {
      /* If this alarm is enabled, display "ON" on clock display. */
      fill_display_buffer_4X7(18, 'N' & FlagFlashing[2]);
    }
    else
    {
      /* Otherwise, display "OF" (for "OFF") on display. */
      fill_display_buffer_4X7(18, 'F' & FlagFlashing[2]);
    }


    /* Clean the non visible part of the display when done. */
    clear_framebuffer(26);
  }



  /* Check if current alarm setup step is either alarm hour setup or alarm minute setup. */
  if (SetupStep == SETUP_ALARM_HOUR || SetupStep == SETUP_ALARM_MINUTE)
  {
    if (SetupStep == SETUP_ALARM_HOUR)
    {
      FlagAlarmHourSet = FLAG_ON;
    }



    if (SetupStep == SETUP_ALARM_MINUTE)
    {
      FlagAlarmMinuteSet = FLAG_ON;
    }


    /* Display current alarm hour and minute on clock display. */
    if (TimeDisplayMode == H12)
    {
      /* We are in "12-hours" display mode. */
      if (Alarm[AlarmNumber].Hour == 0)
      {
        /* Between 00h00 and 00h59. */
        AlarmHourDisplay = 12;
        IndicatorAmOn;
        IndicatorPmOff;
      }
      else if (Alarm[AlarmNumber].Hour == 12)
      {
        /* Between 12h00 and 12h59. */
        AlarmHourDisplay = Alarm[AlarmNumber].Hour;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else if (Alarm[AlarmNumber].Hour > 12)
      {
        /* Between 13h00 and 23h59. */
        AlarmHourDisplay = Alarm[AlarmNumber].Hour - 12;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else
      {
        /* Between 01h00 and 11h59. */
        AlarmHourDisplay = Alarm[AlarmNumber].Hour;
        IndicatorAmOn;
        IndicatorPmOff;
      }
    }
    else
    {
      /* We are in "24-hours" display mode. */
      AlarmHourDisplay = Alarm[AlarmNumber].Hour;
    }

    /* When in 12-hour time display format, first digit is not displayed if it is zero. */
    if ((TimeDisplayMode == H12) && (AlarmHourDisplay < 10))
    {
      fill_display_buffer_4X7(0, ' ' & FlagFlashing[SETUP_HOUR]);
    }
    else
      fill_display_buffer_4X7(0, (AlarmHourDisplay / 10 + '0') & FlagFlashing[3]);

    fill_display_buffer_4X7(5, (AlarmHourDisplay % 10 + '0') & FlagFlashing[3]);
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(13, (Alarm[AlarmNumber].Minute / 10 + '0') & FlagFlashing[4]);
    fill_display_buffer_4X7(18, (Alarm[AlarmNumber].Minute % 10 + '0') & FlagFlashing[4]);


    /* Clean the non visible part of the display when done. */
    clear_framebuffer(26);
  }



  /* Check if current alarm setup step is alarm day-of-week setup. */
  if (SetupStep == SETUP_ALARM_DAY)
  {
    FlagAlarmDaySet = FLAG_ON;  // flag indicating we are in "Alarm Day-of-week" setup step.
    /* Note: day-of-week indicator for alarm setting will blink in the funtion "evaluate_blinking_time()". */
  }


  /* Check if we are done with all alarm setting. */
  if (SetupStep >= SETUP_ALARM_HI_LIMIT)
  {
    /* If alarm #1 is turned on, write it to the RTC IC. */
    if ((AlarmNumber == 0) && (Alarm[0].FlagStatus == FLAG_ON))
    {
      set_alarm1_clock(ALARM_MODE_HOUR_MIN_SEC_MATCHED, 00, Alarm[0].Minute, Alarm[0].Hour, Alarm[0].Day);
    }


    /* If alarm #2 is turned on, write it to the RTC IC. */
    if ((AlarmNumber != 0) && (Alarm[1].FlagStatus == FLAG_ON))
    {
      set_alarm2_clock(Alarm[1].Minute, Alarm[1].Hour, Alarm[1].Day);
    }


    /* Reset all related flags and variables when alarm setup is done. */
    FlagAlarmSetup        = FLAG_OFF;  // reset alarm setup flag when done.
    FlagAlarmDaySet       = FLAG_OFF;  // reset alarm day flag when done.
    FlagAlarmHourSet      = FLAG_OFF;  // reset alarm hour flag when done.
    FlagIdleCheck         = FLAG_OFF;  // no more checking for a timeout.
    FlagTone              = FLAG_OFF;  // reset flag tone.
    FlagUpdateTime        = FLAG_ON;   // it's now time to display time on clock display.
    FlagAlarmNumberSet    = FLAG_OFF;  // reset alarm number setup mode when done.
    FlagAlarmToggleOnOff  = FLAG_OFF;  // reset flag indicating we are toggling alarm On / Off.
    AlarmId               = 0;
    IdleNumberOfSeconds   = 0;         // reset the number of seconds the system has been idle.
    ScrollSecondCounter   = 0;         // reset number of seconds to reach time-to-scroll (obsolete).
    SetupStep             = SETUP_NONE;
  }

  return;
}





/* $TITLE=setup_alarm_variables() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                   Change current alarm parameters.
\* ----------------------------------------------------------------- */
void setup_alarm_variables(UINT8 FlagButtonSelect)
{
  /* Check if we are in "Alarm Number" setup step. */
  if (FlagAlarmNumberSet == FLAG_ON)
  {
    /* Toggle between AlarmNumber 0 and 1. */
    if (AlarmNumber == 0)
      AlarmNumber = 1;
    else
      AlarmNumber = 0;
  }



  /* Toggle alarm ON / OFF. */
  if (FlagAlarmToggleOnOff == FLAG_ON)
  {
    if (Alarm[AlarmNumber].FlagStatus == FLAG_ON)
    {
      Alarm[AlarmNumber].FlagStatus = FLAG_OFF;
      if (AlarmNumber == 0)
        IndicatorAlarm0Off;
      else
        IndicatorAlarm1Off;
    }
    else
    {
      Alarm[AlarmNumber].FlagStatus = FLAG_ON;
      if (AlarmNumber == 0)
        IndicatorAlarm0On;
      else
        IndicatorAlarm1On;
    }
  }



  /* Check if we are setting up "Alarm Hour". */
  if (FlagAlarmHourSet == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Alarm Hour" set mode. */
      ++Alarm[AlarmNumber].Hour;
      if (Alarm[AlarmNumber].Hour == 24) Alarm[AlarmNumber].Hour = 0;  // reset to zero when reaching out-of-bound.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Alarm Hour" set mode. */
      --Alarm[AlarmNumber].Hour;
      if (Alarm[AlarmNumber].Hour == 255) Alarm[AlarmNumber].Hour = 23;  // reset to 23 when reaching out-of-bound.
    }
  }



  /* Check if we are setting up "Alarm Minute". */
  if (FlagAlarmMinuteSet == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Alarm Minute" set mode. */
      ++Alarm[AlarmNumber].Minute;
      if (Alarm[AlarmNumber].Minute == 60) Alarm[AlarmNumber].Minute = 0;  // reset to zero when reaching out-of-bound.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Alarm Minute" set mode. */
      --Alarm[AlarmNumber].Minute;
      if (Alarm[AlarmNumber].Minute == 255) Alarm[AlarmNumber].Minute = 59;  // reset to 59 when reaching out-of-bound.
    }
  }



  /* Check if we are setting up "Alarm Day". */
  if (FlagAlarmDaySet == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button, proceed through all day-of-week. */
      ++Alarm[AlarmNumber].Day;
      if (Alarm[AlarmNumber].Day == 8) Alarm[AlarmNumber].Day = 1;  // when reaching out-of-bound, revert to 1.
    }
    else
    {
      /* User presser the "Down" (bottom) button, proceed through all day-of-week. */
      --Alarm[AlarmNumber].Day;
      if (Alarm[AlarmNumber].Day == 0) Alarm[AlarmNumber].Day = 7;  // when reaching out-of-bound, revert to 7.
    }
  }

  return;
}





/* $TITLE=setup_clock_frame() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                  Display current clock parameters.
\* ----------------------------------------------------------------- */
void setup_clock_frame(void)
{
  UCHAR String[256];

  UINT8 Dum1UChar;



  /* Make sure the function has not been called by mistake. */
  if (SetupStep == SETUP_NONE)
    return;


  CurrentClockMode = MODE_CLOCK_SETUP;


  /* ------------------- Setup Hour and Minute --------------------- */
  /* Setup "Hours" and "Minutes". */
  if ((SetupStep == SETUP_HOUR) || (SetupStep == SETUP_MINUTE))
  {
    if (SetupStep == SETUP_HOUR)
    {
      /* Setup "hours". */
      if (FlagSetTimeHour == FLAG_OFF)
      {
        /* First time in, get RTC hour. */
        CurrentHourSetting = bcd_to_byte(Time_RTC.hour);
      }
      FlagSetTimeHour = FLAG_ON;
    }


    if (SetupStep == SETUP_MINUTE)
    {
      /* Setup "minutes". */
      if (FlagSetTimeMinute == FLAG_OFF)
      {
        /* First time in, get RTC minute. */
        CurrentMinuteSetting = bcd_to_byte(Time_RTC.minutes);
      }
      FlagSetTimeMinute = FLAG_ON;
    }


    /* Check if we are in "24-hours" display mode or "12-hours" display mode.
       NOTE: Real-time clock IC keeps track of the clock in 24-hours format. */
    if (TimeDisplayMode == H12)
    {
      /* If we are in "12-hours" display mode, adjust the digits accordingly. */
      if (CurrentHourSetting == 0)
      {
        /* If the real-time clock is between "00h00" and 00h59, change it to "12hxx AM". */
        CurrentHour = 12;
        IndicatorAmOn;
        IndicatorPmOff;
      }
      else if (CurrentHourSetting == 12)
      {
        /* If the real-time clock is between "12h00" and 12h59, turn ON the "PM" indicator and turn OFF the "AM" indicator. */
        CurrentHour = CurrentHourSetting;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else if (CurrentHourSetting > 12)
      {
        /* If the real-time clock is between 13h00 and 23h59, revert to 1 to 12 and turn ON the "PM" indicator. */
        CurrentHour = CurrentHourSetting - 12;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else
      {
        /* If the real-time clock is between 01h00 and 11h59, turn on the "AM" indicator and turn OFF the "PM" indicator. */
        CurrentHour = CurrentHourSetting;
        IndicatorAmOn;
        IndicatorPmOff;
      }
    }
    else
    {
      /* We are in "24-hours" display mode. */
      CurrentHour = CurrentHourSetting;
    }

    /* When in 12-hour time display format, first digit is not displayed if it is zero. */
    if ((TimeDisplayMode == H12) && (CurrentHour < 10))
    {
      fill_display_buffer_4X7(0, ' ' & FlagFlashing[SETUP_HOUR]);
    }
    else
      fill_display_buffer_4X7(0, (CurrentHour / 10 + '0') & FlagFlashing[SETUP_HOUR]);

    fill_display_buffer_4X7(5, (CurrentHour % 10 + '0') & FlagFlashing[SETUP_HOUR]);
    fill_display_buffer_4X7(10, 0x3A);  // slim ":"
    fill_display_buffer_4X7(12, (CurrentMinute / 10 + '0') & FlagFlashing[SETUP_MINUTE]);
    fill_display_buffer_4X7(17, (CurrentMinute % 10 + '0') & FlagFlashing[SETUP_MINUTE]);
  }



  /* --------------- Setup Month and Day-of-month ------------------ */
  if ((SetupStep == SETUP_DAY_OF_MONTH) || (SetupStep == SETUP_MONTH))
  {
    /* There are conditions where Day-of-month can be higher than allowed. Re-adjust if it is the case. */
    /* For example, we set March 31st, then we change March to April while leaving 31 as the day. */
    Year4Digits = CurrentYearCentile * 100 + CurrentYearLowPart;
    Dum1UChar = get_month_days(Year4Digits, CurrentMonth);
    if (CurrentDayOfMonth > Dum1UChar)
    {
      CurrentDayOfMonth = Dum1UChar;
    }


    if (SetupStep == SETUP_DAY_OF_MONTH)
    {
      FlagSetTimeDayOfMonth = FLAG_ON;
      FlagSetTimeMonth      = FLAG_OFF;
    }
    else
    {
      FlagSetTimeMonth      = FLAG_ON;
      FlagSetTimeDayOfMonth = FLAG_OFF;
    }

    if (Language == FRENCH)
    {
      /* In French, display and set day-of-month before month.
         Make a trick in the flashing below. */
      fill_display_buffer_4X7(0, ((CurrentDayOfMonth / 10 + 0x30) & FlagFlashing[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(5, ((CurrentDayOfMonth % 10 + 0x30) & FlagFlashing[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(10, '-');
      fill_display_buffer_4X7(13, ((CurrentMonth / 10 + 0x30) & FlagFlashing[SETUP_MONTH]));
      fill_display_buffer_4X7(18, ((CurrentMonth % 10 + 0x30) & FlagFlashing[SETUP_MONTH]));
    }
    else
    {
      /* For English and other languages. */
      fill_display_buffer_4X7(0, ((CurrentMonth / 10 + 0x30) & FlagFlashing[SETUP_MONTH]));
      fill_display_buffer_4X7(5, ((CurrentMonth % 10 + 0x30) & FlagFlashing[SETUP_MONTH]));
      fill_display_buffer_4X7(10, '-');
      fill_display_buffer_4X7(13, ((CurrentDayOfMonth / 10 + 0x30) & FlagFlashing[SETUP_DAY_OF_MONTH]));
      fill_display_buffer_4X7(18, ((CurrentDayOfMonth % 10 + 0x30) & FlagFlashing[SETUP_DAY_OF_MONTH]));
    }
  }



  /* ------------------------- Setup Year -------------------------- */
  if (SetupStep == SETUP_YEAR)
  {
    FlagSetTimeYear = FLAG_ON;

    /* Setup Year (centiles and units). */
    Year4Digits = CurrentYearCentile * 100 + CurrentYearLowPart;

    fill_display_buffer_4X7(0, CurrentYearCentile / 10 + 0x30);
    fill_display_buffer_4X7(5, CurrentYearCentile % 10 + 0x30);
    fill_display_buffer_4X7(10, ((CurrentYearLowPart / 10 + 0x30) & FlagFlashing[SETUP_YEAR]));
    fill_display_buffer_4X7(15, ((CurrentYearLowPart % 10 + 0x30) & FlagFlashing[SETUP_YEAR]));
    fill_display_buffer_4X7(23,' ');
  }



  /* ----------------- Setup Daylight Saving Time ------------------ */
  if (SetupStep == SETUP_DST)
  {
    /* Setup Daylight Saving Time. */
    FlagSetDst = FLAG_ON;

    clear_framebuffer(0);
    fill_display_buffer_5X7(2, 'S');
    fill_display_buffer_5X7(8, 'T');
    fill_display_buffer_4X7(14, ':');

    switch (DaylightSavingTime)
    {
      default:
      case (DST_NONE):
        fill_display_buffer_5X7(16, ('0' & FlagFlashing[SETUP_DST]));
      break;

      case (DST_NORTH_AMERICA):
        fill_display_buffer_5X7(16, ('1' & FlagFlashing[SETUP_DST]));
      break;
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ----------------------- Setup Keyclick ------------------------ */
  if (SetupStep == SETUP_KEYCLICK)
  {
    /* Setup "Keyclick sound". */
    FlagSetKeyclick = FLAG_ON;


    /* Setup "keyclick" ON / OFF". */
    FlagTone = FLAG_ON;

    fill_display_buffer_4X7(0, 'B');
    fill_display_buffer_4X7(5, 'P');
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(12,('0' & FlagFlashing[SETUP_KEYCLICK]));

    if (FlagKeyclick == FLAG_ON)
    {
      fill_display_buffer_4X7(17, ('N' & FlagFlashing[SETUP_KEYCLICK]));
    }
    else
    {
      fill_display_buffer_4X7(17, ('F' & FlagFlashing[SETUP_KEYCLICK]));
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ---------------------- Setup Scrolling ------------------------ */
  if (SetupStep == SETUP_SCROLLING)
  {
    /* Setup "Display Scrolling mode ON / OFF". */
    FlagSetScroll = FLAG_ON;  // indicate we are setting up display scroll.

    fill_display_buffer_4X7(0, 'D');
    fill_display_buffer_4X7(5, 'S');
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(12, ('0' & FlagFlashing[SETUP_SCROLLING]));

    if (FlagScrollEnable == FLAG_ON)
    {
      fill_display_buffer_4X7(17, ('N' & FlagFlashing[SETUP_SCROLLING]));
    }
    else
    {
      fill_display_buffer_4X7(17, ('F' & FlagFlashing[SETUP_SCROLLING]));
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ----------------------- Setup Language ------------------------ */
  if (SetupStep == SETUP_LANGUAGE)
  {
    /* Setup Language (for some clock functions). */
    FlagSetLanguage = FLAG_ON;

    clear_framebuffer(0);
    fill_display_buffer_5X7(2, 'L');
    fill_display_buffer_5X7(7, 'G');
    fill_display_buffer_4X7(11, ':');

    switch (Language)
    {
      default:
      case (ENGLISH):
        fill_display_buffer_5X7(15, ('E' & FlagFlashing[SETUP_LANGUAGE]));
      break;

      case (FRENCH):
        fill_display_buffer_5X7(15, ('F' & FlagFlashing[SETUP_LANGUAGE]));
      break;

      case (SPANISH):
        /*** Not implemented yet. ***/
      break;
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ------------------ Setup Time display mode -------------------- */
  if (SetupStep == SETUP_DISPLAY_MODE)
  {
    /* Setup "Hour Display Mode" 24-hours / 12-hours toggling. */
    if (FlagSetDisplayMode == FLAG_OFF)
    {
      CurrentHourSetting   = bcd_to_byte(Time_RTC.hour);
      CurrentMinute        = bcd_to_byte(Time_RTC.minutes);
    }

    FlagSetDisplayMode = FLAG_ON;

    fill_display_buffer_4X7(0, 'H');
    fill_display_buffer_4X7(5, 'D');
    fill_display_buffer_4X7(10, ':');
    if (TimeDisplayMode == H24)
    {
      /* "24" for 24-hours display mode. */
      fill_display_buffer_4X7(12, '2' & FlagFlashing[SETUP_DISPLAY_MODE]);
      fill_display_buffer_4X7(17, '4' & FlagFlashing[SETUP_DISPLAY_MODE]);
    }
    else
    {
      /* "12" for 12-hours display mode. */
      fill_display_buffer_4X7(12, '1' & FlagFlashing[SETUP_DISPLAY_MODE]);
      fill_display_buffer_4X7(17, '2' & FlagFlashing[SETUP_DISPLAY_MODE]);
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(23);
  }



  /* -------------------- Setup Hourly chime ----------------------- */
  if (SetupStep == SETUP_HOURLY_CHIME)
  {
    /* Setup "Hourly chime" (OFF / ON / Day). */
    FlagSetChime = FLAG_ON;

    fill_display_buffer_4X7(0, 'H');
    fill_display_buffer_4X7(5, 'C');
    fill_display_buffer_4X7(10, ':');
    fill_display_buffer_4X7(12, '0' & FlagFlashing[SETUP_HOURLY_CHIME]);

    if (HourlyChimeMode == CHIME_OFF)
    {
      /* 0F for "OFF". */
      fill_display_buffer_4X7(17, 'F' & FlagFlashing[SETUP_HOURLY_CHIME]);
      IndicatorHourlyChimeOff;
    }
    else if (HourlyChimeMode == CHIME_ON)
    {
      /* 0N for "ON". */
      fill_display_buffer_4X7(17, 'N' & FlagFlashing[SETUP_HOURLY_CHIME]);
      IndicatorHourlyChimeOn;
    }
    else if (HourlyChimeMode == CHIME_DAY)
    {
      /* 0I for "On, Intermittent" (during daytime only). */
      fill_display_buffer_4X7(18, 'I' & FlagFlashing[SETUP_HOURLY_CHIME]);
      IndicatorHourlyChimeDay;
    }

    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* -------------------- Setup Chime time on ---------------------- */
  if (SetupStep == SETUP_CHIME_TIME_ON)
  {
    /* Setup Hourly chime time on (it is 9h00 by default). */
    FlagSetChimeTimeOn = FLAG_ON;

    fill_display_buffer_4X7(0, 'O');
    fill_display_buffer_4X7(5, 'N');
    fill_display_buffer_4X7(10, ':');

    /* Check if we are in "24-hours" display mode or "12-hours" display mode.
       NOTE: Real-time clock IC keeps track of the clock in 24-hours format. */
    if (TimeDisplayMode != H24)
    {
      /* We are in "12-hours" display mode. */
      if (ChimeTimeOn == 0)
      {
        /* If Chime time on is between "00h00" and 00h59, change it to "12hxx AM". */
        ChimeTimeOnDisplay = 12;
        IndicatorAmOn;
        IndicatorPmOff;
      }
      else if (ChimeTimeOn == 12)
      {
        /* If Chime time on is between "12h00" and 12h59, turn ON the "PM" indicator and turn OFF the "AM" indicator. */
        ChimeTimeOnDisplay = ChimeTimeOn;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else if (ChimeTimeOn > 12)
      {
        /* If Chime time on is between 13h00 and 23h59, revert to 1 to 12 and turn ON the "PM" indicator. */
        ChimeTimeOnDisplay = ChimeTimeOn - 12;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else
      {
        /* If Chime time on is between 01h00 and 11h59, turn on the "AM" indicator and turn OFF the "PM" indicator. */
        ChimeTimeOnDisplay = ChimeTimeOn;
        IndicatorAmOn;
        IndicatorPmOff;
      }
    }
    else
    {
      /* We are in "24-hours" display mode. */
      ChimeTimeOnDisplay = ChimeTimeOn;
    }
    fill_display_buffer_4X7(12, (0x30 + (ChimeTimeOnDisplay / 10)) & FlagFlashing[SETUP_CHIME_TIME_ON]);
    fill_display_buffer_4X7(17, (0x30 + (ChimeTimeOnDisplay % 10)) & FlagFlashing[SETUP_CHIME_TIME_ON]);


    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  /* ------------------- Setup Chime OFF time ---------------------- */
  if (SetupStep == SETUP_CHIME_TIME_OFF)
  {
    /* Setup Hourly chime time off (it is 22h00 by default). */
    FlagSetChimeTimeOff = FLAG_ON;

    fill_display_buffer_4X7(0, 'O');
    fill_display_buffer_4X7(5, 'F');
    fill_display_buffer_4X7(10, ':');


    /* Check if we are in "24-hours" display mode or "12-hours" display mode.
       NOTE: Real-time clock IC keeps track of the clock in 24-hours format. */
    if (TimeDisplayMode != H24)
    {
      /* We are in "12-hours" display mode. */
      if (ChimeTimeOff == 0)
      {
        /* If Chime time off is between "00h00" and 00h59, change it to "12hxx AM". */
        ChimeTimeOffDisplay = 12;
        IndicatorAmOn;
        IndicatorPmOff;
      }
      else if (ChimeTimeOff == 12)
      {
        /* If Chime time off is between "12h00" and 12h59, turn ON the "PM" indicator and turn OFF the "AM" indicator. */
        ChimeTimeOffDisplay = ChimeTimeOff;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else if (ChimeTimeOff > 12)
      {
        /* If Chime time off is between 13h00 and 23h59, revert to 1 to 12 and turn ON the "PM" indicator. */
        ChimeTimeOffDisplay = ChimeTimeOff - 12;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else
      {
        /* If Chime time off is between 01h00 and 11h59, turn on the "AM" indicator and turn OFF the "PM" indicator. */
        ChimeTimeOffDisplay = ChimeTimeOff;
        IndicatorAmOn;
        IndicatorPmOff;
      }
    }
    else
    {
      /* We are in "24-hours" display mode. */
      ChimeTimeOffDisplay = ChimeTimeOff;
    }
    fill_display_buffer_4X7(12, (0x30 + (ChimeTimeOffDisplay / 10)) & FlagFlashing[SETUP_CHIME_TIME_OFF]);
    fill_display_buffer_4X7(17, (0x30 + (ChimeTimeOffDisplay % 10)) & FlagFlashing[SETUP_CHIME_TIME_OFF]);


    /* Clear the clock framebuffer. */
    clear_framebuffer(26);
  }



  if (SetupStep >= SETUP_CLOCK_HI_LIMIT)
  {
    IdleNumberOfSeconds = 0;            // reset the number of seconds the system has been idle.
    SetupStep           = SETUP_NONE;   // reset SetupStep for next pass.
    FlagIdleCheck       = FLAG_OFF;
    FlagSetChime        = FLAG_OFF;
    FlagSetChimeTimeOff = FLAG_OFF;
    FlagSetChimeTimeOn  = FLAG_OFF;
    FlagSetDst          = FLAG_OFF;
    FlagSetKeyclick     = FLAG_OFF;
    FlagSetLanguage     = FLAG_OFF;
    FlagSetScroll       = FLAG_OFF;
    FlagSetTimeYear     = FLAG_OFF;
    FlagSetUp           = FLAG_OFF;
    FlagSetDisplayMode  = FLAG_OFF;
    FlagTone            = FLAG_OFF;     // reset flag tone.
    FlagUpdateTime      = FLAG_ON;
  }

  return;
}





/* $TITLE=setup_clock_variables() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                   Change current clock parameters.
\* ----------------------------------------------------------------- */
void setup_clock_variables(UINT8 FlagButtonSelect)
{
  UCHAR String[256];

  UINT8 Dum1UInt8;  // dummy variable.



  /* --------------------- Hour setting ---------------------------- */
  /* Check if we are setting up the "Hour". */
  if (FlagSetTimeHour == FLAG_ON)
  {
    /* We are setting up the "Hour". */
    FlagSetupTime = FLAG_ON;  // indicate that we must program the RTC IC.

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Hour" setup mode. */
      ++CurrentHourSetting;

      /* If we pass 23 hours (out-of-bound), roll-back to 0. */
      if (CurrentHourSetting == 24) CurrentHourSetting = 0;  // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Hour" setup mode. */
      --CurrentHourSetting;

      /* If we go below zero (out-of-bound), set hour to 23. */
      if (CurrentHourSetting == 255) CurrentHourSetting = 23;
    }
    CurrentHour = CurrentHourSetting;
  }





  /* --------------------- Minute setting -------------------------- */
  /* Check if we are setting up the "Minute". */
  if (FlagSetTimeMinute == FLAG_ON)
  {
    /* We are setting up the "Minute". */
    FlagSetupTime = FLAG_ON;  // indicate we must program the RTC IC.

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





  /* ------------------------ Month setting ------------------------ */
  if (FlagSetTimeMonth == FLAG_ON)
  {
    /* We are setting up the "Month". */
    FlagSetupTime = FLAG_ON;  // indicate we must program the RTC IC.

    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in "Month" setup mode. */
      ++CurrentMonth;

      /* If we pass 12 (out-of-bound), roll-back to 1. */
      if (CurrentMonth == 13) CurrentMonth = 1;
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in "Month" setup mode. */
      --CurrentMonth;

      /* If we go below 1 (out-of-bound), set month to 12. */
      if (CurrentMonth == 0) CurrentMonth = 12;
    }


    /* If we changed month, make sure day-of-month is compatible. First, make
       sure Year4Digits is correct before determining DayOfMonth (for leap years). */
    Year4Digits = (CurrentYearCentile * 100) + CurrentYearLowPart;

    if (CurrentDayOfMonth > get_month_days(Year4Digits, CurrentMonth))
    {
      /* If we are over this month's maximum day, revert to last day of this month. */
      CurrentDayOfMonth = get_month_days(Year4Digits, CurrentMonth);
    }
  }





  /* ------------------ Day-of-month setting ----------------------- */
  if (FlagSetTimeDayOfMonth == FLAG_ON)
  {
    /* We are setting up day-of-month. */
    FlagSetupTime = FLAG_ON;  // indicate we must program the RTC IC.

    /* Make sure Year4Digits is correct before determining maximum day-of-month (for leap years). */
    Year4Digits = (CurrentYearCentile * 100) + CurrentYearLowPart;

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





  /* ----------------------- Year setting--------------------------- */
  if (FlagSetTimeYear == FLAG_ON)
  {
    /* We are setting up the "Year". */
    FlagSetupTime = FLAG_ON;  // indicate we must program the RTC IC.

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
        ++CurrentYearCentile;
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
        --CurrentYearCentile;
      }
    }
  }





  /* ---------------- Daylight Saving Time setting ----------------- */
  if (FlagSetDst == FLAG_ON)
  {
    /* If we are in setup mode, in "Daylight Saving Time" setup step,
       go through different DST options. */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++DaylightSavingTime;
      if (DaylightSavingTime >= DST_HI_LIMIT) DaylightSavingTime = DST_NONE;
    }
    else
    {
      /* Must be "Button down". */
      --DaylightSavingTime;
      if (DaylightSavingTime == 255) DaylightSavingTime = DST_HI_LIMIT - 1;
    }
  }





  /* ---------------------- Keyclick setting ----------------------- */
  /* Check if we are in "Keyclick" setup step. */
  if (FlagSetKeyclick == FLAG_ON)
  {
    if (FlagTone == FLAG_ON)
    {
      /* If we are in setup mode, in "Keyclick" setup step, toggle the flag. */
      if (FlagKeyclick == FLAG_ON)
        FlagKeyclick = FLAG_OFF;
      else
        FlagKeyclick = FLAG_ON;
    }

    /* Check if we should make a "keyclick tone".
       It's usually done in the timer callback routine, but since we just changed the setting,
       it's a good idea to provide an immediate feedback. */
    evaluate_keyclick_start_time();
  }





  /* ----------------------- Scroll setting ------------------------ */
  if (FlagSetScroll == FLAG_ON)
  {
    /* If we are in setup mode, in "Scroll" setup step, toggle the "scroll enable" flag On / Off. */
    if (FlagScrollEnable == FLAG_OFF)
    {
      FlagScrollEnable = FLAG_ON;
      IndicatorScrollOn;
    }
    else
    {
      FlagScrollEnable = FLAG_OFF;
      IndicatorScrollOff;
    }
  }





  /* ---------------------- Language setting ----------------------- */
  if (FlagSetLanguage == FLAG_ON)
  {
    /* If we are in setup mode, in "Language" setup step,
       go through different language options (French / English). */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++Language;
      if (Language > FRENCH) Language = ENGLISH;
    }
    else
    {
      /* Must be "Button down". */
      --Language;
      if (Language < ENGLISH) Language = FRENCH;
    }


    /* Setup order in French is different than in English. */
    if (Language == FRENCH)
    {
      SETUP_DAY_OF_MONTH   = 0x03;
      SETUP_MONTH          = 0x04;
    }

    if (Language == ENGLISH)
    {
      SETUP_MONTH          = 0x03;
      SETUP_DAY_OF_MONTH   = 0x04;
    }
    /*** Other languages can be added here. ***/
  }





  /* -------------------- Hour format setting ---------------------- */
  /* Select time display format (12-hours or 24-hours). */
  if (FlagSetDisplayMode == FLAG_ON)
  {
    if (TimeDisplayMode == H12)
    {
      /* Toggle from 12-hours to 24-hours display mode. */
      TimeDisplayMode = H24;
      IndicatorAmOff;
      IndicatorPmOff;
    }
    else
    {
      /* Change from 24-hours to 12-hours display mode. */
      TimeDisplayMode = H12;

      /* We are in "12-hours" display mode, adjust the digits accordingly. */
      if (CurrentHourSetting == 0)
      {
        /* If the real-time clock is between "00h00" and 00h59, change it to "12hxx AM". */
        CurrentHour = 12;
        IndicatorAmOn;
        IndicatorPmOff;
      }
      else if (CurrentHourSetting == 12)
      {
        /* If the real-time clock is between "12h00" and 12h59, turn ON the "PM" indicator and turn OFF the "AM" indicator. */
        CurrentHour = CurrentHourSetting;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else if (CurrentHourSetting > 12)
      {
        /* If the real-time clock is between 13h00 and 23h59, revert to 1 to 12 and turn ON the "PM" indicator. */
        CurrentHour = CurrentHourSetting - 12;
        IndicatorPmOn;
        IndicatorAmOff;
      }
      else
      {
        /* If the real-time clock is between 01h00 and 11h59, turn on the "AM" indicator and turn OFF the "PM" indicator. */
        CurrentHour = CurrentHourSetting;
        IndicatorAmOn;
        IndicatorPmOff;
      }
    }
  }





  /* -------------------- Hourly chime setting --------------------- */
  if (FlagSetChime == FLAG_ON)
  {
    /* If we are in setup mode, in "Hourly chime" setup step, go through different status (OFF / ON / DAY). */
    if (FlagButtonSelect == FLAG_UP)
    {
      ++HourlyChimeMode;
      if (HourlyChimeMode > CHIME_DAY) HourlyChimeMode = CHIME_OFF;
    }

    if (FlagButtonSelect == FLAG_DOWN)
    {
      --HourlyChimeMode;
      if (HourlyChimeMode == 255) HourlyChimeMode = CHIME_DAY;
    }
  }





  /* ------------------- Chime time on setting --------------------- */
  /* Check if we are setting up the Chime time on */
  if (FlagSetChimeTimeOn == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in Chime time on setup mode. */
      ++ChimeTimeOn;
      if (ChimeTimeOn == 24) ChimeTimeOn = 0;  // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in Chime time on setup mode. */
      --ChimeTimeOn;
      if (ChimeTimeOn == 255) ChimeTimeOn = 23;  // if out-of-bound, revert to 23.
    }
  }





  /* ------------------- Chime time off setting -------------------- */
  /* Check if we are setting up the Chime time off. */
  if (FlagSetChimeTimeOff == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* User pressed the "Up" (middle) button while in Chime time off setup mode. */
      ++ChimeTimeOff;
      if (ChimeTimeOff == 24) ChimeTimeOff = 0;  // if out-of-bound, revert to 0.
    }
    else
    {
      /* User pressed the "Down" (bottom) button while in Chime time off setup mode. */
      --ChimeTimeOff;
      if (ChimeTimeOff == 255) ChimeTimeOff = 23;  // if out-of-bound, revert to 23.
    }
  }

  return;
}





/* $TITLE=setup_timer_frame() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                  Display current timer parameters.
\* ----------------------------------------------------------------- */
void setup_timer_frame(void)
{
  CurrentClockMode = MODE_TIMER_SETUP;


  if (SetupStep == SETUP_TIMER_UP_DOWN)
  {
    /* Clear clock framebuffer on entry. */
    clear_framebuffer(0);

    FlagSetTimerMode = FLAG_ON;
    fill_display_buffer_5X7(2, 'T');
    fill_display_buffer_5X7(8, 'M');


    if (TimerMode == TIMER_COUNT_UP)
    {
      /* Timer is currently in "Count Up" mode. */
      fill_display_buffer_4X7(13, 'U' & FlagFlashing[1]);
      fill_display_buffer_4X7(18, 'P' & FlagFlashing[1]);
      IndicatorCountUpOn;
      IndicatorCountDownOff;
    }
    else if (TimerMode == TIMER_COUNT_DOWN)
    {
      /* Timer is currently in "Count Down" mode. */
      fill_display_buffer_4X7(13, 'D' & FlagFlashing[1]);
      fill_display_buffer_4X7(18, 'N' & FlagFlashing[1]);
      IndicatorCountDownOn;
      IndicatorCountUpOff;
    }
    else if (TimerMode == TIMER_OFF)
    {
      /* Timer is currently OFF (no Count up, no Count down). */
      fill_display_buffer_4X7(13, '0' & FlagFlashing[1]);
      fill_display_buffer_4X7(18, 'F' & FlagFlashing[1]);
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
      FlagSetTimerMinute = FLAG_ON;
    }
    else if ((SetupStep == SETUP_TIMER_MINUTE) && (TimerMode == TIMER_COUNT_UP))
    {
      /* Timer is currently in "Count Up" mode. We assume that a "count-up" timer will always start from zero. */
      if (FlagSetTimerCountUp == FLAG_OFF)
      {
        TimerMinutes = 0;
        TimerSeconds = 0;
      }

      IdleNumberOfSeconds = 0;          // reset the number of seconds the system has been idle.
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
      FlagSetTimerSecond    = FLAG_ON;
      FlagTimerCountDownEnd = FLAG_OFF;
    }

    if (TimerMode == TIMER_COUNT_DOWN)
    {
      /* Timer is currently in "Count Down" mode. */
      fill_display_buffer_4X7(0, TimerMinutes / 10 + '0' & FlagFlashing[2]);
      fill_display_buffer_4X7(5, TimerMinutes % 10 + '0' & FlagFlashing[2]);
      fill_display_buffer_4X7(10, ':');
      fill_display_buffer_4X7(12, TimerSeconds / 10 + '0' & FlagFlashing[3]);
      fill_display_buffer_4X7(17, TimerSeconds % 10 + '0' & FlagFlashing[3]);

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
      TimerShowSecond     = 0;
      TimerShowCount      = 0;
      FlagSetTimerCountUp = FLAG_OFF;
    }
  }
  else if ((SetupStep == SETUP_TIMER_READY) && (TimerMode == TIMER_COUNT_DOWN) && (FlagTimerCountDownEnd == FLAG_OFF))
  {
    IdleNumberOfSeconds   = 0;            // reset the number of seconds the system has been idle.
    FlagSetTimerCountDown = FLAG_ON;      // make count down timer active.
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
    if (TimerMode == TIMER_COUNT_UP)
    {
      /* Timer is currently in "Count Up" mode. */
      TimerMode       = TIMER_OFF;  // set timer OFF.
      TimerMinutes    = 0;
      TimerSeconds    = 0;
      IndicatorCountUpOff;
    }


    IdleNumberOfSeconds  = 0;           // reset the number of seconds the system has been idle.
    UpId                 = 0;
    ScrollSecondCounter  = 0;           // reset number of seconds to reach time-to-scroll.
    SetupStep            = SETUP_NONE;  // reset SetupStep for next pass.
    FlagButtonUp         = FLAG_OFF;    // reset "up" button.
    FlagIdleCheck        = FLAG_OFF;    // no more checking for an idle time-out.
    FlagSetTimerSecond   = FLAG_OFF;    // reset flag indicating we are in timer "Seconds" parameter setup.
    FlagTone             = FLAG_OFF;    // reset flag tone.
    FlagUpdateTime       = FLAG_ON;     // setup is over, it becomes time to display time.
  }

  return;
}





/* $TITLE=setup_timer_variables() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                  Change current timer parameters.
\* ----------------------------------------------------------------- */
void setup_timer_variables(UINT8 FlagButtonSelect)
{
  /* Select timer mode (Off / Count down / Count up). */
  if (FlagSetTimerMode == FLAG_ON)
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* We come here because user pressed the "Up" (middle) button while in "timer mode" setup. */
      ++TimerMode;  // step upward through different timer modes (Off / Count down / Count up).
      if (TimerMode > TIMER_COUNT_UP) TimerMode = TIMER_OFF;  // if reaching out-of-bound, revert to 0.
    }
    else
    {
      /* We come here because user pressed the "Down" (bottom) button while in "timer mode" setup. */
      --TimerMode;  // step downward through different timer modes (Count up / Count down / Off).
      if (TimerMode == 255) TimerMode = TIMER_COUNT_UP;  // if reaching out-of-bound, revert to 2.
    }
  }



  /* NOTE: There is no "Minute" or "Second" parameters adjustment for "Count up" timer.
           We assume that a "Count up" timer will logically always start counting from 00m00s. */



  /* Select timer "Minute" parameter. */
  if ((TimerMode == TIMER_COUNT_DOWN) && (FlagSetTimerMinute == FLAG_ON))
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* We come here because user pressed the "Up" (middle) button while in timer "Minutes" parameter setup. */
      ++TimerMinutes;
      if (TimerMinutes == 60) TimerMinutes = 0;  // if reaching out-of-bound, revert to 0.
    }
    else
    {
      /* We come here because user pressed the "Down" (bottom) button while in timer "Minutes" parameter setup.. */
      --TimerMinutes;
      if (TimerMinutes == 255) TimerMinutes = 59;  // if reaching out-of-bound, revert to 59.
    }
  }



  /* Select timer "Second" parameter. */
  if ((TimerMode == TIMER_COUNT_DOWN) && (FlagSetTimerSecond == FLAG_ON))
  {
    if (FlagButtonSelect == FLAG_UP)
    {
      /* We come here because user pressed the "Up" (middle) button while in timer "Seconds" parameter setup. */
      ++TimerSeconds;
      if (TimerSeconds == 60) TimerSeconds = 0;  // if reaching out-of-bound, revert to 0.
    }
    else
    {
      /* We come here because user pressed the "Down" (bottom) button while in timer "Seconds" parameter setup. */
      --TimerSeconds;
      if (TimerSeconds == 255) TimerSeconds = 59;  // if reaching out-of-bound, revert to 59.
    }
  }

  return;
}





/* $TITLE=show_time() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
            Read the real-time clock IC and display time.
\* ----------------------------------------------------------------- */
void show_time(void)
{
  char TimeBuffer[4];



  /* Read the real-time clock IC. */
  Time_RTC = Read_RTC();

  Time_RTC.seconds    = Time_RTC.seconds    & 0x7F;
  Time_RTC.minutes    = Time_RTC.minutes    & 0x7F;
  Time_RTC.hour       = Time_RTC.hour       & 0x3F;
  Time_RTC.dayofweek  = Time_RTC.dayofweek  & 0x07;
  Time_RTC.dayofmonth = Time_RTC.dayofmonth & 0x3F;
  Time_RTC.month      = Time_RTC.month      & 0x1F;

  CurrentHourSetting  = bcd_to_byte(Time_RTC.hour);
  CurrentMinute       = bcd_to_byte(Time_RTC.minutes);
  CurrentDayOfMonth   = bcd_to_byte(Time_RTC.dayofmonth);
  CurrentMonth        = bcd_to_byte(Time_RTC.month);
  CurrentYearLowPart  = bcd_to_byte(Time_RTC.year);


  /* Check if we are in "24-hours" display mode. */
  if (TimeDisplayMode == H12)
  {
    /* We are in "12-hours" display mode, adjust the digits accordingly. */
    if (CurrentHourSetting == 0)
    {
      /* If the real-time clock is between "00h00" and 00h59, change it to "12hxx AM". */
      CurrentHour = 12;
      IndicatorAmOn;
      IndicatorPmOff;
    }
    else if (CurrentHourSetting == 12)
    {
      /* If the real-time clock is between "12h00" and 12h59, turn ON the "PM" indicator and turn OFF the "AM" indicator. */
      CurrentHour = CurrentHourSetting;
      IndicatorPmOn;
      IndicatorAmOff;
    }
    else if (CurrentHourSetting > 12)
    {
      /* If the real-time clock is between 13h00 and 23h59, revert to 1 to 12 and turn ON the "PM" indicator. */
      CurrentHour = CurrentHourSetting - 12;
      IndicatorPmOn;
      IndicatorAmOff;
    }
    else
    {
      /* If the real-time clock is between 01h00 and 11h59, turn on the "AM" indicator and turn OFF the "PM" indicator. */
      CurrentHour = CurrentHourSetting;
      IndicatorAmOn;
      IndicatorPmOff;
    }
  }
  else
  {
    /* We are in "24-hours" display mode, nothing to do to what we read from the real-time clock. */
    CurrentHour = CurrentHourSetting;
  }

  /* When in 12-hour time display format, first digit is not displayed if it is zero. */
  if ((TimeDisplayMode == H12) && (CurrentHour < 10))
  {
    TimeBuffer[0]  = (' ');                          // hours first digit.
  }
  else
    TimeBuffer[0]  = ((CurrentHour / 10) + '0');     // hours first digit.

  TimeBuffer[1]  = ((CurrentHour % 10) + '0');       // hours second digit.
  TimeBuffer[2]  = ((Time_RTC.minutes / 16) + '0');  // minutes first digit.
  TimeBuffer[3]  = ((Time_RTC.minutes % 16) + '0');  // minutes second digit.
  SecondCounter  = ((float)Time_RTC.seconds) / 1.5;



  /* Display time of day if we are not scrolling some data. */
  if (FlagScrollStart == FLAG_OFF)
  {
    CurrentClockMode = MODE_SHOW_TIME;

    fill_display_buffer_4X7(0,  TimeBuffer[0]);
    fill_display_buffer_4X7(5,  TimeBuffer[1]);
    fill_display_buffer_4X7(10, 0x3A);  // slim ":"
    fill_display_buffer_4X7(12, TimeBuffer[2]);
    fill_display_buffer_4X7(17, TimeBuffer[3]);
  }



  /* Turn ON the weekday indicator. */
  select_weekday(Time_RTC.dayofweek - 1);

  return;
}





#ifdef TEST_MODE
/* $TITLE=test-mode() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
     Section to keep some useful testing code for eventual re-use.
     NOTE: Clock display is logically divided in sections of 8 columns
           (8 dots). Some of these columns and / or bits of these
           columns being used by indicators.

      Depending on which tests are performed, it may be useful to
       turn off Display scrolling when performing tests, so that
             scrolling does not interfere with the tests.
\* ----------------------------------------------------------------- */



/* ----------------------------------------------------------------- *\
    *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING ***
    *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING ***
    *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING ***

     The pieces of code below have been very useful while testing
    and implementing new features on the clock. However, they must
     be considered as "test code" only. Even if most of the parts
     should work as expected, they have not been tested under all
    conditions and some use cases have certainly not been covered.
    So, most likely, you will need to modify and adapt it to cover
   your own test procedure. Whatever the case, I hope those pieces
                        will be helpful - ASL

    *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING ***
    *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING ***
    *** WARNING *** WARNING *** WARNING *** WARNING *** WARNING ***
\* ----------------------------------------------------------------- */
void test_mode(UINT8 TestType)
{
  UCHAR CharacterBuffer;              // Buffer to temporarily hold a character.
  UCHAR String[DISPLAY_BUFFER_SIZE];  // string to scroll on clock display.

  UINT8 Dum1UInt8;
  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;
  UINT8 Loop3UInt8;
  UINT8 TemperatureF;                 // temperature in Fahrenheit.
  UINT8 WeekDay;

  UINT16 ColumnInSection;               // Column number relative to a specific "section" of the display.
  UINT16 ColumnPosition;                // Column number in virtual display buffer where to put the ASCII character.
  UINT16 Loop1UInt16;
  UINT16 Loop2UInt16;
  UINT16 Loop3UInt16;
  UINT16 RowNumber;                     // Row number on the display (row number 0 corresponds to indicators).
  UINT16 SectionNumber;                 // Section number on the display (display is logically divided in section numbers of 8 dots).



  /* Configure the GPIO associated to the three push-buttons of the clock in "input" mode to use them while testing. */
  /* Tests are usually performed before gpio initialization. */
  gpio_set_dir(SET_BUTTON,  GPIO_IN);
  gpio_set_dir(UP_BUTTON,   GPIO_IN);
  gpio_set_dir(DOWN_BUTTON, GPIO_IN);

  /* Setup a pull-up on those three GPIOs. */
  gpio_pull_up(SET_BUTTON);
  gpio_pull_up(UP_BUTTON);
  gpio_pull_up(DOWN_BUTTON);



  /* NOTE: Display power supply voltage and clock, so that we
           know we are entering the test function. */
  /* --------------------------------------------------------------- *\
                     Display power supply voltage
              and update it a few times on the display.
  \* --------------------------------------------------------------- */
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
  /* --------------------------------------------------------------- *\
                 END - Display power supply voltage
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
      Display time (clock) for a few seconds before beginning tests.
  \* --------------------------------------------------------------- */
  /* Single tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  show_time();
  sleep_ms(1000);
  /* --------------------------------------------------------------- *\
                      END - Display time clock
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
       Jump to the specific test number if one has been specified.
              (0 means to execute all tests in sequence).
  \* --------------------------------------------------------------- */
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





  /* --------------------------------------------------------------- *\
               Test 1 - Display all 4 X 7 character set.
  \* --------------------------------------------------------------- */
Test1:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);


  /* Announce test number. */
  scroll_string(24, "Test 1");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  /* Initialize clock mode. */
  CurrentClockMode = MODE_TEST;


  /* Read RTC IC registers. */
  ds3231_register_read();


  /* Display RTC registers. */
  sprintf(String, "1: %2.2X   2: %2.2X   3: %2.2X   4: %2.2X   5: %2.2X   6: %2.2X   7: %2.2X",
          Ds3231ReadRegister[0], Ds3231ReadRegister[1], Ds3231ReadRegister[2], Ds3231ReadRegister[3], Ds3231ReadRegister[4], Ds3231ReadRegister[5], Ds3231ReadRegister[6]);
  scroll_string(24, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  sprintf(String, "8: %2.2X   9: %2.2X   10: %2.2X   11: %2.2X   12: %2.2X   13: %2.2X   14: %2.2X",
          Ds3231ReadRegister[7], Ds3231ReadRegister[8], Ds3231ReadRegister[9], Ds3231ReadRegister[10], Ds3231ReadRegister[11], Ds3231ReadRegister[12], Ds3231ReadRegister[13]);
  scroll_string(24, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  return;



  /* Display all 4 X 7 character bitmaps. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 89; ++Loop1UInt8)
  {
    clear_framebuffer(0);
    fill_display_buffer_4X7( 0, 0x30 + (Loop1UInt8 / 10));
    fill_display_buffer_4X7( 5, 0x30 + (Loop1UInt8 % 10));
    fill_display_buffer_4X7(15,        (Loop1UInt8 + 0x2D));
    sleep_ms(2000);
  }

  return;
  /* --------------------------------------------------------------- *\
             END - Test 1 - Display all 4 X 7 character set.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
                    Test 2 - Not used for now.
  \* --------------------------------------------------------------- */
Test2:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 2 not used");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  return;
  /* --------------------------------------------------------------- *\
                END - Test 2 - Not used for now.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
                   Test 3 - Not used for now.
  \* --------------------------------------------------------------- */
Test3:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 3 not used");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  return;
 /* --------------------------------------------------------------- *\
                 END - Test 3 - Not used for now.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
                   Test 4 - Not used for now.
  \* --------------------------------------------------------------- */
Test4:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 4");

  sprintf(String, "DST = 0x%2.2X", FlagDayLightSavingTime);
  scroll_string(24, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  return;
  /* --------------------------------------------------------------- *\
                 END - Test 4 - Not used for now.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
                  Test 5 - Display scrolling tests
  \* --------------------------------------------------------------- */
Test5:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 5");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.



  /* Fill DisplayBuffer[] with bitmap corresponding to the ASCII string. */
  sprintf(String, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  ColumnPosition = 24;  // put the bitmap beginning at position 22 + 2, just beyond the visible clock.



  /* Transfer the bitmap of each ASCII character to the display buffer. */
  for (Loop1UInt8 = 0; Loop1UInt8 < strlen(String); ++Loop1UInt8)
  {
    SectionNumber   = ColumnPosition / 8;     // determine the target section number.
    ColumnInSection = ColumnPosition % 8;     // determine the target column number in this section.

    if (ColumnPosition > (UINT16)DISPLAY_BUFFER_SIZE) break;  // get out of the loop if the string is longer than allowed by display buffer size.

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

    ColumnPosition += CharWidth[String[Loop1UInt8] - 0x1E] + 1;  // add 1 space between characters.
  }




  /* ----------------------------------------------------------------- *\
                Then, scroll the bitmap on the display
  \* ----------------------------------------------------------------- */
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
    sleep_ms(100);  // let the time to complete scrolling.

  return;
  /* --------------------------------------------------------------- *\
                END - Test 5 - Display scrolling tests
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
       Test 6 - Scroll all 5 X 7 characters to test variable width
                       and character spacing.
  \* --------------------------------------------------------------- */
Test6:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);


  /* Announce test number. */
  scroll_string(24, "Test 6");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


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

    String[17] = 0x00;          // add end-of-string.
    scroll_string(22, String);  // and scroll the data.
    while (ScrollDotCount)
      sleep_ms(100);  // let the time to complete scrolling.
  }

  return;
  /* --------------------------------------------------------------- *\
    END - Test 6 - Scroll all 5 X 7 characters to test variable width
                        and character spacing.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
              Test 7 - Verify the 4 X 7 character bitmaps.
  \* --------------------------------------------------------------- */
Test7:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 7");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  for (Loop1UInt8 = 0x2D; Loop1UInt8 < 0x2D + 53; ++Loop1UInt8)
  {
    /* Clear framebuffer. */
    clear_framebuffer(0);
    fill_display_buffer_4X7(8, Loop1UInt8);
    sleep_ms(1000);
  }

  return;
  /* --------------------------------------------------------------- *\
           END - Test 7 - Verify the 4 X 7 character bitmaps.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
          Test 8 - Display all characters of the 5 X 7 bitmap,
             using three different chunk of scrolling text.
  \* --------------------------------------------------------------- */
Test8:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);


  /* Announce test number. */
  scroll_string(24, "Test 8");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.



  /* Wipe String[] on entry. */
  for(Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
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
  for(Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
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
    sleep_ms(100);  // let the time to complete scrolling.



  /* Clear framebuffer. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Wipe String[] on entry. */
  for(Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    String[Loop1UInt8] = 0x00;


  /* Display last bunch of characters. */
  String[0]  = 0x5B;
  String[1]  = 0x5C;
  String[2]  = 0x5D;
  String[3]  = 0x5E;
  String[4]  = 0x5F;
  String[0]  = 0x60;
  String[0]  = 0x61;
  String[1]  = 0x62;
  String[2]  = 0x63;
  String[3]  = 0x64;
  String[4]  = 0x65;
  String[5]  = 0x66;
  String[6]  = 0x67;
  String[7]  = 0x68;
  String[8]  = 0x69;
  String[9]  = 0x6A;
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
    sleep_ms(100);  // let the time to complete scrolling.

  return;
  /* --------------------------------------------------------------- *\
        END - Test 8 - Display all characters of the 5 X 7 bitmap,
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
                   Test 9 - Tests with indicators.
  \* --------------------------------------------------------------- */
Test9:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 9");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.



  /* Wait for "Set" (top) button to be pressed... */
  /* Blink the two white LEDs near the buttons to show that we are waiting for "Set" button press,
     then put a "prompt" on the clock display. */
  IndicatorButtonLightsOn;
  sleep_ms(500);
  IndicatorButtonLightsOff;
  sleep_ms(500);
  fill_display_buffer_5X7(2, '>');
  while (gpio_get(SET_BUTTON));



  /* Turn On all indicators on the left-side of the display. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 5; ++Loop1UInt8)
  {
    IndicatorScrollOn;
    IndicatorCountDownOn;
    IndicatorFrnhtOn;
    IndicatorCelsiusOn;
    IndicatorAmOn;
    IndicatorPmOn;
    IndicatorMondayOn;
    IndicatorAlarmOn;
    IndicatorCountUpOn;
    IndicatorHourlyChimeOn;
    IndicatorAutoLightOn ;
    IndicatorButtonLightsOn;


    /* Wait for "Set" (top) button to be pressed... */
    /* Blink the two white LEDs near the buttons to show that we are waiting for "Set" button press,
       then put a "prompt" on the clock display. */
    IndicatorButtonLightsOn;
    sleep_ms(500);
    IndicatorButtonLightsOff;
    sleep_ms(500);
    fill_display_buffer_5X7(2, '>');
    while (gpio_get(SET_BUTTON));


    /* Turn Off all indicators on the left-side of the display. */
    IndicatorScrollOff;
    IndicatorCountDownOff;
    IndicatorFrnhtOff;
    IndicatorCelsiusOff;
    IndicatorAmOff;
    IndicatorPmOff;
    IndicatorMondayOff;
    IndicatorAlarmOff;
    IndicatorCountUpOff;
    IndicatorHourlyChimeOff;
    IndicatorAutoLightOff;
    IndicatorButtonLightsOff;


    /* Wait for "Set" (top) button to be pressed... */
    /* Blink the two white LEDs near the buttons to show that we are waiting for "Set" button press,
       then put a "prompt" on the clock display. */
    IndicatorButtonLightsOn;
    sleep_ms(500);
    IndicatorButtonLightsOff;
    sleep_ms(500);
    fill_display_buffer_5X7(2, '>');
    while (gpio_get(SET_BUTTON));
  }

  return;
  /* --------------------------------------------------------------- *\
                 END - Test 9 - Tests with indicators.
  \* --------------------------------------------------------------- */




  /* --------------------------------------------------------------- *\
                   Test 10 - Tests with indicators.
  \* --------------------------------------------------------------- */
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
  scroll_string(24, "Test 10");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.



  /* Scan all bytes of the clock display matrix. */
  for (Loop1UInt8 = 1; Loop1UInt8 < 24; ++Loop1UInt8)
  {
    if (Loop1UInt8 == 16) IndicatorButtonLightsOn; // To check two while LEDs indicators.

    /* DisplayBuffer[8] and [16] are reserved for indicators bitmap, so they are skipped. */
    if ((Loop1UInt8 == 8) || (Loop1UInt8 == 16))
    {
      tone(60);  // tone the user to indicate that a byte has been skipped.
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
    while (gpio_get(SET_BUTTON));


    /* Clear framebuffer. */
    clear_framebuffer(0);

    sleep_ms(300);
  }
  sleep_ms(1000);

  return;
  /* --------------------------------------------------------------- *\
                END - Test 10 - Tests with indicators.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
         Test 11 - Test to visualize variable character width.
  \* --------------------------------------------------------------- */
Test11:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 2 - not used");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  // Give information to user.
  scroll_string(24, "Variable character width...");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

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
  /* --------------------------------------------------------------- *\
       END - Test 11 - Test to visualize variable character width.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
                  Test 12 - Test 5 X 7 character map
  \* --------------------------------------------------------------- */
Test12:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 12");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  /* Scan all available ASCII characters. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 100; ++Loop1UInt8)
  {
    /* Clear framebuffer. */
    clear_framebuffer(0);

    for (RowNumber = 1; RowNumber < 8; ++RowNumber)
    {
      /* We need to reverse bit order of the character bitmap since the left-most column
         on the clock display corresponds to the lowest bit. */
      DisplayBuffer[8 + RowNumber] = reverse_bits(CharMap[(Loop1UInt8 * 7) + RowNumber -1]);
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
  while (gpio_get(SET_BUTTON));

  return;
  /* --------------------------------------------------------------- *\
                END - Test 12 - Test 5 X 7 character map
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
              Test 13 - Scrolling test with 5 X 7 matrix.
  \* --------------------------------------------------------------- */
Test13:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 13");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  /* Wipe String[] on entry. */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
    String[Loop1UInt8] = 0x00;



  sprintf(String, "A");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  sprintf(String, "B");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  sprintf(String, "AA");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  sprintf(String, "BB");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  sprintf(String, "AB");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  sprintf(String, "ABAB");
  scroll_string(22, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  /* Wait for "Set" button to be pressed... */
  /* Blink the two white LEDs near the buttons to show that we are waiting for "Set" button press,
     then put a "prompt" on the clock display. */
  IndicatorButtonLightsOn;
  sleep_ms(500);
  IndicatorButtonLightsOff;
  sleep_ms(500);
  fill_display_buffer_5X7(2, '>');
  while (gpio_get(SET_BUTTON));

  return;
  /* --------------------------------------------------------------- *\
           END - Test 13 - Scrolling test with 5 X 7 matrix.
  \* --------------------------------------------------------------- */





  /* --------------------------------------------------------------- *\
              Test 14 - Scrolling test with 4 X 7 matrix.
  \* --------------------------------------------------------------- */
Test14:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Prevent dots blinking in the clock. */
  CurrentClockMode = MODE_SCROLLING;


  /* Announce test number. */
  scroll_string(4, "Test 14");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.



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



  /* ----------------------------------------------------------------- *\
                 Scroll the framebuffer on the display.
  \* ----------------------------------------------------------------- */
  for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
  {
    /* Scan the 7 rows of data. Row 0 is reserved for "indicators". */
    for (RowNumber = 1; RowNumber < 8; ++RowNumber)
    {
      CharacterBuffer = DisplayBuffer[RowNumber] & 0x03;  // keep track of the "indicators" status (first 2 bits).

      sleep_ms(10);  // value can be increase (up tp 255) to better see scroll movement.

      /* Scan all "sections" of the framebuffer. One section is 8 bits width. */
      for (SectionNumber = 0; SectionNumber < (sizeof(DisplayBuffer) / 8); ++SectionNumber)
      {
        /* If we're not at the last section of the virtual display buffer, move one bit to the left and add the upmost bit of the next section. */
        if (SectionNumber < sizeof(DisplayBuffer) / 8 - 1)
          DisplayBuffer[(SectionNumber * 8) + RowNumber] = DisplayBuffer[(SectionNumber * 8) + RowNumber] >> 1 | DisplayBuffer[(SectionNumber * 8) + RowNumber + 8] << 7;
        else
          DisplayBuffer[(SectionNumber * 8) + RowNumber] = DisplayBuffer[(SectionNumber * 8) + RowNumber] >> 1;
      }
      DisplayBuffer[RowNumber] = (DisplayBuffer[RowNumber] & (~0x03)) | CharacterBuffer;  // restore the "indicators" status.
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
  fill_display_buffer_4X7( 0, '0');
  sleep_ms(500);
  fill_display_buffer_4X7( 5, '1');
  sleep_ms(500);
  fill_display_buffer_4X7(10, '2');
  sleep_ms(500);
  fill_display_buffer_4X7(15, '3');
  sleep_ms(500);
  fill_display_buffer_4X7(20, '4');
  sleep_ms(5000);
  ScrollSecondCounter  = 0;  // delay eventual date display on clock display
  ScrollDotCount       = 24;
  FlagScrollStart      = FLAG_ON;

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

  FlagScrollData  = FLAG_OFF;  // we don't want to fill display buffer with the date before scrolling.
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
      CharacterBuffer = DisplayBuffer[Loop2UInt8] & 0x03;  // keep track of "indicators" status (first 2 bits).

      for (Loop3UInt8 = 0; Loop3UInt8 < sizeof(DisplayBuffer) / 8; ++Loop3UInt8)
      {
        if (Loop3UInt8 < sizeof(DisplayBuffer) / 8 - 1)
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1 | DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8 + 8] << 7;
        else
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1;
      }

      DisplayBuffer[Loop2UInt8] = (DisplayBuffer[Loop2UInt8] & (~0x03)) | CharacterBuffer;  // restore "indicators" status.
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
      CharacterBuffer = DisplayBuffer[Loop2UInt8] & 0x03;  // keep track of "indicators" status (first 2 bits).

      for (Loop3UInt8 = 0; Loop3UInt8 < sizeof(DisplayBuffer) / 8; ++Loop3UInt8)
      {
        if (Loop3UInt8 < sizeof(DisplayBuffer) / 8 - 1)
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1 | DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8 + 8] << 7;
        else
          DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] = DisplayBuffer[(Loop3UInt8 * 8) + Loop2UInt8] >> 1;
      }

      DisplayBuffer[Loop2UInt8] = (DisplayBuffer[Loop2UInt8] & (~0x03)) | CharacterBuffer;  // restore "indicators" status.
    }
    sleep_ms(SCROLL_DOT_TIME);  // time delay to scroll one dot to the left.
  }
  sleep_ms(5000);

  return;
  /* --------------------------------------------------------------- *\
           END - Test 14 - Scrolling test with 4 X 7 matrix.
  \* --------------------------------------------------------------- */





/* ----------------------------------------------------------------- *\
            Test 15 - Display some info on clock display.
\* ----------------------------------------------------------------- */
Test15:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);


  /* Announce test number. */
  scroll_string(24, "Test 15");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  sprintf(String, "strlen(DisplayBuffer) = %u", strlen(DisplayBuffer));
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  sprintf(String, "sizeof(DisplayBuffer) = %u", sizeof(DisplayBuffer));
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  sprintf(String, "Value of TRUE = %u", TRUE);
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  sprintf(String, "Value of FALSE = %u", FALSE);
  scroll_string(20, String);
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.

  return;
/* ----------------------------------------------------------------- *\
          END - Test 15 - Display some info on clock display.
\* ----------------------------------------------------------------- */





/* ----------------------------------------------------------------- *\
               Test 16 - Turn ON and OFF in sequence
             each bit of each byte of the DisplayBuffer.
\* ----------------------------------------------------------------- */
Test16:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);
  sleep_ms(100);


  /* Announce test number. */
  scroll_string(24, "Test 16");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


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
    while (gpio_get(SET_BUTTON));


    ColumnPosition = 1;  // will be auto-incremented in the fill_display_buffer_5X7() function.
    for (Loop2UInt8 = 0; Loop2UInt8 < strlen(String); ++Loop2UInt8)
    {
      if (ColumnPosition > (UINT16)DISPLAY_BUFFER_SIZE) break;  // get out of the loop if the string is longer than allowed by display buffer size.

      ColumnPosition = fill_display_buffer_5X7(++ColumnPosition, String[Loop2UInt8]);
    }
    sleep_ms(1000);  // give time for user to read the byte under test.



    /* Blank the whole display. */
    for (Loop1UInt8 = 0; Loop1UInt8 < DISPLAY_BUFFER_SIZE; ++Loop1UInt8)
      DisplayBuffer[Loop1UInt8] = 0;
    sleep_ms(200);  // wait some time before scrolling the dot on the display.



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
/* ----------------------------------------------------------------- *\
             END - Test 16 - Turn ON and OFF in sequence
             each bit of each byte of the DisplayBuffer.
\* ----------------------------------------------------------------- */





/* ----------------------------------------------------------------- *\
          Test 17 - Play with weekday indicators in sequence.
\* ----------------------------------------------------------------- */
Test17:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);


  /* Announce test number. */
  scroll_string(24, "Test 17");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  // Turn ON, then OFF, each weekday indicator (2 LEDs per indicator).
  DisplayBuffer[0]  |= (1 << 3) | (1 << 4);       // Monday
  sleep_ms(500);
  DisplayBuffer[0]  &= ~((1 << 3) | (1 << 4));
  sleep_ms(500);

  DisplayBuffer[0]  |= (1 << 6) | (1 << 7);       // Tuesday
  sleep_ms(500);
  DisplayBuffer[0]  &= ~(( 1 << 6) | (1 << 7));
  sleep_ms(500);

  DisplayBuffer[8]  |= (1 << 1) | (1 << 2);       // Wednesday
  sleep_ms(500);
  DisplayBuffer[8]  &= ~((1 << 1) | (1 << 2));
  sleep_ms(500);

  DisplayBuffer[8]  |= (1 << 4) | (1 << 5);       // Thursday
  sleep_ms(500);
  DisplayBuffer[8]  &= ~((1 << 4) | (1 << 5));
  sleep_ms(500);

  DisplayBuffer[8]  |= (1 << 7);                  // Friday
  DisplayBuffer[16] |=  (1 << 0);
  sleep_ms(500);
  DisplayBuffer[8]  &=  ~(1 << 7);
  DisplayBuffer[16] &= ~(1<<0);
  sleep_ms(500);

  DisplayBuffer[16] |= (1 << 2) | (1 << 3);       // Saturday
  sleep_ms(500);
  DisplayBuffer[16] &= ~((1 << 2) | (1 << 3));
  sleep_ms(500);

  DisplayBuffer[16] |= (1 << 5) | (1 << 6);       // Sunday
  sleep_ms(500);
  DisplayBuffer[16] &= ~((1 << 5) | (1 << 6));
  sleep_ms(500);


  // Now do the same with all other display indicators.
  DisplayBuffer[0] |= 0X03;               // scroll indicator
  sleep_ms(500);
  DisplayBuffer[0] &= ~0X03;
  sleep_ms(500);
  DisplayBuffer[1] |= 0X03;               // alarm indicator
  sleep_ms(500);
  DisplayBuffer[1] &= ~0x03;
  sleep_ms(500);
  DisplayBuffer[2] |= 0X03;               // count down timer indicator
  sleep_ms(500);
  DisplayBuffer[2] &= ~0x03;
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 0);           // Farenheit indicator
  sleep_ms(500);
  DisplayBuffer[3] &= ~(1 << 0);
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 1);           // Celsius indicator
  sleep_ms(500);
  DisplayBuffer[3] &= ~(1 << 1);
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 0);           // AM indicator
  sleep_ms(500);
  DisplayBuffer[4] &= ~(1 << 0);
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 1);           // PM indicator
  sleep_ms(500);
  DisplayBuffer[4] &= ~(1 << 1);
  sleep_ms(500);
  DisplayBuffer[5] |= 0X03;               // count up timer indicator
  sleep_ms(500);
  DisplayBuffer[5] &= ~0x03;
  sleep_ms(500);
  DisplayBuffer[6] |= 0X03;               // hourly chime indicator
  sleep_ms(500);
  DisplayBuffer[6] &= ~0X03;
  sleep_ms(500);
  DisplayBuffer[7] |= 0X03;               // auto brightness indicator
  sleep_ms(500);
  DisplayBuffer[7] &= ~0X03;
  sleep_ms(500);
  DisplayBuffer[0] |= (1 << 2)|(1 << 5);  // two white LEDs near the buttons inside the clock
  sleep_ms(500);
  DisplayBuffer[0] &= ~((1 << 2)|(1 << 5));
  sleep_ms(500);



  /* Turn ON all weekday indicators, one after the other. */
  DisplayBuffer[0]  |= (1 << 3) | (1 << 4);  // Monday
  sleep_ms(500);
  DisplayBuffer[0]  |= (1 << 6) | (1 << 7);  // Tuesday
  sleep_ms(500);
  DisplayBuffer[8]  |= (1 << 1) | (1 << 2);  // Wednesday
  sleep_ms(500);
  DisplayBuffer[8]  |= (1 << 4) | (1 << 5);  // Thursday
  sleep_ms(500);
  DisplayBuffer[8]  |= (1 << 7);             // Friday
  DisplayBuffer[16] |= (1 << 0);
  sleep_ms(500);
  DisplayBuffer[16] |= (1 << 2) | (1 << 3);  // Saturday
  sleep_ms(500);
  DisplayBuffer[16] |= (1 << 5) | (1 << 6);  // Sunday
  sleep_ms(500);



  /* Turn ON all left-side indicators, one after the other. */
  DisplayBuffer[0] |= 0X03;               // scroll indicator
  sleep_ms(500);
  DisplayBuffer[1] |= 0X03;               // alarm indicator
  sleep_ms(500);
  DisplayBuffer[2] |= 0X03;               // timer indicator
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 0);           // Farenheit indicator
  sleep_ms(500);
  DisplayBuffer[3] |= (1 << 1);           // Celsius indicator
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 0);           // AM indicator
  sleep_ms(500);
  DisplayBuffer[4] |= (1 << 1);           // PM indicator
  sleep_ms(500);
  DisplayBuffer[5] |= 0X03;               // count up indicator ??
  sleep_ms(500);
  DisplayBuffer[6] |= 0X03;               // hourly chime indicator
  sleep_ms(500);
  DisplayBuffer[7] |= 0X03;               // auto light indicator
  sleep_ms(500);
  DisplayBuffer[0] |= (1 << 2)|(1 << 5);  // back light indicator
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
/* ----------------------------------------------------------------- *\
       END - Test 17 - Play with weekday indicators in sequence.
\* ----------------------------------------------------------------- */





/* ----------------------------------------------------------------- *\
       Test 18 - Quick display test with both character formats.
\* ----------------------------------------------------------------- */
Test18:

  /* Clear framebuffer on entry. */
  clear_framebuffer(0);


  /* Tone to announce entering a new test. */
  tone(10);


  /* Announce test number. */
  scroll_string(24, "Test 18");
  while (ScrollDotCount)
    sleep_ms(100);  // let the time to complete scrolling.


  /* Quick display test with 4 X 6 character bitmap. */
  fill_display_buffer_4X7(1,  '1');
  fill_display_buffer_4X7(6,  '2');
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
/* ----------------------------------------------------------------- *\
    END - Test 18 - Quick display test with both character formats.
\* ----------------------------------------------------------------- */
}
#endif





/* $TITLE=timer_callback_ms() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
           Timer callback function (1 millisecond period).
    As with any ISR ("Interrupt Service Routine"), we must keep the
    processing as quick as possible since we want to make sure that
     the routine has completed before the next interrupt comes in.
\* ----------------------------------------------------------------- */
bool timer_callback_ms(struct repeating_timer *t)
{
  UINT8 Loop1UInt8;



  adc_show_count();               // read ambient light and power supply voltage from Pico ADC port.
  evaluate_sound_stop_time();     // evaluate if it is time to stop a sound.
  evaluate_blinking_time();       // evaluate if it is time to start blinking data on clock display.
  evaluate_scroll_time();         // evaluate if it is time to scroll one dot to the left.



  /* Check if the "Top" button is pressed. */
  if (gpio_get(SET_BUTTON) == 0)
  {
    /* The "Top" button is pressed... cumulate the number of milliseconds it is pressed. */
    ++TopKeyPressTime;
  }
  else
  {
    if ((TopKeyPressTime > 50) && (TopKeyPressTime < 300))
    {
      /* The "Top" button is not pressed, but cumulative time is not zero, it means that the "Top"
         button has just been released and it is a "short press" (between 50 and 300 milliseconds). */

      IdleNumberOfSeconds = 0;      // reset the number of seconds the system has been idle.

      evaluate_keyclick_start_time();       // button has just been released, check if we should make a "keyclick tone".

      if (AlarmId == 1)
        FlagAlarmSetup = FLAG_ON;
      else if (UpId == 1)
        FlagButtonUp = FLAG_ON;
      else
        FlagSetUp = FLAG_ON;             // declare that we enter setup mode.

      set_mode_out();                    // program RTC IC before proceeding with next setting step.
      ++SetupStep;                       // proceed through all setup steps (either clock, alarm or timer).
    }
    else if ((TopKeyPressTime > 300) && (SetupStep == SETUP_NONE))
    {
      /* The "Top" button is not pressed, but cumulative time is not zero, it means that the "Top"
         button has just been released and it is a "long press" (longer than 300 milliseconds). */

      IdleNumberOfSeconds = 0;      // reset the number of seconds the system has been idle.

      evaluate_keyclick_start_time();  // button has been release, check if we should make a "tone" of "keyclick-type".

      /* If we are not already in setup mode, enter alarm setup mode. */
      FlagAlarmSetup  = FLAG_ON;    // enter alarm setup mode.
      AlarmId         = 1;
      ++SetupStep;                  // process through all alarm setup steps.
    }

    /* Reset "Top" button cumulative press time. */
    TopKeyPressTime = 0;
  }



  /* Check if the "Middle" button is pressed. */
  if (gpio_get(UP_BUTTON) == 0)
  {
    /* The "Middle" button is pressed... cumulate the time it is pressed. */
    ++MiddleKeyPressTime;
  }
  else
  {
    if ((MiddleKeyPressTime > 50) && (MiddleKeyPressTime < 300))
    {
      /* The "Middle" button has been released and it is a "short press" (between 50 and 300 milliseconds). */

      IdleNumberOfSeconds = 0;      // reset the number of seconds the system has been idle.

      evaluate_keyclick_start_time();  // button has been release, check if we should make a keyclick tone".

      /* If we are not in setup mode, "Middle" button toggles between Celsius and Fahrenheit. */
      if (SetupStep == SETUP_NONE)
      {
        TemperatureUnit = !TemperatureUnit;
        if (TemperatureUnit == CELSIUS)
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

      /* Go through all device settings to process this "Up" key. */
      setup_alarm_variables(FLAG_UP);  // perform alarm settings.
      setup_timer_variables(FLAG_UP);  // perform timer settings.
      setup_clock_variables(FLAG_UP);  // perform clock settings.
    }
    else if ((MiddleKeyPressTime > 300) && (SetupStep == SETUP_NONE))
    {
      /* The "Middle" button has been released and it is a "long press". */

      IdleNumberOfSeconds = 0;      // reset the number of seconds the system has been idle.

      evaluate_keyclick_start_time();  // button has been released, check if we should make a keyclick tone.

      if (SetupStep == SETUP_NONE)
      {
        FlagButtonUp = FLAG_ON;
        UpId = 1;
        ++SetupStep;
      }
    }

    /* Reset "Middle" button cumulative press time. */
    MiddleKeyPressTime = 0;
  }



  /* Check if the "Bottom" button is press. */
  if (gpio_get(DOWN_BUTTON) == 0)
  {
    /* The "Bottom" button is pressed... cumulate the time it is pressed. */
    ++BottomKeyPressTime;
  }
  else
  {
    if ((BottomKeyPressTime > 0) && (BottomKeyPressTime < 300))
    {
      /* The "Bottom" button has been released and it is a "short press" (less than 300 milliseconds). */

      IdleNumberOfSeconds = 0;      // reset the number of seconds the system has been idle.

      evaluate_keyclick_start_time();  // button has been release, check if we should make a keyclick tone.

      /* If we are not in setup mode, toggle between full brightness and automatic brightness clock display. */
      if (SetupStep == SETUP_NONE)
      {
        if (FlagAutoBrightness)
        {
          FlagAutoBrightness = FLAG_OFF;
          IndicatorAutoLightOff;
        }
        else
        {
          FlagAutoBrightness = FLAG_ON;
          IndicatorAutoLightOn;
        }
      }

      /* Go through all device settings to process this "Down" key. */
      setup_alarm_variables(FLAG_DOWN);  // perform alarm settings.
      setup_timer_variables(FLAG_DOWN);  // perform timer settings.
      setup_clock_variables(FLAG_DOWN);  // perform clock settings.
    }
    else if ((BottomKeyPressTime > 300) && (SetupStep != SETUP_NONE))
    {
      /* The "bottom" key has been released and it is a "long press". */

      IdleNumberOfSeconds = 0;      // reset the number of seconds the system has been idle.

      evaluate_keyclick_start_time();  // button has been released, check if we should make a "tone" of "keyclick-type".

      set_mode_timeout();           // exit setup mode.
      set_mode_out();
      SetupStep = SETUP_NONE;
    }

    /* Reset "Bottom" button cumulative press time. */
    BottomKeyPressTime = 0;
  }



  ++CsCounter;

  if (CsCounter > 7)
  {
    CsCounter = 0;
  }

  if ((FlagAutoBrightness == FLAG_ON) && (FlagAdcLightTime == FLAG_ON))
  {
    cancel_repeating_timer(&timer2);
    OE_CLOSE;
  }
  else
  {
    if (FlagAdcLightTime == FLAG_ON)
    {
      FlagAdcLightTime = FLAG_OFF;
      cancel_repeating_timer(&timer2);
    }
    OE_CLOSE;
  }

  for (Loop1UInt8 = 0; Loop1UInt8 < 4; ++Loop1UInt8)
  {
    send_data(DisplayBuffer[8 * Loop1UInt8 + CsCounter]);
  }

  LE_HIGH;
  LE_LOW;

  if (CsCounter & 0x01)
    A0_HIGH;
  else
    A0_LOW;

  if (CsCounter & 0x02)
    A1_HIGH;
  else
    A1_LOW;

  if (CsCounter & 0x04)
    A2_HIGH;
  else
    A2_LOW;

  if (FlagAutoBrightness == FLAG_ON)
  {
    FlagAdcLightTime = FLAG_ON;
    add_repeating_timer_us(50, timer_callback_us, NULL, &timer2);
  }
  else
  {
    OE_OPEN;
  }

  return TRUE;
}





/* $TITLE=timer_callback_s() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                  One second timer callback function
    As with any ISR ("Interrupt Service Routine"), we must keep the
    processing as quick as possible since we want to make sure that
     the routine has completed before the next interrupt comes in.
\* ----------------------------------------------------------------- */
bool timer_callback_s(struct repeating_timer *t)
{
  UINT8 Loop1UInt8;
  UINT8 String[DISPLAY_BUFFER_SIZE];

  UINT16 ColumnPosition;



  ++SecondCounter;  // cumulate 60 seconds before updating minute count.


  /* Check if an alarm is currently sounding. */
  if (FlagAlarmBeeping == FLAG_ON)
  {
    /* If an alarm is currently sounding, turn it off. */
    gpio_put(BUZZ, 0);
    FlagAlarmBeeping = FLAG_OFF;
  }



  /* Check if we just changed the minute count. */
  if (SecondCounter == 60)
  {
    SecondCounter = 0;  // reset second counter.
    ++CurrentMinute;

    /* Reset flag so that next hourly chime will sound. */
    FlagChimeDone = FLAG_OFF;


    if (CurrentMinute == 60)
    {
      CurrentMinute = 0;
      ++CurrentHourSetting;

      if (CurrentHourSetting == 24)
      {
        CurrentHourSetting = 0;
      }
    }


    /* Check if we reached alarm 0 condition. */
    if (Alarm[0].FlagStatus == FLAG_ON)
    {
      if ((Time_RTC.dayofweek == Alarm[0].Day) && (ds3231_check_alarm_0() == TRUE))
      {
        FlagAlarmBeeping   = FLAG_ON;
        gpio_put(BUZZ, 1);
        FlagToneOn         = FLAG_ON;
        ToneType           = TONE_ALARM0_TYPE;
        ToneRepeatCount    = 0;  // initialize count when starting.
      }
    }


    /* Check if we reached alarm 1 condition. */
    if (Alarm[1].FlagStatus == FLAG_ON)
    {
      if ((Time_RTC.dayofweek == Alarm[1].Day) && (ds3231_check_alarm_1() == TRUE))
      {
        FlagAlarmBeeping   = FLAG_ON;
        gpio_put(BUZZ, 1);
        FlagToneOn         = FLAG_ON;
        ToneType           = TONE_ALARM1_TYPE;
        ToneRepeatCount    = 0;  // initialize count when starting.
      }
    }


    if ((FlagIdleCheck == FLAG_OFF) && (ScrollStartCount == 0))
    {
      /* Request a "time" update (on clock display) if we are not
         in some setup mode and not scrolling data. */
      FlagUpdateTime = FLAG_ON;
    }
  }



  /* ....................................................................... *\
                                   Idle time.
  \* ....................................................................... */
  /* If we are in some setup mode (clock, timer or alarm), check if it is time to declare a time-out. */
  if (FlagIdleCheck == FLAG_ON)
  {
    ++IdleNumberOfSeconds;  // one more second added while the system has been idle.

    if (IdleNumberOfSeconds == TIME_OUT_PERIOD)
    {
      set_mode_timeout();     // get out of setup mode because of inaction (time-out).
      set_mode_out();         // proceed with what needs to be set and then cleanup as we get out of setup mode.
      SetupStep = SETUP_NONE; // reset setup step.


      /* If we timed out while setting up an alarm, keep what has been defined so far if alarm status is "Alarm On". */
      if ((AlarmNumber == 0) && (Alarm[AlarmNumber].FlagStatus == FLAG_ON))
      {
        /* Alarm 1 setup. */
        set_alarm1_clock(ALARM_MODE_HOUR_MIN_SEC_MATCHED, 00, Alarm[AlarmNumber].Minute, Alarm[AlarmNumber].Hour, Alarm[AlarmNumber].Day);
      }


      /* If we timed out while setting up an alarm, keep what has been defined so far if alarm status is "Alarm On". */
      if ((AlarmNumber != 0) && (Alarm[AlarmNumber].FlagStatus == FLAG_ON))
      {
        /* Alarm 2 setup. */
        set_alarm2_clock(Alarm[AlarmNumber].Minute, Alarm[AlarmNumber].Hour, Alarm[AlarmNumber].Day);
      }
    }
  }



  /* ....................................................................... *\
                              Count-down timer.
  \* ....................................................................... */
  if (FlagSetTimerCountDown == FLAG_ON)
  {
    /* If a count-down timer has been set and reaches zero, we always sound the piezo, no matter what is the Chime time on / off setting. */
    if ((TimerSeconds == 0) && (TimerMinutes == 0))
    {
      gpio_put(BUZZ, 1);
      FlagToneOn            = FLAG_ON;
      ToneType              = TONE_TIMER_TYPE;
      ToneRepeatCount       = 0;         // initialize repeat count when starting.
      FlagSetTimerCountDown = FLAG_OFF;  // reset count down timer indicator.
      FlagTimerCountDownEnd = FLAG_ON;   // flag indicating we reached final count of count down timer (0m00).
      IndicatorCountDownOff;             // turn off "count-down" indicator when done.
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
        TimerShowSecond = 0;  // for display purpose
      }


      /* Update timer on clock display every so many seconds. */
      if ((TimerShowCount % 1) == 0)
      {
        TimerShowCount  = 0;
        TimerShowSecond = TimerSeconds;
      }
    }
  }





  /* ....................................................................... *\
                                Count-up timer.
  \* ....................................................................... */
  if (FlagSetTimerCountUp == FLAG_ON)
  {
    ++TimerSeconds;    // one more second for Up timer.
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



  /* ....................................................................... *\
                         Date and temperature scrolling.
  \* ....................................................................... */
  /* Scroll the date on clock display every SCROLL_PERIOD seconds, except if we are currently in some setup mode. */
  if ((SecondCounter == 0) && ((CurrentMinute % SCROLL_PERIOD_MINUTE) == 0))
  {
    if ((FlagScrollEnable == FLAG_ON) && (CurrentClockMode == MODE_SHOW_TIME))
    {
      // We reach the time to trigger scrolling of the date and we are in time display mode... proceed.
      if (scroll_queue(TAG_DATE))
      {
        /* Scroll queue is full, beep an alarm (no matter what time it is) and skip this scroll. */
        gpio_put(BUZZ, 1);
        FlagToneOn      = FLAG_ON;
        ToneType        = TONE_CHIME_TYPE;
        ToneRepeatCount = 0;
      }
    }
  }



  /* ....................................................................... *\
                                Hourly chime.
  \* ....................................................................... */
  /* Hourly chime will never sound if set to Off and will always sound if set to On.
     If Hourly chime is set to day "OI" in clock setup for "On, Intermittent":

     "Daytime workers":
     Hourly chime "normal behavior": Chime time on is smaller than Chime time off.
     For example: Chime time on = 9h00 and Chime time off is 21h00.
     Chime will sound between 9h00 and 21h00 as we would expect.

     "Nighttime workers":
     Hourly chime "special behavior": Chime time on is greater than Chime time off.
     For example, Chime time on = 21h00 and Chime time off is 9h00.
     Chime will sound AFTER 21h00 and BEFORE 9h00 and will not sound during daytime
     (that is, will not sound between 9h00 and 21h00). */

  if ((HourlyChimeMode == CHIME_ON) ||  // always On
     ((HourlyChimeMode == CHIME_DAY) && (ChimeTimeOn < ChimeTimeOff) && (CurrentHourSetting >= ChimeTimeOn)  && (CurrentHourSetting <= ChimeTimeOff)) ||  // "normal behavior" for daytime workers.
     ((HourlyChimeMode == CHIME_DAY) && (ChimeTimeOff < ChimeTimeOn) && ((CurrentHourSetting >= ChimeTimeOff) || (CurrentHourSetting <= ChimeTimeOn))))   // "special behavior" for nighttime workers.
  {
    if ((CurrentMinute == 0) && (FlagChimeDone == FLAG_OFF) && (FlagSetTimeMinute == FLAG_OFF))
    {
      gpio_put(BUZZ, 1);
      FlagToneOn      = FLAG_ON;
      ToneType        = TONE_CHIME_TYPE;
      ToneRepeatCount = 0;  // initialize repeat count when starting
      FlagChimeDone   = FLAG_ON;
    }
  }



  /* ....................................................................... *\
                                Calendar events.
                 Check if we should scroll a calendar event.
              Check is done twice an hour, at XXh05 and XXh35.
  \* ....................................................................... */
  if ((SecondCounter == 0) && ((CurrentMinute == 05) || (CurrentMinute == 35)))
  {
    for (Loop1UInt8 = 0; Loop1UInt8 < MAX_EVENTS; ++Loop1UInt8)
    {
      if ((CalendarEvent[Loop1UInt8].Day == CurrentDayOfMonth) && (CalendarEvent[Loop1UInt8].Month == CurrentMonth))
      {
        if (scroll_queue(Loop1UInt8))
        {
          /* Scroll queue is full, beep an alarm (no matter what time it is) and skip that calendar event. */
          gpio_put(BUZZ, 1);
          FlagToneOn      = FLAG_ON;
          ToneType        = TONE_ALARM0_TYPE;  // ALARM_TYPE  because something is going wrong...
          ToneRepeatCount = 0;  // initialize repeat count when starting.
          break;  // go on with next loop iteration.
        }


        /* Sounds "event tones" only if we are during "day time" as defined between CHIME_TIME_ON and CHIME_TIME_OFF. */
        if ((HourlyChimeMode == CHIME_ON) ||  // always On
           ((HourlyChimeMode == CHIME_DAY) && (ChimeTimeOn < ChimeTimeOff) && (CurrentHourSetting >= ChimeTimeOn)  && (CurrentHourSetting <= ChimeTimeOff)) ||  // "normal behavior" for daytime workers.
           ((HourlyChimeMode == CHIME_DAY) && (ChimeTimeOff < ChimeTimeOn) && ((CurrentHourSetting >= ChimeTimeOff) || (CurrentHourSetting <= ChimeTimeOn))))   // "special behavior" for "nighttime workers".
        {
          gpio_put(BUZZ, 1);
          FlagToneOn      = FLAG_ON;
          ToneType        = TONE_EVENT_TYPE;
          ToneRepeatCount = 0;  // initialize repeat count when starting.
        }
      }
    }
  }




  /* ....................................................................... *\
                              Daylight Saving Time
  \* ....................................................................... */
  /* Check if it is time to change for "daylight saving time" or "normal time". */
  if ((SecondCounter == 0) && (CurrentMinute == 0) && (DAYLIGHT_SAVING_TIME != DST_NONE))
  {
    /* Determine if it is time to adjust for "Daylight Saving Time". */
    switch (DAYLIGHT_SAVING_TIME)
    {
      case (DST_NORTH_AMERICA):
        /* Check if it time to change from "Normal Time" to "Daylight Saving Time". */
        if (CurrentHourSetting == 2)
        {
          /// scroll_queue(TAG_DST);  // to scroll debug information on clock display.

          if ((FlagDayLightSavingTime == FLAG_OFF) && (CurrentMonth == MAR) && ((get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) == SUN) && ((CurrentDayOfMonth >= 8) || (CurrentDayOfMonth <= 14))))
          {
            CurrentHourSetting = 3;
            set_hour(CurrentHourSetting);
            FlagDayLightSavingTime = FLAG_ON;
          }

          /* Check if it time to change from "Daylight Saving Time" time to "Normal Time". */
          if ((FlagDayLightSavingTime == FLAG_ON) && (CurrentMonth == NOV) && (get_weekday(Year4Digits, CurrentMonth, CurrentDayOfMonth) == SUN) && ((CurrentDayOfMonth >= 1) || (CurrentDayOfMonth <= 7)))
          {
            CurrentHourSetting = 1;
            set_hour(CurrentHourSetting);
            FlagDayLightSavingTime = FLAG_OFF;
          }
        }
      break;

      default:
        /* If invalid choice, perform no action. */
      break;
    }
  }

  return TRUE;
}





/* $TITLE=timer_callback_us() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
           Timer callback function (50 milliseconds period)
\* ----------------------------------------------------------------- */
bool timer_callback_us(struct repeating_timer *t)
{
  if (AdcLightValue > 3600)
  {
    ++LightSetting;

    if (LightSetting == 15)
    {
      OE_OPEN;
      LightSetting = 0;
    }
    else
    {
      OE_CLOSE;
    }
  }
  else if (AdcLightValue > 2800)
  {
    ++LightSetting;

    if (LightSetting == 3)
    {
      OE_OPEN;
      LightSetting = 0;
    }
    else
    {
      OE_CLOSE;
    }
  }

  return TRUE;
}





/* $TITLE=tone() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
        Make a tone for the specified number of milliseconds
    The piezo used in Waveshare clock has an integrated oscillator,
    so there is no need to make a software frequency with the gpio.
\* ----------------------------------------------------------------- */
void tone(UINT16 MilliSeconds)
{
  gpio_put(BUZZ, 1);
  sleep_ms(MilliSeconds);
  gpio_put(BUZZ, 0);

  return;
}

