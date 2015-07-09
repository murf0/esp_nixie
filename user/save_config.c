/*
/* save_config.c
*/
#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"

#include "config.h"
#include "user_config.h"
#include "debug.h"

config configuration;

void ICACHE_FLASH_ATTR saveConfig() {
    INFO("Save Config\n");
    if(configuration->timeout == NULL) {
        configuration->timeout = 0;
    }
    spi_flash_erase_sector(CONFIG_LOCATION);
    spi_flash_write((CONFIG_LOCATION) * SPI_FLASH_SEC_SIZE,(uint32 *)&configuration, sizeof(config));
}

void ICACHE_FLASH_ATTR loadConfig(){
	INFO("Load Config\n");
    spi_flash_read((CONFIG_LOCATION) * SPI_FLASH_SEC_SIZE,(uint32 *)&configuration, sizeof(config));
}
