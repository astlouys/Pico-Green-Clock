
// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_myhostname_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_mynetwork_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setdateandtime_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_htalarm_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setdisplaylevel_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);


// cgi handler initialiser
 void cgi_init();

// Text handling
void unescstring(char *src, int srclen, char *dest, int destsize);
