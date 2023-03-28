#START_REMOVE_SECTION
include ../../../dev/docker.mk
#END_REMOVE_SECTION
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
RL = arm-none-eabi-ranlib
OBJCOPY = arm-none-eabi-objcopy

DEV_DIR = dev
SRC_DIR = src
OBJ_DIR = .obj
BIN_DIR = bin
TARGET = sweating_buckets

MBEDTLS_ROOT_DIR = $(ROOT_DIR)/modules/mbedtls/
MBEDTLS_INC_DIR = $(MBEDTLS_ROOT_DIR)/include/
MBEDTLS_LIB_DIR = $(MBEDTLS_ROOT_DIR)/library/
MBEDTLS_LIB_FILE = $(MBEDTLS_LIB_DIR)/libmbedcrypto.a

GCC_LIB_DIR = /usr/lib/gcc/arm-none-eabi/9.2.1/
C_LIB_DIR = /usr/lib/arm-none-eabi/newlib/
NOSYS_LIB_DIR = /usr/lib/arm-none-eabi/newlib/

ASFLAGS = -mcpu=arm926ej-s
CFLAGS = -mcpu=arm926ej-s -I. -I$(MBEDTLS_INC_DIR) -Wall -Werror -O2
LDFLAGS = -L$(GCC_LIB_DIR) -L$(C_LIB_DIR) -L$(NOSYS_LIB_DIR) -L$(MBEDTLS_LIB_DIR) -lgcc -lmbedcrypto -lc -lnosys

SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

ASM_FILES = $(wildcard $(SRC_DIR)/*.s) $(wildcard $(SRC_DIR)/**/*.s)
ASM_OBJ_FILES = $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(ASM_FILES))

.PHONY: _build
_build: $(BIN_DIR)/$(TARGET).bin ## Builds stage binary

.PHONY: _clean
_clean: ## Cleans stage environment
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
#START_REMOVE_SECTION
	rm -rf $(MESSAGE_FILE)
#END_REMOVE_SECTION
	CC=$(CC) RL=$(RL) AR=$(AR) $(MAKE) -C $(MBEDTLS_ROOT_DIR) clean


#START_REMOVE_SECTION

MESSAGE_FILE = $(SRC_DIR)/message.gen.h

$(MESSAGE_FILE):
	python3 $(ROOT_DIR)/dev/generate_cipher.py ../password.txt > $(MESSAGE_FILE)

#END_REMOVE_SECTION

$(MBEDTLS_LIB_FILE):
	cp $(DEV_DIR)/mbedtls_config.h $(MBEDTLS_INC_DIR)/mbedtls/
	CC=$(CC) RL=$(RL) AR=$(AR) $(MAKE) -C $(MBEDTLS_ROOT_DIR) lib

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c  | $(MESSAGE_FILE)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BIN_DIR)/$(TARGET): $(OBJ_FILES) $(ASM_OBJ_FILES) | $(MBEDTLS_LIB_FILE)
	mkdir -p $(dir $@)
	$(LD) -T $(DEV_DIR)/ldscript.ld $^ $(LDFLAGS) -o $@

$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET)
	$(OBJCOPY) -O binary $< $@
