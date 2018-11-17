#include "sound.h"
#include "tetris.h"
#define make_note(c, f, t) (((t & 0xFF) << 16) | ((c & 0xF) << 12) | (f & 0xFFF))
#define CHANNEL_OFFSET 2

uint24_t mary_notes[] = {
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
};

int main() {
    os_ClrHome();
    os_PutStrFull("Initializing");
    if (!initSerialDefault()) {
        os_PutStrFull(" Failure");
        return 1;
    }
    setBPM(148);
    setBeats(8);
    os_PutStrFull(" Writing");
    if (!playHardware((uint24_t*)tetris_cesnd, tetris_cesnd_len/3))
        os_PutStrFull(" Error");
    usb_Cleanup();
    return 0;
}
