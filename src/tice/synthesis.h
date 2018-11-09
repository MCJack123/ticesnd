//
//  synthesis.h
//  ticesnd
//
//  Created by Homework User on 11/8/18.
//

#ifndef ticesnd_synthesis_h
#define ticesnd_synthesis_h

#include "sound.h"

/*
 * A note is defined as a uint24_t that is laid out as so:
 * _____________________________________________________
 * |0|0|0|0|0|1|0|0| |1|0|0|0|0|0|0|1| |1|0|1|1|1|0|0|0|
 * -----------------------------------------------------
 * |time since last | channel|       frequency         |
 *
 * You can get a note by using NOTE_<name> | CHANNEL_<channel> | (time << 16).
 * You can also use make_note(channel, frequency, time).
 * By default, channels 0-7 are sine and 8-F are square.
 * Time since last is the number of ticks to wait until playing the next note.
 * You can set the tick speed with setTickLength().
 *
 */

#define CHANNEL_0 0x0000
#define CHANNEL_1 0x1000
#define CHANNEL_2 0x2000
#define CHANNEL_3 0x3000
#define CHANNEL_4 0x4000
//#define CHANNEL_5 0x5000
//#define CHANNEL_6 0x6000
//#define CHANNEL_7 0x7000
//#define CHANNEL_8 0x8000
//#define CHANNEL_9 0x9000
//#define CHANNEL_A 0xA000
//#define CHANNEL_B 0xB000
//#define CHANNEL_C 0xC000
//#define CHANNEL_D 0xD000
//#define CHANNEL_E 0xE000
//#define CHANNEL_F 0xF000
#define make_note(c, f, t) (((t & 0xFF) << 16) | ((c & 0xF) << 12) | (f & 0xFFF))

/**
 * Sets the length of one tick.
 * @param length The number of milliseconds in one tick.
 */
extern void setTickLength(uint16_t length);
/**
 * Plays a list of sequencer notes.
 * @param notes A list of notes to play.
 * @param size The number of notes to play.
 */
extern bool synthesizeNotes(uint24_t * notes, uint24_t size);

#endif /* sequencer_h */
