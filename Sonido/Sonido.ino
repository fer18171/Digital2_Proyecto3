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
#define NOTE_C4_1 260

int buzzerPin = 32;

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
  6, 16, 16, 1, 4, 4, 4, 16, 16, 8, 16, 8, 8, 2
};
int song = 2; //0-game, 1-opening, 2-Ending
int songA;
int nota = 0;
void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(buzzerPin, OUTPUT);
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
  songA=song;
  }
  
  if (song == 3) {
    int noteDuration = 1000 / gameT[nota];
    tone(buzzerPin, game[nota], noteDuration);
    int pauseBetweenNotes = noteDuration + 50;      //delay between pulse
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
    nota++;
    if (nota >= (sizeof(gameT) / 4 + 1)) {
      nota = 0;
      noTone(buzzerPin);
    }
  }
  else if (song == 2) {
    int noteDuration = 1000 / EndT[nota];
    tone(buzzerPin, End[nota], noteDuration);
    int pauseBetweenNotes = noteDuration + 50;      //delay between pulse
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
    nota++;
    if (nota >= (sizeof(EndT) / 4 + 1)) {
      nota = 0;
      noTone(buzzerPin);
    }
  }
  else if (song == 1) {
    int noteDuration = 1000 / openingT[nota];
    tone(buzzerPin, opening[nota], noteDuration);
    int pauseBetweenNotes = noteDuration + 50;      //delay between pulse
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
    nota++;
    if (nota >= (sizeof(openingT) / 4 + 1)) {
      nota = 0;
      noTone(buzzerPin);
    }
  }              // stop the tone playing

}
