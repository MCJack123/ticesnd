//
//  sound.h
//  
//
//  Created by JackMacWindows on 10/29/18.
//

#ifndef ti_sound_h
#define ti_sound_h

#include <tice.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "usbserial.h"

#define NOTE_C  262 // C4
#define NOTE_DB 277 // C#/Db4
#define NOTE_D  294 // D4
#define NOTE_EB 311 // D#/Eb4
#define NOTE_E  329 // E4
#define NOTE_F  349 // F4
#define NOTE_GB 370 // F#/Gb4
#define NOTE_G  392 // G4
#define NOTE_AB 415 // G#/Ab4
#define NOTE_A  440 // A4
#define NOTE_BB 466 // A#/Bb4
#define NOTE_B  493 // B4
#define NOTE_R  0   // stop

/**
 * Initializes the serial bus.
 * @param baud_rate The baud rate of the connection.
 * @param stop_bits Number of stop bits in the connection.
 * @param bit_parity The parity of the bits.
 * @param data_bits Number of data bits.
 * @returns Whether the initialization was successful.
 */
extern bool initSerial(uint32_t baud_rate, uint8_t stop_bits, uint8_t bit_parity, uint8_t data_bits);
/**
 * Initializes the serial bus with 9600 baud, 1 stop bit, no bit parity, and 8 data bits.
 * @returns Whether the initialization was successful.
 */
extern bool initSerialDefault();

/********************************** NOTE MODE *********************************/

/**
 * Plays a single tone for an amount of time.
 * @param frequency The frequency of the note. Set to 0 to stop.
 * @param length The length of the note in ms. Set to 0 to play indefinitely.
 */
extern void playTone(uint16_t frequency, uint16_t length);
/**
 * Plays a series of notes and lengths.
 * @param notes A list of notes to play.
 * @param lengths A list of lengths of the notes.
 * @param count The number of notes in both lists.
 */
extern void playNotes(uint16_t notes[], uint16_t lengths[], uint16_t count);
/**
 * Stops all sound.
 */
extern void noTone();

/**
 * Sets the beats per minute of song playback.
 * @param bpm The beats per minute to play back at.
 */
extern void setBPM(uint8_t bpm);
/**
 * Sets the number of beats in one measure. (Bottom number in time signature)
 * @param size The number of beats in one measure.
 */
extern void setBeats(uint8_t size);
/**
 * Adjusts the octave of the note.
 * @param note Base note.
 * @param octave The octave of the resulting note.
 * @returns The adjusted pitch of the note.
 */
extern uint16_t adjustNote(uint16_t note, uint8_t octave);
/**
 * Adjusts a set of notes to an octave.
 * @param notes The notes to adjust.
 * @param count The number of notes to adjust.
 * @param octave The target octave of the notes.
 */
extern void adjustNotes(uint16_t * notes, uint16_t count, uint8_t octave);
/**
 * Plays a song with the specified note lengths (as fractions of one measure).
 * @param notes A list of notes to play.
 * @param beats The sizes of each note to play as fractions of a measure.
 * @param count The number of notes to play.
 */
extern void playSong(uint16_t notes[], float beats[], uint16_t count);

/********************************** PCM MODE **********************************/

/**
 * Attempts to switch to PCM mode if the card is compatible.
 * @return Whether the switch was successful.
 */
extern bool switchToPCM();
/**
 * Returns whether the card is in PCM mode.
 * @returns Whether the card is in PCM mode.
 */
extern bool isInPCM();
/**
 * Sends a PCM buffer to the sound card.
 * The sound must be unsigned 8-bit mono audio at 8000 Hz.
 * @param buf The buffer to write.
 * @param size The size of the audio.
 */
extern void sendPCMAudio(uint8_t * buf, uint24_t size);

/**
 * Plays a list of instrumental notes on the sound card.
 * @param notes The list of notes to play. (ticks_since_last << 16 | channel << 12 | frequency)
 * @param count The number of notes in the list.
 * @returns Whether the playback was successful.
 */
extern bool playHardware(uint24_t * notes, uint24_t count);

#endif /* sound_h */
