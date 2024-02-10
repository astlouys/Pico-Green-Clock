#include <string.h>
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "Pico-Green-Clock.h"
#include "ssi.h"


// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"host","volt","temp","led","date","light",
"alm0enab", "alm0time", "alm0mond", "alm0tues", "alm0weds", "alm0thur", "alm0frid", "alm0satu", "alm0sund", "alm0text",
"alm1enab", "alm1time", "alm1mond", "alm1tues", "alm1weds", "alm1thur", "alm1frid", "alm1satu", "alm1sund", "alm1text",
"alm2enab", "alm2time", "alm2mond", "alm2tues", "alm2weds", "alm2thur", "alm2frid", "alm2satu", "alm2sund", "alm2text",
"alm3enab", "alm3time", "alm3mond", "alm3tues", "alm3weds", "alm3thur", "alm3frid", "alm3satu", "alm3sund", "alm3text",
"alm4enab", "alm4time", "alm4mond", "alm4tues", "alm4weds", "alm4thur", "alm4frid", "alm4satu", "alm4sund", "alm4text",
"alm5enab", "alm5time", "alm5mond", "alm5tues", "alm5weds", "alm5thur", "alm5frid", "alm5satu", "alm5sund", "alm5text",
"alm6enab", "alm6time", "alm6mond", "alm6tues", "alm6weds", "alm6thur", "alm6frid", "alm6satu", "alm6sund", "alm6text",
"alm7enab", "alm7time", "alm7mond", "alm7tues", "alm7weds", "alm7thur", "alm7frid", "alm7satu", "alm7sund", "alm7text",
"alm8enab", "alm8time", "alm8mond", "alm8tues", "alm8weds", "alm8thur", "alm8frid", "alm8satu", "alm8sund", "alm8text"
};
//, "day", "month", "year", "hour", "mins", "secs"

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
  case 0: // host
    {
      UCHAR* my_host = wfetch_hostname();
      printed = snprintf(pcInsert, iInsertLen, "%s", my_host);
    }
    break;
  case 1: // volt
    {
      adc_select_input(3);
      bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
      // This must be a Pico W for web access to be called
      // For Pico W, it is important that GPIO 25 be high to read the power supply voltage.
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
      const float voltage = adc_read() * 3.3f / (1 << 12);
      // Set LED back
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_status);
      printed = snprintf(pcInsert, iInsertLen, "%f", voltage);
    }
    break;
  case 2: // temp
    {
      adc_select_input(4);
      const float voltage = adc_read() * 3.3f / (1 << 12);
      const float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
      printed = snprintf(pcInsert, iInsertLen, "%f", tempC);
    }
    break;
  case 3: // led
    {
      bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
      if(led_status == true){
        printed = snprintf(pcInsert, iInsertLen, "ON");
      }
      else{
        printed = snprintf(pcInsert, iInsertLen, "OFF");
      }
    }
    break;
  case 4: // date
    {
      UINT8 my_language = wfetch_current_language();
      struct human_time now_time = wfetch_current_datetime();
      UCHAR* my_weekdayname = wfetch_DayName(my_language, now_time.DayOfWeek);
      UINT16 my_dayofmonth = now_time.DayOfMonth;
      UCHAR* my_monthname = wfetch_MonthName(my_language, now_time.Month);
      UINT16 my_hour = now_time.Hour;
      UINT16 my_minute = now_time.Minute;
      UINT16 my_second = now_time.Second;
      printed = snprintf(pcInsert, iInsertLen, "%s %d %s\r\nTime is : %d:%02d:%02d", my_weekdayname, my_dayofmonth, my_monthname, my_hour, my_minute, my_second);
    }
    break;
  case 5: // light
    {
      struct web_light_value dimmer_light_values = wfetch_light_adc_level();
      printed = snprintf(pcInsert, iInsertLen, "Instant level: %4u   Av1: %4u   Av2: %4u   PWM Cycles: %3u  PWM Brightness Level: %3u   Max ADC level: %4u   Min ADC level: %4u\r", dimmer_light_values.adc_current_value, dimmer_light_values.AverageLightLevel, dimmer_light_values.AverageLevel, dimmer_light_values.Cycles, dimmer_light_values.pwm_level, dimmer_light_values.Max_adc_value, dimmer_light_values.Min_adc_value);
      // printed = snprintf(pcInsert, iInsertLen, "%d %d", 1, 2);
    }
    break;
  case 6: // alm0enab
    {
      struct alarm my_alarm;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 7: // alm0time
    {
      struct alarm my_alarm;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 8: // alm0mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 9: // alm0tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 10: // alm0weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 11: // alm0thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 12: // alm0frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 13: // alm0satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 14: // alm0sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 15: //alm0text
    {
      struct alarm my_alarm;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case 16: // alm1enab
    {
      struct alarm my_alarm;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 17: // alm1time
    {
      struct alarm my_alarm;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 18: // alm1mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 19: // alm1tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 20: // alm1weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 21: // alm1thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 22: // alm1frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 23: // alm1satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 24: // alm1sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 25: // alm1text
    {
      struct alarm my_alarm;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case 26: // alm2enab
    {
      struct alarm my_alarm;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 27: // alm2time
    {
      struct alarm my_alarm;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 28: // alm2mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 29: // alm2tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 30: // alm2weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 31: // alm2thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 32: // alm2frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 33: // alm2satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 34: // alm2sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 35: // alm2text
    {
      struct alarm my_alarm;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case 36: // alm3enab
    {
      struct alarm my_alarm;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 37: // alm3time
    {
      struct alarm my_alarm;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 38: // alm3mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 39: // alm3tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 40: // alm3weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 41: // alm3thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 42: // alm3frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 43: // alm3satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 44: // alm3sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 45: // alm3text
    {
      struct alarm my_alarm;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;

  case 46: // alm4enab
    {
      struct alarm my_alarm;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 47: // alm4time
    {
      struct alarm my_alarm;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 48: // alm4mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 49: // alm4tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 50: // alm4weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 51: // alm4thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 52: // alm4frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 53: // alm4satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 54: // alm4sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 55: // alm4text
    {
      struct alarm my_alarm;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case 56: // alm5enab
    {
      struct alarm my_alarm;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 57: // alm5time
    {
      struct alarm my_alarm;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 58: // alm5mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 59: // alm5tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 60: // alm5weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 61: // alm5thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 62: // alm5frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 63: // alm5satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 64: // alm5sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 65: // alm5text
    {
      struct alarm my_alarm;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case 66: // alm6enab
    {
      struct alarm my_alarm;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 67: // alm6time
    {
      struct alarm my_alarm;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 68: // alm6mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 69: // alm6tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 70: // alm6weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 71: // alm6thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 72: // alm6frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 73: // alm6satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 74: // alm6sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 75: // alm6text
    {
      struct alarm my_alarm;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case 76: // alm7enab
    {
      struct alarm my_alarm;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 77: // alm7time
    {
      struct alarm my_alarm;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 78: // alm7mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 79: // alm7tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 80: // alm7weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 81: // alm7thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 82: // alm7frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 83: // alm7satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 84: // alm7sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 85: // alm7text
    {
      struct alarm my_alarm;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case 86: // alm8enab
    {
      struct alarm my_alarm;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case 87: // alm8time
    {
      struct alarm my_alarm;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case 88: // alm8mond
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Extract the flag bit for monday alarm
      Alarm_day = ((my_alarm.Day >> MON) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 89: // alm8tues
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Extract the flag bit for tuesday alarm
      Alarm_day = ((my_alarm.Day >> TUE) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 90: // alm8weds
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Extract the flag bit for wednesday alarm
      Alarm_day = ((my_alarm.Day >> WED) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 91: // alm8thur
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Extract the flag bit for thursday alarm
      Alarm_day = ((my_alarm.Day >> THU) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 92: // alm8frid
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Extract the flag bit for friday alarm
      Alarm_day = ((my_alarm.Day >> FRI) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 93: // alm8satu
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Extract the flag bit for saturday alarm
      Alarm_day = ((my_alarm.Day >> SAT) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 94: // alm8sund
    {
      struct alarm my_alarm;
      bool Alarm_day;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Extract the flag bit for sunday alarm
      Alarm_day = ((my_alarm.Day >> SUN) & 0x01);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_day ? "checked" : "");
    }
    break;
  case 95: // alm8text
    {
      struct alarm my_alarm;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;

  default:
    printed = 0;
    break;
  }
  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init() {
  // Initialise ADC (internal pin)
  adc_init();
  adc_set_temp_sensor_enabled(true);
  /* Notes - On the Green Clock:
     ADC 0 (gpio 26)  is for photo-resistor (ambient light level).
     ADC 1 (gpio 27)  not used
     ADC 2 (gpio 28)  not used
     ADC 3 (gpio 29)  is for power supply voltage.
     ADC 4 (internal) is internally connected to read Pico's temperature. */
  adc_select_input(4);

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}

