
// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_date_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_myform_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_mynetwork_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_htalarm_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);


// cgi handler initialiser
 void cgi_init();

// Text handling
char *unescstring(char *src, int srclen, char *dest, int destsize);
static int xctod(int c);
