/* ======================================================================== *\
   define.h
   St-Louys Andre - February 2022
   astlouys@gmail.com
   Revision 31-MAR-2022
   Langage: Linux gcc
   Version 1.01

   Raspberry Pi Pico firmware to drive the Waveshare green clock
   From an original software version by "Yufu" on 23-JAN-2021 and
   given with the Waveshare clock by 2020 Raspberry Pi (Trading) Ltd.

   REVISION HISTORY:
   =================
   07-FEB-2022 1.00 - Initial release
   02-MAR-2022 1.01 - Code reformatting.
\* ======================================================================== */



/* ======================================================================== *\
   NOTE:
\* ======================================================================== */



/* $TITLE=Definitions and include files. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                    Definitions and include files.
List of GPIOs used by the Green Clock:

GPIO  2 - (In) Top button
GPIO  3 - (In) SQW (DS3231 RTC IC)
GPIO  4 - Not used
GPIO  5 - Not used
GPIO  6 - (I2C) SDA (Temperature reading)
GPIO  7 - (I2C) SCL (Temperature reading)
GPIO  8 - DHT22 (added... not on the original Green Clock)
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
GPIO 19 - Not used (Reserved)
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
#ifndef DEFINE_H
#define DEFINE_H



#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"



/* GPIO assignations. */
#define SET_BUTTON   2  /* top clock button. */
#define SQW          3
#define SDA          6
#define SCL          7
#define DHT22        8
#define CLK         10
#define SDI         11
#define LE          12
#define OE          13
#define BUZZ        14  /* piezo has its own integrated oscillator. */
#define DOWN_BUTTON 15  /* bottom clock button. */
#define A0          16
#define UP_BUTTON   17  /* middle clock button. */
#define A1          18
#define A2          22

#define ADC_Light   26  /* analog-to-digital converter of the Pico: read ambient light. */
#define ADC_VCC     29  /* analog-to-digital converter of the Pico: read power supply voltage. */



/* GPIO definitions */
#define A0_LOW   gpio_put(A0, 0)
#define A0_HIGH  gpio_put(A0, 1)

#define A1_LOW   gpio_put(A1, 0)
#define A1_HIGH  gpio_put(A1, 1)

#define A2_LOW   gpio_put(A2, 0)
#define A2_HIGH  gpio_put(A2, 1)

#define CLK_LOW  gpio_put(CLK, 0)
#define CLK_HIGH gpio_put(CLK, 1)

#define LE_LOW   gpio_put(LE, 0)
#define LE_HIGH  gpio_put(LE, 1)

#define OE_OPEN  gpio_put(OE, 0)
#define OE_CLOSE gpio_put(OE, 1)

#define SDI_LOW  gpio_put(SDI, 0)
#define SDI_HIGH gpio_put(SDI, 1)

#define FLAG_UP      0xFF  // "Up"   (middle) button has been pressed.
#define FLAG_DOWN    0x00  // "Down" (bottom) button has been pressed.

/* Inter integrated circuit (I2C) protocol definitions. */
#define I2C_PORT i2c1
#define Address     0x68
#define Address_ADS 0x48



/* First two bits of the display matrix are reserved for indicators. */
#define DisplayOffset  2



/* Real-time clock data structure. */
typedef struct
{
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hour;
  uint8_t dayofweek;
  uint8_t dayofmonth;
  uint8_t month;
  uint8_t year;
} TIME_RTC;



typedef enum
{
  ALARM_MODE_ALL_MATCHED = 0,
  ALARM_MODE_HOUR_MIN_SEC_MATCHED,
  ALARM_MODE_MIN_SEC_MATCHED,
  ALARM_MODE_SEC_MATCHED,
  ALARM_MODE_ONCE_PER_SECOND
} AlarmMode;



/* Toggle ON or OFF both LEDs in each "day of week" indicator */
#define IndicatorMondayOn      {DisplayBuffer[0]  |=   (1 << 3) | (1 << 4);}    // turn ON  both LEDs of Monday indicator
#define IndicatorMondayOff     {DisplayBuffer[0]  &= ~((1 << 3) | (1 << 4));}   // turn OFF both LEDs of Monday indicator 
#define IndicatorTuesdayOn     {DisplayBuffer[0]  |=   (1 << 6) | (1 << 7);}    // turn ON  both LEDs of Tuesday indicator
#define IndicatorTuesdayOff    {DisplayBuffer[0]  &= ~((1 << 6) | (1 << 7));}   // turn OFF both LEDs of Tuesday indicator
#define IndicatorWednesdayOn   {DisplayBuffer[8]  |=   (1 << 1) | (1 << 2);}    // turn ON  both LEDs of Wednesday indicator
#define IndicatorWednesdayOff  {DisplayBuffer[8]  &= ~((1 << 1) | (1 << 2));}   // turn OFF both LEDs of Wednesday indicator
#define IndicatorThursdayOn    {DisplayBuffer[8]  |=   (1 << 4) | (1 << 5);}    // turn ON  both LEDs of Thursday indicator
#define IndicatorThursdayOff   {DisplayBuffer[8]  &= ~((1 << 4) | (1 << 5));}   // turn OFF both LEDs of Thursday indicator
#define IndicatorFridayOn      {DisplayBuffer[8]  |=   (1 << 7); DisplayBuffer[16] |=  (1 << 0);}  // turn  ON both LEDs of Friday indicator
#define IndicatorFridayOff     {DisplayBuffer[8]  &=  ~(1 << 7); DisplayBuffer[16] &= ~(1 << 0);}  // turn OFF both LEDs of Friday indicator
#define IndicatorSaturdayOn    {DisplayBuffer[16] |=   (1 << 2) | (1 << 3);}    // turn ON  both LEDs of Saturday indicator
#define IndicatorSaturdayOff   {DisplayBuffer[16] &= ~((1 << 2) | (1 << 3));}   // turn OFF both LEDs of Saturday indicator
#define IndicatorSundayOn      {DisplayBuffer[16] |=   (1 << 5) | (1 << 6);}    // turn ON  both LEDs of Sunday indicator
#define IndicatorSundayOff     {DisplayBuffer[16] &= ~((1 << 5) | (1 << 6));}   // turn OFF both LEDs of Sunday indicator



/* Turn ON or OFF indicator LEDs on the display (some have only one LED, some have two LEDs). */
#define IndicatorScrollOn         DisplayBuffer[0] |=  0x03      /* Two LEDs "Move On" */
#define IndicatorScrollOff        DisplayBuffer[0] &= ~0x03
#define IndicatorAlarmOn          DisplayBuffer[1] |=  0x03      /* Two LEDs "Alarm" */
#define IndicatorAlarm0On         DisplayBuffer[1] |=  0x01      /* Left alarm LED */
#define IndicatorAlarm1On         DisplayBuffer[1] |=  0x02      /* Right alarm LED */
#define IndicatorAlarmOff         DisplayBuffer[1] &= ~0x03
#define IndicatorAlarm0Off        DisplayBuffer[1] &= ~0x01
#define IndicatorAlarm1Off        DisplayBuffer[1] &= ~0x02
#define IndicatorCountDownOn      DisplayBuffer[2] |=  0x03      /* Two LEDs "CountDown" timer*/
#define IndicatorCountDownOff     DisplayBuffer[2] &= ~0x03
#define IndicatorFrnhtOn          DisplayBuffer[3] |=  (1 << 0)  /* One LED "Farenheit" */
#define IndicatorFrnhtOff         DisplayBuffer[3] &= ~(1 << 0)
#define IndicatorCelsiusOn        DisplayBuffer[3] |=  (1 << 1)  /* One LED "Celsius" */
#define IndicatorCelsiusOff       DisplayBuffer[3] &= ~(1 << 1)
#define IndicatorAmOn             DisplayBuffer[4] |=  (1 << 0)  /* One LED "AM" */
#define IndicatorAmOff            DisplayBuffer[4] &= ~(1 << 0)
#define IndicatorPmOn             DisplayBuffer[4] |=  (1 << 1)  /* One LED "PM" */
#define IndicatorPmOff            DisplayBuffer[4] &= ~(1 << 1)
#define IndicatorCountUpOn        DisplayBuffer[5] |=  0x03      /* Two LEDs "CountUp" timer */
#define IndicatorCountUpOff       DisplayBuffer[5] &= ~0x03
#define IndicatorHourlyChimeOn    DisplayBuffer[6] |=  0x03      /* Two LEDs "Hourly chime" */
#define IndicatorHourlyChimeDay  {DisplayBuffer[6] &= ~0x03; DisplayBuffer[6] |= 0x01;}  /* Only one LED "Hourly chime" to indicate "day chime" */
#define IndicatorHourlyChimeOff   DisplayBuffer[6] &= ~0x03
#define IndicatorAutoLightOn      DisplayBuffer[7] |=  0x03      /* Two LEDs "Auto Brightness" */
#define IndicatorAutoLightOff     DisplayBuffer[7] &= ~0x03
#define IndicatorButtonLightsOn   DisplayBuffer[0] |=   (1 << 2)|(1 << 5)   /* Two white LEDs inside the clock, near the buttons (undocumented feature of the clock) */
#define IndicatorButtonLightsOff  DisplayBuffer[0] &= ~((1 << 2)|(1 << 5))

#endif
