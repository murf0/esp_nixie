#define BTNGPIO                     0

static ETSTimer nixieDebounceTimer,nixieResetTimer;

void ICACHE_FLASH_ATTR init_interupt(void);
void ICACHE_FLASH_ATTR nixieDebounce();