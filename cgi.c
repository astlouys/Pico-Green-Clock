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
    // Check if an request for LED has been made (/alarm.cgi?<s/c>alarm==x)
    // Check first parameter only as don't care about others
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
    UINT8 alarmvalue;
    // Check if an request for LED has been made (/alarm.cgi?<s/c>alarm==x)
    // Check first parameter only as don't care about others
    // if (strcmp(pcParam[0], "salarm") == 0) {
    //     alarmvalue = atoi(pcValue[0]);
    //     wwrite_alarm(alarmvalue, 1);
    // }
    // if (strcmp(pcParam[0], "calarm") == 0) {
    //     alarmvalue = atoi(pcValue[0]);
    //     wwrite_alarm(alarmvalue, 0);
    // }
    // Send the index page back to the user
    return "/index.shtml";
}

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
