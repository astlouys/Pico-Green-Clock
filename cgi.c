#include <stdlib.h>
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "Pico-Green-Clock.h"
#include "cgi.h"

// CGI handler which is run when a request for /myhostname.cgi is detected
const char * cgi_myhostname_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  UCHAR destHostname[40];
  UCHAR *new_hostname = destHostname;
  int unesclength;
  int esclength;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "hostname") == 0) {
      // Create a new hostname, but must escape the input
      unesclength = 40;
      esclength = strlen(pcValue[Loop1UInt8]);
      // Create a memory for the destination and refernce to it with a pointer
      UCHAR deststringmem[unesclength + 1];
      UCHAR *unesctext = deststringmem;
      // Check for length too long for destination string
      if (esclength > 40){
        pcValue[Loop1UInt8][39] = '\0';
        esclength = 40;
      }
      unescstring(pcValue[Loop1UInt8], esclength, unesctext, unesclength);
      strcpy(new_hostname, unesctext);
    }
  }
  // Update the network configuration
  wwrite_hostname(new_hostname);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /mynetwork.cgi is detected
const char * cgi_mynetwork_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  UCHAR destSSID[40];
  UCHAR destPassword[70];
  UCHAR *new_wifissid = destSSID;
  UCHAR *new_wifipass = destPassword;
  int unesclength;
  int esclength;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "wifissid") == 0) {
      // Create a new SSID, but must escape the input
      unesclength = 40;
      esclength = strlen(pcValue[Loop1UInt8]);
      // Create a memory for the destination and refernce to it with a pointer
      UCHAR deststringmem[unesclength + 1];
      UCHAR *unesctext = deststringmem;
      // Check for length too long for destination string
      if (esclength > 40){
        pcValue[Loop1UInt8][39] = '\0';
        esclength = 40;
      }
      unescstring(pcValue[Loop1UInt8], esclength, unesctext, unesclength);
      strcpy(new_wifissid, unesctext);
    }
    if (strcmp(pcParam[Loop1UInt8], "wifipass") == 0) {
      // Create a new SSID passphrase, but must escape the input
      unesclength = 70;
      esclength = strlen(pcValue[Loop1UInt8]);
      // Create a memory for the destination and refernce to it with a pointer
      UCHAR deststringmem[unesclength + 1];
      UCHAR *unesctext = deststringmem;
      // Check for length too long for destination string
      if (esclength > 70){
        pcValue[Loop1UInt8][69] = '\0';
        esclength = 70;
      }
      unescstring(pcValue[Loop1UInt8], esclength, unesctext, unesclength);
      strcpy(new_wifipass, unesctext);
    }
  }
  // Update the network configuration
  wwrite_networkcfg(new_wifissid, new_wifipass);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setdateandtime.cgi is detected
const char * cgi_setdateandtime_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  struct human_time new_time = wfetch_current_datetime();
  UINT16 my_date;
  UINT16 my_month;
  UINT16 my_year;
  UINT16 my_hour;
  UINT16 my_minute;
  UINT16 my_second;
  UINT8 NTP_Request = FLAG_OFF;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "newtime") == 0) {
      // Pick out the digits from the field, year is 2 digits only, hour in 24hr format
      my_year = ((UINT)(pcValue[Loop1UInt8][0] - '0') * 1000) + ((UINT)(pcValue[Loop1UInt8][1] - '0') * 100) + ((UINT)(pcValue[Loop1UInt8][2] - '0') * 10) + (UINT)(pcValue[Loop1UInt8][3] - '0');
      my_month = ((UINT)(pcValue[Loop1UInt8][5] - '0') * 10) + (UINT)(pcValue[Loop1UInt8][6] - '0');
      my_date = ((UINT)(pcValue[Loop1UInt8][8] - '0') * 10) + (UINT)(pcValue[Loop1UInt8][9] - '0');
      my_hour = ((UINT)(pcValue[Loop1UInt8][11] - '0') * 10) + (UINT)(pcValue[Loop1UInt8][12] - '0');
      my_minute = ((UINT)(pcValue[Loop1UInt8][16] - '0') * 10) + (UINT)(pcValue[Loop1UInt8][17] - '0');
      my_second = 1; // Allow for processing delays
    }
    if (strcmp(pcParam[Loop1UInt8], "SyncNTP") == 0) {
      NTP_Request = FLAG_ON;
    }
  }
  // Update with the new date and time, other values are unchanged
  new_time.DayOfMonth = my_date;
  new_time.Month = my_month;
  new_time.Year = my_year;
  new_time.Hour = my_hour;
  new_time.Minute = my_minute;
  new_time.Second = my_second;
  if (NTP_Request == FLAG_OFF) {
    // Update the values
    wwrite_current_datetime(new_time);
  }
  else {
    // Sychronise the NTP server
    wrequest_NTP_Sync();
  }
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /settimezone.cgi is detected
const char * cgi_settimezone_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  int8_t New_houroffset;
  // parse the returned key and value pairs,
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "timezoneoffset") == 0) {
      New_houroffset = atoi(pcValue[Loop1UInt8]);
    }
  }
  // Now act on a button press
  wwriteTimezone(New_houroffset);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setdstcountry.cgi is detected
const char * cgi_setdstcountry_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  UINT8 my_DSTCountry = 0;
  UCHAR my_DSTCountry_char = '\0';
  UINT8 new_SummerTime = FLAG_OFF;
  UINT8 DSTCountry_Request = FLAG_OFF;
  UINT8 Summertime_Request = FLAG_OFF;
  // If no value is sent, then nothing in the web form is sent. This is the DST flag cleared
  if (iNumParams == 0) {
      new_SummerTime = FLAG_OFF;
      Summertime_Request = FLAG_ON;
  }
  else {
    for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
      if (strcmp(pcParam[Loop1UInt8], "DSTZoneSel") == 0) {
        my_DSTCountry = atoi(pcValue[Loop1UInt8]);
        DSTCountry_Request = FLAG_ON;
      }
      if (strcmp(pcParam[Loop1UInt8], "dstactive") == 0) {
        new_SummerTime = FLAG_ON;
        Summertime_Request = FLAG_ON;
      }
    }
  }
  if (DSTCountry_Request == FLAG_ON) {
    // Convert the contry code from an integer to a UCHAR
    my_DSTCountry_char = (UCHAR)(my_DSTCountry);
    // Set the DST country code
    mwrite_DSTCountry(my_DSTCountry_char);
  }
  if (Summertime_Request == FLAG_ON) {
    // Update the configuration flag
    wwriteSummerTime(new_SummerTime);
  }
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /clearntperrors.cgi is detected
const char * cgi_clearntperrors_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  // Clear the errors
  wwrite_clear_ntp_error();
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setlanguage.cgi is detected
const char * cgi_setlanguage_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
  UINT8 Loop1UInt8;
  UINT8 new_language = 0;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "langmode") == 0) {
      if (strcmp(pcValue[Loop1UInt8], "Engl") == 0) {
        new_language = ENGLISH;
      }
      else if (strcmp(pcValue[Loop1UInt8], "Frch") == 0) {
        new_language = FRENCH;
      }
      else if (strcmp(pcValue[Loop1UInt8], "Germ") == 0) {
        new_language = GERMAN;
      }
      else if (strcmp(pcValue[Loop1UInt8], "Czec") == 0) {
        new_language = CZECH;
      }
      else if (strcmp(pcValue[Loop1UInt8], "Span") == 0) {
        new_language = SPANISH;
      }
    }
  }
  // Update the language in the flash config
  mwritelanguage(new_language);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setkeyclick.cgi is detected
const char * cgi_setkeyclick_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
  UINT8 Loop1UInt8;
  UINT8 new_keyclick = FLAG_OFF;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "keyclick") == 0) {
      new_keyclick = FLAG_ON;
    }
  }
  // Update the keyclick setting
  wwriteKeyclick(new_keyclick);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setdisplayscroll.cgi is detected
const char * cgi_setdisplayscroll_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
  UINT8 Loop1UInt8;
  UINT8 new_dispscroll = FLAG_OFF;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "dispscroll") == 0) {
      new_dispscroll = FLAG_ON;
    }
  }
  // Update the display scrolling setting
  wwriteScrollEnable(new_dispscroll);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /disphourmode.cgi is detected
const char * cgi_disphourmode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
  UINT8 Loop1UInt8;
  // default flag to initial 24Hr setting
  UINT8 new_hourdispmode = H24;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "disphourmode") == 0) {
      if (strcmp(pcValue[Loop1UInt8], "set24hrmode") == 0) {
        new_hourdispmode = H24;
      }
      if (strcmp(pcValue[Loop1UInt8], "set12hrmode") == 0) {
        new_hourdispmode = H12;
      }
    }
  }
  // Update the hour display mode
  wwrite_ClockHourMode(new_hourdispmode);
  // Send the index page back to the user
  return "/index.shtml";
}


// CGI handler which is run when a request for /shortsetkey.cgi is detected
const char * cgi_shortsetkey_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
  UINT8 Loop1UInt8;
  // default flag to normal operation - short press on the set key controls the date & time
  UINT8 new_setkeymode = FLAG_ON;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "setshortkey") == 0) {
      if (strcmp(pcValue[Loop1UInt8], "settime") == 0) {
        new_setkeymode = FLAG_ON;
      }
      if (strcmp(pcValue[Loop1UInt8], "setalarm") == 0) {
        new_setkeymode = FLAG_OFF;
      }
    }
  }
  // Update the set key mode
  wwriteShortSeyKey(new_setkeymode);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setchimemode.cgi is detected
const char * cgi_setchimemode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
  UINT8 Loop1UInt8;
  // default flag to initial CHIME_OFF setting
  UINT8 new_ChimeMode = CHIME_OFF;
  UINT8 new_ChimeStart = 0;
  UINT8 new_ChimeStop = 0;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "chimemode") == 0) {
      if (strcmp(pcValue[Loop1UInt8], "chimeoff") == 0) {
        new_ChimeMode = CHIME_OFF;
      }
      if (strcmp(pcValue[Loop1UInt8], "chimeday") == 0) {
        new_ChimeMode = CHIME_DAY;
      }
      if (strcmp(pcValue[Loop1UInt8], "chimeon") == 0) {
        new_ChimeMode = CHIME_ON;
      }
    }
    if (strcmp(pcParam[Loop1UInt8], "chimestart") == 0) {
      new_ChimeStart = atoi(pcValue[Loop1UInt8]);
    }
    if (strcmp(pcParam[Loop1UInt8], "chimestop") == 0) {
      new_ChimeStop = atoi(pcValue[Loop1UInt8]);
    }
  }
  wwrite_ChimeMode(new_ChimeMode);
  wwrite_ChimeStart(new_ChimeStart);
  wwrite_ChimeStop(new_ChimeStop);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setnightlightmode.cgi is detected
const char * cgi_setnightlightmode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
  UINT8 Loop1UInt8;
  // default flag to initial NIGHT_LIGHT_OFF setting
  UINT8 new_NightLightMode = NIGHT_LIGHT_OFF;
  UINT8 new_NightLightStart = 0;
  UINT8 new_NightLightStop = 0;
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "nighgtmode") == 0) {
      if (strcmp(pcValue[Loop1UInt8], "nighgtoff") == 0) {
        new_NightLightMode = NIGHT_LIGHT_OFF;
      }
      if (strcmp(pcValue[Loop1UInt8], "nighgtauto") == 0) {
        new_NightLightMode = NIGHT_LIGHT_AUTO;
      }
      if (strcmp(pcValue[Loop1UInt8], "nighgtnite") == 0) {
        new_NightLightMode = NIGHT_LIGHT_NIGHT;
      }
      if (strcmp(pcValue[Loop1UInt8], "nighgton") == 0) {
        new_NightLightMode = NIGHT_LIGHT_ON;
      }
    }
    if (strcmp(pcParam[Loop1UInt8], "nighgtstart") == 0) {
      new_NightLightStart = atoi(pcValue[Loop1UInt8]);
    }
    if (strcmp(pcParam[Loop1UInt8], "nighgtstop") == 0) {
      new_NightLightStop = atoi(pcValue[Loop1UInt8]);
    }
  }
  wwrite_NightLightMode(new_NightLightMode);
  wwrite_NightLightStart(new_NightLightStart);
  wwrite_NightLightStop(new_NightLightStop);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm0.cgi is detected
const char * cgi_htalarm_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  struct alarm my_alarm;
  UINT8 alarmnumber;
  // initialise a blank alarm
  my_alarm.FlagStatus = 0;
  my_alarm.Second = 29;
  my_alarm.Minute = 0;
  my_alarm.Hour = 12;
  my_alarm.Day = 0;
  my_alarm.Jingle = 0;
  my_alarm.Text[0] = '\0';
  // Fetch the alarm number from the first returned value - fixed start of alx, note '0' is a char and "0" is a null terminated string array
  alarmnumber = (pcParam[0][2] - '0');
  // parse the returned key and value pairs,
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    // Set the alarm number in the paramters to a default of 0 before processing, note '0' is a char and "0" is a null terminated string array
    *(pcParam[Loop1UInt8] + 2) = '0';
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0enab") == 0) {
      my_alarm.FlagStatus = 1;
    }
    // Time form element returns id= hh%3Amm) %02d
    if (strcmp(pcParam[Loop1UInt8], "al0time") == 0) {
        my_alarm.Hour = (UINT)(pcValue[Loop1UInt8][0] - '0') * 10 + (UINT)(pcValue[Loop1UInt8][1] - '0');
        my_alarm.Minute = (UINT)(pcValue[Loop1UInt8][5] - '0') * 10 + (UINT)(pcValue[Loop1UInt8][6] - '0');
        my_alarm.Second = 29;
    }
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0mond") == 0) {
      my_alarm.Day = my_alarm.Day + (1 << MON);
    }
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0tues") == 0) {
        my_alarm.Day = my_alarm.Day + (1 << TUE);
    }
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0wedn") == 0) {
        my_alarm.Day = my_alarm.Day + (1 << WED);
    }
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0thur") == 0) {
        my_alarm.Day = my_alarm.Day + (1 << THU);
    }
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0frid") == 0) {
        my_alarm.Day = my_alarm.Day + (1 << FRI);
    }
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0satu") == 0) {
        my_alarm.Day = my_alarm.Day + (1 << SAT);
    }
    // Check box form element returns id=value
    if (strcmp(pcParam[Loop1UInt8], "al0sund") == 0) {
        my_alarm.Day = my_alarm.Day + (1 << SUN);
    }
    if (strcmp(pcParam[Loop1UInt8], "al0text") == 0) {
      // Text box returns escaped characters and spaces replaced by '+', so handle this with a small conversin function
      // from https://www.eskimo.com/~scs/cclass/handouts/cgi.html
      int unesclength = 40;
      int esclength = strlen(pcValue[Loop1UInt8]);
      // Create a memory for the destination and refernce to it with a pointer
      UCHAR deststringmem[unesclength + 1];
      UCHAR *unesctext = deststringmem;
      // Check for length too long for destination string
      if (esclength > 40){
        pcValue[Loop1UInt8][39] = '\0';
        esclength = 40;
      }
      unescstring(pcValue[Loop1UInt8], esclength, unesctext, unesclength);
      strcpy(my_alarm.Text, unesctext);
    }
    // Select drop down for a jingle tune id=value
    if (strcmp(pcParam[Loop1UInt8], "al0tone") == 0) {
      my_alarm.Jingle = atoi(pcValue[Loop1UInt8]);
    }
  }
  // If no day is set, then clear the alarn enable too
  if (my_alarm.Day == 0)
    my_alarm.FlagStatus = 0;
  // Update the alarm
  wwrite_alarm(alarmnumber, my_alarm);
  return "/index.shtml";
}

// CGI handler which is run when a request for /setdisplayadclevel.cgi is detected
const char * cgi_setdisplaylevel_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  UINT16 New_adcvalue;
  // parse the returned key and value pairs,
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "mindimadclevel") == 0) {
      New_adcvalue = atoi(pcValue[Loop1UInt8]);
    }
  }
  // Now act on a button press
  wwrite_dimminlightlevel(New_adcvalue);
  // Send the index page back to the user
  return "/index.shtml";
}

// CGI handler which is run when a request for /setdisplayadclevel.cgi is detected
const char * cgi_setdisplaymode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
  UINT8 Loop1UInt8;
  UINT8 New_DimMode_Auto = FLAG_OFF;
  UINT8 New_DimMode_Manual = 0;
  // parse the returned key and value pairs,
  for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8) {
    if (strcmp(pcParam[Loop1UInt8], "dispdimmode") == 0) {
      // Default to auto mode
      New_DimMode_Auto = FLAG_ON;
      New_DimMode_Manual = 0;
      if (strcmp(pcValue[Loop1UInt8], "full") == 0) {
        New_DimMode_Auto = FLAG_OFF;
        New_DimMode_Manual = 1;
      }
      if (strcmp(pcValue[Loop1UInt8], "high") == 0) {
        New_DimMode_Auto = FLAG_OFF;
        New_DimMode_Manual = 2;
      }
      if (strcmp(pcValue[Loop1UInt8], "mid") == 0) {
        New_DimMode_Auto = FLAG_OFF;
        New_DimMode_Manual = 3;
      }
      if (strcmp(pcValue[Loop1UInt8], "low") == 0) {
        New_DimMode_Auto = FLAG_OFF;
        New_DimMode_Manual = 4;
      }
      if (strcmp(pcValue[Loop1UInt8], "dark") == 0) {
        New_DimMode_Auto = FLAG_OFF;
        New_DimMode_Manual = 5;
      }
    }
  }
  // Now act on a button press
  wwriteAutoBrightness(New_DimMode_Auto, New_DimMode_Manual);
  // Send the index page back to the user
  return "/index.shtml";
}


// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {
        "/myhostname.cgi", cgi_myhostname_handler
    },
    {
        "/mynetwork.cgi", cgi_mynetwork_handler
    },
    {
        "/setdateandtime.cgi", cgi_setdateandtime_handler
    },
    {
        "/settimezone.cgi", cgi_settimezone_handler
    },
    {
        "/setdstcountry.cgi", cgi_setdstcountry_handler
    },
    {
        "/clearntperrors.cgi", cgi_clearntperrors_handler
    },
    {
        "/setlanguage.cgi", cgi_setlanguage_handler
    },
    {
        "/setkeyclick.cgi", cgi_setkeyclick_handler
    },
    {
        "/setdisplayscroll.cgi", cgi_setdisplayscroll_handler
    },
    {
        "/disphourmode.cgi", cgi_disphourmode_handler
    },
    {
        "/shortsetkey.cgi", cgi_shortsetkey_handler
    },
    {
        "/setchimemode.cgi", cgi_setchimemode_handler
    },
    {
        "/setnightlightmode.cgi", cgi_setnightlightmode_handler
    },
    {
        "/htalarm.cgi", cgi_htalarm_handler
    },
    {
        "/setdisplayadclevel.cgi", cgi_setdisplaylevel_handler
    },
    {
        "/setdisplaymode.cgi", cgi_setdisplaymode_handler
    }
    // and update the cgi_init number of handlers
};

void cgi_init(void) {
    // We have sixteen handlers
    http_set_cgi_handlers(cgi_handlers, 16);
}

// Routine to process the returned text box values to return
// escaped characters to their ASCII equivalent and replace + with space
// walk through the text buffers
// from https://www.eskimo.com/~scs/cclass/handouts/cgi.html
void unescstring(char *src, int srclen, char *dest, int destsize) {
  char *endp = src + srclen;
  char *srcp;
  char *destp = dest;
  int nwrote = 0;
  for(srcp = src; srcp < endp; srcp++) {
    // Check for buffer overflow
    if(nwrote > destsize){
      // terminate string and leave if that's the case
      *destp = '\0';
      return;
    }
    if(*srcp == '+')
      *destp++ = ' ';
    else if(*srcp == '%') {
      int x;
      sscanf(srcp+1, "%2x", &x);
      *destp++ = x;
      srcp += 2;
    }
    else
      *destp++ = *srcp;
    nwrote++;
  }
  *destp = '\0';
  return;
}

