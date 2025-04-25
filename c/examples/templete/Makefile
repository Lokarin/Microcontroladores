MCU = atmega328p
F_CPU = 16000000UL

CC      = avr-gcc
CXX     = avr-g++
OBJCOPY = avr-objcopy
AVRA    = avra

CFLAGS   = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)
CXXFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)

TARGET   = main

ASM_SRC  = $(wildcard $(TARGET).asm)
C_SRC    = $(wildcard $(TARGET).c)
CPP_SRC  = $(wildcard $(TARGET).cpp)

BUILD    = build

# Arquivos para asm
ASM_HEX  = $(BUILD)/$(TARGET).hex
ASM_OBJ  = $(BUILD)/$(TARGET).obj
ASM_EEP  = $(BUILD)/$(TARGET).eep
ASM_MAP  = $(BUILD)/$(TARGET).map
ASM_LSS  = $(BUILD)/$(TARGET).lst

# Arquivos para C
C_OBJ    = $(BUILD)/$(TARGET)_c.o
C_ELF    = $(BUILD)/$(TARGET)_c.elf
C_HEX    = $(BUILD)/$(TARGET)_c.hex

# Arquivos para C++
CPP_OBJ  = $(BUILD)/$(TARGET)_cpp.o
CPP_ELF  = $(BUILD)/$(TARGET)_cpp.elf
CPP_HEX  = $(BUILD)/$(TARGET)_cpp.hex

.PHONY: all asm c cpp clean help

all: help

asm: $(ASM_HEX)

c: $(C_HEX)

cpp: $(CPP_HEX)

$(BUILD):
	mkdir -p $(BUILD)

# Compilação Assembly com AVRA
$(ASM_HEX): $(ASM_SRC) | $(BUILD)
	$(AVRA) -fI -m "$(ASM_MAP)" -l "$(ASM_LSS)" -e "$(ASM_EEP)" -d "$(ASM_OBJ)" -o "$(ASM_HEX)" $(firstword $(ASM_SRC))

# Compilação C
$(C_OBJ): $(C_SRC) | $(BUILD)
	$(CC) $(CFLAGS) -c $(C_SRC) -o $(C_OBJ)

$(C_ELF): $(C_OBJ)
	$(CC) $(CFLAGS) $(C_OBJ) -o $(C_ELF)

$(C_HEX): $(C_ELF)
	$(OBJCOPY) -O ihex $(C_ELF) $(C_HEX)

# Compilação C++
$(CPP_OBJ): $(CPP_SRC) | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $(CPP_SRC) -o $(CPP_OBJ)

$(CPP_ELF): $(CPP_OBJ)
	$(CXX) $(CXXFLAGS) $(CPP_OBJ) -o $(CPP_ELF)

$(CPP_HEX): $(CPP_ELF)
	$(OBJCOPY) -O ihex $(CPP_ELF) $(CPP_HEX)

clean:
	rm -rf $(BUILD)

help:
	@echo "make asm   - Compila projeto em Assembly (.asm com avra)"
	@echo "make c     - Compila projeto em C"
	@echo "make cpp   - Compila projeto em C++"
	@echo "make clean - Remove arquivos gerados"

