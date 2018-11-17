#include <wiringPi.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define SAMPLE_RATE 8000
#define START_BEEP
#define PCM_SWITCH_MAGIC 0x1ba60741
#define PWM_RANGE 256
#define PWM_CLOCK 8
#define SERIAL_DEVICE "/dev/ttyAMA0"

void alarmWakeup(int sig_num);

int lastOut = 0;
int running = 0;
int state = 0;
int freq = 0;
int channel_freq[16];
unsigned int samplenum = 0;
int channel = 0;
int tfreq = 0;
uint32_t lastFour = 0;
uint16_t lastSample = 0;
FILE * fd;
int looping = 1;
int debugEnabled = 0;
int ser;

uint8_t sine(uint32_t x, uint16_t freq) {
  return (sin((M_PI * (float)x) / 8000.0 * (float)freq * (440.0/217.0) /* (440.0/303.0)*/) + 1) * 127;
}

double sine_d(uint32_t x, uint16_t freq) {
  return sin((M_PI * (float)x) / 8000.0 * (float)freq * (440.0/217.0));
}

uint8_t noise(uint32_t x, uint16_t freq) {
  return rand() % 127;
}

/*
#define SINE_SQUARE
//*/

uint8_t square(uint32_t x, uint16_t freq) {
  if (freq == 0 || SAMPLE_RATE / freq == 0) return 0;
#ifdef SINE_SQUARE
  double sum = 0;
  for (int i = 1; i < 25; i+=2) sum += sine_d(i*x, freq) / i;
  if (sum > 1 || sum < -1) printf("square too high!\n");
  return (sum + 1) * 127;
#else
  return round((sin((2.0*M_PI*(float)x*(float)freq)/8000.0)+1.0)/2.0)*255;
#endif
}

void noTone(int pin) {
    running = 0;
    ualarm(125, 125);
}

void tone(int pin, int frequency) {
    if (frequency == 0) noTone(pin);
    else {
        running = 1;
        ualarm(500000/frequency, 500000/frequency);
    }
}

int main(int argc, char *argv[])
{
    unsigned int j;
    debugEnabled = argc > 1;
    wiringPiSetupPhys();//use the physical pin numbers on the P1 connector


//    if (debugEnabled) fd = fopen("output.raw", "w");
    pinMode(12, PWM_OUTPUT);
    //pinMode(38, OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(PWM_RANGE);
    pwmSetClock(PWM_CLOCK);
    signal(SIGALRM, alarmWakeup);
    srand(time(NULL));
    if (debugEnabled) signal(SIGINT, alarmWakeup);
    //ualarm(125, 125);
    tone(11, 300);
    for(j=0; j<10000000; j++);
    noTone(11);
    ser = serialOpen(SERIAL_DEVICE, 9600);
    //while(1)
    //{
        //digitalWrite(12, HIGH); //pin 40 high
        //for(j=0; j<1000000; j++);//do something
        //digitalWrite(12, LOW);  //pin 40 low
        //for(j=0; j<1000000; j++);//do something
    //}
    printf("Ready.\n");
    if (debugEnabled) printf("Debug mode on.\n");
    while (looping) {
        if (state == 3) continue;
        int num = serialGetchar(ser);
        if (num == -1) continue;
        //if (num == 0) serialPutchar(ser, 0);
        switch (state) {
    case 0:
      if (num & 0xF0 != 0xF0) {
        printf("HUH?");
        tone(11, 880);
        for(j=0; j<10000000; j++);
        noTone(11);
        break;
      }
      channel = num & 0x0F;
      //serialPutchar(ser, 1);
      state = 1;
      //digitalWrite(13, LOW);
      break;
    case 1:
      tfreq = num;
      state = 2;
      //digitalWrite(13, LOW);
      break;
    case 2:
      if (channel == 0x0F && tfreq == 0xFE && num == 0xFE) {
        printf("switch to PCM mode\n");
        long baud = 115200;
        short samples = 8000;
        // notify calc of baud change
        #ifdef START_BEEP
        tone(11, 600);
        for(j=0; j<10000000; j++);
        noTone(11);
        #endif
        //printf("writing\n");
        write(ser, (void*)&baud, 4);
        //serialFlush(ser);
        //printf("closing\n");
        serialClose(ser);
//for(j=0; j<10000000; j++);
        //printf("opening\n");
        ser = serialOpen(SERIAL_DEVICE, 115200);
        // check for compatibility
        //Serial.setTimeout(3000);
        printf("getting char\n");
        num = serialGetchar(ser);
        printf("got char\n");
        //Serial.setTimeout(10);
        if (num == -1) {
          printf("not compatible, return to frequency mode");
          serialClose(ser);
          serialOpen(SERIAL_DEVICE, 9600);
          state = 0;
          #ifdef START_BEEP
          tone(11, 300);
          for(j=0; j<10000000; j++);
          noTone(11);
          for(j=0; j<20000000; j++);
          tone(11, 300);
          for(j=0; j<10000000; j++);
          noTone(11);
          #endif
          break;
        }
        printf("notify calc of format\n");
        write(ser, (void*)&samples, 2);
        serialPutchar(ser, 8);
        ualarm(125, 125);
        #ifdef START_BEEP
        tone(11, 1200);
        for(j=0; j<10000000; j++);
        noTone(11);
        #endif
        state = 3;
        break;
      }
      channel_freq[channel] = (tfreq << 8) | num;
      //channel_snum[channel] = 0;
      if (debugEnabled) printf("frequency: %d, channel: %d\n", channel_freq[channel], channel);
      state = 0;
      //digitalWrite(13, LOW);
      break;
    case 3: // this will never be reached
      //printf("%c", num);
      lastFour = (lastFour << 8) | num;
      if (lastFour == PCM_SWITCH_MAGIC) { // TODO: make sure the sender will never send this
        for (int i = 0; i < 16; i++) {
          channel_freq[i] = 0;
          //channel_snum[i] = 0;
        }
        state = 0;
        #ifdef START_BEEP
        tone(11, 300);
        for(j=0; j<10000000; j++);
        noTone(11);
        #endif
        break;
      }
      lastSample = num;
      break;
    default:
      tone(11, 440);
      for(j=0; j<10000000; j++);
      noTone(11);
  }
    }

    return 0;

}//int main(int argc, char *argv[])


void alarmWakeup(int sig_num)
{
    unsigned int i;

    if(sig_num == SIGALRM)
    {
        if (running) {
            pwmWrite(12, lastOut * (PWM_RANGE-1)); //pin 38 high
            lastOut = !lastOut;
            return;
            //for(i=0; i<65535; i++); //do something
            //digitalWrite(12, LOW);  //pin 38 low
        } else {
            if (state != 3) {
                int counted = 0;
                lastSample = 0;
                for (int i = 0; i < 16; i++) {
                    if (channel_freq[i] != 0) {
                        lastSample += i == 0 ? noise(samplenum, channel_freq[i]) : (i < 8 ? sine(samplenum, channel_freq[i]) : square(samplenum, channel_freq[i]));
                        counted++;
                    }
                }
                samplenum++;
                if (samplenum - 1 > samplenum) printf("overflow!\n");
                if (counted != 0) lastSample /= counted;
//                lastSample /= 2;
            } else {
      //printf("%c", num);
      int num = serialGetchar(ser);
      if (num == -1) {lastFour = 1; return;}
      //printf("%d", num);
      if (num == 0 && lastFour == 1) {serialPutchar(ser, 0); printf("0");}
      lastFour = (lastFour << 8) | num;
      if (lastFour == PCM_SWITCH_MAGIC) { // TODO: make sure the sender will ne$
        for (int i = 0; i < 16; i++) {
          channel_freq[i] = 0;
          //channel_snum[i] = 0;
        }
        samplenum = 0;
        state = 0;
        #ifdef START_BEEP
        tone(11, 300);
        for(int j=0; j<10000000; j++);
        noTone(11);
        #endif
        return;
      }
      lastSample = num;
            }
            if (lastSample > 255) printf("too high!\n");
            pwmWrite(12, lastSample);
    //        if (debugEnabled) fwrite(&lastSample, 1, 1, fd);
        }
    } else if (sig_num == SIGINT) {
        printf("Stopping...\n");
  //      fclose(fd);
        looping = 0;
        raise(SIGKILL);
    }

}

