include ../../../dev/docker.mk

SRC_DIR = src
OBJ_DIR = .obj
BIN_DIR = bin
TARGET = baby_steps
TARGET_BIN = $(TARGET).bin

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=arm926ej-s -I.
ASFLAGS = -mcpu=arm926ej-s
LDFLAGS = # none

SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

ASM_FILES = $(wildcard $(SRC_DIR)/*.s) $(wildcard $(SRC_DIR)/**/*.s)
ASM_OBJ_FILES = $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(ASM_FILES))

LDSCRIPT = dev/ldscript.ld
LDSCRIPT_GEN = dev/ldscript.gen.ld
MESSAGE_FILE = $(SRC_DIR)/message.gen.h

.PHONY: _build
_build: $(BIN_DIR)/$(TARGET) $(BIN_DIR)/$(TARGET_BIN) ## Builds stage binary

.PHONY: _clean
_clean: ## Cleans stage environment
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
	rm -rf $(MESSAGE_FILE)
	rm -rf $(LDSCRIPT_GEN)

$(MESSAGE_FILE):
	python3 $(ROOT_DIR)/dev/generate_cipher.py dev/message.txt > $(MESSAGE_FILE)

$(LDSCRIPT_GEN): $(LDSCRIPT)
	$(CC) -E -P -x c -DSECRET=0x$(shell cat ../password.txt) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(MESSAGE_FILE)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/$(TARGET): $(OBJ_FILES) $(ASM_OBJ_FILES) | $(LDSCRIPT_GEN)
	mkdir -p $(dir $@)
	$(LD) -T $(LDSCRIPT_GEN) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/$(TARGET_BIN): $(BIN_DIR)/$(TARGET)
	mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@
