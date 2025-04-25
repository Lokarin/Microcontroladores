MCU = atmega328p
F_CPU = 16000000UL

CXX = avr-g++
OBJCOPY = avr-objcopy

CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c++17

SRC = $(wildcard *.cpp) $(wildcard src/*.cpp)
OBJ = $(patsubst %.cpp, build/%.o, $(notdir $(SRC)))
TARGET = build/main.elf
HEX = build/main.hex

all: $(HEX)

build/%.o: %.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

$(HEX): $(TARGET)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -rf build

flash: $(HEX)
	avrdude -p $(MCU) -c usbtiny -U flash:w:$(HEX):i

.PHONY: all clean flash

