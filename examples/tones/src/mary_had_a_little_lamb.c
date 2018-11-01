#include "sound.h"

uint16_t melody[] = { NOTE_E, NOTE_D, NOTE_C, NOTE_D, NOTE_E, NOTE_E, NOTE_E, NOTE_D, NOTE_D, NOTE_D, NOTE_E, NOTE_G, NOTE_G, NOTE_E, NOTE_D, NOTE_C, NOTE_D, NOTE_E, NOTE_E, NOTE_E, NOTE_E, NOTE_E, NOTE_D, NOTE_D, NOTE_D, NOTE_E, NOTE_D, NOTE_C};

float beats[] = {0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.5, 0.25, 0.25, 0.5, 0.25, 0.25, 0.5, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.125, 0.125, 0.25, 0.125, 0.125, 0.25, 0.25, 1.0};

void main(void) {
	os_ClrHome();
	os_PutStrFull("Initializing");
    if (!initSerialDefault()) {
        os_PutStrFull(" Failure");
        return;
    }
    adjustNotes(melody, 28, 5);
	os_PutStrFull(" Writing");
    playSong(melody, beats, 28);
    usb_Cleanup();
}
