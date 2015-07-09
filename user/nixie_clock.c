/* 
 * By Murf
*/

#include "ets_sys.h"
#include "osapi.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"

#include "nixie_clock.h"
#include "sntp.h"
#include "ds1307.h"

#undef DS1307_12HOUR_FLAG

struct tm *nixietime;
uint32 BINTIME;
uint8 left_encoded=8;

void ICACHE_FLASH_ATTR nixieClockCb() {
    os_printf("TEST");
}

//Called from the wifi callback, Set the clock from NTP
void ICACHE_FLASH_ATTR NIXE_Set_Clock_Wifi() {
    sntp_setservername(0, "se.pool.ntp.org"); // set server 0 by domain name
    sntp_setservername(1, "0.europe.pool.ntp.org"); // set server 1 by domain name
    sntp_set_timezone(+1);
    sntp_init();
    
    uint32 current_stamp;
    current_stamp = sntp_get_current_timestamp();
    os_printf("sntp: %d, %s \n",current_stamp, sntp_get_real_time(current_stamp));
    //Update the External RTC via i2c
    
    // init tm Struct
    /*
    
    
    time->tm_sec
    time->tm_min
    time->tm_hour
    time->tm_wday
    time->tm_mday
    time->tm_mon
    time->tm_year
    */
    time_t unix = sntp_get_current_timestamp();

    //nixietime = gmtime(&unix); //gmtime not in sdk..
    tm_sec +
    tm_min*60
    tm_hour*3600
    tm_yday*86400 +
    (tm_year-70)*31536000 + ((tm_year-69)/4)*86400 -
    ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400
    
    ds1307_setTime(nixietime);
    
}

void ICACHE_FLASH_ATTR addToBintime(int value) {
    while (value > 0) {
        int digit = value % 10;
        //add digit to BINTIME
        BINTIME=BINTIME << 4; //Move bits over to make place for 4 more BCD encoded.
        digit=((digit / 10) * 16) + (digit % 10);
        BINTIME = BINTIME | digit;
        value /= 10;
        left_encoded--;
    }
}
void ICACHE_FLASH_ATTR padBintime() {
    do {
        left_encoded--;
        BINTIME=BINTIME << 4;
    } while(left_encoded>0);
}

void ICACHE_FLASH_ATTR NIXE_Timercb() {
    ds1307_getTime(nixietime);
    int sec=nixietime->tm_sec;
    addToBintime(sec);
    int min=nixietime->tm_min;
    addToBintime(min);
    int hour=nixietime->tm_hour;
    addToBintime(hour);
    //BCD 1111 1111 - 1111 1111 - 1111 1111 - 1111 1111
    //Last 8 bits are led.

    //nixietime->tm_year
    //uint32 BINTIME = 0b;
    // Display mytime
    padBintime();
    spi_tx32(HSPI, BINTIME);
    left_encoded=8;
}
void ICACHE_FLASH_ATTR NIXE_year() {
    ds1307_getTime(nixietime);
    int year=nixietime->tm_year;
    addToBintime(year);
    padBintime();
    spi_tx32(HSPI, BINTIME);
    left_encoded=8;
}

/*

struct tm {
    int tm_sec;          seconds,  range 0 to 59
    int tm_min;          minutes, range 0 to 59
    int tm_hour;         hours, range 0 to 23
    int tm_mday;         day of the month, range 1 to 31
    int tm_mon;          month, range 0 to 11
    int tm_year;         The number of years since 1900
    int tm_wday;         day of the week, range 0 to 6
    int tm_yday;         day in the year, range 0 to 365
    int tm_isdst;        daylight saving time
};
*/