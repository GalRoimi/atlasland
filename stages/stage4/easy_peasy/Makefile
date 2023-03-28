include ../../../dev/docker.mk

SRC_DIR = src
OBJ_DIR = .obj
BIN_DIR = bin
TARGET = easy_peasy
TARGET_BIN = $(TARGET).bin

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

GCC_LIB_DIR = /usr/lib/gcc/arm-none-eabi/9.2.1/

ASFLAGS = -mcpu=arm926ej-s
CFLAGS = -mcpu=arm926ej-s -I. -I$(MBEDTLS_INC_DIR) -Wall -Werror -O2
LDFLAGS = -L$(GCC_LIB_DIR) -lgcc

SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

ASM_FILES = $(wildcard $(SRC_DIR)/*.s) $(wildcard $(SRC_DIR)/**/*.s)
ASM_OBJ_FILES = $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(ASM_FILES))

LDSCRIPT = dev/ldscript.ld
MESSAGE_FILE = $(SRC_DIR)/message.gen.h

.PHONY: _build
_build: $(BIN_DIR)/$(TARGET) $(BIN_DIR)/$(TARGET_BIN) ## Builds stage binary

.PHONY: _clean
_clean: ## Cleans stage environment
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
	rm -rf $(MESSAGE_FILE)

$(MESSAGE_FILE):
	python3 $(ROOT_DIR)/dev/generate_cipher.py dev/message.txt > $(MESSAGE_FILE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(MESSAGE_FILE)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/$(TARGET): $(OBJ_FILES) $(ASM_OBJ_FILES)
	mkdir -p $(dir $@)
	$(LD) -T $(LDSCRIPT) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/$(TARGET_BIN): $(BIN_DIR)/$(TARGET)
	mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@
