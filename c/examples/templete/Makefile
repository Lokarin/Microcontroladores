MCU = atmega328p
F_CPU = 16000000UL

CC      = avr-gcc
CXX     = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRA    = avra

CFLAGS   = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -MMD
CXXFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -MMD

TARGET    = main
APP_C     = application_c
APP_CPP   = application_cpp

ASM_SRC   = $(wildcard $(TARGET).asm)
C_SRC     = $(wildcard $(TARGET).c)
CPP_SRC   = $(wildcard $(TARGET).cpp)

BUILD     = build

# ASM
ASM_HEX   = $(BUILD)/$(TARGET).hex
ASM_OBJ   = $(BUILD)/$(TARGET).obj
ASM_EEP   = $(BUILD)/$(TARGET).eep
ASM_MAP   = $(BUILD)/$(TARGET).map
ASM_LSS   = $(BUILD)/$(TARGET).lst

# C
C_OBJ     = $(BUILD)/$(TARGET)_c.o
C_ELF     = $(BUILD)/$(APP_C).elf
C_HEX     = $(BUILD)/$(APP_C).hex
C_BIN     = $(BUILD)/$(APP_C).bin
C_MAP     = $(BUILD)/$(APP_C).map
C_LST     = $(BUILD)/$(TARGET)_c.lst
C_DEP     = $(BUILD)/$(TARGET)_c.d

# CPP
CPP_OBJ   = $(BUILD)/$(TARGET)_cpp.o
CPP_ELF   = $(BUILD)/$(APP_CPP).elf
CPP_HEX   = $(BUILD)/$(APP_CPP).hex
CPP_BIN   = $(BUILD)/$(APP_CPP).bin
CPP_MAP   = $(BUILD)/$(APP_CPP).map
CPP_LST   = $(BUILD)/$(TARGET)_cpp.lst
CPP_DEP   = $(BUILD)/$(TARGET)_cpp.d

.PHONY: all asm c cpp clean help

all: help

asm: $(ASM_HEX)

c: $(C_HEX) $(C_BIN) $(C_MAP) $(C_LST)

cpp: $(CPP_HEX) $(CPP_BIN) $(CPP_MAP) $(CPP_LST)

$(BUILD):
	mkdir -p $(BUILD)

# ASM
$(ASM_HEX): $(ASM_SRC) | $(BUILD)
	$(AVRA) -fI -m "$(ASM_MAP)" -l "$(ASM_LSS)" -e "$(ASM_EEP)" -d "$(ASM_OBJ)" -o "$(ASM_HEX)" $(firstword $(ASM_SRC))

# C
ifneq ("$(C_SRC)","")
$(C_OBJ): $(C_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $(C_SRC) -o $(C_OBJ)

$(C_ELF): $(C_OBJ)
	$(CC) $(CFLAGS) $(C_OBJ) -o $(C_ELF) -Wl,-Map=$(C_MAP)

$(C_HEX): $(C_ELF)
	$(OBJCOPY) -O ihex $(C_ELF) $(C_HEX)

$(C_BIN): $(C_ELF)
	$(OBJCOPY) -O binary $(C_ELF) $(C_BIN)

$(C_LST): $(C_ELF)
	$(OBJDUMP) -S $(C_ELF) > $(C_LST)
endif

# C++
ifneq ("$(CPP_SRC)","")
$(CPP_OBJ): $(CPP_SRC) | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $(CPP_SRC) -o $(CPP_OBJ)

$(CPP_ELF): $(CPP_OBJ)
	$(CXX) $(CXXFLAGS) $(CPP_OBJ) -o $(CPP_ELF) -Wl,-Map=$(CPP_MAP)

$(CPP_HEX): $(CPP_ELF)
	$(OBJCOPY) -O ihex $(CPP_ELF) $(CPP_HEX)

$(CPP_BIN): $(CPP_ELF)
	$(OBJCOPY) -O binary $(CPP_ELF) $(CPP_BIN)

$(CPP_LST): $(CPP_ELF)
	$(OBJDUMP) -S $(CPP_ELF) > $(CPP_LST)
endif

# Dependências
-include $(BUILD)/*.d

clean:
	rm -rf $(BUILD)

help:
	@echo "make asm   - Compila projeto em Assembly (.asm com avra)"
	@echo "make c     - Compila projeto em C (gera .hex .elf .bin .map .lst .d)"
	@echo "make cpp   - Compila projeto em C++ (gera .hex .elf .bin .map .lst .d)"
	@echo "make clean - Remove arquivos gerados"

