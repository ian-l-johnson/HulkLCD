/* HulkLCD display driver */

#include <LiquidCrystal.h>

// LCD control pins
#define PIN_D4 4  // LCD D4
#define PIN_D5 5  // LCD D5
#define PIN_D6 6  // LCD D6
#define PIN_D7 7  // LCD D7
#define PIN_E  3  // LCD E 
#define PIN_RS 2  // LCD RS
#define PIN_LCD 8   // LCD backlight control

// LCD char position tracking
byte row=0, col=0;
int  first_boot;

// initialize the library with the numbers of the interface pins
//            lcd(RS, E, D4, D5, D6, D7);
LiquidCrystal lcd(2,  3,  4,  5,  6,  7);

// ROM characters of interest:
//   161 -> degree lower
//   162 -> upper bracket
//   163 -> lower bracket
//   164 -> small \
//   165 -> degree middle
//   219 -> hollow square
//   223 -> degree symbol
//   224 -> alpha
//   226 -> beta
//   228 -> mu
//   235 -> small star
//   244 -> omega
//   247 -> pi
//   255 -> solid square

// ----- SPECIAL CHARS (export this if needed) -----
#define LCD_CHAR_CLR   17  /* clear LCD        */
#define LCD_CHAR_BON   18  /* backlight ON     */
#define LCD_CHAR_BOF   19  /* backlight OFF    */
#define LCD_CHAR_BAR   20  /* '_' (bar)        */
#define LCD_CHAR_STR   21  /* '|' (right side) */
#define LCD_CHAR_STL   22  /* '|' (left side)  */
#define LCD_CHAR_DOT   23  /* 'o' (dot)        */
#define LCD_CHAR_IJI   24  /* ij's I logo      */
#define LCD_CHAR_IJJ   25  /* ij's J logo      */

// custom characters 
#define CHAR_BAR  0
#define CHAR_STL  1
#define CHAR_STR  2
#define CHAR_DOT  3
#define CHAR_ij_I 4
#define CHAR_ij_J 5

// test chars
#define CHAR_c 6
#define CHAR_d 7


byte stick_left[8] = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b00000
};

byte stick_right[8] = {
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00000
};

byte bar[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111
};

byte dot[8] = {
  0b00000,
  0b00000,
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000
};

byte ij_I[8] = {  // ian's "I"  \x18
  0b01111,
  0b00110,
  0b00110,
  0b00110,
  0b00110,
  0b00110,
  0b01111,
  0b00000
};
byte ij_J[8] = {  // ian's "J"   \x19
  0b11111,
  0b00110,
  0b00110,
  0b00110,
  0b00110,
  0b00110,
  0b01100,
  0b11000
};



byte ccc[8] = {  // "t"     \x1A
  0b00011,
  0b00011,
  0b00110,
  0b00110,
  0b00110,
  0b01100,
  0b01100,
  0b11111
};
byte ddd[8] = {   // "-"     \x1B
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00111,
  0b01100,
  0b11000
};









//-------------------------------------------------------------------------------
void LCD_clear(void)
{
   lcd.clear();
   row=0;  col=0;
}  

//-------------------------------------------------------------------------------
void checkClear(void)
{
   col=0;
   row++;
   if(row>3)
   {
     row=0;  
     LCD_clear();
   }
   
}
  
//-------------------------------------------------------------------------------
void writeChar(char c)
{   

   if(c == LCD_CHAR_CLR)
   {
      LCD_clear();
      return;
   }

   if((c == LCD_CHAR_BON) || (c=='{')) // LCD backlight ON
   {
      digitalWrite(PIN_LCD,  HIGH);
      return;
   }
     
   if((c == LCD_CHAR_BOF) || (c=='}'))  // LCD backlight OFF
   {
      digitalWrite(PIN_LCD,  LOW);
      return;
   }    

   if(c==13)
   {
      checkClear();
      return;
   }


   // update the pos for the next call to writeChar()
   if(col==20)
   {
      row++;
      if(row>3)
      {
         row=0;
         LCD_clear();
      }
      col=0;
   }  
   lcd.setCursor(col, row);

   // handle custom char mapping
   if(c==LCD_CHAR_BAR) c=CHAR_BAR;  // '_' (bar)
   if(c==LCD_CHAR_STR) c=CHAR_STR;  // '|' (right side)
   if(c==LCD_CHAR_STL) c=CHAR_STL;  // '|' (left side)
   if(c==LCD_CHAR_DOT) c=CHAR_DOT;  // 'o' (dot)

   if((c >= 0) && (c <= 7))
      lcd.write((byte)c);
   else
      lcd.write(c);
   col++;
}


//-------------------------------------------------------------------------------
void setup() 
{
   int i;

   lcd.begin(20, 4);
   lcd.createChar(CHAR_BAR, bar);
   lcd.createChar(CHAR_STL, stick_left);
   lcd.createChar(CHAR_STR, stick_right);
   lcd.createChar(CHAR_DOT, dot);
    
   Serial.begin(9600);
   LCD_clear();
   delay(5);
   digitalWrite(PIN_LCD,  HIGH);  // backlight ON

   lcd.print("                                            HulkLCD v2.0");
   first_boot=1;
}


//===============================================================================
void loop() 
{
   char c;
   
   // read incoming serial data
   if (Serial.available())
   {
      if(first_boot)
      {
         // clear boot message(s)
         LCD_clear();
         first_boot=0;
      }

      c = Serial.read();   // read a single char
      writeChar(c);        // write to LCD          
   }
}
