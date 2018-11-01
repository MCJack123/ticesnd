// This is put into an Arduino sketch and run on an Arduino.
// Put speaker between pin 11 & ground.

int state = 0;
int freq = 0;
int start_beep = 1; // set to 0 for no beep at the start

void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  if (start_beep) {
    tone(11, 300);
    delay(50);
    noTone(11);
  }
  Serial.setTimeout(10);
}

void loop() {
  int num = Serial.read();
  if ((state == 0 && num != 0xFF) || num == -1) return;
  if (num == 0xFF) state = 0;
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
      else tone(11, freq);
      state = 0;
      break;
  }
}
