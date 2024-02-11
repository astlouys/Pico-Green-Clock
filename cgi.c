#include <stdlib.h>
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "Pico-Green-Clock.h"
#include "cgi.h"

// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    // Check if an request for LED has been made (/led.cgi?led=x)
    // Check first parameter only as don't care about others
    if (strcmp(pcParam[0] , "led") == 0){
        // Look at the argument to check if LED is to be turned on (x=1) or off (x=0)
        if(strcmp(pcValue[0], "0") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        else if(strcmp(pcValue[0], "1") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    // Send the index page back to the user
    return "/index.shtml";
}

// CGI handler which is run when a request for /date.cgi is detected
const char * cgi_date_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    // Check if an request for day of the month has been made (date.cgi?dayofmonth==x)
    // Check first parameter only as don't care about others
    if (strcmp(pcParam[0], "dayofmonth") == 0) {
        if(strcmp(pcValue[0], "1") == 0)
          wwrite_day_of_month(1);
        else if(strcmp(pcValue[0], "2") == 0)
          wwrite_day_of_month(2);
    }
    // Send the index page back to the user
    return "/index.shtml";
}

// CGI handler which is run when a request for /myform.cgi is detected
const char * cgi_myform_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 Loop1UInt8;
    UINT16 mvalue;
    UINT16 nvalue;
    // parse the resturned key and value pairs,
    for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8)
    {
      if (strcmp(pcParam[Loop1UInt8], "m") == 0) {
          mvalue = atoi(pcValue[Loop1UInt8]);
      }
      if (strcmp(pcParam[Loop1UInt8], "n") == 0) {
          nvalue = atoi(pcValue[Loop1UInt8]);
      }
    }
    wcalc_form(mvalue, nvalue);
    // Send the index page back to the user
    return "/index.shtml";
}

// CGI handler which is run when a request for /mynetwork.cgi is detected
const char * cgi_mynetwork_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    // Send the index page back to the user
    return "/index.shtml";
}


// CGI handler which is run when a request for /htalarm0.cgi is detected
const char * cgi_htalarm0_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 Loop1UInt8;
    struct alarm my_alarm;
    UINT8 alarmnumber;
    UINT8 textlength;
    UINT8 alarmvalue;
    // initialise a blank alarm
    my_alarm.FlagStatus = 0;
    my_alarm.Second = 29;
    my_alarm.Minute = 0;
    my_alarm.Hour = 12;
    my_alarm.Day = 0;
    my_alarm.Text[0] = '\0';
    // Fetch the alarm number from the first returned value, note '0' is a char and "0" is a null terminated string array
    alarmnumber = (pcParam[0][2] - '0');
    // parse the returned key and value pairs,
    for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8)
    {
      // Set the alarm number in the  to a default of 0 before processing, note '0' is a char and "0" is a null terminated string array
      *(pcParam[Loop1UInt8] + 2) = '0';
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00enab") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alamenab") == 0)
          my_alarm.FlagStatus = 1;
        else
          my_alarm.FlagStatus = 0;
      }
      // Time form element returns id= hh%3Amm) %02d
      if (strcmp(pcParam[Loop1UInt8], "a00time") == 0) {
          my_alarm.Hour = (UINT)(pcValue[Loop1UInt8][0] - '0') * 10 + (UINT)(pcValue[Loop1UInt8][1] - '0');
          my_alarm.Minute = (UINT)(pcValue[Loop1UInt8][5] - '0') * 10 + (UINT)(pcValue[Loop1UInt8][6] - '0');
          my_alarm.Second = 29;
      }
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00mond") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alammond") == 0)
          my_alarm.Day = my_alarm.Day + (1 << MON);
      }
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00tued") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alamtued") == 0)
          my_alarm.Day = my_alarm.Day + (1 << TUE);
      }
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00weds") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alamweds") == 0)
          my_alarm.Day = my_alarm.Day + (1 << WED);
      }
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00thur") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alamthur") == 0)
          my_alarm.Day = my_alarm.Day + (1 << THU);
      }
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00frid") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alamfrid") == 0)
          my_alarm.Day = my_alarm.Day + (1 << FRI);
      }
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00satu") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alamsatu") == 0)
          my_alarm.Day = my_alarm.Day + (1 << SAT);
      }
      // Check box form element returns id=value
      if (strcmp(pcParam[Loop1UInt8], "a00sund") == 0) {
        if (strcmp(pcValue[Loop1UInt8], "alamsund") == 0)
          my_alarm.Day = my_alarm.Day + (1 << SUN);
      }
      if (strcmp(pcParam[Loop1UInt8], "a00text") == 0) {
        // Check for length too long for destination string
        if (strlen(pcValue[Loop1UInt8]) > 40)
          pcValue[Loop1UInt8][39] = '\0';
        strcpy(my_alarm.Text, pcValue[Loop1UInt8]);
      }
    }
    wwrite_alarm(alarmnumber, my_alarm);
    return "/index.shtml";
}

// struct alarm
// {
//   UINT8 FlagStatus;
//   UINT8 Second;
//   UINT8 Minute;
//   UINT8 Hour;
//   UINT8 Day;
//   UCHAR Text[40];
// };

// CGI handler which is run when a request for /htalarm1.cgi is detected
const char * cgi_htalarm1_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm2.cgi is detected
const char * cgi_htalarm2_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm3.cgi is detected
const char * cgi_htalarm3_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm4.cgi is detected
const char * cgi_htalarm4_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm5.cgi is detected
const char * cgi_htalarm5_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm6.cgi is detected
const char * cgi_htalarm6_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm7.cgi is detected
const char * cgi_htalarm7_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// CGI handler which is run when a request for /htalarm8.cgi is detected
const char * cgi_htalarm8_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    UINT8 alarmvalue;
    return "/index.shtml";
}

// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {
        // Html request for "/led.cgi" triggers cgi_handler
        "/led.cgi", cgi_led_handler
    },
    {
        "/date.cgi", cgi_date_handler
    },
    {
        "/myform.cgi", cgi_myform_handler
    },
    {
        "/mynetwork.cgi", cgi_mynetwork_handler
    },
    {
        "/htalarm0.cgi", cgi_htalarm0_handler
    },
    {
        "/htalarm1.cgi", cgi_htalarm1_handler
    },
    {
        "/htalarm2.cgi", cgi_htalarm2_handler
    },
    {
        "/htalarm3.cgi", cgi_htalarm3_handler
    },
    {
        "/htalarm4.cgi", cgi_htalarm4_handler
    },
    {
        "/htalarm5.cgi", cgi_htalarm5_handler
    },
    {
        "/htalarm6.cgi", cgi_htalarm6_handler
    },
    {
        "/htalarm7.cgi", cgi_htalarm7_handler
    },
    {
        "/htalarm8.cgi", cgi_htalarm8_handler
    },
    // Add more functions here...
};

void cgi_init(void) {
    // We have three handler
    http_set_cgi_handlers(cgi_handlers, 13);
}
