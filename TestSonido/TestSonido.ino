/*
  Educational BoosterPack MK II - Birthday Tune
  http://boosterpackdepot.info/wiki/index.php?title=Educational_BoosterPack_MK_II

  Play birthday tune through the buzzer, demonstrating
  buzzer tune() API and pitch/tone (hence music) generation

  Dec 2012 - Created for Educational BoosterPack
            buzzer Pin = 19
  Dec 2013 - Modified for Educational BoosterPack MK II
            buzzer Pin = 40
  by Dung Dang

*/
#include "pitches.h"
#include "wiring_private.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include <math.h>


#define NOTE_C4_1 260

int buzzerPin = 32;
uint8_t nota = 0;
uint8_t cont = 1;
// notes in the melody:
int game[] = {NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_D4, NOTE_AS3,
              NOTE_G3, NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_F4,
              NOTE_D4, SILENCIO, NOTE_C5, NOTE_G4, NOTE_DS4,
              NOTE_D4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_DS4,
              NOTE_C4, NOTE_AS3, NOTE_C4, NOTE_G3, NOTE_C5,
              NOTE_G4, NOTE_DS4, NOTE_D4, NOTE_C4, NOTE_C4,
              NOTE_D4, NOTE_DS4, NOTE_F4, NOTE_D4, NOTE_AS3,
              NOTE_C4, NOTE_G3, NOTE_C4, NOTE_B4, NOTE_DS4,
              NOTE_F4, NOTE_G4, NOTE_A4, NOTE_C5, NOTE_C5,
              NOTE_G4, NOTE_DS4, NOTE_D4, NOTE_C4, NOTE_C4,
              NOTE_D4, NOTE_DS4, NOTE_C4, NOTE_AS3, NOTE_C4,
              NOTE_G3, NOTE_C5, NOTE_G4, NOTE_DS4, NOTE_F4,
              NOTE_G4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_D4,
              NOTE_AS3,
              NOTE_C4, NOTE_F3, NOTE_F3, NOTE_A3, NOTE_C4,
              NOTE_DS4, NOTE_D4, NOTE_C4, NOTE_G3, NOTE_F3,
              NOTE_F3, NOTE_A3, NOTE_C4, NOTE_DS4, NOTE_F4,
              NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_DS4,
              NOTE_F4, NOTE_G4, NOTE_C4, NOTE_G4, NOTE_F4,
              NOTE_F4, NOTE_F4, NOTE_DS4, NOTE_D4, NOTE_DS4,
              NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_DS4,
              NOTE_F3, NOTE_F3, NOTE_A3, NOTE_C4, NOTE_DS4,
              NOTE_D4, NOTE_C4, NOTE_G3,  NOTE_F4, NOTE_F4,
              NOTE_A3, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4,
              NOTE_A4, NOTE_G4, NOTE_F4, NOTE_A4, NOTE_AS4,
              NOTE_C5, NOTE_G4, NOTE_DS4, NOTE_C4, NOTE_D4,
              NOTE_D4, NOTE_D4, NOTE_F4, NOTE_D4, NOTE_AS3,
              NOTE_G3, NOTE_AS3
             };

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int gameT[] = {
  4, 8, 8, 8, 8, 4, 8, 8,
  8, 8, 4, 4, 2, 2, 4, 4, 2,
  4, 4, 4, 4, 4, 4, 2, 2,
  2, 4, 4, 4, 8, 8, 4, 4,
  4, 4, 4, 4, 8, 16, 16,
  16, 16, 16, 16, 2, 2, 4,
  4, 2, 4, 4, 4, 4, 4, 2,
  2, 2, 4, 4, 4, 4, 4, 4, 4, 4,
  1, 4, 4, 4, 4,
  4, 4, 4, 4, 4,
  4, 4, 4, 4, 4,
  2, 4, 8, 4, 8,
  8, 4, 4, 2, 8,
  16, 8, 8, 4, 8,
  8, 4, 4, 4, 4,
  4, 4, 4, 4, 4,
  4, 4, 4, 4, 4,
  4, 4, 4, 4, 2,
  4, 8, 4, 8, 8,
  4, 4, 4, 4, 8,
  16, 8, 8, 8, 8,
  8, 8, 8
};

int End[] = {
  NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4, SILENCIO, NOTE_C4,
  NOTE_C4, SILENCIO, NOTE_C4, NOTE_C4, SILENCIO, NOTE_C4, NOTE_C4,
  NOTE_B4, NOTE_C4, NOTE_B3, SILENCIO, NOTE_B3, NOTE_B3, NOTE_A3,
  NOTE_B3, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4, SILENCIO,
  NOTE_C4, NOTE_C4, SILENCIO, NOTE_C4, NOTE_C4, SILENCIO, NOTE_C4,
  NOTE_C4, NOTE_B4, NOTE_G3, NOTE_B3, NOTE_G3, NOTE_A3, NOTE_B3,
  NOTE_CS4, SILENCIO, NOTE_C4, NOTE_C4, SILENCIO, NOTE_C4, NOTE_C4,
  SILENCIO, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_A3, SILENCIO,
  NOTE_A3, NOTE_B3, SILENCIO, NOTE_CS4, NOTE_A3, NOTE_A3, NOTE_B3,
  NOTE_D4, NOTE_CS4, SILENCIO, NOTE_CS4, NOTE_CS4, SILENCIO,
  NOTE_D4, NOTE_E4, SILENCIO, NOTE_A3, NOTE_B3, SILENCIO, NOTE_C4
};

int EndT[] = {
  8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8, 8, 8, 8,  8, 8, 8, 8,
  8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

int opening[] = {
  NOTE_E3, NOTE_FS3, NOTE_FS3, NOTE_G3, SILENCIO, NOTE_E3, 
  NOTE_G3, NOTE_D4, NOTE_CS4, NOTE_B3, NOTE_B3, NOTE_CS4, 
  NOTE_A3, NOTE_C4, NOTE_B3, NOTE_B3, NOTE_A3, SILENCIO, 
  NOTE_G3, NOTE_A3, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_G3, 
  NOTE_A3, NOTE_FS3, NOTE_A3, NOTE_A3, NOTE_D4, NOTE_E4, 
  NOTE_E2, NOTE_A2, NOTE_B2, NOTE_E3, NOTE_E3, NOTE_A3, 
  NOTE_B3, NOTE_E4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_E5, 
  NOTE_G5, NOTE_G3, NOTE_A3, NOTE_FS3, NOTE_E3, NOTE_FS3, 
  NOTE_G3, NOTE_A3, NOTE_A3, NOTE_B3, NOTE_A3

};

int openingT[] = {
  8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, 8, 2, 8, 
  16, 8, 8, 8, 8, 8,  8, 8, 16, 8, 8, 8, 16, 
  8, 8, 4, 16, 16, 16, 16, 16, 16, 16 ,16, 16,
  16, 16, 16, 1, 4, 4, 4, 16, 16, 8, 16, 8, 8, 2
};
int song = 2; //3-game, 1-opening, 2-Ending
int songA;
void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  configureTimer1A();
  songA=0;
}
void loop()
{
if (Serial2.available()>0){
  song=Serial2.read();
  Serial.println(song);
  }
if(song != songA){
  nota = 0;
  cont = 1;
  songA=song;
  }
  

}

void configureTimer1A() {
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // Enable Timer 1 Clock
  ROM_IntMasterEnable(); // Enable Interrupts
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC); // Configure Timer Operation as Periodic
  // La frecuecia est?? dada por: MasterClock / CustomValue
  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, 10000000); // El ??ltimo argumento es el CustomValue
  TimerIntRegister(TIMER1_BASE, TIMER_A, &Timer1AHandler);
  ROM_IntEnable(INT_TIMER1A);  // Enable Timer 1A Interrupt
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // Timer 1A Interrupt when Timeout
  ROM_TimerEnable(TIMER1_BASE, TIMER_A); // Start Timer 1A
}

void Timer1AHandler(void) {
  //Required to launch next interrupt
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);
  if (song == 3) {
    if (nota == 0) {
      int noteDuration = 2000 / gameT[nota];
      tone(buzzerPin, game[nota], noteDuration);
      nota++;
    }
    else {
      if (16 == cont * gameT[nota - 1]) {
        noTone(buzzerPin);
        cont = 1;
        int noteDuration = 2000 / gameT[nota];
        if (game[nota] == 1) {

        }
        else {
          tone(buzzerPin, game[nota], noteDuration);
        }
        nota++;
      }
      else {
        cont++;
      }
      if (nota >= (sizeof(gameT) / 4 + 1)) {
        nota = 0;
        cont = 1;
        noTone(buzzerPin);
      }
    }
  }
  //Cancion para el opening del juego
  else if (song == 1) {
    if (nota == 0) {
      int noteDuration = 2000 / openingT[nota];
      tone(buzzerPin, opening[nota], noteDuration);
      nota++;
    }
    else {
      if (16 == cont * openingT[nota - 1]) {
        noTone(buzzerPin);
        cont = 1;
        int noteDuration = 2000 / openingT[nota];
        if (opening[nota] == 1) {

        }
        else {
          tone(buzzerPin, opening[nota], noteDuration);
        }
        nota++;
      }
      else {
        cont++;
      }
      if (nota >= (sizeof(openingT) / 4 + 1)) {
        nota = 0;
        cont = 1;
        noTone(buzzerPin);
      }
    }
  }
  //Cancion para el ending de cuando alguien gana
  else if (song == 2) {
    if (nota == 0) {
      int noteDuration = 2000 / EndT[nota];
      tone(buzzerPin, End[nota], noteDuration);
      nota++;
    }
    else {
      if (16 == cont * EndT[nota - 1]) {
        noTone(buzzerPin);
        cont = 1;
        int noteDuration = 2000 / EndT[nota];
        if (End[nota] == 1) {

        }
        else {
          tone(buzzerPin, End[nota], noteDuration);
        }
        nota++;
      }
      else {
        cont++;
      }
      if (nota >= (sizeof(EndT) / 4 + 1)) {
        nota = 0;
        cont = 1;
        noTone(buzzerPin);
      }
    }

  }
}
