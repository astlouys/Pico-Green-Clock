#include <string.h>
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "Pico-Green-Clock.h"
#include "ssi.h"

// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"host","wifissid","wifipass","volt","temp","led","date","light","autodim","dimlevel",
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

// Set the tag offset for the alarm table entries.
#define ALARMBASE0 10
#define ALARMBASE1 (ALARMBASE0 + 10)
#define ALARMBASE2 (ALARMBASE1 + 10)
#define ALARMBASE3 (ALARMBASE2 + 10)
#define ALARMBASE4 (ALARMBASE3 + 10)
#define ALARMBASE5 (ALARMBASE4 + 10)
#define ALARMBASE6 (ALARMBASE5 + 10)
#define ALARMBASE7 (ALARMBASE6 + 10)
#define ALARMBASE8 (ALARMBASE7 + 10)

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
  case 0: // host
    {
      UCHAR* my_host = wfetch_hostname();
      printed = snprintf(pcInsert, iInsertLen, "%s", my_host);
    }
    break;
  case 1: // wifissid
      UCHAR* my_wifissid = wfetch_wifissid();
      printed = snprintf(pcInsert, iInsertLen, "%s", my_wifissid);
    break;
  case 2: // wifipass
      UCHAR* my_wifipassphrase = wfetch_wifipass();
      printed = snprintf(pcInsert, iInsertLen, "%s", my_wifipassphrase);
    break;
  case 3: // volt
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
  case 4: // temp
    {
      adc_select_input(4);
      const float voltage = adc_read() * 3.3f / (1 << 12);
      const float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
      printed = snprintf(pcInsert, iInsertLen, "%f", tempC);
    }
    break;
  case 5: // led
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
  case 6: // date
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
  case 7: // light
    {
      struct web_light_value dimmer_light_values = wfetch_light_adc_level();
      printed = snprintf(pcInsert, iInsertLen, "PWM Duty Cycle: %4.1f,  Instant light level: %4u,   Average light level: %4u,   Max light level: %4u,   Min light level: %4u", (dimmer_light_values.Cycles/10.0), dimmer_light_values.adc_current_value, dimmer_light_values.AverageLightLevel, dimmer_light_values.Max_adc_value, dimmer_light_values.Min_adc_value);
      // printed = snprintf(pcInsert, iInsertLen, "%d %d", 1, 2);
    }
    break;
  case 8: // autodim
    {
        printed = snprintf(pcInsert, iInsertLen, "%d", fetch_AutoBrightness());
    }
    break;
  case 9: // dimlevel
    {
      struct web_light_value dimmer_light_values = wfetch_light_adc_level();
      printed = snprintf(pcInsert, iInsertLen, "%d", dimmer_light_values.MinLightLevel);
    }
    break;
  case (ALARMBASE0 + 0): // alm0enab
    {
      struct alarm my_alarm;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE0 + 1): // alm0time
    {
      struct alarm my_alarm;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE0 + 2): // alm0mond
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
  case (ALARMBASE0 + 3): // alm0tues
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
  case (ALARMBASE0 + 4): // alm0weds
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
  case (ALARMBASE0 + 5): // alm0thur
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
  case (ALARMBASE0 + 6): // alm0frid
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
  case (ALARMBASE0 + 7): // alm0satu
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
  case (ALARMBASE0 + 8): // alm0sund
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
  case (ALARMBASE0 + 9): //alm0text
    {
      struct alarm my_alarm;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case (ALARMBASE1 + 0): // alm1enab
    {
      struct alarm my_alarm;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE1 + 1): // alm1time
    {
      struct alarm my_alarm;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE1 + 2): // alm1mond
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
  case (ALARMBASE1 + 3): // alm1tues
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
  case (ALARMBASE1 + 4): // alm1weds
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
  case (ALARMBASE1 + 5): // alm1thur
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
  case (ALARMBASE1 + 6): // alm1frid
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
  case (ALARMBASE1 + 7): // alm1satu
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
  case (ALARMBASE1 + 8): // alm1sund
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
  case (ALARMBASE1 + 9): // alm1text
    {
      struct alarm my_alarm;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case (ALARMBASE2 + 0): // alm2enab
    {
      struct alarm my_alarm;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE2 + 1): // alm2time
    {
      struct alarm my_alarm;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE2 + 2): // alm2mond
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
  case (ALARMBASE2 + 3): // alm2tues
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
  case (ALARMBASE2 + 4): // alm2weds
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
  case (ALARMBASE2 + 5): // alm2thur
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
  case (ALARMBASE2 + 6): // alm2frid
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
  case (ALARMBASE2 + 7): // alm2satu
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
  case (ALARMBASE2 + 8): // alm2sund
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
  case (ALARMBASE2 + 9): // alm2text
    {
      struct alarm my_alarm;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case (ALARMBASE3 + 0): // alm3enab
    {
      struct alarm my_alarm;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE3 + 1): // alm3time
    {
      struct alarm my_alarm;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE3 + 2): // alm3mond
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
  case (ALARMBASE3 + 3): // alm3tues
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
  case (ALARMBASE3 + 4): // alm3weds
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
  case (ALARMBASE3 + 5): // alm3thur
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
  case (ALARMBASE3 + 6): // alm3frid
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
  case (ALARMBASE3 + 7): // alm3satu
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
  case (ALARMBASE3 + 8): // alm3sund
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
  case (ALARMBASE3 + 9): // alm3text
    {
      struct alarm my_alarm;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;

  case (ALARMBASE4 + 0): // alm4enab
    {
      struct alarm my_alarm;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE4 + 1): // alm4time
    {
      struct alarm my_alarm;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE4 + 2): // alm4mond
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
  case (ALARMBASE4 + 3): // alm4tues
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
  case (ALARMBASE4 + 4): // alm4weds
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
  case (ALARMBASE4 + 5): // alm4thur
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
  case (ALARMBASE4 + 6): // alm4frid
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
  case (ALARMBASE4 + 7): // alm4satu
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
  case (ALARMBASE4 + 8): // alm4sund
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
  case (ALARMBASE4 + 9): // alm4text
    {
      struct alarm my_alarm;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case (ALARMBASE5 + 0): // alm5enab
    {
      struct alarm my_alarm;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE5 + 1): // alm5time
    {
      struct alarm my_alarm;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE5 + 2): // alm5mond
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
  case (ALARMBASE5 + 3): // alm5tues
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
  case (ALARMBASE5 + 4): // alm5weds
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
  case (ALARMBASE5 + 5): // alm5thur
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
  case (ALARMBASE5 + 6): // alm5frid
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
  case (ALARMBASE5 + 7): // alm5satu
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
  case (ALARMBASE5 + 8): // alm5sund
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
  case (ALARMBASE5 + 9): // alm5text
    {
      struct alarm my_alarm;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case (ALARMBASE6 + 0): // alm6enab
    {
      struct alarm my_alarm;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE6 + 1): // alm6time
    {
      struct alarm my_alarm;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE6 + 2): // alm6mond
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
  case (ALARMBASE6 + 3): // alm6tues
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
  case (ALARMBASE6 + 4): // alm6weds
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
  case (ALARMBASE6 + 5): // alm6thur
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
  case (ALARMBASE6 + 6): // alm6frid
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
  case (ALARMBASE6 + 7): // alm6satu
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
  case (ALARMBASE6 + 8): // alm6sund
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
  case (ALARMBASE6 + 9): // alm6text
    {
      struct alarm my_alarm;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case (ALARMBASE7 + 0): // alm7enab
    {
      struct alarm my_alarm;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE7 + 1): // alm7time
    {
      struct alarm my_alarm;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE7 + 2): // alm7mond
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
  case (ALARMBASE7 + 3): // alm7tues
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
  case (ALARMBASE7 + 4): // alm7weds
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
  case (ALARMBASE7 + 5): // alm7thur
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
  case (ALARMBASE7 + 6): // alm7frid
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
  case (ALARMBASE7 + 7): // alm7satu
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
  case (ALARMBASE7 + 8): // alm7sund
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
  case (ALARMBASE7 + 9): // alm7text
    {
      struct alarm my_alarm;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // Return the text
      printed = snprintf(pcInsert, iInsertLen, my_alarm.Text);
    }
    break;
  case (ALARMBASE8 + 0): // alm8enab
    {
      struct alarm my_alarm;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // send back the enable flag as a decimal number
      printed = snprintf(pcInsert, iInsertLen, "%s", my_alarm.FlagStatus ? "checked" : "");
    }
    break;
  case (ALARMBASE8 + 1): // alm8time
    {
      struct alarm my_alarm;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // send back the current time in 24hr format
      printed = snprintf(pcInsert, iInsertLen, "%02d:%02d",my_alarm.Hour,my_alarm.Minute);
    }
    break;
  case (ALARMBASE8 + 2): // alm8mond
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
  case (ALARMBASE8 + 3): // alm8tues
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
  case (ALARMBASE8 + 4): // alm8weds
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
  case (ALARMBASE8 + 5): // alm8thur
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
  case (ALARMBASE8 + 6): // alm8frid
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
  case (ALARMBASE8 + 7): // alm8satu
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
  case (ALARMBASE8 + 8): // alm8sund
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
  case (ALARMBASE8 + 9): // alm8text
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

