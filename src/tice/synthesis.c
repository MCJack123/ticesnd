//
//  synthesis.c
//  ticesnd
//
//  Created by Homework User on 11/8/18.
//

#include "synthesis.h"
#include <math.h>

#define frequency(t) (t & 0xFFF)
#define channel(t) ((t & 0xF000) >> 12)
#define ticks(t) ((t & 0xFF0000) >> 16)

uint16_t tick_length;
uint16_t frequencies[4];
const uint8_t zeroNumber = 0;

void setTickLength(uint16_t length) {
    tick_length = length;
}

uint8_t sine(uint24_t x, uint16_t freq) {
    return (sin((M_PI / 8000.0) * (float)x * (float)freq * (440.0/217.0) * (440.0/303.0)) + 1) * 127;
}

/*uint16_t square(uint24_t x, uint16_t freq) {
    if (freq == 0) return 0;
    return (x % (8000 / freq) > 8000 / freq / 2) * 255;
}*/
// redefined as macro to increase speed
#define square(x, freq) ((x % (8000 / freq) > 8000 / freq / 2) * 255)

#define SAMPLE_SIZE 256

bool synthesizeNotes(uint24_t * notes, uint24_t size) {
    uint8_t samples[SAMPLE_SIZE], j = 0, c;
    uint24_t i = 0, last_time, current_pos = 0;
    if (!isInPCM()) return false;
    srl_Write(&zeroNumber, 1);
    srl_Await();
    for (; i < size - 1; i++) {
        frequencies[channel(notes[i])] = frequency(notes[i]);
        if (ticks(notes[i+1]) > 0) {
            for (last_time = (8000 / (1000 / tick_length)) * ticks(notes[i+1]); last_time >= 0; last_time--, current_pos++) {
                c = 0;
                // not using loops beacuse it is (probably) slower
                if (frequencies[0] != 0) c = square(current_pos, frequencies[0]);
                if (frequencies[1] != 0) c = ((uint16_t)c + square(current_pos, frequencies[0])) / 2;
                if (frequencies[2] != 0) c = ((uint16_t)c + square(current_pos, frequencies[0])) / 2;
                if (frequencies[3] != 0) c = ((uint16_t)c + square(current_pos, frequencies[0])) / 2;
                if (c == 0) c = 1;
                samples[current_pos % SAMPLE_SIZE] = c;
                if (current_pos % SAMPLE_SIZE == SAMPLE_SIZE-1) {
                    //srl_Write(&zeroNumber, 1);
                    //srl_Await();
                    srl_Write(samples, SAMPLE_SIZE);
                    //delay(48);
                }
            }
        }
    }
    srl_Write(&zeroNumber, 1);
    return true;
}
