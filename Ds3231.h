/* ======================================================================== *\
   Ds3231.h
   St-Louys Andre - February 2022
   astlouys@gmail.com
   Revision 31-MAR-2022
   Langage: Linux gcc
   Version 1.00

   Raspberry Pi Pico firmware to drive the Waveshare green clock
   From an original software version by "Yufu" on 25-JAN-2021 and
   given with the Waveshare clock by 2020 Raspberry Pi (Trading) Ltd.

   REVISION HISTORY:
   =================
   07-FEB-2022 1.00 - Initial release
\* ======================================================================== */



/* ======================================================================== *\
   NOTE:
\* ======================================================================== */



/* $TITLE=Definitions and include files. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                    Definitions and include files.
\* ----------------------------------------------------------------- */
#ifndef PICO_EXAMPLES_DS3231_H
#define PICO_EXAMPLES_DS3231_H



#include "stdio.h"
#include "hardware/i2c.h"
#include "define.h"
#include "string.h"



#define DS3231_REG_SECOND   0x00
#define DS3231_REG_MINUTE   0x01
#define DS3231_REG_HOUR     0x02
#define DS3231_REG_DAY      0x03
#define DS3231_REG_DATE     0x04
#define DS3231_REG_MONTH    0x05
#define DS3231_REG_YEAR     0x06
#define DS3231_REG_A1S      0x07
#define DS3231_REG_A1M      0x08
#define DS3231_REG_A1H      0x09
#define DS3231_REG_A1D      0x0A
#define DS3231_REG_A2M      0x0B
#define DS3231_REG_A2H      0x0C
#define DS3231_REG_A2D      0x0D
#define DS3231_REG_CONTROL  0X0E
#define DS3231_REG_STATUS   0x0F
#define DS3231_REG_HTEMP    0x11
#define DS3231_REG_LTEMP    0x12
#define DS3231_STA_A1F      0x01
#define DS3231_STA_A2F      0x02
#define Control_default     0x20



#define Status_default      0x00  // 32.768K



TIME_RTC Read_RTC();



extern bool HourMode;

extern char Meridiem[2][3];
extern char StateOfTime[3];

extern uint8_t ByteData[16];
extern uint8_t Ds3231ReadRegister[17];



/* $TITLE=Function declarations. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                       Function declarations
\* ----------------------------------------------------------------- */
/* Convert the binary coded decimal value to integer value. */
uint8_t bcd_to_byte(uint8_t BcdValue);

/* Read registers from RTC IC and convert them to integer value. */
void byte_data(void);

/* Convert the integer value to binary-coded-decimal. */
uint8_t dec_to_bcd(int DecValue);

/* Check status of alarm 0 from the RTC IC. */
bool ds3231_check_alarm_0();

/* Check status of alarm 1 from the RTC IC. */
bool ds3231_check_alarm_1();

/* Read the main registers of the real-time clock IC. */
void ds3231_register_read();

/// void ds3231_set_alarm_s(uint8_t Hour, uint8_t Minute, uint8_t Second);

void ds3231_sqw_enable(bool Enable);

/* Not used for now. 24-hours format is always used with RTC IC
   and Firmware takes care of the 12-hours conversion if required. */
/// void format_time_mode();

/* Not used for now. */
/// void init_ds3231();

/* Set alarm #1 in the RTC IC. */
void set_alarm1_clock(uint8_t Mode, uint8_t Second, uint8_t Minute, uint8_t hour, uint8_t Data);

/* Set alarm #2 in the RTC IC. */
void set_alarm2_clock(uint8_t Minute, uint8_t Hour, uint8_t Date);

/* Not used for now. */
/// void set_clock_mode(bool h12);

/* Initialize the day-of-month in the RTC IC. */
void set_day_of_month(uint8_t DayOfMonth);

/* Initialize the day-of-week in the RTC IC. */
void set_day_of_week(uint8_t DayOfWeek);

/* Initialize the hour in the RTC IC. */
void set_hour(uint8_t Hour);

/* Initialize the minute in the RTC IC. */
void set_minute(uint8_t Minute);

/* Initialize the month in the RTC IC. */
void set_month(uint8_t Month);

/* Initialize all time-related data in the RTC IC. */
void set_time(uint8_t Second, uint8_t Minute, uint8_t Hour, uint8_t DayOfWeek, uint8_t DayOfMonth, uint8_t Month, uint8_t Year);

/* Initialize the year in the RTC IC. */
void set_year(uint8_t Year);

/* Not used for now. */
/// void Show_Time();

#endif //PICO_EXAMPLES_DS3231_H
