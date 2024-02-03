
// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_date_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_alarm_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);


// cgi handler initialiser
 void cgi_init();

