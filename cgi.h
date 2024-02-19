
// CGI handler which is run when a request for a form action is triggered
const char * cgi_myhostname_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_mynetwork_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setdateandtime_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_settimezone_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setdstcountry_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_clearntperrors_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setlanguage_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setkeyclick_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setdisplayscroll_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_disphourmode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_shortsetkey_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setchimemode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setnightlightmode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_htalarm_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setdisplaylevel_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_setdisplaymode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

// cgi handler initialiser
 void cgi_init();

// Text handling
void unescstring(char *src, int srclen, char *dest, int destsize);
