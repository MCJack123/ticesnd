int state = 0;
int freq = 0;
unsigned long lastRead;
#define SAMPLE_RATE 8000
int ledPin = 13;
int speakerPin = 11; // Can be either 3 or 11, two PWM outputs connected to Timer 2
volatile uint16_t sample;
byte lastSample;
int snum = 0;

void stopPlayback()
{
    // Disable playback per-sample interrupt.
    TIMSK1 &= ~_BV(OCIE1A);

    // Disable the per-sample timer completely.
    TCCR1B &= ~_BV(CS10);

    // Disable the PWM timer.
    TCCR2B &= ~_BV(CS10);

    digitalWrite(speakerPin, LOW);
}

// This is called at 8000 Hz to load the next sample.
ISR(TIMER1_COMPA_vect) {
    
        if (Serial.available()) snum = Serial.read();
        if(speakerPin==11){
            OCR2A = snum;
        } else {
            OCR2B = snum;            
        }
    ++sample;
}

void startPlayback()
{
    pinMode(speakerPin, OUTPUT);
    int num = Serial.read();
    if (num == -1) num = 0;
    // Set up Timer 2 to do pulse width modulation on the speaker
    // pin.

    // Use internal clock (datasheet p.160)
    ASSR &= ~(_BV(EXCLK) | _BV(AS2));

    // Set fast PWM mode  (p.157)
    TCCR2A |= _BV(WGM21) | _BV(WGM20);
    TCCR2B &= ~_BV(WGM22);

    if(speakerPin==11){
        // Do non-inverting PWM on pin OC2A (p.155)
        // On the Arduino this is pin 11.
        TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
        TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
        // No prescaler (p.158)
        TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

        // Set initial pulse width to the first sample.
        OCR2A = pgm_read_byte(&num);
    } else {
        // Do non-inverting PWM on pin OC2B (p.155)
        // On the Arduino this is pin 3.
        TCCR2A = (TCCR2A | _BV(COM2B1)) & ~_BV(COM2B0);
        TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
        // No prescaler (p.158)
        TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

        // Set initial pulse width to the first sample.
        OCR2B = pgm_read_byte(&num);
    }

    // Set up Timer 1 to send a sample every interrupt.

    cli();

    // Set CTC mode (Clear Timer on Compare Match) (p.133)
    // Have to set OCR1A *after*, otherwise it gets reset to 0!
    TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
    TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

    // No prescaler (p.134)
    TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

    // Set the compare register (OCR1A).
    // OCR1A is a 16-bit register, so we have to do this with
    // interrupts disabled to be safe.
    OCR1A = F_CPU / SAMPLE_RATE;    // 16e6 / 8000 = 2000

    // Enable interrupt when TCNT1 == OCR1A (p.136)
    TIMSK1 |= _BV(OCIE1A);
    lastSample = pgm_read_byte(&num);
    sample = 0;
    sei();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  tone(11, 300);
  delay(50);
  noTone(11);
  Serial.setTimeout(10);
}

void loop() {
  int num = Serial.read();
  if ((state == 0 && num != 0xFF) || num == -1) return;
  if (num == 'p') Serial.println(state);
  if (num == 0xFF && state != 3) state = 0;
  switch (state) {
    case 0:
      state = 1;
      break;
    case 1:
      freq = num << 8;
      state = 2;
      break;
    case 2:
      freq |= num;
      if (freq == 0) noTone(11);
      else if (freq == 0xFEFE) {
        // switch to PCM mode
        state = 3;
        long baud = 115200;
        short samples = 8000;
        // notify calc of baud change
        tone(11, 600);
        delay(50);
        noTone(11);
        Serial.write((char*)&baud, 4);
        Serial.end();
        Serial.begin(115200);
        // check for compatibility
        Serial.setTimeout(3000);
        Serial.readBytes((char*)&num, 1);
        Serial.setTimeout(10);
        if (num == -1) {
          // not compatible, return to frequency mode
          Serial.end();
          Serial.begin(9600);
          state = 0;
          tone(11, 300);
          delay(50);
          noTone(11);
          delay(100);
          tone(11, 300);
          delay(50);
          noTone(11);
          return;
        }
        // notify calc of format
        Serial.write((char*)&samples, 2);
        Serial.write(8);
        tone(11, 1200);
        delay(50);
        noTone(11);
        lastRead = micros();
        startPlayback();
        while (true) ;
        return;
      }
      else tone(11, freq);
      state = 0;
      break;
  }
}
