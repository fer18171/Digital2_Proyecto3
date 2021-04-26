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
uint8_t nota=0;
uint8_t cont=1;
// notes in the melody:
int melody[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4,
                NOTE_E4, NOTE_F4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_A4,
                NOTE_G4, NOTE_F4, NOTE_E4, NOTE_C4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, 
                NOTE_E4, NOTE_C4, NOTE_D4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_G4, NOTE_C4, NOTE_C4};
   
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 4, 4,
  4, 4, 4, 4, 
  4, 4, 2,
  4, 4, 2, 
  8, 8, 8, 8, 4, 4,
  8, 8, 8, 8, 4, 4, 
  4, 4, 2,  
  4, 4, 2,1};

void setup() 
{
  Serial.begin(9600);
pinMode(buzzerPin,OUTPUT);
 configureTimer1A();
}
void loop() 
{
  Serial.println(sizeof(melody)/4);
  /*for (int thisNote = 0; thisNote < 32; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote],noteDuration);

    int pauseBetweenNotes = noteDuration + 50;      //delay between pulse
    delay(pauseBetweenNotes);
    
    noTone(buzzerPin);                // stop the tone playing
  }*/
}

void configureTimer1A(){
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // Enable Timer 1 Clock
  ROM_IntMasterEnable(); // Enable Interrupts
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC); // Configure Timer Operation as Periodic
  
  // Configure Timer Frequency
  // El tercer argumento ("CustomValue") de la siguiente función debe ser un entero (uint32_t),
  // NO un float. Ese valor determina la frecuencia (y por lo tanto el período) del timer.
  // La frecuecia está dada por: MasterClock / CustomValue
  // En el Tiva C, con las configuraciones actuales, el MasterClock es de 80 MHz.
  // Ejemplos:
  // Si se quiere una frecuencia de 1 Hz, el CustomValue debe ser 80000000: 80MHz/80M = 1 Hz
  // Si se quiere una frecuencia de 1 kHz, el CustomValue debe ser 80000: 80MHz/80k = 1 kHz
  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, 10000000); // El último argumento es el CustomValue

  // Al parecer, no hay función ROM_TimerIntRegister definida. Usar la de memoria FLASH
  // El prototipo de la función es:
  //    extern void TimerIntRegister(uint32_t ui32Base, uint32_t ui32Timer, void (*pfnHandler)(void));
  // Con el tercer argumento se especifica el handler de la interrupción (puntero a la función).
  // Usar esta función evita tener que hacer los cambios a los archivos internos de Energia,
  // sugeridos en la página de donde se tomó el código original.
  TimerIntRegister(TIMER1_BASE, TIMER_A, &Timer1AHandler);
  
  ROM_IntEnable(INT_TIMER1A);  // Enable Timer 1A Interrupt
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // Timer 1A Interrupt when Timeout
  ROM_TimerEnable(TIMER1_BASE, TIMER_A); // Start Timer 1A
}

void Timer1AHandler(void){
  //Required to launch next interrupt
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);
  if (nota==0){
    int noteDuration = 1000/noteDurations[nota];
    tone(buzzerPin, melody[nota],noteDuration);
    nota++;
    }
  else {
  if (pow(2,4-cont)==noteDurations[nota-1]){
    noTone(buzzerPin);
    cont=1;
    int noteDuration = 1000/noteDurations[nota];
    tone(buzzerPin, melody[nota],noteDuration);
    nota++;
    }
  else {
    cont++;
    }
  if (nota==sizeof(noteDurations)/4){
    Serial.println("A0");
    nota=0;
    cont=1;
    noTone(buzzerPin);
    }
  }
}
