

/*********************************************************************************
 **
 **  LVFA_Firmware - Provides Basic Arduino Sketch For Interfacing With LabVIEW.
 **
 **  Written By:    Sam Kristoff - National Instruments
 **  Written On:    November 2010
 **  Last Updated:  Dec 2011 - Kevin Fort - National Instruments
 **
 **  This File May Be Modified And Re-Distributed Freely. Original File Content
 **  Written By Sam Kristoff And Available At www.ni.com/arduino.
 **
 *********************************************************************************/


/*********************************************************************************
 **
 ** Includes.
 **
 ********************************************************************************/ 
// Standard includes.  These should always be included.
#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include "LabVIEWInterface.h"   // Library for Labview interface
#include "DHT.h"    // DHT11 library    
#include "string.h"
#include "math.h"
#include "U8glib.h" // LCD 128X64 library

/********************************************************************************/
U8GLIB_ST7920_128X64 u8g(13,11,10, U8G_PIN_NONE); // LCD's pin definition
const int DHTPIN = 4;      // DHT11 output port
const int DHTTYPE = DHT11;  

DHT dht(DHTPIN, DHTTYPE); 

// used to convert float to string
void reverse(char* str, int len) {
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// used to convert float to string
int intToStr(int x, char str[], int d) {
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
  
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
  
    reverse(str, i);
    str[i] = '\0';
    return i;
}

// function to convert float to string
void ftoa(float n, char* res, int afterpoint) {
    // Extract integer part
    int ipart = (int)n;
    // Extract floating part
    float fpart = n - (float)ipart;
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot
        fpart = fpart * pow(10, afterpoint);
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

void setup_LCD(void) {
  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  pinMode(8, OUTPUT);
}

void draw(char text[]) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 0, 22, text);
}

void loop_LCD(char text[]) {
  // picture loop
  u8g.firstPage();  
  do {
    draw(text);
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  //delay(50);
}

/*********************************************************************************
 **  setup()
 **
 **  Initialize the Arduino and setup serial communication.
 **
 **  Input:  None
 **  Output: None
 *********************************************************************************/
void setup() {  
  // Initialize Serial Port With The Default Baud Rate
  syncLV();

  // Place your custom setup code here
  Serial.begin(9600);
  dht.begin();       
  setup_LCD();
}


/*********************************************************************************
 **  loop()
 **
 **  The main loop.  This loop runs continuously on the Arduino.  It 
 **  receives and processes serial commands from LabVIEW.
 **
 **  Input:  None
 **  Output: None
 *********************************************************************************/
void loop()
{   
  // Check for commands from LabVIEW and process them.    
  checkForCommand();
  
  // Place your custom loop code here (this may slow down communication with LabVIEW)
  float h = dht.readHumidity();    
  float t = dht.readTemperature(); 
  char DHT_result[100];
  char h_str[10];
  char t_str[10];
  ftoa(h, h_str, 2);
  ftoa(t, t_str, 2);
  memset(DHT_result, 0, strlen(DHT_result)); //empty default string
  // asign value to DHT_default string
  if (h >= 30 && h <= 80 && t >= 20 && t <= 35) { 
    strcat(DHT_result, t_str);
    strcat(DHT_result, "C ");
    strcat(DHT_result, h_str);
    strcat(DHT_result, "% ");
  } else {
    if (h < 30) {
      strcat(DHT_result, "Humid low! ");
      strcat(DHT_result, h_str);
      strcat(DHT_result, "% ");
    };
    if (t < 20) {
      strcat(DHT_result, "Temp low! ");
      strcat(DHT_result, t_str);
      strcat(DHT_result, "C ");
    };
    if (h > 80) {
      strcat(DHT_result, "Humid high! ");
      strcat(DHT_result, h_str);
      strcat(DHT_result, "% ");
    };
    if (t > 35) {
      strcat(DHT_result, "Temp high! ");
      strcat(DHT_result, t_str);
      strcat(DHT_result, "C ");
    };
  };
  Serial.print(DHT_result);              
  Serial.println();               
  delay(1000);           
  loop_LCD(DHT_result);  

  // Default code
  if(acqMode==1)
  {
    sampleContinously();
  }

}
