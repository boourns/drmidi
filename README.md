drmidi
======

AVR firmware for Boss DR-110 Drum Machine

Prerequisites:
  avr-gcc
  avrlib (GPL, available http://gitorious.org/avrlib)

Source code for making an AVR ATMEGA32 microprocessor act as a MIDI->trigger device for the Boss DR-110 drum machine.

The code was tested on a DR-110 with CPU board removed and only had an AVR attached generating triggers in response to MIDI events.

The DR-110 internally uses negative reference voltages for it's triggers so to power the AVR, connect DR-110 -6v to AVR GND and DR-110 GND to AVR +5v.  My DR-110 -6v was around -5.5v and did not damage the AVR.

The MIDI->trigger mapping follows.  PORTC is used for trigger outputs.

	unsigned char drums[NUM_DRUMS] = {
	DRUM_BD, // C0
	DRUM_SD,  // C1
	DRUM_HCP, // C2
	DRUM_CH, // C3
	DRUM_OH, // C4
	DRUM_CRASH, // C5
	};

The MIDI events for each trigger are defined in gmdrums.h:

	#define DRUM_BD    35
	#define DRUM_RIM   37
	#define DRUM_SD    38
	#define DRUM_HCP   39
	#define DRUM_LT    41
	#define DRUM_CH    42
	#define DRUM_MT    45
	#define DRUM_OH    46
	#define DRUM_HT    48
	#define DRUM_CRASH 49
	#define DRUM_RIDE  51
	#define DRUM_HC    62
	#define DRUM_LC    63
	#define DRUM_TB    65
	#define DRUM_HCB   67
	#define DRUM_LCB   68

Attach a typical MIDI circuit to the serial input of the AVR.  You will need to also use an external crystal for osc input to have a clock solid enough for MIDI.



