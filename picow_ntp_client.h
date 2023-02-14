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

#endif