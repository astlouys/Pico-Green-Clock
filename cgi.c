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
    my_alarm.Text[0] = '\0';
    // Fetch the alarm number from the first returned value - fixed start of alx, note '0' is a char and "0" is a null terminated string array
    alarmnumber = (pcParam[0][2] - '0');
    // parse the returned key and value pairs,
    for (int Loop1UInt8 = 0; Loop1UInt8 < iNumParams; ++Loop1UInt8)
    {
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
    }
    // If no day is set, then clear the alarn enable too
    if (my_alarm.Day == 0)
      my_alarm.FlagStatus = 0;
    // Update the alarm
    wwrite_alarm(alarmnumber, my_alarm);
    return "/index.shtml";
}

// http://172.22.42.169/htalarm.cgi?al0enab=alamenab&al0time=07%3A20&al0text=Time+to+get+up%21%21%21

// struct alarm
// {
//   UINT8 FlagStatus;
//   UINT8 Second;
//   UINT8 Minute;
//   UINT8 Hour;
//   UINT8 Day;
//   UCHAR Text[40];
// };



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
        "/htalarm.cgi", cgi_htalarm_handler
    }

    // Add more functions here...
    // and update the cgi_init number of handlers
};

void cgi_init(void) {
    // We have three handler
    http_set_cgi_handlers(cgi_handlers, 5);
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

