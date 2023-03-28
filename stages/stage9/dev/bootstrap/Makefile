SRC_DIR = src
OBJ_DIR = .obj
BIN_DIR = bin
TARGET = bootstrap
TARGET_BIN = $(TARGET).bin

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=arm926ej-s -I. -fpie -fpic
ASFLAGS = -mcpu=arm926ej-s
LDFLAGS = # none

SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

ASM_FILES = $(wildcard $(SRC_DIR)/*.s) $(wildcard $(SRC_DIR)/**/*.s)
ASM_OBJ_FILES = $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(ASM_FILES))

LDSCRIPT = dev/ldscript.ld
MESSAGE_FILE = $(SRC_DIR)/message.gen.h

.PHONY: build
build: $(BIN_DIR)/$(TARGET) $(BIN_DIR)/$(TARGET_BIN)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/$(TARGET): $(OBJ_FILES) $(ASM_OBJ_FILES)
	mkdir -p $(dir $@)
	$(LD) -T $(LDSCRIPT) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/$(TARGET_BIN): $(BIN_DIR)/$(TARGET)
	mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@
