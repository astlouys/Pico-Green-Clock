/* ======================================================================== *\
   define.h
   St-Louys Andre - February 2022
   astlouys@gmail.com
   Revision 23-DEC-2022
   Langage: GCC 7.3.1 arm-none-eabi
   Version 2.00

   Raspberry Pi Pico firmware to drive the Waveshare Pico-Green-Clock
   From an original software version by "Yufu" on 23-JAN-2021 and
   given with the Waveshare's Green Clock by 2020 Raspberry Pi (Trading) Ltd.

   REVISION HISTORY:
   =================
   07-FEB-2022 1.00 - Initial release.
   02-MAR-2022 1.01 - Code reformatting.
   23-DEC-2022 2.00 - General code cleanup.
\* ======================================================================== */



/* ======================================================================== *\
   NOTE:
\* ======================================================================== */



/* $TITLE=Definitions and include files. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                     Definitions and include files.
\* ----------------------------------------------------------------- */
#ifndef DEFINE_H
#define DEFINE_H



#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"



/* GPIO definitions. */
#define UART_TX_PIN  0  // optional serial line to transmit data to an external VT101-type monitor.
#define UART_RX_PIN  1  // optional serial line to receive data from an external VT101-type monitor.
#define SET_BUTTON   2  // "Set" top clock button. */
#define SQW          3  // DS3231 square wave (not used).
#define SDA          6  // I2C data line for DS3231 real time clock and optional BME280 temperature, humidity and barometric sensor.
#define SCL          7  // I2C clock line for DS3231 real time clock and optional BME280 temperature, humidity and barometric sensor.
#define DHT22        8  // outside temperature & humidity sensor (must be added by user). Only one of DHT22 or BME280 must be defined.
#define IR_RX        9  // infrared sensor for remote control rx (must be added by user). */
#define CLK         10  // clock for LED matrix controler IC (SM1606SC)
#define SDI         11
#define LE          12
#define OE          13
#define BUZZ        14  // active piezo (integrated on Green clock PC board), and has its own integrated oscillator). */
#define DOWN_BUTTON 15  // "Down" bottom clock button. */
#define A0          16
#define UP_BUTTON   17  // "Up" middle clock button. */
#define A1          18
#define PPIEZO      19  // passive piezo / buzzer (must be added by user). */
#define A2          22
#define PICO_LED    25  // Pico's integrated LED
#define ADC_LIGHT   26  // analog-to-digital converter of the Pico: read ambient light. */
#define ADC_VCC     29  // analog-to-digital converter of the Pico: read power supply voltage. */



/* GPIO macro definitions */
#define A0_LOW     gpio_put(A0, 0)
#define A0_HIGH    gpio_put(A0, 1)

#define A1_LOW     gpio_put(A1, 0)
#define A1_HIGH    gpio_put(A1, 1)

#define A2_LOW     gpio_put(A2, 0)
#define A2_HIGH    gpio_put(A2, 1)

#define CLK_LOW    gpio_put(CLK, 0)
#define CLK_HIGH   gpio_put(CLK, 1)

#define LE_LOW     gpio_put(LE, 0)
#define LE_HIGH    gpio_put(LE, 1)

/// #define OE_ENABLE  gpio_put(OE, 0)
/// #define OE_DISABLE gpio_put(OE, 1)

#define SDI_LOW    gpio_put(SDI, 0)
#define SDI_HIGH   gpio_put(SDI, 1)

#define FLAG_UP         0xFF  // "Up"   (middle) button has been pressed.
#define FLAG_LONG_UP    0x55  // long press on "Up" (middle) button
#define FLAG_DOWN       0x00  // "Down" (bottom) button has been pressed.
#define FLAG_LONG_DOWN  0xAA  // long press on "Down" (bottom) button

/* Inter integrated circuit (I2C) protocol definitions. */
#define I2C_PORT i2c1
#define DS3231_ADDRESS  0x68
//   #define Address_ADS 0x48


/* Device address for the specific BME280 breakout used (from Waveshare) and BME280 internal register addresses. */
#ifdef BME280_SUPPORT
  #define BME280_ADDRESS 0x77

  #define BME280_REGISTER_UNIQUE_ID  0x83;

  #define BME280_REGISTER_CALIB00    0x88
  #define BME280_REGISTER_CALIB01    0x89
  #define BME280_REGISTER_CALIB02    0x8A
  #define BME280_REGISTER_CALIB03    0x8B
  #define BME280_REGISTER_CALIB04    0x8C
  #define BME280_REGISTER_CALIB05    0x8D
  #define BME280_REGISTER_CALIB06    0x8E
  #define BME280_REGISTER_CALIB07    0x8F
  #define BME280_REGISTER_CALIB08    0x90
  #define BME280_REGISTER_CALIB09    0x91
  #define BME280_REGISTER_CALIB10    0x92
  #define BME280_REGISTER_CALIB11    0x93
  #define BME280_REGISTER_CALIB12    0x94
  #define BME280_REGISTER_CALIB13    0x95
  #define BME280_REGISTER_CALIB14    0x96
  #define BME280_REGISTER_CALIB15    0x97
  #define BME280_REGISTER_CALIB16    0x98
  #define BME280_REGISTER_CALIB17    0x99
  #define BME280_REGISTER_CALIB18    0x9A
  #define BME280_REGISTER_CALIB19    0x9B
  #define BME280_REGISTER_CALIB20    0x9C
  #define BME280_REGISTER_CALIB21    0x9D
  #define BME280_REGISTER_CALIB22    0x9E
  #define BME280_REGISTER_CALIB23    0x9F
  #define BME280_REGISTER_CALIB24    0xA0
  #define BME280_REGISTER_CALIB25    0xA1
  
  #define BME280_REGISTER_ID         0xD0
  #define BME280_REGISTER_RESET      0xE0
   
  #define BME280_REGISTER_CALIB26    0xE1
  #define BME280_REGISTER_CALIB27    0xE2
  #define BME280_REGISTER_CALIB28    0xE3
  #define BME280_REGISTER_CALIB29    0xE4
  #define BME280_REGISTER_CALIB30    0xE5
  #define BME280_REGISTER_CALIB31    0xE6
  #define BME280_REGISTER_CALIB32    0xE7
  #define BME280_REGISTER_CALIB33    0xE8
  #define BME280_REGISTER_CALIB34    0xE9
  #define BME280_REGISTER_CALIB35    0xEA
  #define BME280_REGISTER_CALIB36    0xEB
  #define BME280_REGISTER_CALIB37    0xEC
  #define BME280_REGISTER_CALIB38    0xED
  #define BME280_REGISTER_CALIB39    0xEE
  #define BME280_REGISTER_CALIB40    0xEF
  #define BME280_REGISTER_CALIB41    0xF0

  #define BME280_REGISTER_CTRL_HUM   0xF2
  #define BME280_REGISTER_STATUS     0xF3
  #define BME280_REGISTER_CTRL_MEAS  0xF4
  #define BME280_REGISTER_CONFIG     0xF5

  #define BME280_REGISTER_PRESS_MSB  0xF7
  #define BME280_REGISTER_PRESS_LSB  0xF8
  #define BME280_REGISTER_PRESS_XLSB 0xF9

  #define BME280_REGISTER_TEMP_MSB   0xFA
  #define BME280_REGISTER_TEMP_LSB   0xFB
  #define BME280_REGISTER_TEMP_XLSB  0xFC

  #define BME280_REGISTER_HUM_MSB    0xFD
  #define BME280_REGISTER_HUM_LSB    0xFE

#endif  // BME280_SUPPORT

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

#endif  // DEFINE_H
