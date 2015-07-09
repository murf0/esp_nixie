/* save_config.h
*
*/

#ifndef SAVE_CONFIG_H_
#define SAVE_CONFIG_H_
#include "os_type.h"
#include "user_config.h"

#define CONFIG_LOCATION 0x3C

typedef struct{
	uint32_t timeout;
} config;

void ICACHE_FLASH_ATTR saveConfig();
void ICACHE_FLASH_ATTR loadConfig();

endif
