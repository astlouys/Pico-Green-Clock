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

  UCHAR MonthName[LANGUAGE_HI_LIMIT][13][13] =
  {
    {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}},
    {{}, {"January"},   {"February"},  {"March"},     {"April"},     {"May"},       {"June"},      {"July"},      {"August"},    {"September"}, {"October"},   {"November"},  {"December"}},
    {{}, {"Janvier"},   {"Fevrier"},   {"Mars"},      {"Avril"},     {"Mai"},       {"Juin"},      {"Juillet"},   {"Aout"},      {"Septembre"}, {"Octobre"},   {"Novembre"},  {"Decembre"}},
    {{}, {"Januar"},    {"Februar"},   {"Maerz"},     {"April"},     {"Mai"},       {"Juni"},      {"Juli"},      {"August"},    {"September"}, {"Oktober"},   {"November"},  {"Dezember"}},
    {{}, {"leden"},     {"unor"},      {"brezen"},    {"duben"},     {"kveten"},    {"cerven"},    {"cervenec"},  {"srpen"},     {"zari"},      {"rijen"},     {"listopad"},  {"prosinec"}},
    {{}, {"enero"},     {"febrero"},   {"marzo"},     {"abril"},     {"mayo"},      {"junio"},     {"julio"},     {"agosto"},    {"septiembre"},{"octubre"},   {"noviembre"}, {"diciembre"}}
  };


  UCHAR ShortMonth[LANGUAGE_HI_LIMIT][13][4] =
  {
    {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}},
    {{}, {"JAN"},  {"FEB"},  {"MAR"},  {"APR"},  {"MAY"},  {"JUN"},  {"JUL"},  {"AUG"},  {"SEP"},  {"OCT"},  {"NOV"},  {"DEC"}},
    {{}, {"JAN"},  {"FEV"},  {"MAR"},  {"AVR"},  {"MAI"},  {"JUN"},  {"JUL"},  {"AOU"},  {"SEP"},  {"OCT"},  {"NOV"},  {"DEC"}},
    {{}, {"JAN"},  {"FEB"},  {"MAR"},  {"APR"},  {"MAI"},  {"JUN"},  {"JUL"},  {"AUG"},  {"SEP"},  {"OKT"},  {"NOV"},  {"DEZ"}},
    {{}, {"led."}, {"uno."}, {"bre."}, {"dub."}, {"kve."}, {"cvn."}, {"cvc."}, {"srp."}, {"zar."}, {"rij."}, {"lis."}, {"pro."}},
    {{}, {"ene."}, {"feb."}, {"mar."}, {"abr."}, {"may."}, {"jun."}, {"jul."}, {"ago."}, {"sep."}, {"oct."}, {"nov."}, {"dic."}}
  };

  UCHAR DayName[LANGUAGE_HI_LIMIT][8][13] =
  {
    {{}, {}, {}, {}, {}, {}, {}, {}},
    {{}, {"Sunday"},     {"Monday"},     {"Tuesday"},    {"Wednesday"}, {"Thursday"},   {"Friday"},     {"Saturday"}},
    {{}, {"Dimanche"},   {"Lundi"},      {"Mardi"},      {"Mercredi"},  {"Jeudi"},      {"Vendredi"},   {"Samedi"}},
    {{}, {"Sonntag"},    {"Montag"},     {"Dienstag"},   {"Mittwoch"},  {"Donnerstag"}, {"Freitag"},    {"Samstag"}},
    {{}, {"nedele"},     {"pondeli"},    {"utery"},      {"streda"},    {"ctvrtek"},    {"patek"},      {"sobota"}},
    {{}, {"domingo"},    {"lunes"},      {"martes"},     {"miercoles"}, {"jueves"},     {"viernes"},    {"sabado"}}
  };

  UCHAR ShortDay[LANGUAGE_HI_LIMIT][8][10] =
  {
    {{}, {}, {}, {}, {}, {}, {}, {}},
    {{}, {"SUN"}, {"MON"}, {"TUE"}, {"WED"}, {"THU"}, {"FRI"}, {"SAT"}},
    {{}, {"DIM"}, {"LUN"}, {"MAR"}, {"MER"}, {"JEU"}, {"VEN"}, {"SAM"}},
    {{}, {"SON"}, {"MON"}, {"DIE"}, {"MIT"}, {"DON"}, {"FRE"}, {"SAM"}},
    {{}, {"ne"},  {"po"},  {"ut"},  {"st"},  {"ct"},  {"pa"},  {"so"}},
    {{}, {"do."}, {"lu."}, {"ma."}, {"mi."}, {"ju."}, {"vi."}, {"sa."}}
  };


  /* ---------------------------------------------------------------- *\
                   Localization of days and months.
  \* ---------------------------------------------------------------- */

  /* Add accents to some French weekdays and months. */
  MonthName[FRENCH][FEB][1] = (UCHAR)0x90; // e - acute on fevrier.
  MonthName[FRENCH][AUG][2] = (UCHAR)30;   // u - circumflex on aout.
  MonthName[FRENCH][DEC][1] = (UCHAR)0x90; // e - acute on decembre.

  ShortMonth[FRENCH][FEB][1] = (UCHAR)0x90; // e - acute on FEV.
  ShortMonth[FRENCH][AUG][2] = (UCHAR)30;   // u - circumflex on AOU.
  ShortMonth[FRENCH][DEC][1] = (UCHAR)0x90; // e - acute on DEC.


  /* Add accents to some Czech weekdays. */
  DayName[CZECH][MON][4] = (UCHAR)130; // e-caron
  DayName[CZECH][MON][6] = (UCHAR)131; // i-acute
  DayName[CZECH][TUE][0] = (UCHAR)133; // u-acute
  DayName[CZECH][TUE][4] = (UCHAR)132; // y-acute
  DayName[CZECH][WED][2] = (UCHAR)135; // r-caron
  DayName[CZECH][THU][0] = (UCHAR)136; // c-caron
  DayName[CZECH][FRI][1] = (UCHAR)129; // a-acute
  DayName[CZECH][SUN][3] = (UCHAR)130; // e-caron

  ShortDay[CZECH][TUE][0] = (UCHAR)133; // u-acute
  ShortDay[CZECH][THU][0] = (UCHAR)136; // c-caron
  ShortDay[CZECH][FRI][1] = (UCHAR)129; // a-acute
  ShortDay[CZECH][SUN][3] = (UCHAR)130; // e-caron

  /* Add accents to some Czech months. */
  MonthName[CZECH][FEB][0] = (UCHAR)133; // u-acute
  MonthName[CZECH][MAR][1] = (UCHAR)135; // r-caron
  MonthName[CZECH][MAY][2] = (UCHAR)130; // e-caron
  MonthName[CZECH][JUN][0] = (UCHAR)136; // c-caron
  MonthName[CZECH][JUL][0] = (UCHAR)136; // c-caron
  MonthName[CZECH][SEP][1] = (UCHAR)129; // a-acute
  MonthName[CZECH][SEP][2] = (UCHAR)135; // r-caron
  MonthName[CZECH][OCT][0] = (UCHAR)135; // r-caron
  MonthName[CZECH][OCT][1] = (UCHAR)131; // i-acute

  ShortMonth[CZECH][FEB][0] = (UCHAR)133; // u-acute
  ShortMonth[CZECH][MAR][1] = (UCHAR)135; // r-caron
  ShortMonth[CZECH][MAY][2] = (UCHAR)130; // e-caron
  ShortMonth[CZECH][JUN][0] = (UCHAR)136; // c-caron
  ShortMonth[CZECH][JUL][0] = (UCHAR)136; // c-caron
  ShortMonth[CZECH][SEP][1] = (UCHAR)129; // a-acute
  ShortMonth[CZECH][SEP][2] = (UCHAR)135; // r-caron
  ShortMonth[CZECH][OCT][0] = (UCHAR)135; // r-caron
  ShortMonth[CZECH][OCT][1] = (UCHAR)131; // i-acute

  /* Add accents to some Spanish weekdays. */
  DayName[SPANISH][TUE][2] = (UCHAR)130; // e-acute
  DayName[SPANISH][SAT][1] = (UCHAR)129; // a-acute

  ShortDay[SPANISH][SAT][1] = (UCHAR)129; // a-acute


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
      UCHAR* my_weekdayname = DayName[my_language][now_time.DayOfWeek];
      UINT16 my_dayofmonth = now_time.DayOfMonth;
      UCHAR* my_monthname = MonthName[my_language][now_time.Month];
      printed = snprintf(pcInsert, iInsertLen, "%s %d %s", my_weekdayname, my_dayofmonth, my_monthname);
      // printed = snprintf(pcInsert, iInsertLen, "%s %d %s", DayName[my_language][now_time.DayOfWeek], now_time.DayOfMonth, MonthName[my_language][now_time.Month]);
      // UCHAR* my_weekname = wfetch_current_dayname();
      // UINT16 my_dayofmonth = wfetch_current_dayofmonth();
      // UCHAR* my_monthname = wfetch_current_monthname();
      // printed = snprintf(pcInsert, iInsertLen, "%s %d %s", my_weekname, my_dayofmonth, my_monthname);
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

