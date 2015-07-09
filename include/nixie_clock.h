#ifndef NIXIE_CLOCK_H
#define NIXIE_CLOCK_H

#include "driver/spi.h"
#include "user_config.h"

typedef struct{
    uint8_t mqtt_host[64];
    uint32_t mqtt_port;
    uint8_t mqtt_user[32];
    uint8_t mqtt_pass[32];
    uint8_t client_id[16];
    uint32_t mqtt_keepalive;
    uint8_t security;
} NIXIE_SETTINGS;

#endif
