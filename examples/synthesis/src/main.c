#include "sound.h"
#include "tetris.h"
#define make_note(c, f, t) (((t & 0xFF) << 16) | ((c & 0xF) << 12) | (f & 0xFFF))
#define CHANNEL_OFFSET 2

/*uint24_t notes[] = {
    make_note(8-CHANNEL_OFFSET, NOTE_E, 0),
    make_note(9-CHANNEL_OFFSET, NOTE_C / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_C, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_G / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_C / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_G / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 4),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_B / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 4),
    make_note(9-CHANNEL_OFFSET, NOTE_C / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_G, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_G, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_G / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 4),
    make_note(9-CHANNEL_OFFSET, NOTE_C / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_C, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_G / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_C / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_G / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 1),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 1),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 1),
    make_note(8-CHANNEL_OFFSET, NOTE_E, 1),
    make_note(9-CHANNEL_OFFSET, NOTE_B / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_D, 2),
    make_note(8-CHANNEL_OFFSET, NOTE_C, 2),
    make_note(9-CHANNEL_OFFSET, NOTE_C / 2, 0),
    make_note(8-CHANNEL_OFFSET, NOTE_R, 8),
    make_note(9-CHANNEL_OFFSET, NOTE_R, 0)
};*/

#define frequency(t) (t & 0xFFF)
#define channel(t) ((t & 0xF000) >> 12)
#define ticks(t) ((t & 0xFF0000) >> 16)

bool playHardware(uint24_t * notes, uint24_t count) {
    int i;
    uint8_t bytes[3];
    for (i = 0; i < count; i++) {
        delay(ticks(notes[i])*50.7);
        bytes[0] = 0xF0 | channel(notes[i]);
        bytes[1] = (frequency(notes[i]) & 0xF00) >> 8;
        bytes[2] = frequency(notes[i]) & 0xFF;
        srl_Write(bytes, 3);
    }
    delay(10);
    return true;
}

int main() {
    os_ClrHome();
    os_PutStrFull("Initializing");
    if (!initSerialDefault()) {
        os_PutStrFull(" Failure");
        return;
    }
    //setTickLength(125);
    //os_PutStrFull(" Switching");
    //if (!switchToPCM()) {
    //    os_PutStrFull(" Failure");
    //    return;
    //}
    //delay(3000);
    os_PutStrFull(" Writing");
    
    if (!playHardware((uint24_t*)tetris_cesnd, tetris_cesnd_len/3)) {os_PutStrFull(" Error"); delay(3000);}
    usb_Cleanup();
}
