#include "synthesis.h"

uint24_t notes[] = {
    make_note(0, NOTE_E, 0),
    make_note(1, NOTE_C / 2, 0),
    make_note(8-8, NOTE_D, 2),
    make_note(8-8, NOTE_C, 2),
    make_note(9-8, NOTE_G / 2, 0),
    make_note(8-8, NOTE_D, 2),
    make_note(8-8, NOTE_E, 2),
    make_note(9-8, NOTE_C / 2, 0),
    make_note(8-8, NOTE_E, 2),
    make_note(8-8, NOTE_E, 2),
    make_note(9-8, NOTE_G / 2, 0),
    make_note(8-8, NOTE_D, 4),
    make_note(8-8, NOTE_D, 2),
    make_note(8-8, NOTE_D, 2),
    make_note(9-8, NOTE_B / 2, 0),
    make_note(8-8, NOTE_E, 4),
    make_note(9-8, NOTE_C / 2, 0),
    make_note(8-8, NOTE_G, 2),
    make_note(8-8, NOTE_G, 2),
    make_note(9-8, NOTE_G / 2, 0),
    make_note(8-8, NOTE_E, 4),
    make_note(9-8, NOTE_C / 2, 0),
    make_note(8-8, NOTE_D, 2),
    make_note(8-8, NOTE_C, 2),
    make_note(9-8, NOTE_G / 2, 0),
    make_note(8-8, NOTE_D, 2),
    make_note(8-8, NOTE_E, 2),
    make_note(9-8, NOTE_C / 2, 0),
    make_note(8-8, NOTE_E, 2),
    make_note(8-8, NOTE_E, 2),
    make_note(9-8, NOTE_G / 2, 0),
    make_note(8-8, NOTE_E, 2),
    make_note(8-8, NOTE_E, 1),
    make_note(8-8, NOTE_D, 1),
    make_note(8-8, NOTE_D, 2),
    make_note(8-8, NOTE_D, 1),
    make_note(8-8, NOTE_E, 1),
    make_note(9-8, NOTE_B / 2, 0),
    make_note(8-8, NOTE_D, 2),
    make_note(8-8, NOTE_C, 2),
    make_note(9-8, NOTE_C / 2, 0),
    make_note(8-8, NOTE_R, 8)
};

int main() {
    os_ClrHome();
    os_PutStrFull("Initializing");
    if (!initSerialDefault()) {
        os_PutStrFull(" Failure");
        return;
    }
    setTickLength(125);
    os_PutStrFull(" Switching");
    if (!switchToPCM()) {
        os_PutStrFull(" Failure");
        return;
    }
    delay(3000);
    os_PutStrFull(" Writing");
    if (!synthesizeNotes(notes, 42)) {os_PutStrFull(" Error"); delay(3000);}
    usb_Cleanup();
}
