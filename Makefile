## Makefile for The Nixe Clock driven by ESP8266

BUILD_BASE	= build
FW_BASE		= firmware

USE_HEATSHRINK ?= yes
GZIP_COMPRESSION ?= yes

# Base directory for the compiler
XTENSA_TOOLS_ROOT ?= /Volumes/esp-open-sdk/xtensa-lx106-elf/bin

# base directory of the ESP8266 SDK package, absolute
SDK_BASE        ?= /Volumes/esp-open-sdk/sdk

#Esptool.py path and port
ESPTOOL		?= /Volumes/esp-open-sdk/esptool/esptool.py
ESPPORT		?= /dev/tty.wchusbserial14*

# name for the target project
TARGET		= nixie

#Position and maximum length of espfs in flash memory
ESPFS_POS = 0x12000
ESPFS_SIZE = 0x2E000
# we create two different files for uploading into the flash
# these are the names and options to generate them
FW_BOOT_FILE	= 0x00000
FW_FILE = 0x3c000

#FW_FILE_1_ARGS	= -bo $@ -bs .text -bs .data -bs .rodata -bc -ec
FW_FILE_ARGS	= elf2image -o firmware/

MODULES	=  lib/mqtt/mqtt lib/libesphttpd/espfs lib/libesphttpd/core user lib/spidriver/driver lib/raburton/drivers/ driver

CFLAGS	= -g -O2 -Os\
		-Wl,-EL -Wpointer-arith -Wundef -Werror\
		-fno-inline-functions -ffunction-sections -fno-jump-tables -nostdlib\
		-mlongcalls -mtext-section-literals\
		-DESPFS_POS=$(ESPFS_POS) -DESPFS_SIZE=$(ESPFS_SIZE) -DESPFS_HEATSHRINK -DGZIP_COMPRESSION\
		-D__ets__ -DICACHE_FLASH -DOTA

EXTRA_INCDIR	= include include/driver lib/libesphttpd/include/ lib/mqtt/include/ lib/heatshrink/ lib/spidriver/include/ lib/raburton/drivers/

# libraries used in this project, mainly provided by the SDK
LIBS		= c gcc hal phy pp net80211 lwip wpa upgrade main ssl

# linker flags used to generate the main object file
LDFLAGS		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static -DICACHE_FLASH

# linker script used for the above linkier step
#LD_SCRIPT	= eagle.app.v6.new.2048.ld
LD_SCRIPT	= eagle.app.v6.ld


# various paths from the SDK used in this project
SDK_LIBDIR	= lib
SDK_LDDIR	= ld
SDK_INCDIR	= include include/json



# select which tools to use as compiler, librarian and linker
CC		:= $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc
AR		:= $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-ar
LD		:= $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc

####
#### no user configurable options below here
####

FW_TOOL		?= $(ESPTOOL)
SRC_DIR		:= $(MODULES)
BUILD_DIR	:= $(addprefix $(BUILD_BASE)/,$(MODULES))
SDK_LIBDIR	:= $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))
SDK_INCDIR	:= $(addprefix -I$(SDK_BASE)/,$(SDK_INCDIR))


SRC		:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC))
LIBS		:= $(addprefix -l,$(LIBS))
APP_AR		:= $(addprefix $(BUILD_BASE)/,$(TARGET)_app.a)
TARGET_OUT	:= $(addprefix $(BUILD_BASE)/,$(TARGET).out)

#LD_SCRIPT	:= $(addprefix -T$(SDK_BASE)/$(SDK_LDDIR)/,$(LD_SCRIPT))
LD_SCRIPT      := $(addprefix -T,$(LD_SCRIPT))

INCDIR	:= $(addprefix -I,$(SRC_DIR))
EXTRA_INCDIR	:= $(addprefix -I,$(EXTRA_INCDIR))
MODULE_INCDIR	:= $(addsuffix /include,$(INCDIR))

BLANKER	:= $(addprefix $(SDK_BASE)/,bin/blank.bin)

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

vpath %.c $(SRC_DIR)

define compile-objects
$1/%.o: %.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CFLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs $(TARGET_OUT) $(FW_FILE) webpages.espfs

$(FW_FILE): $(TARGET_OUT)
	$(vecho) "FW $@"
	$(Q) $(FW_TOOL) $(FW_FILE_ARGS) $(TARGET_OUT) 

$(TARGET_OUT): $(APP_AR)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $(LIBS) $(APP_AR) -Wl,--end-group -o $@

$(APP_AR): $(OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^

checkdirs: $(BUILD_DIR) $(FW_BASE)

$(BUILD_DIR):
	$(Q) mkdir -p $@

firmware:
	$(Q) mkdir -p $@

flash: $(FW_FILE) webpages.espfs
	$(Q) $(PYTHON) $(ESPTOOL) -p $(ESPPORT) write_flash $(FW_BOOT_FILE) firmware/$(FW_BOOT_FILE).bin $(FW_FILE) firmware/$(FW_FILE).bin $(ESPFS_POS) webpages.espfs

webpages.espfs: html/ mkespfsimage
	$(Q) cd html; find . | ../mkespfsimage > ../webpages.espfs; cd ..

mkespfsimage: lib/libesphttpd/espfs/mkespfsimage/
	$(Q) make -C lib/libesphttpd/espfs/mkespfsimage USE_HEATSHRINK="$(USE_HEATSHRINK)" GZIP_COMPRESSION="$(GZIP_COMPRESSION)"
	$(Q) mv lib/libesphttpd/espfs/mkespfsimage/mkespfsimage ./

resetflash:
	$(Q) $(PYTHON) $(ESPTOOL) -p $(ESPPORT) write_flash 0x7e000 $(BLANKER) $(FW_BOOT_FILE) $(BLANKER) $(FW_FILE) $(BLANKER) $(ESPFS_POS) $(BLANKER) 0x40000 $(BLANKER) 0x2c000 $(BLANKER) 

blankflash:
	$(Q) $(PYTHON) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash 0x7E000 $(SDK_BASE)/bin/blank.bin
	
htmlflash: webpages.espfs
	if [ $$(stat -f '%z' webpages.espfs) -gt $$(( $(ESPFS_SIZE) )) ]; then echo "webpages.espfs too big!"; false; fi
	$(PYTHON) $(ESPTOOL) -p $(ESPPORT) write_flash $(ESPFS_POS) webpages.espfs

test: flash
	$(vecho) screen $(ESPPORT) 115200

clean:
	$(Q) rm -f $(APP_AR)
	$(Q) rm -f $(TARGET_OUT)
	$(Q) rm -rf $(BUILD_DIR)
	$(Q) rm -rf $(BUILD_BASE)
	$(Q) rm -f $(FW_FILE_1)
	$(Q) rm -f $(FW_FILE_2)
	$(Q) rm -rf $(FW_BASE)
	$(Q) rm -f mkespfsimage
	$(Q) rm -f webpages.espfs
	$(Q) rm -f lib/esphttpd/espfs/mkespfsimage/mkespfsimage
	$(Q) rm -f lib/esphttpd/espfs/mkespfsimage/*.o

$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))