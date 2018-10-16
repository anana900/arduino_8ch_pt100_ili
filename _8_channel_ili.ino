/*
 * You can copy and reuse this code but do NOT remove this header informatin.
 * Author: Rafal Fratczak, 2017
 * Contact: fratczak.karolew@gmail.com
 */

#include <SPI.h>
#include "variables.h"
#include "tftlcd24.h"
//#include "SDLogger.h"
//#include "ds1307.h"

void setup(void){
  Serial.begin(9600);
  tft.begin();
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }

  pinMode(SOUND, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LCD_LIGHT, OUTPUT);
  digitalWrite(SOUND, 0); //zmienilem na 1 z 0
  digitalWrite(RELAY, 0);
  analogWrite(LCD_LIGHT, 255);

//  SDInitialization();

//  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
//    while (1);
//  }

//  if (! rtc.isrunning()) {
//    Serial.println("RTC is NOT running!");
//    // following line sets the RTC to the date & time this sketch was compiled
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    // This line sets the RTC with an explicit date & time, for example to set
//    // January 21, 2014 at 3am you would call:
//    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
//  }

//  _eepromReset();
  _eepromReadConfig();
  setColorMode();
//  printHello(20000);
  printTemperature(temp, temperatureUnit[global_temp_unit_set]);
}

void loop(){
  if (ts.touched()) {
    backlightTimer = millis();

    if(pressedActive == false){
      pressedActive = true;
      pressedTimer = millis();
      soundClic();
    }
    if((millis() - pressedTimer > LONG_CLICK_DELAY) && (longPressActive == false)){
      soundClic(100);
      longPressActive = true;
      printMainMenu();
      backlightTimer = millis();
    }
  } else {   
    if (pressedActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        if(tmpBacklightFirstT == 1) {
          printSetAlarm(getPoint24());
          backlightTimer = millis();
        } else {
          delay(500);
          tmpBacklightFirstT = 1;
        }
      }
      pressedActive = false;
    }
  }

  backLight();
  calculateTemperature();
  printTemperatureUpdate(temperatureUnit[global_temp_unit_set]);
  relayAndSoundAlarmsControl();

//  if((millis() - sdRecordTimer > 5000)){
//      //printTime();
////      SDInitialization();
//   // test();
//     printsd();
//    sdRecordTimer = millis();
//  }
  
}

