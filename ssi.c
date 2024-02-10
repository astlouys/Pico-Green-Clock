#include <string.h>
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "Pico-Green-Clock.h"
#include "ssi.h"


// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"host","volt","temp","led","date","light","htalarm0","htalarm0","htalarm1","htalarm2","htalarm3","htalarm4","htalarm5","htalarm6","htalarm7","htalarm8"};
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
  case 6: // htalarm0
    {
      struct alarm my_alarm;
      UCHAR html_alarm_form [512];
      UINT8 Alarm_Mon, Alarm_Tue, Alarm_Wed, Alarm_Thu, Alarm_Fri, Alarm_Sat, Alarm_Sun;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Build in a string
      strcpy(html_alarm_form, "");
      // Enable button
      strcat(html_alarm_form, "<label for=\"a0en\">Enable</label><input type=\"checkbox\" id=\"a0en\" name=\"a0en\" value=\"%d\">\n");
      // Time of alarm
      strcat(html_alarm_form, "<label for=\"a0time\">time:</label><input type=\"time\" id=\"a0time\" name=\"a0time\" value=\"%02d:%02d\">\n");
      // Extract day of week flags
      Alarm_Mon = ((my_alarm.Day >> MON) & 0x01);
      Alarm_Tue = ((my_alarm.Day >> TUE) & 0x01);
      Alarm_Wed = ((my_alarm.Day >> WED) & 0x01);
      Alarm_Thu = ((my_alarm.Day >> THU) & 0x01);
      Alarm_Fri = ((my_alarm.Day >> FRI) & 0x01);
      Alarm_Sat = ((my_alarm.Day >> SAT) & 0x01);
      Alarm_Sun = ((my_alarm.Day >> SUN) & 0x01);

      // printed = snprintf(pcInsert, iInsertLen, "<label for=\"a0en\">Enable</label><input type=\"checkbox\" id=\"a0en\" name=\"a0en\" value=\"%d\">\n<label for=\"a0time\">time:</label><input type=\"time\" id=\"a0time\" name=\"a0time\" value=\"%02d:%02d\">\n<label for=\"a0mond\">Monday</label><input type=\"checkbox\" id=\"a0mond\" name=\"a0mond\" value=\"%d\">\n"
      // ,my_alarm.FlagStatus,my_alarm.Hour,my_alarm.Minute,Alarm_Mon);
      // Generate the radio buttons
// strcat(html_alarm_form, "<label for=\"a0time\">time:</label><input type=\"time\" id=\"a0time\" name=\"a0time\" value=\"%02d:%02d\">\n");
      strcat(html_alarm_form, "<label for=\"a0mond\">Monday</label><input type=\"checkbox\" id=\"a0mond\" name=\"a0mond\" value=\"%d\">\n");

      // html_alarm_form = strcat(html_alarm_form, "<label for=\"a01en\">Enable</label><input type=\"checkbox\" id=\"a01en\" name=\"a01en\" value=\"%02d\">");
      // printed = snprintf(pcInsert, iInsertLen, "<label for=\"a%02den\">Enable</label><input type=\"checkbox\"id=\"a%02den\" name=\"a%02den\" value=\"%1d\"><label for=\"a%02dtime\">time:</label><input type=\"time\" id=\"a%02dtime\" name=\"a%02dtime\" value=\"%02d:%02d\"> ",
      //   alarmindx,alarmindx,alarmindx,my_alarm[alarmindx].FlagStatus,alarmindx,alarmindx,alarmindx,my_alarm[alarmindx].Hour,my_alarm[alarmindx].Minute);
      // printed = snprintf(pcInsert, iInsertLen, "<br>");
      // Alarm.FlagStatus = ;
      // Alarm.Second = ;
      // Alarm.Minute = ;
      // Alarm.Hour = ;
      // Alarm.Day = ;
      // Alarm.Text = ;
// struct alarm
// {
//   UINT8 FlagStatus;
//   UINT8 Second;
//   UINT8 Minute;
//   UINT8 Hour;
//   UINT8 Day;
//   UCHAR Text[40];
// };

      // printed = snprintf(pcInsert, iInsertLen, "%d %d", wfetch_alarm(0), wfetch_alarm(1));
      // printed = snprintf(pcInsert, iInsertLen, "%d %d", 1, 2);
/*
<label for="a01en">Enable</label><input type="checkbox" id="a01en" name="a01en" value="?">
<label for="a01time">time:</label><input type="time" id="a01time" name="a01time" value="??:??">
<label for="a01mond">Enable</label><input type="checkbox" id="a01mond" name="a01mond" value="?">
<label for="a01tued">Enable</label><input type="checkbox" id="a01tued" name="a01tued" value="?">
<label for="a01wedd">Enable</label><input type="checkbox" id="a01wedd" name="a01wedd" value="?">
<label for="a01thud">Enable</label><input type="checkbox" id="a01thud" name="a01thud" value="?">
<label for="a01frid">Enable</label><input type="checkbox" id="a01frid" name="a01frid" value="?">
<label for="a01satd">Enable</label><input type="checkbox" id="a01satd" name="a01satd" value="?">
<label for="a01sund">Enable</label><input type="checkbox" id="a01sund" name="a01sund" value="?">
*/
      printed = snprintf(pcInsert, iInsertLen, html_alarm_form , my_alarm.FlagStatus,my_alarm.Hour,my_alarm.Minute,Alarm_Mon);
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

