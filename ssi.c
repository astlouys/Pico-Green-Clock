#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "Pico-Green-Clock.h"
#include "ssi.h"


// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"host","volt","temp","led","date","alarm"};
//, "day", "month", "year", "hour", "mins", "secs"

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;

  struct human_time now_time;
  UINT8 my_language;
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
      printed = snprintf(pcInsert, iInsertLen, "%f", voltage);
      // Set LED back
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_status);
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
      my_language = wfetch_current_language();
      now_time = wfetch_current_datetime();
      UCHAR* my_weekdayname = wfetch_DayName(my_language, now_time.DayOfWeek);
      UINT16 my_dayofmonth = now_time.DayOfMonth;
      UCHAR* my_monthname = wfetch_MonthName(my_language, now_time.Month);
      UINT16 my_hour = now_time.Hour;
      UINT16 my_minute = now_time.Minute;
      UINT16 my_second = now_time.Second;
      printed = snprintf(pcInsert, iInsertLen, "Date is : %s %d %s\nTime is : %d:%d:%d", my_weekdayname, my_dayofmonth, my_monthname, my_hour, my_minute, my_second);
    }
    break;
  case 5: // alarm
    {
      printed = snprintf(pcInsert, iInsertLen, "%d %d", wfetch_alarm(0), wfetch_alarm(1));
      // printed = snprintf(pcInsert, iInsertLen, "%d %d", 1, 2);
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

