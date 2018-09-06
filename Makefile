#  Makefile
#  2018-06-28  Markku-Juhani O. Saarinen <markku@teserakt.io>

#  (c) 2018  Teserakt AG

PROJ		= sha3-avr
TARGET		= atmega2560
CPUFREQ		= 16000000
WIRE		= wiring
DEVICE		= /dev/ttyACM0
CC		= avr-gcc
OBJCOPY		= avr-objcopy
AR		= avr-ar
STRIP		= avr-strip
AVRDUDE		= avrdude

CFLAGS  	= -Wall -Os -mmcu=$(TARGET) -DF_CPU=$(CPUFREQ)

OBJS 		= obj/f1600_avr.o obj/sha3.o obj/common_sha3_ctx.o

$(PROJ):	speed.c ser_print.c test_sha3.c obj/$(PROJ).a
		$(CC) $(CFLAGS) $^ -o $@

%.hex:		%
		$(OBJCOPY) -O ihex -R .eeprom $^ $@

obj/$(PROJ).a: 	$(OBJS)
		$(AR) -ar cr $@ $^

obj/%.o: %.[cS]
		mkdir -p obj/
		$(CC) $(CFLAGS) -c $^ -o $@

obj/%.S: %.c
		$(CC) $(CFLAGS) -S $^ -o $@

clean:
		rm -rf obj $(PROJ) $(PROJ).hex $(PROJ)-*.tgz

dist:		clean
		cd ..; \
		tar cfvz $(PROJ)/$(PROJ)-`date "+%Y%m%d%H%M"`.tgz $(PROJ)/*

# simulate with simavr
sim:		$(PROJ)
		simavr -v -v -v -m $(TARGET) $(PROJ)

# flash on device, dump serial output
flash:		$(PROJ).hex
		$(AVRDUDE) -v -c $(WIRE) -p m2560 -P $(DEVICE) \
			-U flash:w:$(PROJ).hex -D
		stty -F $(DEVICE) raw icanon eof \^d 38400
		cat < $(DEVICE)
