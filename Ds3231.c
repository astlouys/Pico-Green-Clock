/* ======================================================================== *\
   Ds3231.c
   St-Louys Andre - February 2022
   astlouys@gmail.com
   Revision 31-MAR-2022
   Langage: GCC 7.3.1 arm-none-eabi
   Version 1.01

   Raspberry Pi Pico firmware to drive the Waveshare green clock
   From an original software version by "Yufu" on 25-JAN-2021 and
   given with the Waveshare clock by 2020 Raspberry PI (Trading) Ltd.

   REVISION HISTORY:
   =================
   06-FEB-2022 1.00 - Initial release.
   02-MAR-2022 1.01 - Code reformatting.
\* ======================================================================== */



/* ======================================================================== *\
   NOTE:
\* ======================================================================== */



/* $TITLE=Definitions and include files. */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                    Definitions and include files
\* ----------------------------------------------------------------- */
#define UCHAR unsigned char

#include "Ds3231.h"

bool HourMode;

char Meridiem[2][3] = {"AM", "PM"};
char StateOfTime[3] = "";

uint8_t Ds3231ReadRegister[17];
uint8_t ByteData[16];
uint8_t REG_ADDRESSES[17] =
{
  DS3231_REG_SECOND,
  DS3231_REG_MINUTE,
  DS3231_REG_HOUR,
  DS3231_REG_DAY,
  DS3231_REG_DATE,
  DS3231_REG_MONTH,
  DS3231_REG_YEAR,
  DS3231_REG_A1S,
  DS3231_REG_A1M,
  DS3231_REG_A1H,
  DS3231_REG_A1D,
  DS3231_REG_A2M,
  DS3231_REG_A2H,
  DS3231_REG_A2D,
  DS3231_REG_CONTROL,
  DS3231_REG_HTEMP,
  DS3231_REG_LTEMP
};





/* $TITLE=bcd_to_byte() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
       Convert the binary coded decimal value to integer value.
\* ----------------------------------------------------------------- */
uint8_t bcd_to_byte(uint8_t BcdValue)
{
  return ((((BcdValue & 0xF0) >> 4) * 10) + (BcdValue & 0x0F));
}





/* $TITLE=byte_data() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
     Read registers from RTC IC and convert them to integer value.
\* ----------------------------------------------------------------- */
void byte_data(void)
{
  int Loop1Int;

  ds3231_register_read();
  for (Loop1Int = 0; Loop1Int < 14; ++Loop1Int)
  {
    ByteData[Loop1Int] = bcd_to_byte(Ds3231ReadRegister[Loop1Int]);
  }

  return;
}





/* $TITLE=dec_to_bcd() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
         Convert the integer value to binary-coded-decimal.
\* ----------------------------------------------------------------- */
uint8_t dec_to_bcd(int DecValue)
{
  return (uint8_t)((DecValue / 10 * 16) + (DecValue % 10));
}





/* $TITLE=ds3231_check_alarm_0() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
              Check status of alarm 0 from the RTC IC.
\* ----------------------------------------------------------------- */
bool ds3231_check_alarm_0()
{
  uint8_t regVal[2] = {DS3231_REG_STATUS, 0x00};

  bool res = false;

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &regVal[0], 1,  true);
  i2c_read_blocking( I2C_PORT, DS3231_ADDRESS, &regVal[1], 1, false);

  if (regVal[1] & DS3231_STA_A1F)
  {
    res = true;
    regVal[1] &= ~DS3231_STA_A1F;
    i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, regVal, 2, false);
  }

  return res;
}





/* $TITLE=ds3231_check_alarm_1() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
              Check status of alarm 1 from the RTC IC.
\* ----------------------------------------------------------------- */
bool ds3231_check_alarm_1()
{
  uint8_t regVal[2] = {DS3231_REG_STATUS, 0x00};

  bool res = false;

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &regVal[0], 1,  true);
  i2c_read_blocking( I2C_PORT, DS3231_ADDRESS, &regVal[1], 1, false);

  if (regVal[1] & DS3231_STA_A2F)
  {
    res = true;
    regVal[1] &= ~DS3231_STA_A2F;
    i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, regVal, 2, false);
  }

  return res;
}





/* $TITLE=ds3231_register_read() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
          Read the main registers of the real-time clock IC.
\* ----------------------------------------------------------------- */
void ds3231_register_read()
{
  UCHAR Loop1UInt8;


  for (Loop1UInt8 = 0; Loop1UInt8 < 16; ++Loop1UInt8)
  {
    i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &REG_ADDRESSES[Loop1UInt8],      1,  true);
    i2c_read_blocking( I2C_PORT, DS3231_ADDRESS, &Ds3231ReadRegister[Loop1UInt8], 1, false);
  }

  return;
}





/* $TITLE=ds3231_sqw_enable() */
/* $PAGE */
/* ----------------------------------------------------------------- *\

\* ----------------------------------------------------------------- */
void ds3231_sqw_enable(bool Enable)
{
  uint8_t control;
  uint8_t val[2];

  val[0] = DS3231_REG_CONTROL;
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &val[0],  1,  true);
  i2c_read_blocking( I2C_PORT, DS3231_ADDRESS, &control, 1, false);

  if (Enable)
  {
    control |=   0b01000000; // set BBSQW to 1
    control &=  ~0b00000100; // set INTCN to 1
  }
  else
  {
    control &= ~0b01000000; // set BBSQW to 0
  }

  val[1] = control;
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, val, 2, false);

  return;
}





/* $TITLE=format_time_mode() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
     Not used for now. 24-hours format is always used with RTC IC
    and Firmware takes care of the 12-hours conversion if required.
\* ----------------------------------------------------------------- */
/***
void format_time_mode()
{
  byte_data();

  if ((Ds3231ReadRegister[2] & 0x40) != 0)
  {
    HourMode = true;

    if ((Ds3231ReadRegister[2] & 0x20) != 0)
    {
      strcpy(StateOfTime, Meridiem[1]);
    }
    else
      strcpy(StateOfTime, Meridiem[0]);
  }
  else
    HourMode = false;

  return
}
***/




/* $TITLE=init_ds3231() */
/* $PAGE */
/* ----------------------------------------------------------------- *\

\* ----------------------------------------------------------------- */
/***
void init_ds3231()
{
  unsigned char i;

  uint8_t val[2];

  val[0] = DS3231_REG_CONTROL;
  val[1] = Control_default;
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, val, 2, false);

  val[0] = DS3231_REG_STATUS;
  val[1] = Status_default;
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, val, 2, false);

  return;
}
***/





/* $TITLE=Read_RTC() */
/* $PAGE */
/* ----------------------------------------------------------------- *\

\* ----------------------------------------------------------------- */
TIME_RTC Read_RTC()
{
  TIME_RTC timeRtc;

  unsigned char RTC_buf[7];

  uint8_t val = 0x00;

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &val,    1,  true);
  i2c_read_blocking( I2C_PORT, DS3231_ADDRESS, RTC_buf, 7, false);

  timeRtc.seconds    = RTC_buf[0];
  timeRtc.minutes    = RTC_buf[1];
  timeRtc.hour       = RTC_buf[2];
  timeRtc.dayofweek  = RTC_buf[3];
  timeRtc.dayofmonth = RTC_buf[4];
  timeRtc.month      = RTC_buf[5];
  timeRtc.year       = RTC_buf[6];

  return timeRtc;
}





/* $TITLE=set_alarm1_clock() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                     Set alarm #1 in the RTC IC.
\* ----------------------------------------------------------------- */
void set_alarm1_clock(uint8_t Mode, uint8_t Second, uint8_t Minute, uint8_t Hour, uint8_t Date)
{
  uint8_t AlarmSecond = dec_to_bcd(Second);
  uint8_t AlarmMinute = dec_to_bcd(Minute);
  uint8_t AlarmHour   = dec_to_bcd(Hour);
  uint8_t AlarmDate   = dec_to_bcd(Date);

  switch (Mode)
  {
    case ALARM_MODE_ALL_MATCHED:
    break;

    case ALARM_MODE_HOUR_MIN_SEC_MATCHED:
      AlarmDate |= 0x80;
    break;

    case ALARM_MODE_MIN_SEC_MATCHED:
      AlarmDate |= 0x80;
      AlarmHour |= 0x80;
    break;

    case ALARM_MODE_SEC_MATCHED:
      AlarmDate   |= 0x80;
      AlarmHour   |= 0x80;
      AlarmMinute |= 0x80;
    break;

    case ALARM_MODE_ONCE_PER_SECOND:
      AlarmDate   |= 0x80;
      AlarmHour   |= 0x80;
      AlarmMinute |= 0x80;
      AlarmSecond |= 0x80;
    break;
  }
  uint8_t StartAddress = DS3231_REG_A1S;
  uint8_t Val[5]={StartAddress, AlarmSecond, AlarmMinute, AlarmHour, AlarmDate};
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, Val, 5, false);
  uint8_t AddrRegVal[2]= {DS3231_REG_CONTROL, 0x00};

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &AddrRegVal[0], 1,  true);
  i2c_read_blocking (I2C_PORT, DS3231_ADDRESS, &AddrRegVal[1], 1, false);
  AddrRegVal[1] |= 0x01;
  AddrRegVal[1] |= 0x04;
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, AddrRegVal, 2, false);

  return;
}





/* $TITLE=set_alarm2_clock() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                     Set alarm #2 in the RTC IC.
\* ----------------------------------------------------------------- */
void set_alarm2_clock(uint8_t Minute, uint8_t Hour, uint8_t Date)
{
  uint8_t AlarmMinute = dec_to_bcd(Minute);
  uint8_t AlarmHour   = dec_to_bcd(Hour);
  uint8_t AlarmDate   = dec_to_bcd(Date);

  AlarmDate |= 0x80;

  uint8_t StartAddress = DS3231_REG_A2M;
  uint8_t Val[4]={StartAddress, AlarmMinute, AlarmHour, AlarmDate};

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, Val, 4, false);

  uint8_t AddrRegVal[2]= {DS3231_REG_CONTROL, 0x00};

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &AddrRegVal[0], 1,  true);
  i2c_read_blocking (I2C_PORT, DS3231_ADDRESS, &AddrRegVal[1], 1, false);
  AddrRegVal[1] |= 0x02;
  AddrRegVal[1] |= 0x04;
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, AddrRegVal, 2, false);

  return;
}





/* $TITLE=set_clock_mode() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                         Not used for now.
\* ----------------------------------------------------------------- */
/***
void set_clock_mode(bool h12)
{
  uint8_t val[2];

  val[0]= DS3231_REG_HOUR;
  if (h12)
  {
    i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &val[0], 1, true);
    i2c_read_blocking(I2C_PORT, DS3231_ADDRESS, &val[1], 1, false);
    val[1] = (val[1] | 0b01000000);
    i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, val, 2, false);
  }
  else
  {
    i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, &val[0], 1, true);
    i2c_read_blocking(I2C_PORT, DS3231_ADDRESS, &val[1], 1, false);
    val[1] = (val[1] & 0b10111111);
    i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, val, 2, false);
  }

  return;
}
***/




/* $TITLE=set_day_of_month() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
              Initialize the day-of-month in the RTC IC.
\* ----------------------------------------------------------------- */
void set_day_of_month(uint8_t DayOfMonth)
{
  uint8_t setDom[2] = {0x04, 0x00};

  setDom[1] = dec_to_bcd(DayOfMonth);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, setDom, 2, false);
  
  return;
}





/* $TITLE=set_day_of_week() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
               Initialize the day-of-week in the RTC IC.
\* ----------------------------------------------------------------- */
void set_day_of_week(uint8_t DayOfweek)
{
  uint8_t setDow[2] = {0x03, 0x00};

  setDow[1] = dec_to_bcd(DayOfweek);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, setDow, 2, false);
  
  return;
}





/* $TITLE=set_hour() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                Initialize the hour in the RTC IC.
\* ----------------------------------------------------------------- */
void set_hour(uint8_t Hour)
{
  uint8_t setHour[2] = {0x02, 0x00};

  setHour[1] = dec_to_bcd(Hour);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, setHour, 2, false);
  
  return;
}





/* $TITLE=set_minute() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
               Initialize the minute in the RTC IC.
\* ----------------------------------------------------------------- */
void set_minute(uint8_t min)
{
  uint8_t setMin[3] = {0x00, 0x00, 0x00};

  /// Optionally set seconds to 50 to accelerate minute change when debugging specific sections of code.
  setMin[1] = dec_to_bcd(50);  /// 50 for debugging purposes.
  setMin[2] = dec_to_bcd(min);

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, setMin, 3, false);
  
  return;
}





/* $TITLE=set_month() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
               Initialize the month in the RTC IC.
\* ----------------------------------------------------------------- */
void set_month(uint8_t Month)
{
  uint8_t setMouth[2] = {0x05, 0x00};

  setMouth[1] = dec_to_bcd(Month);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, setMouth, 2, false);
  
  return;
}





/* $TITLE=set_time() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
          Initialize all time-related data in the RTC IC.
\* ----------------------------------------------------------------- */
void set_time(uint8_t Second, uint8_t Minute, uint8_t Hour, uint8_t DayOfWeek, uint8_t DayOfMonth, uint8_t Month, uint8_t Year)
{
  uint8_t TimeToSet[8];

  TimeToSet[0]=0x00;
  TimeToSet[1]=dec_to_bcd(Second);
  TimeToSet[2]=dec_to_bcd(Minute);
  TimeToSet[3]=dec_to_bcd(Hour);
  TimeToSet[4]=dec_to_bcd(DayOfWeek);
  TimeToSet[5]=dec_to_bcd(DayOfMonth);
  TimeToSet[6]=dec_to_bcd(Month);
  TimeToSet[7]=dec_to_bcd(Year);

  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, TimeToSet, 8, false);
  
  return;
}





/* $TITLE=set_year() */
/* $PAGE */
/* ----------------------------------------------------------------- *\
                Initialize the year in the RTC IC.
\* ----------------------------------------------------------------- */
void set_year(uint8_t Year)
{
  uint8_t setYear[2] = {0x06, 0x00};

  setYear[1] = dec_to_bcd(Year);
  i2c_write_blocking(I2C_PORT, DS3231_ADDRESS, setYear, 2, false);
  
  return;
}
