/*
	picow_ntp_client.h

	Copyright (c) 2022 olav andrade; all rights reserved.

 */

#ifndef _NTP_CLIENT_
#define _NTP_CLIENT_

#include "pico/cyw43_arch.h"
#include "time.h"


/* Initialize the cyw43 on Pico W. */
void init_cyw43(unsigned int CountryCode);

/* Retrieve current utc time from NTP server. */
void ntp_get_time(void);

/* Initialize NTP connection. */
int ntp_init(void);

void rtc_from_ntp_epoch(time_t *epoch_seconds);	 // work outside interrupt



typedef unsigned int  UINT;   // processor-optimized.
typedef uint8_t       UINT8;
typedef uint16_t      UINT16;
typedef uint32_t      UINT32;
typedef uint64_t      UINT64;
typedef unsigned char UCHAR;

struct NTP_T_
{
  ip_addr_t        ntp_server_address;
  bool             dns_request_sent;
  struct udp_pcb  *ntp_pcb;
  absolute_time_t  ntp_test_time;
  alarm_id_t       ntp_resend_alarm;
};


typedef struct NTP_T_ NTP_T;


#define NTP_SERVER        "pool.ntp.org"
#define NTP_MSG_LEN        48
#define NTP_PORT           123
#define NTP_DELTA          2208988800   // number of seconds between 01-JAN-1900 and 01-JAN-1970.
#define NTP_TEST_TIME      (60 * 1000)
#define NTP_RESEND_TIME    (10 * 1000)


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


#endif
