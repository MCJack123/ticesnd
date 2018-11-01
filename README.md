# ticesnd
Sound library + card over USB for TI-84+ CE

## Requirements
* TI-84+ CE
* Arduino (Uno or Nano recommended, but others also work)
* Some sort of speaker you can connect to the Arduino
* Cable to connect them together
  * Uno: mini-USB -> USB-A & USB-A -> USB-B
  * Nano: mini-USB -> mini-USB (link cable)
* External power supply
  * Uno has a 9v power connector, so you can use that (not sure about others)
  * All Arduinos accept a Vin/GND connection that can connect to a USB cable
## Adding
This requires the (CE-serial library)[https://github.com/commandblockguy/CE-serial] to send data. I have already included it in the examples, but if you compile your own program you will need to download it yourself.  
To add to your program, copy the files in `tice` to your source directory (as well as the contents of `src` in CE-serial) and `#include "sound.h"` at the top of your source files.  
To upload the sound card firmware, open up the Arduino IDE and copy the source from `src/arduino/soundcard.c` into a new sketch, and upload it to your Arduino.
## Usage
Connect your speaker between pin 11 and ground. You can use a normal speaker with stripped wires, or you can cut an aux cable and connect the wires for both sides to pin 11.  
Then connect a USB cable between your Arduino and your calculator. If you have an Arduino with mini-USB (such as the Micro), you can use the standard link cable that comes with the calculator. Otherwise, you will need a mini-USB to USB-A adapter (cheap on eBay) and a USB-A to USB-B "printer" cable.  
Lastly, connect some sort of external power to the Arduino. If there is a power plug on it (Unos have them), you can use a 9v power adapter or battery. You can also connect a USB cable's power to the Vin & GND pins to power it.
## Functions
These are the functions provided by `sound.h`:
### Initialization
* `initSerial(baud_rate, stop_bits, bit_parity, data_bits)`: Wrapper around `srl_InitDevice`/`srl_ConfigSerial` that automatically fills in the config. Returns false if serial config failed, otherwise true.
* `initSerialDefault()`: Synonym for `initSerial(9600, 0, 0, 8)`.
### Tone Mode
* `playTone(frequency, length)`: Plays a tone at `frequency` Hz for `length` ms. Set `length` to 0 to play infinitely. Set `frequency` to 0 to stop.
* `noTone()`: Stops tones.
* `playNotes(notes[], lengths[], count)`: Plays a series of `notes`. Basically just `for (i=0; i<count; i++) playTone(notes[i], lengths[i]);`.
#### Songs
These functions are slightly different from normal tones as they rely on BPM/measures/note length rather than absolute time.
* `setBPM(bpm)`: Sets the beats per minute of the song output to `bpm`.
* `setBeats(size)`: Sets the number of beats per measure to `size`.
* `adjustNote(note, octave)`: Shifts the `note` to the specified `octave`. (May end up having it change by semitones instead.) Returns new `note` at `octave`.
* `adjustNotes(*notes, count, octave)`: Shifts all of `notes` to the specified `octave`. THIS CHANGES THE VALUES PASSED TO IT!
* `playSong(notes[], beats[], count)`: Plays `notes` for each `beats`. A beat is a float specifying a fraction of a measure, so a quarter note would be `.25`, eigth note would be `.125`, etc.
### PCM Mode
* `switchToPCM()`: Switches the card to PCM mode. Note that you cannot leave PCM mode without hard resetting the Arduino. Returns false if failed, otherwise true.
* `isInPCM()`: Returns whether the card has been switched to PCM mode.
* `sendPCMAudio(*buf, size)`: Sends `buf` to the card. It must be formatted as unsigned 8-bit at 8000 Hz.
### Macros for tone mode
* `NOTE_C`
* `NOTE_DB`
* `NOTE_D`
* `NOTE_EB`
* `NOTE_E`
* `NOTE_F`
* `NOTE_GB`
* `NOTE_G`
* `NOTE_AB`
* `NOTE_A`
* `NOTE_BB`
* `NOTE_B`
* `NOTE_R` (no sound)  
All of these macros are in the fourth octave.
