/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* ================================================================================================================ *\
   Adapted for Pico-Green-Clock
   St-Louys, Andre - January 2023
   astlouys@gmail.com
   Revision 10-FEB-2023
   Compiler: arm-none-eabi-gcc 7.3.1
   Version 1.00

   REVISION HISTORY:
   =================
   10-FEB-2023 1.00 - Initial release.
\* ================================================================================================================ */

#include "debug.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "pico/stdlib.h"
#include "picow_ntp_client.h"
#include <string.h>
#include <time.h>



typedef unsigned int  UINT;   // processor-optimized.
typedef uint8_t       UINT8;
typedef uint16_t      UINT16;
typedef uint32_t      UINT32;
typedef uint64_t      UINT64;
typedef unsigned char UCHAR;

typedef struct NTP_T_
{
  ip_addr_t        ntp_server_address;
  bool             dns_request_sent;
  struct udp_pcb  *ntp_pcb;
  absolute_time_t  ntp_test_time;
  alarm_id_t       ntp_resend_alarm;
} NTP_T;



enum
{
  ERR_NTP_CONNECT = 1,
  ERR_NTP_ALLOC,
  ERR_NTP_PCB
} ERR_NTP;



#define FLAG_OFF           0x00
#define FLAG_ON            0x01
#define FLAG_POLL          0x02
#define NTP_SERVER        "pool.ntp.org"
#define NTP_MSG_LEN        48
#define NTP_PORT           123
#define NTP_DELTA          2208988800   // number of seconds between 01-JAN-1900 and 01-JAN-1970.
#define NTP_TEST_TIME      (60 * 1000)
#define NTP_RESEND_TIME    (10 * 1000)

NTP_T *NTPStruct;


extern uint64_t             DebugBitMask;
extern datetime_t           CurrentTime;
extern unsigned char        DayName[3][8][10];
extern UINT8                FlagNTPSuccess;

extern struct alarm
{
  UINT8 FlagStatus;
  UINT8 Second;
  UINT8 Minute;
  UINT8 Hour;
  UINT8 Day;
  UCHAR Text[40];
}Alarm;

extern struct dst_parameters
{
  UINT8  StartMonth;
  UINT8  StartDayOfWeek;
  int8_t StartDayOfMonthLow;
  int8_t StartDayOfMonthHigh;
  UINT8  StartHour;
  UINT16 StartDayOfYear;
  UINT8  EndMonth;
  UINT8  EndDayOfWeek;
  int8_t EndDayOfMonthLow;
  int8_t EndDayOfMonthHigh;
  UINT8  EndHour;
  UINT16 EndDayOfYear;
  UINT8  ShiftMinutes;
}DstParameters[25];


extern struct flash_config
{
  UCHAR  Version[6];          // firmware version number (format: "06.00" - including end-of-string).
  UINT8  CurrentYearCentile;  // assume we are in years 20xx on power-up but is adjusted when configuration is read (will your clock live long enough for a "21" ?!).
  UINT8  Language;            // language used for data display (including date scrolling).
  UCHAR  DaylightSavingTime;  // specifies how to handle the daylight saving time (see User Guide and / or clock options above).
  UINT8  TemperatureUnit;     // CELSIUS or FAHRENHEIT default value (see clock options above).
  UINT8  TimeDisplayMode;     // H24 or H12 default value (see clock options above).
  UINT8  ChimeMode;           // chime mode (Off / On / Day).
  UINT8  ChimeTimeOn;         // hourly chime will begin at this hour.
  UINT8  ChimeTimeOff;        // hourly chime will stop after this hour.
  UINT8  NightLightMode;      // night light mode (On / Off / Auto / Night).
  UINT8  NightLightTimeOn;    // default night light time On.
  UINT8  NightLightTimeOff;   // default night light time Off.
  UINT8  FlagAutoBrightness;  // flag indicating we are in "Auto Brightness" mode.
  UINT8  FlagKeyclick;        // flag for keyclick ("button-press" tone)
  UINT8  FlagScrollEnable;    // flag indicating the clock will scroll the date and temperature at regular intervals on the display.
  UINT8  FlagSummerTime;      // flag indicating the current status of Daylight Saving Time / Summer Time.
  int8_t Timezone;            // (in minutes) value to add to local time to reach UTC (Universal Time Coordinate).
  UINT8  Reserved1[48];       // reserved for future use.
  struct alarm Alarm[9];      // alarms 1 to 9 parameters. Day is a bit mask.
  UCHAR  SSID[40];            // SSID for Wi-Fi network. Note: SSID begins at position 5, so that a "footprint" can be confirmed prior to writing to flash.
  UCHAR  Password[70];        // password for Wi-Fi network. Note: password begins at position 5, for the same reason as SSID above.
  UCHAR  Reserved2[48];       // reserved for future use.
  UINT16 Crc16;               // crc16 of previous data to validate configuration.
} FlashConfig;


extern struct ntp_data
{
  /* Time-related data. */
  UINT8  CurrentDayOfWeek;
  UINT8  CurrentDayOfMonth;
  UINT8  CurrentMonth;
  UINT16 CurrentYear; 
  UINT8  CurrentYearLowPart;
  UINT8  CurrentHour;
  UINT8  CurrentMinute;
  UINT8  CurrentSecond;

  /* Generic data. */
  time_t Epoch;
  UINT8  FlagNTPResync;   // flag set to On if there is a specific reason to request an NTP update without delay.
  UINT8  FlagNTPSuccess;  // flag indicating that NTP date and time request has succeeded.
  UINT64 NTPDelta;
  UINT32 NTPErrors;       // cumulative number of errors while trying to re-sync with NTP.
  UINT64 NTPGetTime;
  UINT64 NTPLastUpdate;
  UINT32 NTPReadCycles;   // total number of re-sync cycles through NTP.
}NTPData;



/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- *\
                                                                             Function prototypes.
\* ------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* Initialize the cyw43 on Pico W. */
void init_cyw43(UINT CountryCode);

/* Call back with a DNS result. */
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg);

/* NTP request failed. */
static int64_t ntp_failed_handler(alarm_id_t id, void *user_data);

/* NTP data received. */
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

/* Make an NTP request. */
static void ntp_request(NTP_T *NTPStruct);

/* Called with results of operation. */
static void ntp_result(NTP_T* NTPStruct, int status, time_t *result);

/* Convert epoch time received from NTP to local real-time. */
void epoch_time_to_utc_time(time_t *EpochTime);



/* Return day-of-week of parameters passed as arguments. */
extern UINT8 get_day_of_week(UINT16 Year, UINT8 Month, UINT8 DayOfMonth);

/* Send a string to external monitor through Pico UART (or USB CDC). */
extern void uart_send(UINT LineNumber, UCHAR *Format, ...);




/* $PAGE */
/* $TITLE=init_cyw43() */
/* ------------------------------------------------------------------ *\
                   Initialize the cyw43 on Pico W.
        This must be done at the very beginning of the program
          to prevent the "no more output" bug of the Pico W.
\* ------------------------------------------------------------------ */
void init_cyw43(UINT CountryCode)
{
  UCHAR String[256];


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering init_cyw43()\r");


  /* --------------------------------------------------------------------------------------------------------------------------- *\
     <<< WARNING >>> There seems to be a bug in the Pico W. During the CYW43 configuration sequence, output to CDC USB will stop
                     for a while. Working through a UART (instead of USB) is a work around to the problem. USB output seems to
                     resume after a quick time. Be aware if / when you work with the code.
  \* --------------------------------------------------------------------------------------------------------------------------- */


  #ifdef RELEASE_VERSION
  /// if (cyw43_arch_init() == 0)
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_WORLDWIDE) == 0)
  #else
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_CANADA) == 0)
  #endif
  {
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "cyw43 initialized without error.\r");
  }
  else
  {
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "Error while trying to initialize cyw43.\r");
  }
}





/* $PAGE */
/* $TITLE=ntp_dns_found() */
/* ------------------------------------------------------------------ *\
                       Call back with a DNS result.
\* ------------------------------------------------------------------ */
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
  UCHAR String[256];

  NTP_T *NTPStruct = (NTP_T*)arg;


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering ntp_dns_found()\r");


  if (ipaddr)
  {
    NTPStruct->ntp_server_address = *ipaddr;
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "NTP server address: %s\r", ip4addr_ntoa(ipaddr));
    ntp_request(NTPStruct);
  }
  else
  {
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "NTP DNS request failed.\r");
    ntp_result(NTPStruct, -1, NULL);
  }

  return;
}





/* $PAGE */
/* $TITLE=ntp_failed_handler() */
/* ------------------------------------------------------------------ *\
                           NTP request failed.
\* ------------------------------------------------------------------ */
static int64_t ntp_failed_handler(alarm_id_t id, void *user_data)
{
  UCHAR String[256];


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering ntp_failed_handler()\r");

  NTP_T* NTPStruct = (NTP_T*)user_data;
  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "NTP request failed.\n");
  ntp_result(NTPStruct, -1, NULL);

  return 0;
}





/* $PAGE */
/* $TITLE=ntp_get_time() */
/* ------------------------------------------------------------------ *\
               Retrieve current UTC time from NTP server.
\* ------------------------------------------------------------------ */
void ntp_get_time(void)
{
  UCHAR String[256];

  int ReturnCode;

  absolute_time_t AbsoluteTime;
  int64_t         AbsoluteTimeDiff;


  if (DebugBitMask & DEBUG_NTP)
  {
    uart_send(__LINE__, "Entering ntp_get_time()\r");
    uart_send(__LINE__, "FlagNTPResync:                        %2.2u\r", NTPData.FlagNTPResync);
    uart_send(__LINE__, "time_us_64():               0x%10.10llX\r", time_us_64());
    uart_send(__LINE__, "NTPGetTime + NTPDelta:      0x%10.10llX\r", NTPData.NTPGetTime + NTPData.NTPDelta);
  }

  if (NTPStruct)
  {
    if (DebugBitMask & DEBUG_NTP) uart_send(__LINE__, "NTPStruct validation Ok.\r");
    if ((!NTPData.FlagNTPResync) && (NTPData.NTPGetTime + NTPData.NTPDelta > time_us_64()))
    {
      if (DebugBitMask & DEBUG_NTP)
        uart_send(__LINE__, "NTP poll...\r");
        NTPData.FlagNTPSuccess = FLAG_POLL;
        return;
    }


    AbsoluteTime = get_absolute_time();
    if (DebugBitMask & DEBUG_NTP)
    {
      uart_send(__LINE__, "NTPStruct->dns_request_sent:  %10d\r",   NTPStruct->dns_request_sent);
      uart_send(__LINE__, "AbsoluteTime:               %12lld\r",   AbsoluteTime);
      uart_send(__LINE__, "NTPStruct->ntp_test_time:     %10lld\r", to_ms_since_boot(NTPStruct->ntp_test_time));
      uart_send(__LINE__, "Time check: %lld usec.\r", absolute_time_diff_us(AbsoluteTime, NTPStruct->ntp_test_time));
    }
    if ((absolute_time_diff_us(AbsoluteTime, NTPStruct->ntp_test_time) < 0) && (NTPStruct->dns_request_sent == 0))
    {
      NTPData.NTPReadCycles++;

      /* Set alarm in case udp requests are lost (10 seconds). */
      NTPStruct->ntp_resend_alarm = add_alarm_in_ms(NTP_RESEND_TIME, ntp_failed_handler, NTPStruct, true);

      /* NOTE: cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking. You can omit them if you are
               in a callback from lwIP. Note that when using pico_cyw_arch_poll these calls are a no-op and can be omitted, but it is
               a good practice to use them in case you switch the cyw43_arch type later. */
      cyw43_arch_lwip_begin();
      {
        ReturnCode = dns_gethostbyname(NTP_SERVER, &NTPStruct->ntp_server_address, ntp_dns_found, NTPStruct);
      }
      cyw43_arch_lwip_end();


      NTPStruct->dns_request_sent = true;
      if (DebugBitMask & DEBUG_NTP)
        uart_send(__LINE__, "Sent a request to DNS server to get a NTP server IP address (return code: %d)\r", ReturnCode);


      if (ReturnCode == 0)
      {
        if (DebugBitMask & DEBUG_NTP)
          uart_send(__LINE__, "Cache DNS response.\r");

        ntp_request(NTPStruct);  // cached result.
      }
      else if (ReturnCode != ERR_INPROGRESS)
      {
        /* ERR_INPROGRESS means expect a callback. */
        if (DebugBitMask & DEBUG_NTP)
          uart_send(__LINE__, "DNS request failed.\r");
        ntp_result(NTPStruct, -1, NULL);
      }
      else
      {
        /* Other error code. */
        if (DebugBitMask & DEBUG_NTP)
          uart_send(__LINE__, "DNS request return code: %d (-5 is Ok).\r", ReturnCode);
      }
    }
    else
    {
      if (DebugBitMask & DEBUG_NTP)
      {
        uart_send(__LINE__, "Evaluate latency:   get_absolute_time: %llu   NTPStruct->ntp_test_time: %llu   NTPStruct->dns_request_sent: %u)\r", get_absolute_time(), NTPStruct->ntp_test_time, NTPStruct->dns_request_sent);
        uart_send(__LINE__, "absolute_time_diff_us(get_absolute_time(), NTPStruct->ntp_test_time): %d\r", absolute_time_diff_us(get_absolute_time(), NTPStruct->ntp_test_time));
      }
    }


#if PICO_CYW43_ARCH_POLL
    if (DebugBitMask & DEBUG_NTP)
    {
      sprintf(String, "Proceeding with CYW43 arch poll.\r");
      uart_send(__LINE__, String);
    }

    /* If using cyw43_arch_poll(), a periodic poll must be done to check for Wi-Fi driver or lwIP work that needs to be done. */
    cyw43_arch_poll();
    sleep_ms(1);
#else
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "Not using CYW43 arch poll.\r");

    /* if cyw43_arch_poll is not used, then Wi-Fi driver and lwIP work is done via interrupt in the background. 
       The sleep below is just an example of some (blocking) work you might be doing. */
    /// sleep_ms(1000);
#endif
  }
  else
  {
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "NTPStruct validation failed.\r");
  }

  return;
}





/* $PAGE */
/* $TITLE=ntp_init() */
/* ------------------------------------------------------------------ *\
                       Initialize Wi-Fi connection.
\* ------------------------------------------------------------------ */
int ntp_init(void)
{
  UCHAR String[128];

  UINT8 Loop1UInt8;
  UINT8 Loop2UInt8;

  int ReturnCode;
  
  
  ReturnCode = 0;  // assume no error on entry.


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering ntp_init()   (Data below will be displayed from CYW43 IC)...\r\r\r");

  /* Enable Wi-Fi Station mode. */
  cyw43_arch_enable_sta_mode();
  sleep_ms(250);

  if (DebugBitMask & DEBUG_NTP)
  {
    uart_send(__LINE__, "\r\r");
    uart_send(__LINE__, "Enabled station mode.\r");
  }

  /*** cmake -DPICO_BOARD=pico_w -DPICO_STDIO_USB=1 -DWIFI_SSID=<NetworkName> -DWIFI_PASSWORD=<Password> .. ***/
  if (DebugBitMask & DEBUG_NTP)
  {
    uart_send(__LINE__, "Trying to establish Wi-Fi connection.\r");
    uart_send(__LINE__, "SSID:     [%s]\r",     &FlashConfig.SSID[4]);
    uart_send(__LINE__, "Password: [%s]\r\r\r", &FlashConfig.Password[4]);
  }

  for (Loop1UInt8 = 0; Loop1UInt8 < 10; ++Loop1UInt8)
  {
    ReturnCode = cyw43_arch_wifi_connect_blocking(&FlashConfig.SSID[4], &FlashConfig.Password[4], CYW43_AUTH_WPA2_AES_PSK);
    if (ReturnCode == 0)
    {
      /* Wi-Fi connection successful, get out of for loop. */
      if (DebugBitMask & DEBUG_NTP)
      {
        uart_send(__LINE__, "\r\r");
        uart_send(__LINE__, "Wi-Fi connection succeeded.\r");
      }
      break;
    }
    else
    {
      /* In case if error, blink Pico W LED a number of time corresponding to the current retry count and try again (up to 10 times). */
      /* (This overcome a problem with the Pico W that sometimes becomes "USB-mute" after CYW43 Wi-Fi connection). */
      for (Loop2UInt8 = 0; Loop2UInt8 < Loop1UInt8; ++Loop2UInt8)
      {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(200);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(300);
      }
    
      if (DebugBitMask & DEBUG_NTP)
      {
        if (Loop1UInt8 < 10)
        {
          uart_send(__LINE__, "\r\r");
          uart_send(__LINE__, "Wi-Fi connection failure (return code: %d)   Pass %u, retrying...\r\r\r", ReturnCode, Loop1UInt8 + 1);
          /* Error while trying to establish Wi-Fi connection... Wait and try again. */
          sleep_ms(300);
        }
        else
        {
          uart_send(__LINE__, "Wi-Fi connection failure after %u retries...  aborting.\r", Loop1UInt8);
        }
      }
    }
  }


  /* If we went out of "for" loop after 10 connection failures, turn "steady On" Pico W LED to indicate fatal error. */
  if (Loop1UInt8 >= 10)
  {
    /* To overcome the inherent bug with Pico W USB CDC output after CYW43 init, use Pico's LED to visualize outcome. Steady On means error. */
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
      
    return ERR_NTP_CONNECT;
  }


  /* To overcome the inherent bug with output Pico W print in this situation, blink Pico's LED 3 times to indicate success. */
  for (Loop1UInt8 = 0; Loop1UInt8 < 3; ++Loop1UInt8)
  {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(200);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(400);
  }


  /* Allocate and initialize RAM space for NTP_T structure. */
  NTPStruct = calloc(1, sizeof(NTP_T));

  /* If calloc returned a valid pointer, proceed. */
  if (NTPStruct)
  {
    NTPStruct->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (NTPStruct->ntp_pcb)
    {
      udp_recv(NTPStruct->ntp_pcb, ntp_recv, NTPStruct);
    }
    else
    {
      ReturnCode = ERR_NTP_PCB;
    }
  }
  else
  {
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "Error while trying to allocate memory for NTP structure.\r");
    ReturnCode = ERR_NTP_ALLOC;
  }

    
  /* If return code indicates an error but calloc() returned a valid pointer, free the allocated memory. */
  if (ReturnCode && NTPStruct)
  {
    free(NTPStruct);
    NTPStruct = NULL;
  }


  /* If return code indicates an error which is not an Wi-Fi connection error, but calloc() return an error, uninitialize cyw43. */
  if (ReturnCode && (NTPStruct == 0))
  {
    if (ERR_NTP_CONNECT != ReturnCode)
    {
      cyw43_arch_deinit();
    }
  }

  return ReturnCode;
}





/* $PAGE */
/* $TITLE=ntp_recv() */
/* ------------------------------------------------------------------ *\
                             NTP data received.
\* ------------------------------------------------------------------ */
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  UCHAR String[256];

  time_t EpochTime;


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering ntp_recv()\r");

  NTP_T *NTPStruct = (NTP_T*)arg;
  uint8_t mode     = pbuf_get_at(p, 0) & 0x7;
  uint8_t stratum  = pbuf_get_at(p, 1);


  /* Check the result. */
  if (ip_addr_cmp(addr, &NTPStruct->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN && mode == 0x4 && stratum != 0)
  {
    uint8_t seconds_buf[4] = {0};
    pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
    uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
    uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
    EpochTime = seconds_since_1970;
    NTPData.Epoch = EpochTime;
    if (DebugBitMask & DEBUG_NTP)
    {
      // uart_send(__LINE__, "Seconds since 1900: %lu\r", seconds_since_1900);
      // uart_send(__LINE__, "Seconds since 1970: %lu\r", seconds_since_1970);
      // uart_send(__LINE__, "NTP_DELTA:          %lu\r", NTP_DELTA);
    }
    
    ntp_result(NTPStruct, 0, &EpochTime);
  }
  else
  {
    if (DebugBitMask & DEBUG_NTP)
      uart_send(__LINE__, "Invalid ntp response\r");
    ntp_result(NTPStruct, -1, NULL);
  }
  
  pbuf_free(p);

  return;
}





/* $PAGE */
/* $TITLE=ntp_request() */
/* ------------------------------------------------------------------ *\
                           Make an NTP request.
\* ------------------------------------------------------------------ */
static void ntp_request(NTP_T *NTPStruct)
{
  UCHAR String[256];


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering ntp_request()\r");


  // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
  // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
  // these calls are a no-op and can be omitted, but it is a good practice to use them in
  // case you switch the cyw43_arch type later.
  cyw43_arch_lwip_begin();
  {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *) p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b;
    udp_sendto(NTPStruct->ntp_pcb, p, &NTPStruct->ntp_server_address, NTP_PORT);
    pbuf_free(p);
  }
  cyw43_arch_lwip_end();

  return;
}





/* $PAGE */
/* $TITLE=ntp_result() */
/* ------------------------------------------------------------------ *\
                    Called with results of operation.
\* ------------------------------------------------------------------ */
static void ntp_result(NTP_T* NTPStruct, int status, time_t *EpochTime)
{
  UCHAR String[256];


  if (DebugBitMask & DEBUG_NTP) uart_send(__LINE__, "Entering ntp_result()\r");

  if (status == 0 && EpochTime)
  {
    if (DebugBitMask & DEBUG_NTP) uart_send(__LINE__, "Going to convert Epoch\r");
    epoch_time_to_utc_time(EpochTime);
  }

  if (NTPStruct->ntp_resend_alarm > 0)
  {
    if (DebugBitMask & DEBUG_NTP) uart_send(__LINE__, "Cancelling alarm\r");
    cancel_alarm(NTPStruct->ntp_resend_alarm);
    NTPStruct->ntp_resend_alarm = 0;
  }

  if (DebugBitMask & DEBUG_NTP) uart_send(__LINE__, "Resetting dns_request_sent\r");
  NTPStruct->ntp_test_time    = make_timeout_time_ms(NTP_TEST_TIME);
  NTPStruct->dns_request_sent = false;

  return;
}





/* $PAGE */
/* $TITLE=epoch_time_to_utc_time() */
/* ------------------------------------------------------------------ *\
          Convert epoch time received from NTP to local time.
\* ------------------------------------------------------------------ */
void epoch_time_to_utc_time(time_t *EpochTime)
{
  UCHAR String[256];

  struct tm *UtcTime;


  if (DebugBitMask & DEBUG_NTP)
    uart_send(__LINE__, "Entering epoch_time_to_utc_time(): %lu\r", *EpochTime);

  /* Adjust Epoch for local timezone, so that we will convert "Epoch local time" to "Current local time". */
  *EpochTime += (FlashConfig.Timezone * 60 * 60);  // Flash.TimeZone is given in hour.
  UtcTime = gmtime(EpochTime);
  if (DebugBitMask & DEBUG_NTP)
  {
    uart_send(__LINE__, "EpochTime adjusted for local time: %lu\r", *EpochTime);
    uart_send(__LINE__, "Date: %2d/%2.2d/%4.4d   %2d:%2.2d:%2.2d\r", UtcTime->tm_mday, UtcTime->tm_mon + 1, UtcTime->tm_year + 1900, UtcTime->tm_hour, UtcTime->tm_min, UtcTime->tm_sec);
  }
  

  NTPData.CurrentDayOfMonth = UtcTime->tm_mday;
  NTPData.CurrentMonth      = UtcTime->tm_mon + 1;	     // 0 -> 11 converted to 1 -> 12
  NTPData.CurrentYear       = UtcTime->tm_year + 1900;
  NTPData.CurrentHour       = UtcTime->tm_hour;
  NTPData.CurrentMinute     = UtcTime->tm_min;
  NTPData.CurrentSecond     = UtcTime->tm_sec++;        // compensate for time elapsed between NTP request and real-time IC complete setup.

  NTPData.NTPGetTime        = time_us_64();

  /* If seconds is near a minute change, postpone the update to prevent the need for an atomic operation requiring a critical section
     that doesn't goes along well with our millisecond callback. */
  if (NTPData.CurrentSecond > 57)
  {
    if (DebugBitMask & DEBUG_NTP)
    {
      uart_send(__LINE__, "NTP time seconds are closed to minute change (%2u).\r", NTPData.CurrentSecond);
      uart_send(__LINE__, "Postpone the update to prevent the need for a critical section.\r");
    }

    return;  // main program will make a retry in a few minutes.
  }

  NTPData.FlagNTPSuccess = FLAG_ON;

  /* Get current day-of-week, given the day-of-month, month and year. */
  NTPData.CurrentDayOfWeek  = get_day_of_week(NTPData.CurrentYear, NTPData.CurrentMonth, NTPData.CurrentDayOfMonth);

  if (DebugBitMask & DEBUG_NTP)
  {
    uart_send(__LINE__, "Convert Epoch [%llu] to current time. FlashConfig.Timezone: %d hour (%d in seconds).\r", *EpochTime, FlashConfig.Timezone, (int32_t)FlashConfig.Timezone * 60 * 60);
    uart_send(__LINE__, "NTP time:\r");
    uart_send(__LINE__, "DoW: %s   Date: %2.2u/%2.2u/%4.4u   Time: %2.2u:%2.2u:%2.2u\r", DayName[FlashConfig.Language][NTPData.CurrentDayOfWeek], NTPData.CurrentDayOfMonth, NTPData.CurrentMonth, NTPData.CurrentYear, NTPData.CurrentHour, NTPData.CurrentMinute, NTPData.CurrentSecond);
  }

  return;
}
