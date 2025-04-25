MCU = atmega328p
F_CPU = 16000000UL

CXX = avr-g++
OBJCOPY = avr-objcopy
AVRA = avra

CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c++17

TARGET_CPP = build/cpp.elf
TARGET_ALL = build/main.elf
TARGET_HEX = build/main.hex

SRC_CPP = $(wildcard *.cpp) $(wildcard src/*.cpp)
OBJ_CPP = $(patsubst %.cpp, build/%.o, $(notdir $(SRC_CPP)))

ASM_SRC = $(wildcard *.asm) $(wildcard src/*.asm)
ASM_FILE = $(firstword $(notdir $(ASM_SRC)))
ASM_BASE = $(basename $(ASM_FILE))

ASM_HEX = build/asm.hex
ASM_MAP = build/asm.map
ASM_LSS = build/asm.lss
ASM_EEP = build/asm.eep
ASM_OBJ = build/asm.o

all: $(TARGET_HEX)

cpp: $(TARGET_CPP)

asm: $(ASM_HEX)

# Compilação C++
build/%.o: %.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link C++
$(TARGET_CPP): $(OBJ_CPP)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Link geral
$(TARGET_ALL): $(OBJ_CPP)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TARGET_HEX): $(TARGET_ALL)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Compilação Assembly com AVRA
$(ASM_HEX): $(ASM_SRC) | build
	$(AVRA) -fI -m "$(ASM_MAP)" -l "$(ASM_LSS)" -e "$(ASM_EEP)" -d "$(ASM_OBJ)" -o "$(ASM_HEX)" $(firstword $(ASM_SRC))

build:
	mkdir -p build

clean:
	rm -rf build

flash: $(TARGET_HEX)
	avrdude -p $(MCU) -c usbtiny -U flash:w:$(TARGET_HEX):i

.PHONY: all cpp asm clean flash build

