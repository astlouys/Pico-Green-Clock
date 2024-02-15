/* ------------------------------------------------------------------ *\
                         Type definitions
\* ------------------------------------------------------------------ */
typedef unsigned int  UINT;   // processor-optimized.
typedef uint8_t       UINT8;
typedef uint16_t      UINT16;
typedef uint32_t      UINT32;
typedef uint64_t      UINT64;
typedef unsigned char UCHAR;


/* ------------------------------------------------------------------ *\
                    Definitions and include files
                       (in alphabetical order)
\* ------------------------------------------------------------------ */
/* Miscellaneous defines. */
#define ALARM_PERIOD              5         // alarm ringer restart every x seconds (part of the whole "nine alarms algorithm").
#define CELSIUS                   0x00
#define CHIME_DAY                 0x02      // hourly chime is ON during defined daily hours (between CHIME_TIME_ON and CHIME_TIME_OFF).
#define CHIME_OFF                 0x00      // hourly chime is OFF.
#define CHIME_ON                  0x01      // hourly chime is ON.
#define COUNT_DOWN_DELAY          7         // number of seconds between each count-down alarm sound burst.
#define CRC16_POLYNOM             0x1021    // different polynom values are used by different authorities. (0x8005, 0x1021, 0x1DCF, 0x755B, 0x5935, 0x3D65, 0x8BB7, 0x0589, 0xC867, 0xA02B, 0x2F15, 0x6815, 0xC599, 0x202D, 0x0805, 0x1CF5)
#define DEFAULT_YEAR_CENTILE      20        // to be used as a default before flash configuration is read (to be displayed in debug log).
#define DELTA_TIME                60000000ll
#define DISPLAY_BUFFER_SIZE       248       // size of framebuffer.
#define EVENT_MINUTE1             14        // (Must be between 0 and 57) Calendar Events will checked when minutes reach this number (should preferably be selected out of peak periods).
#define EVENT_MINUTE2             44        // (Must be between 0 and 57) Calendar Events will checked when minutes reach this number (should preferably be selected out of peak periods).
#define FAHRENHEIT                !CELSIUS  // temperature unit to display.
#define FALSE                     0x00
#define FLAG_OFF                  0x00      // flag is OFF.
#define FLAG_ON                   0x01      // flag is ON.
#define FLAG_POLL                 0x02
#define FLAG_WAIT                 0x03      // special flag asking passive sound queue to wait for active sound queue to complete.
#define FLASH_CONFIG_OFFSET       0x1FF000  // offset in the Pico's 2 MB where to save data. Starting at 2.00MB - 4096 bytes (very end of flash).
#define H12                       FLAG_OFF  // 12-hours time format.
#define H24                       FLAG_ON   // 24-hours time format.
#define MAX_ACTIVE_SOUND_QUEUE    100       // maximum number of "sounds" in the active buzzer sound queue.
#define MAX_ALARMS                9         // total number of alarms available.
#define MAX_LIGHT_SLOTS           24        // number of slots for ambient light level hysteresis.
#define MAX_COMMAND_QUEUE         25        // maximim number of active commands in command queue.
#define MAX_CORE_QUEUE            25        // maximum number of active commands in each circular buffers for inter-core communication (core0-to-core1 and core1-to-core0).
#define MAX_COUNT_DOWN_ALARM_DURATION 30    // maximum period of time (in minutes) during which count-down alarm will ring if not reset by user (quick press on "Set" button).
#define MAX_DHT_READINGS          100       // maximum number of "logic level changes" while reading DHT22 data stream.
#define MAX_EVENTS                50        // maximum number of "calendar events" that can be programmed in the source code.
#define MAX_IR_READINGS           500       // maximum number of "logic level changes" while receiving data from IR remote control.
#define MAX_PASSIVE_SOUND_QUEUE   500       // maximum number of "sounds" in the passive buzzer sound queue.
#define MAX_REMINDERS1            50        // maximum number of "reminders" of type 1 that can be defined.
#define MAX_SCROLL_QUEUE          75        // maximum number of messages in the scroll buffer queue (big enough to cover MAX_EVENTS defined for the same day + a few extra date scrolls).
#define NIGHT_LIGHT_AUTO          0x03      // night light will turn On when ambient light is low enough
#define NIGHT_LIGHT_NIGHT         0x02      // night light On between NightLightTimeOn and NightLightTimeOff.
#define NIGHT_LIGHT_OFF           0x00      // night light always Off.
#define NIGHT_LIGHT_ON            0x01      // night light always On.
#define TIMER_COUNT_DOWN          0x01      // timer mode is "Count Down".
#define TIMER_COUNT_UP            0x02      // timer mode is "Count Up".
#define TIMER_OFF                 0x00      // timer is currently OFF.
#define TRUE                      0x01
#define TYPE_PICO                 0x01      // microcontroller is a Pico
#define TYPE_PICO_W               0x02      // microcontroller is a Pico W

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
#define JAN 1   // January
#define FEB 2   // February
#define MAR 3   // March
#define APR 4   // April
#define MAY 5   // May
#define JUN 6   // June
#define JUL 7   // July
#define AUG 8   // August
#define SEP 9   // September
#define OCT 10  // October
#define NOV 11  // November
#define DEC 12  // December


/* Language and locals used. */
#define LANGUAGE_LO_LIMIT 0x00
#define ENGLISH           0x01
#define FRENCH            0x02
#define GERMAN            0x03
#define CZECH             0x04
#define SPANISH           0x05
#define LANGUAGE_HI_LIMIT 0x06



/* List of commands to be processed by command queue handler (while in the "main()" thread context). */
#define COMMAND_PLAY_JINGLE       0x01


/* Inter-core commands / messages. */
/* For target: core 0. */
#define CORE0_DHT_ERROR           0x01
#define CORE0_DHT_READ_COMPLETED  0x02

/* For target: core 1. */
#define CORE1_READ_DHT            0x01


/* "Display modes" used with remote control while in "Generic display mode". */
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


/* DST_COUNTRY valid choices (see details in User Guide). */
// #define DST_DEBUG                    /// this define to be used only for intensive DST debugging purposes.
#define DST_LO_LIMIT        0           // this specific define only to make the logic easier in the code.
#define DST_NONE            0           // there is no "Daylight Saving Time" in user's country.
#define DST_AUSTRALIA       1           // daylight saving time for most of Australia.
#define DST_AUSTRALIA_HOWE  2           // daylight saving time for Australia - Lord Howe Island.
#define DST_CHILE           3           // daylight saving time for Chile.
#define DST_CUBA            4           // daylight saving time for Cuba.
#define DST_EUROPE          5           // daylight saving time for European Union.
#define DST_ISRAEL          6           // daylight saving time for Israel.
#define DST_LEBANON         7           // daylight saving time for Lebanon.
#define DST_MOLDOVA         8           // daylight saving time for Moldova.
#define DST_NEW_ZEALAND     9           // daylight saving time for New Zealand.
#define DST_NORTH_AMERICA  10           // daylight saving time for most of Canada and United States.
#define DST_PALESTINE      11           // daylight saving time for Palestine.
#define DST_PARAGUAY       12           // daylight saving time for Paraguay.IR_DISPLAY_GENERIC
#define DST_HI_LIMIT       13           // to make the logic easier in the firmware.


/* List or commands available with remote control. */
#ifdef  IR_SUPPORT
#define IR_LO_LIMIT                  0x00
#define IR_BUTTON_TOP_QUICK          0x01
#define IR_BUTTON_TOP_LONG           0x02
#define IR_BUTTON_MIDDLE_QUICK       0x03
#define IR_BUTTON_MIDDLE_LONG        0x04
#define IR_BUTTON_BOTTOM_QUICK       0x05
#define IR_BUTTON_BOTTOM_LONG        0x06
#define IR_DICE_ROLLING              0x07
#define IR_DISPLAY_AMBIENT_LIGHT     0x08
#define IR_DISPLAY_EVENTS_THIS_WEEK  0x09
#define IR_DISPLAY_EVENTS_TODAY      0x0A
#define IR_DISPLAY_GENERIC           0x0B
#define IR_DISPLAY_OUTSIDE_TEMP      0x0C
#define IR_DISPLAY_SECOND            0x0D
#define IR_FULL_TEST                 0x0E
#define IR_IDLE_TIME                 0x0F
#define IR_POWER_ON_OFF              0x10
#define IR_SILENCE_PERIOD            0x11
#define IR_HI_LIMIT                  0x12
#endif


/* Clock mode definitions. */
#define MODE_LO_LIMIT      0x00
#define MODE_UNDEFINED     0x00  // mode is currently undefined.
#define MODE_ALARM_SETUP   0x01  // user is setting up one or more alarms.
#define MODE_CLOCK_SETUP   0x02  // user is setting up clock parameters.
#define MODE_DISPLAY       0x03  // generic display mode used with IR remote control.
#define MODE_POWER_UP      0x04  // clock has just been powered-up.
#define MODE_SCROLLING     0x05  // clock is scrolling data on the display.
#define MODE_SHOW_TIME     0x06  // clock is displaying time ("normal" mode).
#define MODE_SHOW_VOLTAGE  0x07  // clock is displaying power supply voltage.
#define MODE_TEST          0x08  // clock is in test mode (to disable automatic clock behaviors on the display).
#define MODE_TIMER_SETUP   0x09  // user is setting up a timer.
#define MODE_HI_LIMIT      0x10


/* PWM - "Pulse Wide Modulation" is currently used to control sound on passive buzzer and clock display brightness. */
#define PWM_LO_LIMIT       0x00
#define PWM_SOUND          0x00
#define PWM_BRIGHTNESS     0x01
#define PWM_HI_LIMIT       0x02


/* Alarm setup steps definitions. */
#define SETUP_ALARM_LO_LIMIT  0x00
#define SETUP_ALARM_NUMBER    0x01
#define SETUP_ALARM_ON_OFF    0x02
#define SETUP_ALARM_HOUR      0x03
#define SETUP_ALARM_MINUTE    0x04
#define SETUP_ALARM_DAY       0x05
#define SETUP_ALARM_HI_LIMIT  0x06

/* NOTE: Clock setup step definitions are kept as variables and can be seen in the variables section below. */

/* Setup source definitions. */
#define SETUP_SOURCE_NONE     0x00
#define SETUP_SOURCE_ALARM    0x01
#define SETUP_SOURCE_CLOCK    0x02
#define SETUP_SOURCE_TIMER    0x03


/* Timer setup steps definitions. */
#define SETUP_TIMER_LO_LIMIT  0x00
#define SETUP_TIMER_UP_DOWN   0x01
#define SETUP_TIMER_MINUTE    0x02
#define SETUP_TIMER_SECOND    0x03
#define SETUP_TIMER_READY     0x04
#define SETUP_TIMER_HI_LIMIT  0x05


/* Tags that can be used in process_scroll() function. */
#define TAG_AMBIENT_LIGHT      0xFF   // tag used to scroll current ambient light information.
#define TAG_BME280_DEVICE_ID   0xFE   // tag used to scroll the BME280 device id (should be 0x60 for a "real" BME280).
#define TAG_BME280_TEMP        0xFD   // tag used to display temperature, relative humidity and atmospheric pressure read from an optional BME280 sensor.
#define TAG_WIFI_CREDENTIALS   0xFC
#define TAG_DATE               0xFB   // tag used to scroll current date, temperature and power supply voltage.
#define TAG_DEBUG              0xFA   // tag used to scroll variables for debug purposes, while in main() context.
#define TAG_DHT22_TEMP         0xF9   // tag used to display temperature read from an optional DHT22 temperature and humidity sensor.
#define TAG_DS3231_TEMP        0xF8   // tag used to display ambient temperature read from DS3231 real-time IC in the Green Clock.
#define TAG_DST                0xF7   // tag used to scroll daylight saving time ("DST") information and status on clock display.
#define TAG_FIRMWARE_VERSION   0xF6   // tag used to display Pico Green Clock firmware version.
#define TAG_IDLE_MONITOR       0xF5   // tag used to scroll current System Idle Monitor on clock display.
#define TAG_INFO               0xF4   // tag used to display information while in "main()" context.
#define TAG_NTP_ERRORS         0xF3   // tag used to scroll cumulative number of errors in NTP requests.
#define TAG_NTP_STATUS         0xF2   // tag used to scroll cumulative number of errors in NTP requests.
#define TAG_PICO_TEMP          0xF1   // tag used to display Pico internal temperature.
#define TAG_PICO_TYPE          0xF0   // tag used to display the type of microcontroller installed (Pico or Pico W).
#define TAG_PICO_UNIQUE_ID     0xEF   // tag used to display Pico (flash) unique ID.
#define TAG_QUEUE              0xEE   // tag used to display "Head", "Tail", and "Tag" of currently used scroll queue (for debugging purposes).
#define TAG_TIMEZONE           0xED   // tag used to display Universal Coordinated Time information.
#define TAG_VOLTAGE            0xEC   // tag used to display power supply voltage.


#define SILENT        0
#define WAIT_4_ACTIVE 0xFFFF  // request passive sound queue to wait for active sound queue to complete.


/* Music tones definitions. */
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



/* ------------------------------------------------------------------ *\
                   Data Structure  definitions
\* ------------------------------------------------------------------ */
/* Calendar events definition. */
struct event
{
  UINT64 Day;
  UINT8  Month;
  UINT16 Jingle;
  UCHAR  Description[51];
};


/* Alarm definitions. */
struct alarm
{
  UINT8 FlagStatus;
  UINT8 Second;
  UINT8 Minute;
  UINT8 Hour;
  UINT8 Day;
  UCHAR Text[40];
};


/* Command definitions for command queue. */
struct command
{
  UINT16 Command;
  UINT16 Parameter;
};


/* Summer Time / Winter Time parameters definitions. */
struct dst_parameters
{
  UINT8  StartMonth;
  UINT8  StartDayOfWeek;
  int8_t StartDayOfMonthLow;
  int8_t StartDayOfMonthHigh;
  UINT8  StartHour;
  UINT16 StartDayOfYear;
  UINT8  EndMonth;
  UINT8  EndDayOfWeek;
  int8_t EndDayOfMonthLow;
  int8_t EndDayOfMonthHigh;
  UINT8  EndHour;
  UINT16 EndDayOfYear;
  UINT8  ShiftMinutes;
};


/* Structure to contain time stamp under "human" format instead of "tm" standard. */
struct human_time
{
  UINT16 Hour;
  UINT16 Minute;
  UINT16 Second;
  UINT16 DayOfMonth;
  UINT16 Month;
  UINT16 Year;
  UINT16 DayOfWeek;
  UINT16 DayOfYear;
  UINT8  FlagDst;
};


/* NTP data structure. */
struct ntp_data
{
  /* Time-related data. */
  UINT8  CurrentDayOfWeek;
  UINT8  CurrentDayOfMonth;
  UINT8  CurrentMonth;
  UINT16 CurrentYear;
  UINT8  CurrentYearLowPart;
  UINT8  CurrentHour;
  UINT8  CurrentMinute;
  UINT8  CurrentSecond;

  /* Generic data. */
  time_t Epoch;
  UINT8  FlagNTPResync;   // flag set to On if there is a specific reason to request an NTP update without delay.
  UINT8  FlagNTPSuccess;  // flag indicating that NTP date and time request has succeeded.
  UINT64 NTPDelta;
  UINT32 NTPErrors;       // cumulative number of errors while trying to re-sync with NTP.
  UINT64 NTPGetTime;
  UINT64 NTPLastUpdate;
  UINT32 NTPReadCycles;   // total number of re-sync cycles through NTP.
};


/* Clock display pixel definitions. */
struct pixel
{
  UINT8 DisplayBuffer;
  UINT8 BitNumber;
};


/* Pulse Wide Modulation (PWM) parameters definitions. */
struct pwm
{
  UINT8  Channel;
  UINT32 Clock;
  float  ClockDivider;
  UINT16 DutyCycle;
  UINT32 Frequency;
  UINT8  Gpio;
  UINT16 Level;
  UINT8  OnOff;
  UINT8  Slice;
  UINT16 Wrap;
};


/* Type 1 Reminders. */
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


/* Active buzzer sound parameters definitions. */
struct sound_active
{
  UINT16 MSec;
  UINT16 RepeatCount;
};


/* Passive buzzer sound parameters definitions. */
struct sound_passive
{
  UINT16 Freq;
  UINT16 MSec;
};


/* Structure containing the Green Clock configuration being saved to flash memory.
   Those variables will be restored after a reboot and / or power failure. */
/* IMPORTANT: Version must always be the first element of the structure and
              CRC16   must always be the  last element of the structure.
              Size is 675 bytes long */
struct flash_config
{
  UCHAR  Version[6];          // firmware version number (format: "06.00" - including end-of-string).
  UINT8  CurrentYearCentile;  // assume we are in years 20xx on power-up but is adjusted when configuration is read (will your clock live long enough for a "21" ?!).
  UINT8  Language;            // language used for data display (including date scrolling).
  UCHAR  DSTCountry;  // specifies how to handle the daylight saving time (see User Guide and / or clock options above).
  UINT8  TemperatureUnit;     // CELSIUS or FAHRENHEIT default value (see clock options above).
  UINT8  TimeDisplayMode;     // H24 or H12 default value (see clock options above).
  UINT8  ChimeMode;           // chime mode (Off / On / Day).
  UINT8  ChimeTimeOn;         // hourly chime will begin at this hour.
  UINT8  ChimeTimeOff;        // hourly chime will stop after this hour.
  UINT8  NightLightMode;      // night light mode (On / Off / Auto / Night).
  UINT8  NightLightTimeOn;    // default night light time On.
  UINT8  NightLightTimeOff;   // default night light time Off.
  UINT8  FlagAutoBrightness;  // flag indicating we are in "Auto Brightness" mode.
  UINT8  FlagKeyclick;        // flag for keyclick ("button-press" tone)
  UINT8  FlagScrollEnable;    // flag indicating the clock will scroll the date and temperature at regular intervals on the display.
  UINT8  FlagSummerTime;      // flag indicating the current status (On or Off) of Daylight Saving Time / Summer Time.
  int8_t Timezone;            // (in hours) value to add to UTC time (Universal Time Coordinate) to get the local time.
  UINT16 DimmerMinLightLevel; // Value of the average light level to be used for the minimum display dim level, range 0 to 1500
  UINT8  ShortSetKey;         // flag indicating the set is a short key press and alarm is long or vice versa.
  UINT8  Reserved1[45];       // reserved for future use.
  struct alarm Alarm[9];      // alarms 0 to 8 parameters (numbered 1 to 9 for clock users). Day is a bit mask.
  UCHAR  Hostname[40];        // Hostname for Wi-Fi network. Note: Hostname begins at position 5 of the variable string, so that a "footprint" can be confirmed prior to writing to flash.
  UCHAR  SSID[40];            // SSID for Wi-Fi network. Note: SSID begins at position 5 of the variable string, so that a "footprint" can be confirmed prior to writing to flash.
  UCHAR  Password[70];        // password for Wi-Fi network. Note: password begins at position 5 of the variable string, for the same reason as SSID above.
  UCHAR  Reserved2[48];       // reserved for future use.
  UINT16 Crc16;               // crc16 of all data above to validate configuration.
};

/* Web page fetch routine structures to read multiple data values at the same time
   to keep the data consistent. */
struct web_light_value
{
  UINT16 adc_current_value;   // Current ADC value
  UINT16 AverageLightLevel;   // Average ADC value across the 20 measurement slots
  UINT16 MinLightLevel;       // Configured minimum average ADC value for dimmest display level
  UINT16 DutyCycle;           // Current dimmer pwm duty cycle - value 0 to 1000 for 0 - 100.0%
  UINT16 Max_adc_value;       // Cumulative max value
  UINT16 Min_adc_value;       // Cumulative min value
};

/* ------------------------------------------------------------------ *\
                         Function prototypes
\* ------------------------------------------------------------------ */
/* Read ambient light level from Pico's analog-to-digital gpio. */
UINT16 adc_read_light(void);

/* Read Pico's internal temperature from Pico's analog-to-digital gpio. */
void adc_read_pico_temp(float *DegreeC, float *DegreeF);

/* Read power supply voltage from Pico's analog-to-digital gpio. */
float adc_read_voltage(void);

/* If auto-brightness is On, adjust clock brightness according to average ambient light level. */
void adjust_clock_brightness(void);

/* Clear all the leds on clock display. */
void clear_all_leds(void);

/* Clear the display framebuffer. */
void clear_framebuffer(UINT8 StartColumn);

/* Convert a 24-hour format time to 12-hour format. */
UINT8 convert_h24_to_h12(UINT8 Hour, UINT8 *AmFlag, UINT8 *PmFlag);

/* Convert "TmTime" to "Unix Time". */
UINT64 convert_tm_to_unix(struct tm *TmTime);

/* Convert "Unix Time" (Epoch) to "tm Time". Both Unix Time and TmTime are relative to UCT.  */
void convert_unix_to_tm(time_t UnixTime, struct tm *TmTime, UINT8 FlagLocalTime);

/* Queue a command to process while in main thread context. */
UINT8 command_queue(UINT8 Command, UINT16 Parameter);

/* Unqueue a command while in main thread context. */
UINT8 command_unqueue(UINT8 *Command, UINT16 *Parameter);

/* Thread to run on the second Pico's core. */
void core1_main(void);

/* Queue a command for the other core. */
UINT8 core_queue(UINT8 CoreNumber, UINT8 Command);

/* Unqueue a command received from the other core. */
UINT8 core_unqueue(UINT8 CoreNumber);

/* Generate a date stamp for debug info. */
void date_stamp(UCHAR *String);

/* Display data sent in argument through serial port. */
void display_data(UCHAR *Data, UINT32 Size);

/* Evaluate if it is time to blink data on the display (while in setup mode). */
void evaluate_blinking_time(void);

/* Evaluate if it is time to scroll characters ("one dot left") on clock display. */
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

/* Erase data in Pico flash memory. */
void flash_erase(UINT32 FlashMemoryOffset);

/* Read Green Clock configuration from flash memory. */
UINT8 flash_read_config(void);

/* Save current Green Clock configuration to flash memory. */
UINT8 flash_save_config(void);

/* Write data to Pico flash memory. */
UINT flash_write(UINT32 FlashMemoryOffset, UINT8 NewData[], UINT16 NewDataSize);

/* Format string to display temperature on clock display. */
void format_temp(UCHAR *TempString, UCHAR *PreString, float TempCelsius, float Humidity, float Pressure);

/* Get temperature from DS3231. */
float get_ambient_temperature(UINT8 TemperatureUnit);

/* Build the string containing the date to scroll on the clock display. */
void get_date_string(UCHAR *String);

/* Determine the day-of-year of date given in argument. */
UINT16 get_day_of_year(UINT16 YearNumber, UINT8 MonthNumber, UINT8 DayNumber);

/* Determine the day-of-year for DST start day and DST end day. */
void get_dst_days(void);

/* Determine if the microcontroller is a Pico or a Pico W. */
UINT8 get_microcontroller_type(void);

/* Return the number of days in a specific month (while checking if it is a leap year or not for February). */
UINT8 get_month_days(UINT16 CurrentYear, UINT8 MonthNumber);

/* Retrieve Pico's Unique ID from flash IC. */
void get_pico_unique_id(void);

/* Return the day-of-week, given the day-of-month, month and year. */
UINT8 get_day_of_week(UINT16 year_cnt, UINT8 month_cnt, UINT8 date_cnt);

/* Initialize all required GPIO ports of the Raspberry Pi Pico. */
int init_gpio(void);

/* Read a string from stdin. */
void input_string(UCHAR *String);

/* Interrupt handler for signal received from IR sensor. */
gpio_irq_callback_t isr_signal_trap(UINT8 gpio, UINT32 Events);

/* Test clock LED matrix, column-by-column, and also all display indicators. */
void matrix_test(UINT8 TestNumber);

/* Make pixel animation for the specified number of seconds. */
void pixel_twinkling(UINT16 Seconds);

/* Play a specific jingle. */
void play_jingle(UINT16 JingleNumber);

/* Handle the command that has been put in command queue. */
void process_command_queue(void);

/* Handle the tag that has been put in the scroll queue. */
void process_scroll_queue(void);

/* Initialize GPIOs to be used as PWM source for clock display brightness and passive buzzer. */
void pwm_initialize(void);

/* Turn On or Off the PWM signal specified in argument. */
void pwm_on_off(UINT8 PwmNumber, UINT8 FlagSwitch);

/* Set the active duty cycle for the PWM used for clock display brightness. */
void pwm_set_duty_cycle(UINT16 DutyCycle);

/* Set the frequency for the PWM used for passive buzzer. */
void pwm_set_frequency(UINT16 Frequency);

/* Reverse the bit order of the byte given in argument. */
UINT8 reverse_bits(UINT8 InputByte);

/* Scroll the virtual framebuffer one dot to the left. */
void scroll_one_dot(void);

/* Queue the given tag in the scroll queue. */
UINT8 scroll_queue(UINT8 Tag);

/* Put a specific value in the queue so that we can check for a specific event even inside an ISR.
   MUST BE USED ONLY FOR DEBUG PURPOSES SINCE IT INTERFERES WITH (overwrites) CALENDAR EVENTS. */
UINT8 scroll_queue_value(UINT8 Tag, UCHAR *String);

/* Scroll the specified string on the display. */
void scroll_string(UINT8 StartColumn, UCHAR *String);

/* Unqueue next tag from the scroll queue. */
UINT8 scroll_unqueue(void);

/* Send data to the matrix controller IC. */
void send_data(UINT8 data);

/* Request Wi-Fi SSID and password from user and save them to Pico's flash. */
void set_and_save_credentials(void);

/* Exit current setup mode step. */
void set_mode_out(void);

/* Exit setup mode because of inactivity (time-out). */
void set_mode_timeout(void);

/* Turn On or Off the specified pixel. */
void set_pixel(UINT8 Row, UINT8 Column, UINT8 Flag);

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

/* Queue the given sound in the active buzzer sound queue. */
UINT16 sound_queue_active(UINT16 MSeconds, UINT16 RepeatCount);

/* Queue the given sound in the passive buzzer sound queue. */
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

/* Make a tone for the specified number of milliseconds. */
void tone(UINT16 MilliSeconds);

/* Return total number of "one" bits in the given integer. */
uint8_t total_one_bits(UINT32 Data, UINT8 Size);

/* Send a string to an external terminal emulator program through Pico's UART (or serial port). */
void uart_send(UINT LineNumber, UCHAR *Format, ...);

/* Return the string representing the uint64_t parameter in binary. */
void uint64_to_binary_string(UINT64 Value, UINT8 StringLength, UCHAR *BinaryString);

/* Set DST parameters (FlagSummerTime and Timezone) according to current DST country code and current date and time. */
void update_dst_status();

/* Update indicators at the left of the clock display. */
void update_left_indicators(void);

/* Turn ON the right DayOfWeek indicator on clock display and turn Off all others. */
void update_top_indicators(UINT8 DayOfWeek, UINT8 Flag);





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
#ifdef DHT_SUPPORT
/* Read the temperature from external DHT22 (this is done by Pico's core 1). */
UINT8 read_dht(float *Temperature, float *Humidity);
#endif  // DHT_SUPPORT



/* ------------------------------------------------------------------ *\
                 IR sensor-related function prototypes
\* ------------------------------------------------------------------ */
#ifdef IR_SUPPORT
/* Decode IR remote control command. */
UINT8 decode_ir_command(UCHAR FlagDebug, UINT8 *IrCommand);

/* Execute the command received from IR remote control. */
void process_ir_command(UINT8 IrCommand);
#endif  // IR_SUPPORT

UCHAR* wfetch_hostname(void);
UCHAR* wfetch_wifissid(void);
UCHAR* wfetch_wifipass(void);
void wwrite_networkcfg(UCHAR * new_hostname, UCHAR * new_wifissid, UCHAR * new_wifipass);

struct human_time wfetch_current_datetime(void);

UINT8 wfetch_current_language(void);

UCHAR* wfetch_DayName(UINT8 the_language, UINT16 the_dayofweek);

UINT16 wfetch_current_dayofmonth(void);

UCHAR* wfetch_MonthName(UINT8 the_language, UINT16 the_month);

void wwrite_day_of_month(UINT8 NewDayOfMonth);

struct alarm wfetch_alarm(UINT8 alarm_to_fetch);

void wwrite_alarm(UINT8 alarm_to_write, struct alarm alarm_data);

struct web_light_value wfetch_light_adc_level(void);

UINT8 fetch_AutoBrightness(void);

void wwrite_dimminlightlevel(UINT16 new_lightlevel);

