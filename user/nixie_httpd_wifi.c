

/*
* Adaptation Murf
 */

#include "string.h"
#include "osapi.h"



#include "user_interface.h"
#include "mem.h"
#include "debug.h"

#include "mqtt.h"
#include "../lib/libesphttpd/include/httpd.h"

void ICACHE_FLASH_ATTR nixieSetSoftAP() {
        static struct softap_config config;
        wifi_softap_get_config(&config);
        INFO("Current SOFTAP APName: %s\n",config.ssid);
        os_sprintf(config.ssid,"Nixie_%08X", system_get_chip_id());
        config.ssid_len=os_strlen(config.ssid);
        wifi_softap_set_config(&config);
        INFO("New SOFTAP APName: %s\n",config.ssid);
        wifi_set_opmode(3);
}

int ICACHE_FLASH_ATTR nixieTplWlan(HttpdConnData *connData, char *token, void **arg) {
    char buff[1024];
    int x;
    static struct station_config stconf;
    if (token==NULL) return HTTPD_CGI_DONE;
    wifi_station_get_config(&stconf);
    
    os_strcpy(buff, "Unknown");
    if (os_strcmp(token, "WiFiMode")==0) {
        x=wifi_get_opmode();
        if (x==1) os_strcpy(buff, "Client");
        if (x==2) os_strcpy(buff, "SoftAP");
        if (x==3) os_strcpy(buff, "Client+AP");
    } else if (os_strcmp(token, "currSsid")==0) {
        os_strcpy(buff, (char*)stconf.ssid);
    } else if (os_strcmp(token, "WiFiPasswd")==0) {
        os_strcpy(buff, (char*)stconf.password);
    } else if (os_strcmp(token, "DeviceID")==0) {
        uint8_t client_id[16];
        os_sprintf(client_id, "%08X", system_get_chip_id());
        os_strcpy(buff, (char*)client_id);
    } else if (os_strcmp(token, "WiFiapwarn")==0) {
        x=wifi_get_opmode();
        if (x==2) {
            os_strcpy(buff, "<b>Can't scan in this mode.</b><a href=\"setmode.cgi?mode=3\">Go to STA+AP mode.</a>");
        } else if(x==1) {
            os_strcpy(buff, "<a href=\"setmode.cgi?mode=3\">Go back to Client+AP mode.</a>");
        } else {
            os_strcpy(buff, "<a href=\"setmode.cgi?mode=1\">Go to Client Only Mode</a><br /> To reset hold Button for 5 sec and release");
        }
    }
    httpdSend(connData, buff, -1);
    return HTTPD_CGI_DONE;
}



