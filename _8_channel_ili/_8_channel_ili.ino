/*
 * You can copy and reuse this code but do NOT remove this header informatin.
 * Author: Rafal Fratczak, 2017
 * Contact: fratczak.karolew@gmail.com
 */

#include <SPI.h>
#include "variables.h"
#include "tftlcd24.h"

void setup(void){
  //Serial.begin(9600);
  pinMode(SOUND, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(LCD_LIGHT, OUTPUT);
  digitalWrite(SOUND, 0); //zmienilem na 1 z 0
  digitalWrite(RELAY, 0);
  analogWrite(LCD_LIGHT, 255);
  
  tft.begin();
  if (!ts.begin()) {
    digitalWrite(SOUND, 1);
    delay(5000);
    digitalWrite(SOUND, 0);
  }

 // _eepromReset();
  _eepromReadConfig();
  setColorMode();
  printTemperature(temp, temperatureUnit[global_temp_unit_set]);
}

void loop(){
  if (ts.touched()) {
    delay(20);
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
}
