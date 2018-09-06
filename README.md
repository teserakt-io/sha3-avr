sha3-avr
==========

A public domain implementation of **SHA-3** on 8-bit AVR
microcontroller, just because *you never know when you might need one*!

Written by Markku-Juhani O. Saarinen <markku@teserakt.io> for Teserakt.


## Prerequisites

In addition to basic things like `make` you will need (just `apt install` on 
Debian/Ubuntu) at least the AVR compiler suite `avr-gcc` and `avrdude` if
you're flashing a physical Arduino. Even though SHA3 or the test suite 
doesn't use any libraries at all (bare metal!), installing `arduino-core` 
doesn't hurt.


## Assember vs C implementation

The Assember version of "Keccak" f1600 permutation is in `f1600_avr.S` and the C equivalent is in `f1600_c.c`. You can switch between the two by changing the object file name in `Makefile`.

You'll see that, depending on compile flags, the Assembler version is about 2.5 to 5 times faster and 2 to 7 times smaller than the C version. The assembler permutation is about 1 kilobyte.

Full SHA3 and SHAKE API is provided by `sha3.h` and `sha3.c`. Note that for the assembler version the context needs to be aligned to 256 bytes. I recommend using the shared `common_sha3_ctx` rather than trying to get it from stack (or, worse still, via `malloc()`). In rare case where you need to run two hashes at the same time just add one at `common_sha3_ctx.S`. Since SRAM is scarse you can use that 256-byte scratch space for other purposes when not hashing.

The file `test_sha3.c` contains known-answer tests and you probably don't want to include that (or the serial printing and benchmarking stuff) in your project. 


## Running on SIMAVR

If you don't have an Arduino lying about, you can use
`simavr` (it's packaged for latest Ubuntu and also available at 
https://github.com/buserror/simavr) and test the code with `make sim`.

This cycle-perfect simulator simulates even the UART so the output will be
basically equivalent to real thing:

```
$ make sim
mkdir -p obj/
avr-gcc -Wall -Os -mmcu=atmega2560 -DF_CPU=16000000 -c f1600_avr.S -o obj/f1600_avr.o
(..)
simavr -v -v -v -m atmega2560 sha3-avr
Loaded 4042 .text at address 0x0
Loaded 1496 .data
UART: 0 configured to 0019 = 2403.8462 bps (x1), 8 data 1 stop
UART: Roughly 4576 usec per byte
test_sha3() = 00.
test_shake() = 00.
Run #00 145880 ticks / block.
Run #01 145864 ticks / block.
Run #02 145855 ticks / block.
^Csignal caught, simavr terminating
```

The two test zeros in the beginning meant that test vectors matched.
The ticks are for a full Keccak f1600 permutation (24 rounds).


## Running on Arduino

Plug Arduino into a USB port and do a `make flash`. Now the thing will compile 
code, flash it, and dump output via UART with stty magic to your standard 
output. Most relevant settings are in `Makefile` -- you should edit that
first if something fails. The default settings work with my Arduino Mega 2560
with Ubuntu 18.04. 

```
$ make flash
avrdude -v -c wiring -p m2560 -P /dev/ttyACM0 \
		-U flash:w:sha3-avr.hex -D

avrdude: Version 6.3
```

.. more stuff.. and then the interesting bit:

```
stty -F /dev/ttyACM0 raw icanon eof \^d 38400
cat < /dev/ttyACM0
test_sha3() = 00
test_shake() = 00
Run #00 145880 ticks / block
Run #01 145864 ticks / block
Run #02 145855 ticks / block
Run #03 145851 ticks / block
Run #04 145850 ticks / block
Run #05 145848 ticks / block
```



## Intellectual property

Copyright (c) 2018 Teserakt AG 

See [LICENSE](LICENSE) for license details (it's unlicense, essentially free). 
