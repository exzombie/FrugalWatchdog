PRG            = FrugalWatchdog
OBJ            = main.o softuart.o
MCU_TARGET     = attiny45
AVRDUDE_PORT   = /dev/ttyACM0
AVRDUDE_TARGET = t45
AVRDUDE_PRG    = arduino
OPTIMIZE       = -Os -flto -fuse-linker-plugin
DEFS           = -DF_CPU=8000000UL
LIBS           = 
AVRDUDE        = avrdude -P $(AVRDUDE_PORT) -b 19200 -c $(AVRDUDE_PRG) -p $(AVRDUDE_TARGET)

# Enable serial programming, 8Mhz
HFUSE          = 0xdf
LFUSE          = 0xe2
EFUSE          = 0xff

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
main.o: FastPin.h RecvCmd.h
softuart.o: softuart.h

# You should not have to change anything below here.

CXX            = avr-g++
CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = --std=gnu11 $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override CXXFLAGS      = --std=gnu++14 -fno-rtti -fno-exceptions -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = $(OPTIMIZE) -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

$(PRG).elf: $(OBJ)
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# dependency:
#demo.o: demo.c iocompat.h

.PHONY: upload fuse
upload: $(PRG).hex
	$(AVRDUDE) -U flash:w:$^:i

fuses: $(PRG).hex
	$(AVRDUDE) -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m -U efuse:w:$(EFUSE):m

clean:
	rm -rf $(OBJ) $(PRG).elf *.eps *.png *.pdf *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

%.lst: %.elf
	$(OBJDUMP) -h -S -D $< > $@

lst:  $(PRG).lst

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
	|| { echo empty $@ not generated; exit 0; }

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@
