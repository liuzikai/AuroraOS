
#include "beep.h"
#include "lib.h"
#include "rtc.h"
#include "file_system.h"

// reference: https://wiki.osdev.org/PC_Speaker
/* down from here */
//Play sound using built in speaker
void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
    uint32_t flag;
 
    cli_and_save(flag); {
        //Set the PIT to the desired frequency
 	    Div = 1193180 / nFrequence;
 	    outb(0xb6, 0x43);
 	    outb((uint8_t) (Div), 0x42);
 	    outb((uint8_t) (Div >> 8), 0x42);
 
        //And play the sound using the PC speaker
 	    tmp = inb(0x61);
  	    if (tmp != (tmp | 3)) {
 	    	outb(tmp | 3, 0x61);
 	    }
    }
    restore_flags(flag);
 }
 
 //make it shutup
void nosound() {
 	uint32_t flag;
    
    cli_and_save(flag); {
        uint8_t tmp = inb(0x61) & 0xFC;
 	    outb(tmp, 0x61);
    }
    restore_flags(flag);
    
 }

// reference: https://wiki.osdev.org/PC_Speaker
/* up from here */

/**
 * Beep
 * @param   frequency: the frequency of beep 
 * @param   time_in_ms: the duration of the beep 
 * @effect: the frequency of PIT channel 2 will be changed!
 */
void beep(uint32_t frequency, uint32_t time_in_ms){
    play_sound(frequency);

    // TODO: keep the beep for time_in_ms by sleeping 
    // for test: just use current rtc frequency
    uint8_t buf[]="rtc";
    int fd=system_open(buf);
    system_read(fd, 0, 0);

    nosound();
}
