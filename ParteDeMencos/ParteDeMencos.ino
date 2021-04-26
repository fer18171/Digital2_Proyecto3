//*********************************************
//Universidad del Valle de Guatemala
//Depto de Ingenieria Mecatronica, Electronica y Biomedica
//Curso de Digital 2
//Diego Mencos 18300
//Santiago Fernandez 18171

//Proyecto 3-Juego Smash Bros


#include <SPI.h>
#include <SD.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "pitches.h"
#include "wiring_private.h"
#include <math.h>

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1



int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};
//*********************************************
// Functions Prototypes
//*********************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);


void LCD_Init5408(void);
void writecmddata(uint16_t cmd, uint16_t dat);
void LCD_CMD16(uint16_t cmd);
void LCD_DATA16(uint16_t cmd);

void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

//extern uint8_t PantallaInicio[];
extern uint8_t Escenario1[];
extern uint8_t LinkSalto[];
extern uint8_t LinkEspada[];
//extern uint8_t LinkCorriendo[];

extern uint8_t PersonajeMiniatura[];
//extern uint8_t PersonajeGrande[];

struct control {
  const int izquierda;
  const int derecha ;
  const int arriba;
  const int ataque1 ;

};

int x = 0;
int salto = 0;
int reinicio = 0;
int ladojugador = 0;
unsigned char LinkCorriendo[20300];

unsigned char LinkParado[22*48*2*2];


//**************** COSAS DE LA MUSICA ***************************
int buzzerPin = 32;
uint8_t nota=0;
uint8_t cont=0;
// notes in the melody: b3-AS3
int melody[] = {NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_D4, NOTE_AS3, 
                NOTE_G3, NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_F4,
                NOTE_D4, NOTE_C5, NOTE_G4, NOTE_DS4,
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
                NOTE_AS3};
   
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 8, 8, 4, 8, 8, 
  8, 8, 4, 2, 2, 4, 4, 2, 
  4, 4, 4, 4, 4, 4, 2, 2,
  2, 4, 4, 4, 8, 8, 4, 4, 4, 4, 4, 4, 8, 16, 16, 16, 16, 16, 16, 2, 2, 4, 4, 2, 4, 4, 4, 4, 4, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4};
//********************************************************************

  
File myFile;

//*********************************************
// Inicialización
//*********************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  pinMode(buzzerPin,OUTPUT);
 configureTimer1A();
  //Memoria SD
  SPI.setModule(0);  //iniciamos comunicacion SPI en el modulo 0
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(12, OUTPUT);  //Colocamos el CS del modulo SPI-0 como Output
  //Se verifica que se haya iniciado correctamente la SD
  if (!SD.begin(12)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  //**


  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);
  //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
  LCD_Bitmap(0, 0, 320, 240, Escenario1);
  // LCD_Sprite(100, 100, 46, 48, LinkParado, 23, 1, 0, 0);



  pinMode(PUSH1, INPUT_PULLUP);
  pinMode(PUSH2, INPUT_PULLUP);
  pinMode(PA_7, INPUT);
  pinMode(PF_1, INPUT);

  reinicio = 1;

  //ExtraerDelSD("LINKC.txt",LinkCorriendo);
  //ExtraerDelSD("LINKP.txt",LinkParado);
  LCD_FondoSD("INIT.txt");
 // LCD_Sprite(100, 120, 35, 48, LinkCorriendo, 6, 1, 0, 0);
 // LCD_Sprite(50, 120, 22, 48, LinkParado, 2, 1, 0, 0);
}
//*********************************************
// Loop Infinito
//*********************************************
void loop() {
  String textoInicio = "PRESS START";
  LCD_Print(textoInicio, 75, 160, 2, 0xffff, 0x421b);
  uint8_t* array1;
  unsigned arraySize;
//  arraySize = 35*48*2*6;
//  free(array1);
//  array1 = (uint8_t*)malloc(arraySize*sizeof(uint8_t));
//  //array1 = new uint8_t[arraySize];
//  ExtraerDelSD("LINKC.txt",&array1[0]);
//  LCD_Sprite(100, 120, 35, 48, &array1[0], 6, 1, 0, 0);
  
  struct control control1 = {digitalRead(PUSH1), digitalRead(PUSH2), digitalRead(PA_7), digitalRead(PF_1)};



  if ((control1.derecha == LOW || control1.izquierda == LOW) && reinicio == 1) {
    reinicio = 0;
    LCD_Clear(0xFFFFFF);
    //LCD_Bitmap(0, 0, 320, 240, Escenario1);
    //0x0042 color azul de primer escenario


    // LCD_Sprite(70, 180, 40, 40, PersonajeMiniatura, 8, 0, 0, 0);
    for (int y = 0; y < 100; y++) {
      delay(5);
      int anim = (y / 11) % 8;
      //  LCD_Sprite(x, 120, 103, 48, LinkEspada, 4, anim, 0, 0);
      LCD_Sprite(46, 68, 40, 40, PersonajeMiniatura, 8, anim, 0, 0);
    }
    LCD_Sprite(46, 68, 40, 40, PersonajeMiniatura, 8, 0, 0, 0);
    delay(2000);
    LCD_Bitmap(0, 0, 320, 240, Escenario1);

    while (reinicio != 1) {
      struct control control1 = {digitalRead(PUSH1), digitalRead(PUSH2), digitalRead(PA_7), digitalRead(PF_1)};


      LCD_Sprite(70, 180, 40, 40, PersonajeMiniatura, 8, 0, 0, 0);
      
      if (control1.izquierda == LOW) {

        x++;
        int anim = (x / 11) % 8;
        //LCD_Sprite(x, 120, 35, 48, "LINKC.txt", 6, anim, 0, 0);
        LCD_Sprite(x, 120, 35, 48, LinkCorriendo, 6, anim, 0, 0);


        V_line( x - 1, 115, 32, 0x0042);
        ladojugador = 1;

      }

      else if (control1.derecha == LOW) {

        int anim = (x / 11) % 8;
        x--;
        //LCD_Sprite(x, 120, 35, 48, LinkCorriendo, 6, anim, 1, 0);
         LCD_Sprite(x, 120, 35, 48, LinkCorriendo, 6, anim, 1, 0);
        V_line( x + 35, 120, 32, 0x0042);
        ladojugador = 2;

      }


      else if (control1.arriba == LOW) {
        //Para arriba
        for (int y = 0; y < 50; y++) {
          delay(5);
          int anim = (y / 11) % 8;
          salto++;

          //          LCD_SpriteSD(x, 120 - salto, 31, 74, "LINKS.txt", 3, anim, 0, 0);
          LCD_Sprite(x, 100 - salto, 31, 74, LinkSalto, 3, anim, 0, 0);
          V_line( x + 16, 120, 32, 0x0042);
        }
        int alturafinal = 0;
        alturafinal = 120 - salto;
        salto = 0;

        for (int y = 0; y < 40; y++) {
          delay(5);
          int anim = (y / 11) % 8;
          salto++;

          // LCD_SpriteSD(x, 120 + salto, 31, 74, "LINKS.txt", 3, anim, 0, 0);
          LCD_Sprite(x, alturafinal + salto, 31, 74, LinkSalto , 3, anim, 0, 0);
          V_line( x + 16, 120, 32, 0x0042);
        }
        salto = 0;

      }



      else if (control1.ataque1 == LOW) {
        arraySize = 103*48*2*4;
        array1 = (uint8_t*)malloc(arraySize*sizeof(uint8_t));
        ExtraerDelSD("LINKA1.txt",&array1[0]);
        for (int y = 0; y < 60; y++) {
          delay(5);
          int anim = (y / 11) % 8;
          //  LCD_SpriteSD(x, 120, 103, 48, "LINKA1.txt", 4, anim, 0, 0);
          LCD_Sprite(x, 120, 103, 48, &array1[0], 4, anim, 0, 0);
        }

      }


      else {
        arraySize = 22*48*2*2;
        free(array1);
        array1 = (uint8_t*)malloc(arraySize*sizeof(uint8_t));
        ExtraerDelSD("LINKP.txt", &array1[0]);
        int anim = (x / 11) % 8;
        //  LCD_SpriteSD(x, 120, 22, 48, "LINKP.txt", 2, anim, 0, 0);
        LCD_Sprite(x, 120, 22, 48, &array1[0], 2, anim, 0, 0);
        V_line( x + 16, 120, 22, 0x0042);
      }
    }

  }
}



//*********************************************
// Función para inicializar LCD
//*********************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //**************
  // Secuencia de Inicialización
  //**************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //**************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //**************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //**************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //**************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //**************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //**************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //**************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //**************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //**************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //**************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //**************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //**************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //**************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}


void LCD_Init5408(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //**************
  // Secuencia de Inicialización
  //**************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //**************

  writecmddata(0x00e5, 0x8000);
  writecmddata(0x0000, 0x0001);
  writecmddata(0x0001, 0x100);
  writecmddata(0x0002, 0x0700);
  writecmddata(0x0003, 0x1030);
  writecmddata(0x0004, 0x0000);
  writecmddata(0x0008, 0x0202);
  writecmddata(0x0009, 0x0000);
  writecmddata(0x000A, 0x0000);
  writecmddata(0x000C, 0x0000);
  writecmddata(0x000D, 0x0000);
  writecmddata    (0x000F, 0x0000);
  //-----Power On sequence-----------------------
  writecmddata    (0x0010, 0x0000);
  writecmddata    (0x0011, 0x0007);
  writecmddata    (0x0012, 0x0000);
  writecmddata    (0x0013, 0x0000);
  delay(50);
  writecmddata    (0x0010, 0x17B0); //SAP=1, BT=7, APE=1, AP=3
  writecmddata    (0x0011, 0x0007); //DC1=0, DC0=0, VC=7
  delay(10);
  writecmddata    (0x0012, 0x013A); //VCMR=1, PON=3, VRH=10
  delay(10);
  writecmddata    (0x0013, 0x1A00); //VDV=26
  writecmddata    (0x0029, 0x000c); //VCM=12
  delay(10);
  //-----Gamma control-----------------------
  writecmddata    (0x0030, 0x0000);
  writecmddata    (0x0031, 0x0505);
  writecmddata    (0x0032, 0x0004);
  writecmddata    (0x0035, 0x0006);
  writecmddata    (0x0036, 0x0707);
  writecmddata    (0x0037, 0x0105);
  writecmddata    (0x0038, 0x0002);
  writecmddata    (0x0039, 0x0707);
  writecmddata    (0x003C, 0x0704);
  writecmddata    (0x003D, 0x0807);
  //-----Set RAM area-----------------------
  writecmddata    (0x0060, 0xA700);    //GS=1
  writecmddata    (0x0061, 0x0001);
  writecmddata    (0x006A, 0x0000);
  writecmddata    (0x0021, 0x0000);
  writecmddata    (0x0020, 0x0000);
  //-----Partial Display Control------------
  writecmddata    (0x0080, 0x0000);
  writecmddata    (0x0081, 0x0000);
  writecmddata    (0x0082, 0x0000);
  writecmddata    (0x0083, 0x0000);
  writecmddata    (0x0084, 0x0000);
  writecmddata    (0x0085, 0x0000);
  //-----Panel Control----------------------
  writecmddata    (0x0090, 0x0010);
  writecmddata    (0x0092, 0x0000);
  writecmddata    (0x0093, 0x0003);
  writecmddata    (0x0095, 0x0110);
  writecmddata    (0x0097, 0x0000);
  writecmddata    (0x0098, 0x0000);
  //-----Display on----------------------
  writecmddata    (0x0007, 0x0173);
  delay(50);

  //************
  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}

//*********************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//*********************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//*********************************************
// Función para enviar datos a la LCD - parámetro (dato)
//*********************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//*********************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//*********************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//*********************************************
// Función para borrar la pantalla - parámetros (color)
//*********************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*********************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*********************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*********************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//*********************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*********************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y + i, w, c);
  }
}
//*********************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//*********************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//*********************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//*********************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//*********************************************
void LCD_Sprite(int x, int y, int width, int height, uint8_t* bitmap, int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(*(bitmap+k));
        LCD_DATA(*(bitmap+k+1));
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(*(bitmap+k));
        LCD_DATA(*(bitmap+k+1));
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}

void ExtraerDelSD(char name[], uint8_t* data){
  myFile = SD.open(name);
  char a;
  char C[4];
  int n;
  int m=0;
  if (myFile) { //Si se logro abrir de manera correcta
    // read from the file until there's nothing else in it:
    while (myFile.available()) { //Se va leyendo cada caracter del archivo hasta que ya se hayan leido todos
      n=0;
      while (1){
       // Serial.println("C3");
      a=myFile.read();  
      //Serial.println("C4");
      if ((a==',')|(myFile.available()==0)){
        break;
        }
      else{
        if (a==' '){
          }
        else{
        C[n]=a;
        n++;
        }
        }
      }                 
      uint8_t num = (uint8_t)strtol(C, NULL, 16);
      //Serial.println("C5");
      *(data+m)=num;
      //Serial.println("C6");
      m++;
    }
    // close the file:
    myFile.close(); //Se cierra el archivo
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Hola.txt");
  }
  }

void LCD_FondoSD(char name[]){  
  Serial.println("C0");
  LCD_CMD(0x02c); // write_memory_start
  Serial.println("C1");
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  Serial.println("C2");
  SetWindows(0, 0, 319, 239);
  Serial.println("C3");
  myFile = SD.open(name);
  Serial.println("C4");
  char a;
  char C[2];
  int n;
  if (myFile) { //Si se logro abrir de manera correcta
    Serial.println("C5");
    // read from the file until there's nothing else in it:
    while (myFile.available()) { //Se va leyendo cada caracter del archivo hasta que ya se hayan leido todos
      n=0;
      while (1){
      a=myFile.read();  
      if ((a==',')|(myFile.available()==0)){
        break;
        }
      else{
        C[n]=a;
        n++;
        }
      }               
      uint8_t num = (uint8_t)strtol(C, NULL, 16);
      LCD_DATA(num);
    }
    // close the file:
    myFile.close(); //Se cierra el archivo
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Hola.txt");
  }
  digitalWrite(LCD_CS, HIGH);
}

void configureTimer1A(){
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // Enable Timer 1 Clock
  ROM_IntMasterEnable(); // Enable Interrupts
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC); // Configure Timer Operation as Periodic
  // La frecuecia está dada por: MasterClock / CustomValue
  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, 10000000); // El último argumento es el CustomValue
  TimerIntRegister(TIMER1_BASE, TIMER_A, &Timer1AHandler);
  ROM_IntEnable(INT_TIMER1A);  // Enable Timer 1A Interrupt
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // Timer 1A Interrupt when Timeout
  ROM_TimerEnable(TIMER1_BASE, TIMER_A); // Start Timer 1A
}

void Timer1AHandler(void){
  //Required to launch next interrupt
  ROM_TimerIntClear(TIMER1_BASE, TIMER_A);
  if (nota==0){
    int noteDuration = 2000/noteDurations[nota];
    tone(buzzerPin, melody[nota],noteDuration);
    nota++;
    }
  else {
  if (pow(2,4-cont)==noteDurations[nota-1]){
    noTone(buzzerPin);
    cont=0;
    int noteDuration = 2000/noteDurations[nota];
    tone(buzzerPin, melody[nota],noteDuration);
    nota++;
    }
  else {
    cont++;
    }
  if (nota==(sizeof(noteDurations)/4+1)){
    Serial.println("A0");
    nota=0;
    cont=0;
    noTone(buzzerPin);
    }
  }
}
