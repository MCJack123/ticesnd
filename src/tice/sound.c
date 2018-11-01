//
//  sound.c
//  
//
//  Created by JackMacWindows on 10/29/18.
//

#include "sound.h"
#include <math.h>

const char zero = 0xFF;
const char stop[3] = {0xFF, 0x00, 0x00};
uint8_t global_bpm = 120;
uint8_t measure_size = 4;

bool initSerial(uint32_t baud_rate, uint8_t stop_bits, uint8_t bit_parity, uint8_t data_bits) {
    lineCoding_t lc;
    lc.rate = baud_rate;
    lc.charFormat = stop_bits;
    lc.parityType = bit_parity;
    lc.dataBits = data_bits;
    usb_PrePower(3000);
    if(!srl_InitDevice()) {
        usb_Cleanup();
        return false;
    }
    srl_ConfigSerial(&lc);
    return true;
}

bool initSerialDefault() {
    return initSerial(9600, 0, 0, 8);
}

void playNotes(uint16_t notes[], uint16_t lengths[], uint16_t count) {
    int i;
    for (i = 0; i < count; i++) playTone(notes[i], lengths[i]);
}

void playTone(uint16_t frequency, uint16_t length) {
    uint8_t na, nb;
    na = frequency >> 8;
    nb = frequency & 0xFF;
    if (na == 255 || nb == 255) return;
    srl_Write((void*)&zero, 1);
    srl_Write((void*)&na, 1);
    srl_Write((void*)&nb, 1);
    delay(length);
    if (length > 0) noTone();
}

void noTone() {
    srl_Write(&stop, 3);
}

void setBPM(uint8_t bpm) {
    global_bpm = bpm;
}

void setBeats(uint8_t size) {
    measure_size = size;
}

uint16_t adjustNote(uint16_t note, uint8_t octave) {
    return note * pow(2, octave - 4);
}

void adjustNotes(uint16_t * notes, uint16_t count, uint8_t octave) {
    int i;
    for (i = 0; i < count; i++) notes[i] = adjustNote(notes[i], octave);
}

void playSong(uint16_t notes[], float beats[], uint16_t count) {
    int i;
    for (i = 0; i < count; i++) playTone(notes[i], (uint16_t)(beats[i] * (float)measure_size * (60000.0/(float)global_bpm)));
}
