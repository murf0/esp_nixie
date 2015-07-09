

/*
* Adaptation Murf
 */

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "debug.h"
#include "mqtt.h"
#include "gpio.h"
#include "mem.h"

#include "../lib/libesphttpd/include/httpd.h"
#include "nixie_mqtt.h"
#include "nixie_httpd_wifi.h"
#include "nixie_interrupt.h"


int btnDebounceIncrement=0;
int resetCnt=0;

void nixieDebounceCb() {
    char temp[64];
    if(!GPIO_INPUT_GET(BTNGPIO)) {
        INFO("****************\nTIMER: CHECKDEBOUNCE BTN0 Pressed\n****************\n");
        //Send unRegistermsgs
        INFO("Button: Send UNREGISTER to: %s\r\n", registertopic);
        os_sprintf(temp,"{\"UNREGISTER\": \"%s\"}\r\n", mqttcfg.client_id);
        MQTT_Publish(&mqttClient, registertopic, temp, os_strlen(temp), 2, 0);
        os_timer_disarm(&nixieDebounceTimer);
        os_delay_us(1000000);
        //os_timer_setfn(&nixieDebounceTimer, (os_timer_func_t *)nixieDebounceCb, NULL);
        os_timer_arm(&nixieDebounceTimer, 100, 1);
    }
}

static void ICACHE_FLASH_ATTR nixieResetTimerCb(void *arg) {
    if (!GPIO_INPUT_GET(BTNGPIO)) {
        resetCnt++;
    } else {
        if (resetCnt>=6) { //3 sec pressed
            wifi_station_disconnect();
            wifi_set_opmode(0x3); //reset to AP+STA mode
            os_printf("Reset to AP mode. Restarting system...\n");
            system_restart();
        }
        resetCnt=0;
    }
}
void ICACHE_FLASH_ATTR btnInitIO(void) {
    int initstate = 0;
    //SDK gpio init
    gpio_init();
    
    //Set GPIOS to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
    // PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
    // PIN_FUNC_SELECT(PERIPHS_IO_MUX_XPD_DCDC_U, FUNC_GPIO16);
    
    GPIO_OUTPUT_SET(12,0); //SH_CP, ClockPin
    GPIO_OUTPUT_SET(13,0); //DS, DataPin
    GPIO_OUTPUT_SET(14,0); //ST_CP, LatchPin
    //gpio_output_set(0, 0, (1<<LEDGPIO), (1<<BTNGPIO));
    //GPIO_OUTPUT_SET(BTNGPIO,1);  //GPIO0 set high to detect.. i dunno. Going to check
    
    //Do Reset functionality here instead
    os_timer_disarm(&nixieResetTimer);
    os_timer_setfn(&nixieResetTimer, nixieResetTimerCb, NULL);
    os_timer_arm(&nixieResetTimer, 500, 1);
    // Do single press timer here
    INFO("Button Arm timer\n");
    os_timer_disarm(&nixieDebounceTimer);
    os_timer_setfn(&nixieDebounceTimer, (os_timer_func_t *)nixieDebounceCb, NULL);
    os_timer_arm(&nixieDebounceTimer, 100, 1);
    
}
