#include <string.h>
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "Pico-Green-Clock.h"
#include "ssi.h"

// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {
"host", "wifissid", "wifipass", "date", "light", "dimlevel", "datetime", "ntpstat", "dstzone", "dstactve",
"autodim",  "mdimfull", "mdimhigh", "mdimmid",  "mdimlow",  "mdimdark", "timezone", "shsktime", "shskalrm", "langengl",
"langfrch", "langgerm", "langczec", "langspan", "keyclick", "discroll", "sepconst", "sepflash", "seppattn", "sepbargh",
"hr12mode", "hr24mode", "chmenoff", "chmenday", "chmenon", "chmstart", "chimstop", "nliteoff", "nlitauto", "nltnight",
"ntliteon", "nltstart", "nlgtstop",
"alm0enab", "alm0time", "alm0mond", "alm0tues", "alm0weds", "alm0thur", "alm0frid", "alm0satu", "alm0sund", "alm0text","alm0actv","alm0tone",
"alm1enab", "alm1time", "alm1mond", "alm1tues", "alm1weds", "alm1thur", "alm1frid", "alm1satu", "alm1sund", "alm1text","alm1actv","alm1tone",
"alm2enab", "alm2time", "alm2mond", "alm2tues", "alm2weds", "alm2thur", "alm2frid", "alm2satu", "alm2sund", "alm2text","alm2actv","alm2tone",
"alm3enab", "alm3time", "alm3mond", "alm3tues", "alm3weds", "alm3thur", "alm3frid", "alm3satu", "alm3sund", "alm3text","alm3actv","alm3tone",
"alm4enab", "alm4time", "alm4mond", "alm4tues", "alm4weds", "alm4thur", "alm4frid", "alm4satu", "alm4sund", "alm4text","alm4actv","alm4tone",
"alm5enab", "alm5time", "alm5mond", "alm5tues", "alm5weds", "alm5thur", "alm5frid", "alm5satu", "alm5sund", "alm5text","alm5actv","alm5tone",
"alm6enab", "alm6time", "alm6mond", "alm6tues", "alm6weds", "alm6thur", "alm6frid", "alm6satu", "alm6sund", "alm6text","alm6actv","alm6tone",
"alm7enab", "alm7time", "alm7mond", "alm7tues", "alm7weds", "alm7thur", "alm7frid", "alm7satu", "alm7sund", "alm7text","alm7actv","alm7tone",
"alm8enab", "alm8time", "alm8mond", "alm8tues", "alm8weds", "alm8thur", "alm8frid", "alm8satu", "alm8sund", "alm8text","alm8actv","alm8tone"
};

// Set the tag offset for the alarm table entries.
#define ALARMBASE0 43
#define ALARMLENGTH 12
#define ALARMBASE1 (ALARMBASE0 + ALARMLENGTH)
#define ALARMBASE2 (ALARMBASE1 + ALARMLENGTH)
#define ALARMBASE3 (ALARMBASE2 + ALARMLENGTH)
#define ALARMBASE4 (ALARMBASE3 + ALARMLENGTH)
#define ALARMBASE5 (ALARMBASE4 + ALARMLENGTH)
#define ALARMBASE6 (ALARMBASE5 + ALARMLENGTH)
#define ALARMBASE7 (ALARMBASE6 + ALARMLENGTH)
#define ALARMBASE8 (ALARMBASE7 + ALARMLENGTH)

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
    {
      UCHAR* my_wifissid = wfetch_wifissid();
      printed = snprintf(pcInsert, iInsertLen, "%s", my_wifissid);
    }
    break;
  case 2: // wifipass
    {
      UCHAR* my_wifipassphrase = wfetch_wifipass();
      printed = snprintf(pcInsert, iInsertLen, "%s", my_wifipassphrase);
    }
    break;
  case 3: // date
    {
      UINT8 my_language = wfetch_current_language();
      // Fetch current date and time, year is in 4 digit format and hour in 24hr range
      struct human_time now_time = wfetch_current_datetime();
      UCHAR* my_weekdayname = wfetch_DayName(my_language, now_time.DayOfWeek);
      UINT16 my_dayofmonth = now_time.DayOfMonth;
      UCHAR* my_monthname = wfetch_MonthName(my_language, now_time.Month);
      UINT16 my_year = now_time.Year;
      // Hour is a 24 hour value
      UINT16 my_hour = now_time.Hour;
      UINT16 my_minute = now_time.Minute;
      UINT16 my_second = now_time.Second;
      UINT8 Flag_24hr = wfetch_current_hour_mode();
      UCHAR* time_suffix;
      if (Flag_24hr == H24) {
        // No time suffix, null string
        time_suffix = '\0';
      } else {
        switch (my_hour) {
          case 0:
            {
              time_suffix = "AM";
              my_hour = 12;
            }
            break;
          case 1 ... 11:
            {
              time_suffix = "AM";
            }
            break;
          case 12:
            {
              time_suffix = "PM";
            }
            break;
          case 13 ... 23:
            {
              time_suffix = "PM";
              my_hour = my_hour - 12;
            }
            break;
          default:
            {
              time_suffix = '\0';
            }
            break;
        }
      }
      printed = snprintf(pcInsert, iInsertLen, "Current date is : %s %d %s %04d     Current time is : %d:%02d:%02d %s", my_weekdayname, my_dayofmonth, my_monthname, my_year, my_hour, my_minute, my_second, time_suffix);
    }
    break;
  case 4: // light
    {
      struct web_light_value dimmer_light_values = wfetch_light_adc_level();
      printed = snprintf(pcInsert, iInsertLen, "PWM Duty Cycle: %4.1f,  Instant light level: %4u,   Average light level: %4u,   Max average light level: %4u,   Min average light level: %4u", (dimmer_light_values.DutyCycle/10.0), dimmer_light_values.adc_current_value, dimmer_light_values.AverageLightLevel, dimmer_light_values.Max_adc_value, dimmer_light_values.Min_adc_value);
      // printed = snprintf(pcInsert, iInsertLen, "%d %d", 1, 2);
    }
    break;
  case 5: // dimlevel
    {
      struct web_light_value dimmer_light_values = wfetch_light_adc_level();
      printed = snprintf(pcInsert, iInsertLen, "%d", dimmer_light_values.MinLightLevel);
    }
    break;
  case 6: // datetime,   newtime=2024-02-16T09:20
    {
      // Fetch current date and time, year is in 4 digit format and hour in 24hr range
      struct human_time now_time = wfetch_current_datetime();
      UINT16 my_date = now_time.DayOfMonth;
      UINT16 my_month = now_time.Month;
      UINT16 my_year = now_time.Year;
      UINT16 my_hour = now_time.Hour;
      UINT16 my_minute = now_time.Minute;
      UINT16 my_second = now_time.Second;
      printed = snprintf(pcInsert, iInsertLen, "%04d-%02d-%02dT%02d:%02d", my_year, my_month, my_date, my_hour, my_minute);
    }
    break;
  case 7: // ntpstat
    {
      UINT32 NTP_Errors = wfetch_NTP_Errors();
      printed = snprintf(pcInsert, iInsertLen, "NTP Error Count : %d", NTP_Errors);
    }
    break;
  case 8: // dstzone
    {
      UINT8 DSTCOUNTRY = (UINT8)(fetch_DSTCountry());
      printed = snprintf(pcInsert, iInsertLen, "%d", DSTCOUNTRY);
    }
    break;
  case 9: // dstactve
    {
      // send back the enable flag as a populated or empty text option string
      printed = snprintf(pcInsert, iInsertLen, "%s", (fetch_SummerTime() == FLAG_ON) ? "checked" : "");
    }
    break;
  case 10: // autodim
    {
      printed = snprintf(pcInsert, iInsertLen, "%s", (fetch_AutoBrightness() == FLAG_ON) ? "checked" : "");
    }
    break;
  case 11: // mdimfull
    {
      UINT8 Automode = fetch_AutoBrightness();
      UINT8 Manualhigh = FLAG_OFF;
      UINT16 Manuallevel = fetch_ManualBrightness();
      // Set if not auto and pwm.Cycles equal to BRIGHTNESS_LIGHTLEVELSTEP
      if (Automode == FLAG_OFF && Manuallevel == BRIGHTNESS_LIGHTLEVELSTEP) {
        Manualhigh = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (Manualhigh == FLAG_ON) ? "checked" : "");
    }
    break;
  case 12: // mdimhigh
    {
      UINT8 Automode = fetch_AutoBrightness();
      UINT8 Manualhigh = FLAG_OFF;
      UINT16 Manuallevel = fetch_ManualBrightness();
      // Set if not auto and pwm.Cycles less than BRIGHTNESS_LIGHTLEVELSTEP and greater than or equal to BRIGHTNESS_MANUALDIV1
      if (Automode == FLAG_OFF && (Manuallevel < BRIGHTNESS_LIGHTLEVELSTEP && Manuallevel >= BRIGHTNESS_MANUALDIV1)) {
        Manualhigh = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (Manualhigh == FLAG_ON) ? "checked" : "");
    }
    break;
  case 13: // mdimmid
    {
      UINT8 Automode = fetch_AutoBrightness();
      UINT8 Manualmid = FLAG_OFF;
      UINT16 Manuallevel = fetch_ManualBrightness();
      // Set if not auto and pwm.Cycles less than BRIGHTNESS_MANUALDIV1 and greater than or equal to BRIGHTNESS_MANUALDIV2
      if (Automode == FLAG_OFF && (Manuallevel < BRIGHTNESS_MANUALDIV1 && Manuallevel >= BRIGHTNESS_MANUALDIV2)) {
        Manualmid = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (Manualmid == FLAG_ON) ? "checked" : "");
    }
    break;
  case 14: // mdimlow
    {
      UINT8 Automode = fetch_AutoBrightness();
      UINT8 Manuallow = FLAG_OFF;
      UINT16 Manuallevel = fetch_ManualBrightness();
      // Set if not auto and pwm.Cycles less than BRIGHTNESS_MANUALDIV2 and greater than or equal to BRIGHTNESS_MANUALDIV3
      if (Automode == FLAG_OFF && (Manuallevel < BRIGHTNESS_MANUALDIV2 && Manuallevel >= BRIGHTNESS_MANUALDIV3)) {
        Manuallow = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (Manuallow == FLAG_ON) ? "checked" : "");
    }
    break;
  case 15: // mdimdark
    {
      UINT8 Automode = fetch_AutoBrightness();
      UINT8 Manualdark = FLAG_OFF;
      UINT16 Manuallevel = fetch_ManualBrightness();
      // Set if not auto and pwm.Cycles equal to 0
      if (Automode == FLAG_OFF && Manuallevel == 0) {
        Manualdark = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (Manualdark == FLAG_ON) ? "checked" : "");
    }
    break;
  case 16: // timezone
    {
      printed = snprintf(pcInsert, iInsertLen, "%d", fetch_Timezone());
    }
    break;

  case 17: // shsktime
    {
      UINT8 my_setkeymode = fetch_ShortSeyKey();
      UINT8 my_setkeyflag = FLAG_OFF;
      // Allow for all non-zero values
      if (my_setkeymode != FLAG_OFF) {
        my_setkeyflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_setkeyflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 18: // shskalrm
    {
      UINT8 my_setkeymode = fetch_ShortSeyKey();
      UINT8 my_setkeyflag = FLAG_OFF;
      // Invert mode for alternate radio button, alarm
      if (my_setkeymode == FLAG_OFF) {
        my_setkeyflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_setkeyflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 19: // langengl
    {
      UINT8 my_language = wfetch_current_language();
      UINT8 LangFlag = FLAG_OFF;
      if (my_language == ENGLISH) {
        LangFlag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (LangFlag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 20: // langfrch
    {
      UINT8 my_language = wfetch_current_language();
      UINT8 LangFlag = FLAG_OFF;
      if (my_language == FRENCH) {
        LangFlag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (LangFlag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 21: // langgerm
    {
      UINT8 my_language = wfetch_current_language();
      UINT8 LangFlag = FLAG_OFF;
      if (my_language == GERMAN) {
        LangFlag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (LangFlag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 22: // langczec
    {
      UINT8 my_language = wfetch_current_language();
      UINT8 LangFlag = FLAG_OFF;
      if (my_language == CZECH) {
        LangFlag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (LangFlag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 23: // langspan
    {
      UINT8 my_language = wfetch_current_language();
      UINT8 LangFlag = FLAG_OFF;
      if (my_language == SPANISH) {
        LangFlag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (LangFlag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 24: // keyclick
    {
      printed = snprintf(pcInsert, iInsertLen, "%s", (fetch_Keyclick() == FLAG_ON) ? "checked" : "");
    }
    break;
  case 25: // discroll
    {
      printed = snprintf(pcInsert, iInsertLen, "%s", (fetch_ScrollEnable() == FLAG_ON) ? "checked" : "");
    }
    break;
  case 26:// sepconst
    {
      UINT8 my_separatormode = fetch_SeparatorMode();
      UINT8 my_separatorflag = FLAG_OFF;
      if (my_separatormode == SEPARATOR_CONST) {
        my_separatorflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_separatorflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 27:// sepflash
    {
      UINT8 my_separatormode = fetch_SeparatorMode();
      UINT8 my_separatorflag = FLAG_OFF;
      if (my_separatormode == SEPARATOR_FLASH) {
        my_separatorflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_separatorflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 28:// seppattn
    {
      UINT8 my_separatormode = fetch_SeparatorMode();
      UINT8 my_separatorflag = FLAG_OFF;
      if (my_separatormode == SEPARATOR_PATTERN) {
        my_separatorflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_separatorflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 29:// sepbargh
    {
      UINT8 my_separatormode = fetch_SeparatorMode();
      UINT8 my_separatorflag = FLAG_OFF;
      if (my_separatormode == SEPARATOR_BAR) {
        my_separatorflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_separatorflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 30:// hr12mode
    {
      UINT8 my_hr12mode = fetch_ClockHourMode();
      UINT8 my_hr12flag = FLAG_OFF;
      if (my_hr12mode == H12) {
        my_hr12flag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_hr12flag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 31:// hr24mode
    {
      UINT8 my_hr24mode = fetch_ClockHourMode();
      UINT8 my_hr24flag = FLAG_OFF;
      if (my_hr24mode == H24) {
        my_hr24flag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_hr24flag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 32:// chmenoff
    {
      UINT8 my_chimemode = fetch_ChimeMode();
      UINT8 my_chimeflag = FLAG_OFF;
      if (my_chimemode == CHIME_OFF) {
        my_chimeflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_chimeflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 33:// chmenday
    {
      UINT8 my_chimemode = fetch_ChimeMode();
      UINT8 my_chimeflag = FLAG_OFF;
      if (my_chimemode == CHIME_DAY) {
        my_chimeflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_chimeflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 34:// chmenon
    {
      UINT8 my_chimemode = fetch_ChimeMode();
      UINT8 my_chimeflag = FLAG_OFF;
      if (my_chimemode == CHIME_ON) {
        my_chimeflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_chimeflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 35:// chmstart
    {

      printed = snprintf(pcInsert, iInsertLen, "%02d", fetch_ChimeStart());
    }
    break;
  case 36:// chimstop
    {

      printed = snprintf(pcInsert, iInsertLen, "%02d", fetch_ChimeStop());
    }

    break;
  case 37:// nliteoff
    {
      UINT8 my_nightlightmode = fetch_NightLightMode();
      UINT8 my_nightlightflag = FLAG_OFF;
      if (my_nightlightmode == NIGHT_LIGHT_OFF) {
        my_nightlightflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_nightlightflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 38:// nlitauto
    {
      UINT8 my_nightlightmode = fetch_NightLightMode();
      UINT8 my_nightlightflag = FLAG_OFF;
      if (my_nightlightmode == NIGHT_LIGHT_AUTO) {
        my_nightlightflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_nightlightflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 39:// nltnight
    {
      UINT8 my_nightlightmode = fetch_NightLightMode();
      UINT8 my_nightlightflag = FLAG_OFF;
      if (my_nightlightmode == NIGHT_LIGHT_NIGHT) {
        my_nightlightflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_nightlightflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 40:// ntliteon
    {
      UINT8 my_nightlightmode = fetch_NightLightMode();
      UINT8 my_nightlightflag = FLAG_OFF;
      if (my_nightlightmode == NIGHT_LIGHT_ON) {
        my_nightlightflag = FLAG_ON;
      }
      printed = snprintf(pcInsert, iInsertLen, "%s", (my_nightlightflag == FLAG_ON) ? "checked" : "");
    }
    break;
  case 41:// nltstart
    {

      printed = snprintf(pcInsert, iInsertLen, "%02d", fetch_NightLightStart());
    }
    break;
  case 42:// nlgtstop
    {

      printed = snprintf(pcInsert, iInsertLen, "%02d", fetch_NightLightStop());
    }
    break;

  case (ALARMBASE0 + 0): // alm0enab
    {
      struct alarm my_alarm;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE0 + 10): // alm0actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE0 + 11): //alm0tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 1st alarm 0
      my_alarm = wfetch_alarm(0);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE1 + 0): // alm1enab
    {
      struct alarm my_alarm;
      // Get the flash information for 2nd alarm 1
      my_alarm = wfetch_alarm(1);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE1 + 10): // alm1actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 2nd alarm 0
      my_alarm = wfetch_alarm(1);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE1 + 11): //alm1tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 2nd alarm 0
      my_alarm = wfetch_alarm(1);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE2 + 0): // alm2enab
    {
      struct alarm my_alarm;
      // Get the flash information for 3rd alarm 2
      my_alarm = wfetch_alarm(2);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE2 + 10): // alm2actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 3rd alarm 0
      my_alarm = wfetch_alarm(2);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE2 + 11): //alm2tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 3rd alarm 0
      my_alarm = wfetch_alarm(2);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE3 + 0): // alm3enab
    {
      struct alarm my_alarm;
      // Get the flash information for 4th alarm 3
      my_alarm = wfetch_alarm(3);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE3 + 10): // alm3actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 4th alarm 0
      my_alarm = wfetch_alarm(3);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE3 + 11): //alm3tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 4th alarm 0
      my_alarm = wfetch_alarm(3);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE4 + 0): // alm4enab
    {
      struct alarm my_alarm;
      // Get the flash information for 5th alarm 4
      my_alarm = wfetch_alarm(4);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE4 + 10): // alm4actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 5th alarm 0
      my_alarm = wfetch_alarm(4);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE4 + 11): //alm4tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 5th alarm 0
      my_alarm = wfetch_alarm(4);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE5 + 0): // alm5enab
    {
      struct alarm my_alarm;
      // Get the flash information for 6th alarm 5
      my_alarm = wfetch_alarm(5);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE5 + 10): // alm5actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 6th alarm 0
      my_alarm = wfetch_alarm(5);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE5 + 11): //alm5tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 6th alarm 0
      my_alarm = wfetch_alarm(5);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE6 + 0): // alm6enab
    {
      struct alarm my_alarm;
      // Get the flash information for 7th alarm 6
      my_alarm = wfetch_alarm(6);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE6 + 10): // alm6actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 7th alarm 0
      my_alarm = wfetch_alarm(6);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE6 + 11): //alm6tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 7th alarm 0
      my_alarm = wfetch_alarm(6);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE7 + 0): // alm7enab
    {
      struct alarm my_alarm;
      // Get the flash information for 8th alarm 7
      my_alarm = wfetch_alarm(7);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE7 + 10): // alm7actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 8th alarm 0
      my_alarm = wfetch_alarm(7);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE7 + 11): //alm7tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 8th alarm 0
      my_alarm = wfetch_alarm(7);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
    }
    break;
  case (ALARMBASE8 + 0): // alm8enab
    {
      struct alarm my_alarm;
      // Get the flash information for 9th alarm 8
      my_alarm = wfetch_alarm(8);
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
      // send back the enable flag as a populated or empty text option string
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
  case (ALARMBASE8 + 10): // alm7actv
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // get the flash information  for 9th alarm 0
      my_alarm = wfetch_alarm(8);
      // Extract the ms bit to see if the active buzzer is enabled or nor
      Alarm_Jingle = (my_alarm.Jingle & 0x80) >> 7;
      printed = snprintf(pcInsert, iInsertLen, "%s", Alarm_Jingle ? "checked" : "");
    }
    break;
  case (ALARMBASE8 + 11): //alm7tone
    {
      struct alarm my_alarm;
      UINT8 Alarm_Jingle = 0;
      // Get the flash information for 9th alarm 0
      my_alarm = wfetch_alarm(8);
      // Return the jingle index
      Alarm_Jingle = (my_alarm.Jingle & 0x7F);
      printed = snprintf(pcInsert, iInsertLen, "%d", Alarm_Jingle);
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

