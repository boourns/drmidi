// dr-110 midi->trigger code
// <c> Tom Burns 2008 
// Released under MIT license

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "global.h"
#include "uart.h"
#include "timer.h"
#include "midi.h"
#include "gm-drums.h"

unsigned char pos;

#define NUM_DRUMS 6

unsigned char drums[NUM_DRUMS] = {
DRUM_BD, // C0
DRUM_SD,  // C1
DRUM_HCP, // C2
DRUM_CH, // C3
DRUM_OH, // C4
DRUM_CRASH, // C5
};

// shared counter for accents, -1
unsigned char accLen;
#define TRIG_ACC 7
#define TRIG_THRESHOLD 100

// not a seperate drum but gets triggered at end of clap
#define TRIG_NOISE 6

// index in drum array for clap
#define CLAP_INDEX 2

unsigned char trigLen[NUM_DRUMS];

unsigned char noteToDrum(unsigned char note)
{
  unsigned char i;

  for (i = 0; i < NUM_DRUMS; i++)
  {
    if (drums[i] == note)
       return i+1; 
  }
  return 0;
}

void updateTriggers(void)
{
  unsigned char i;
  for (i = 0; i < NUM_DRUMS; i++)
  {
    if (trigLen[i] > 0)
    {
      trigLen[i]-=2;
      if (i == CLAP_INDEX)
      {
        if (trigLen[i] == 120 || trigLen[i] == 80 || trigLen[i] == 40)
        {
           // bring clap trig high 10ms
           PORTC|=(1<<i);
        }
        else if (trigLen[i] == 100 || trigLen[i] == 60)
        {
           // clap trig low 10ms
           PORTC&=~(1<<i);
        }
        else if (trigLen[i] == 20)
        {
           // last trigger -> noise low 10ms
           PORTC&=~(1<<TRIG_NOISE);
        }
        else if (trigLen[i] == 0)
        {
           // end with noise high
           PORTC|=(1<<TRIG_NOISE);
        }
      } 
      else if (trigLen[i] == 0)
      {
         // bring pin back high
         PORTC|=(1<<i);
      }
    }
  }
  if (accLen > 0)
  { 
     accLen--;
     if (accLen == 0)
     {
        // return accent trigger to high.
        PORTC|=(1<<TRIG_ACC);
     }
  }
}

void processMidiEvent(unsigned char evt0, unsigned char evt1, unsigned char evt2)
{
  unsigned char idx;
  if ((evt0 & 0xF0) == MIDI_NOTE_ON)
  {
     // if velocity is 0 then fake it as a note off
     if (evt2 != 0)
     {
        idx = noteToDrum(evt1);
        if (idx > 0)
        {
           idx--;
           PORTC&= ~(1<<idx);
           if (idx == CLAP_INDEX)
           {
              // have to do 10ms low/high/low/high/low/high/(noise low)
              //                140120  100  80   60  40   20         0
              trigLen[idx] = 140;
           }
           else
           {
              trigLen[idx] = 20;
           }
        }
        if (evt2 > TRIG_THRESHOLD)
        {
           // reset shared accent counter, ensure
           // portc is low
           accLen = 20;
           PORTC&=~(1<<TRIG_ACC);
        } 
     }
  }
}


int main(void)
{
	// initialize our libraries
	// initialize the UART (serial port)
	midiInit();
    pos = 0;
    accLen = 0;

    timerInit();
 
    // initialize port b to all outputs
    DDRC=0xFF;
    PORTC=0xff;

    while (1)
    {
       updateTriggers();
       midiPoll();
       delay_ms(1);
   	}
	
	return 0;
}
