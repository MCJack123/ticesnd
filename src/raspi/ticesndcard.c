/*
 * Compile with -lwiringPi -lm
 *
 * To use, upload an empty sketch to an Arduino and connect the Tx and Rx of the
 * Arduino to the Tx and Rx of a Raspberry Pi, respectively. Then connect 5v and
 * GND to the Arduino from the Pi. Next connect a speaker to physical pin 12
 * (pinout.xyz) and ground. Lastly connect the Arduino to the calc, and run this
 * program on the Pi.
 *
 * DO NOT USE WITH arduino/soundcard!
 */
#include <wiringPi.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define SAMPLE_RATE 8000
#define START_BEEP
#define PCM_SWITCH_MAGIC 0x1ba60741
#define PWM_RANGE 256
#define PWM_CLOCK 2

void alarmWakeup(int sig_num);

int lastOut = 0;
int running = 0;
int state = 0;
int freq = 0;
int channel_freq[16];
int channel_snum[16];
int channel = 0;
int tfreq = 0;
uint32_t lastFour = 0;
uint16_t lastSample = 0;

uint8_t sine(uint32_t x, uint16_t freq) {
  return (sin((M_PI / 8000.0) * (float)x * (float)freq * (440.0/217.0) * (440.0/303.0)) + 1) * 127;
}

uint8_t square(uint32_t x, uint16_t freq) {
  if (freq == 0) return 0;
  return (x % (SAMPLE_RATE / freq) > SAMPLE_RATE / freq / 2) * 255;
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

    wiringPiSetupPhys();//use the physical pin numbers on the P1 connector


    pinMode(12, PWM_OUTPUT);
    //pinMode(38, OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(PWM_RANGE);
    pwmSetClock(PWM_CLOCK);
    signal(SIGALRM, alarmWakeup);   
    //ualarm(125, 125);
    tone(11, 300);
    for(j=0; j<10000000; j++);
    noTone(11);
    int ser = serialOpen("/dev/ttyS0", 9600);
    //while(1)
    //{
        //digitalWrite(12, HIGH); //pin 40 high
        //for(j=0; j<1000000; j++);//do something
        //digitalWrite(12, LOW);  //pin 40 low
        //for(j=0; j<1000000; j++);//do something
    //}
    printf("Ready.\n");
    while (1) {
        int num = serialGetchar(ser);
        if (num == -1) continue;
        switch (state) {
    case 0:
      if (num & 0xF0 != 0xF0) {
        tone(11, 880);
        for(j=0; j<10000000; j++);
        noTone(11);
        break;
      }
      channel = num & 0x0F;
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
        state = 3;
        long baud = 115200;
        short samples = 8000;
        // notify calc of baud change
        #ifdef START_BEEP
        tone(11, 600);
        delay(50);
        noTone(11);
        #endif
        write(ser, (void*)&baud, 4);
        serialClose(ser);
        ser = serialOpen("/dev/ttyS0", 115200);
        // check for compatibility
        //Serial.setTimeout(3000);
        num = serialGetchar(ser);
        //Serial.setTimeout(10);
        if (num == -1) {
          printf("not compatible, return to frequency mode");
          serialClose(ser);
          serialOpen("/dev/ttyS0", 9600);
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
        break;
      }
      channel_freq[channel] = (tfreq << 8) | num;
      channel_snum[channel] = 0;
      printf("frequency: %d, channel: %d\n", channel_freq[channel], channel);
      state = 0;
      //digitalWrite(13, LOW);
      break;
    case 3:
      //printf("%c", num);
      lastFour = (lastFour << 8) | num;
      if (lastFour == PCM_SWITCH_MAGIC) { // TODO: make sure the sender will never send this
        for (int i = 0; i < 16; i++) {
          channel_freq[i] = 0;
          channel_snum[i] = 0;
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
                        lastSample += (i < 8 ? sine(channel_snum[i]++, channel_freq[i]) : square(channel_snum[i]++, channel_freq[i]));
                        counted++;
                    }
                }
                if (counted != 0) lastSample /= counted;
            } //else printf("%c", lastSample);
            pwmWrite(12, lastSample);
        }
    }

}
