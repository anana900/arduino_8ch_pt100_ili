/*
 * You can copy and reuse this code but do NOT remove this header informatin.
 * Author: Rafal Fratczak, 2017
 * Contact: fratczak.karolew@gmail.com
 */
 
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include "eeprom.h"
#include "temperature.h"
#include "FreeSans9pt7b.h"

#define CUSTOM_FONT FreeSans9pt7b

XPT2046_Touchscreen ts(STMPE_CS);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void printHelp();
void printMainMenu();
void printMainMenu2();
  
unsigned calculateChannelPositionY(int rowY, int shiftY = 0, int widthY = 40){
  return widthY*rowY+shiftY;
}

void prepareText(unsigned x, unsigned y, int textSize = 1, uint16_t textColor = TEXT_COLOR_W, boolean customf = false){
  tft.setTextSize(textSize);
  tft.setCursor(x, y);
  tft.setTextColor(textColor);
  if(customf) {
    tft.setFont(&CUSTOM_FONT);
  } else {
    tft.setFont();
  }
}

TS_Point getPoint24(){
  TS_Point tmpP, p = ts.getPoint();
  p.x = map(p.x, 0, 3800, 0, tft.height());
  p.y = map(p.y, 0, 4000,  tft.width(), 0);
  tmpP.x=p.x;
  p.x = p.y;
  p.y = tmpP.x;
  return p;
}

void printButton(int x, int y, int width, int height, uint16_t buttonColor, String text = "", int tx = 0, int ty = 0, int textSize = 1, uint16_t textColor = TEXT_COLOR_W){
   tft.drawRect(x, y, width, height, buttonColor);
   tft.fillRect(x, y, width, height, buttonColor);
   prepareText(tx, ty, textSize, textColor);
   tft.print(text);
}

void drawTempSlide(int temp_alarm, int Sy){
        tft.fillRect(21, 40, 18, (Sy-8)-40, BACKGROUND_COLOR);  //clean whole slide space + borders
        tft.fillRect(21, (Sy-8), 18, 16, ILI9341_RED);  //draw cursor
        tft.fillRect(21, (Sy+8), 18, 260-(Sy+8), BACKGROUND_COLOR); //clean rectangle above cursor starting from current position + half size of cursor
        tft.fillRect(20, 32, 20, 8, BACKGROUND_COLOR);  //clean rectangle above white slide
        tft.fillRect(20, 260, 20, 8, BACKGROUND_COLOR); //clean rectangle below white slide
        tft.drawRect(20, 40, 20, 220, TEXT_COLOR_W);  //draw finall white rectangle
}

void drawTemperatureAlarm(int alarmTemp){
     tft.fillRect(90, 40, 95, 50, BACKGROUND_COLOR);
     prepareText(90, 70, 3, TEXT_COLOR_W, true);
     tft.print(convertTempUnit(alarmTemp));
     tft.setFont();
}

uint16_t convertRGB( byte R, byte G, byte B){
  return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );
}

void setColorMode(){
  if(global_colors_set==0 || global_colors_set == 3){
   BACKGROUND_COLOR = 0x0000;
   TEXT_COLOR_W = 0xFFFF;
  } else {
     BACKGROUND_COLOR = 0xFFFF;
   TEXT_COLOR_W = 0x0000;
  }
}

void plusMinusButtons(int port, int x, int y){
  int przes = 50;
   prepareText(x-15, y+2, 2, TEXT_COLOR_W);
   tft.print(termometrsNumeration[port]);
   tft.drawRect(x, y, 20, 20, TEXT_COLOR_W);
   prepareText(x+3, y-1, 3, ILI9341_RED);
   tft.print("+");
   prepareText(x+przes-25, y+2, 2, TEXT_COLOR_W);
   tft.print(temp[13][port]);
   tft.drawRect(x+przes, y, 20, 20, TEXT_COLOR_W);
   prepareText(x+przes+3, y-1, 3, ILI9341_GREEN);
   tft.print("-");   
}

void displayTemperature(int i, int y){
  if(global_colors_set == 3 || global_colors_set == 1){
    byte R_color, G_color, B_color;
      tft.fillRect(TEMPERATURE_X_POSITION, y, 65, 30, BACKGROUND_COLOR);
      if(temp[0][i] >= TEMP_MAX/2){
        G_color = 0;
        B_color = 0;
        R_color = 255;
      }else if(temp[0][i] > TEMP_MAX/3 && temp[0][i] < TEMP_MAX/2){
        G_color = map(temp[0][i], TEMP_MAX/3, TEMP_MAX/2, 180 , 0);
        B_color = 0;
        R_color = 255;
      } else {
      //  G_color = 0;//255;
        G_color = map(temp[0][i], TEMP_MIN+10, 50, 255, 200);
        B_color = map(temp[0][i], TEMP_MIN+10, 50, BACKGROUND_COLOR, TEXT_COLOR_W);
       //B_color = map(temp[0][i], TEMP_MIN, 50, 200, 255);
       R_color = map(temp[0][i], TEMP_MIN+10, 50, 255, 200);
       Serial.println(R_color);
      }
      prepareText(TEMPERATURE_X_POSITION, y+22, 2,  convertRGB( R_color, G_color, B_color), true);
      tft.print(convertTempUnit(temp[0][i]));
  } else {
      tft.fillRect(TEMPERATURE_X_POSITION, y, 65, 30, BACKGROUND_COLOR);
      prepareText(TEMPERATURE_X_POSITION, y+22, 2, TEXT_COLOR_W, true);
      tft.print(convertTempUnit(temp[0][i]));
  }
}

//void printHello(unsigned long delay_ms = 5000){
//  TS_Point action;
//  tft.fillScreen(ILI9341_CYAN);
//  prepareText(60, 50, 2, TEXT_COLOR_B, true);
//  tft.println("Monitor");
//  prepareText(15, 90, 2, TEXT_COLOR_B, true);
//  tft.println("Temperatury");
//  tft.setCursor(40, 150);
//  tft.println("8 kanalow");
//  prepareText(40, 190, 1, TEXT_COLOR_B, true);
//  tft.println("Zakres pomiarowy:");
//  prepareText(30, 230, 2, TEXT_COLOR_B, true);
//  tft.print(TEMP_MIN);
//  tft.print(" do ");
//  tft.print(TEMP_MAX);
//  tft.println(" C");
//  prepareText(30, 260, 2, ILI9341_RED);
//  tft.println("www.pomiary.com");
//  if(delay_ms == 0){
//    while(true){
//      if (ts.touched()) {
//        action = getPoint24();
//        if (action.y > 0){
//          soundClic();
//          printMainMenu();
//          break;
//        }
//        delay(SHORT_CLICK_DELAY);
//      }
//    }    
//  } else {
//  delay(delay_ms);
//  tft.setCursor(0, 0);
//  tft.setTextSize(5);
//  tft.setTextWrap(true);
//  tft.fillScreen(BACKGROUND_COLOR);
//  }
//}

void printTemperature(int temperature[][SENSORS_NUMBER], char temperatureUnit){
  tft.fillScreen(BACKGROUND_COLOR);
  for(int i=0 ; i<SENSORS_NUMBER ; i++){
    y = calculateChannelPositionY(i);

    prepareText(1, y+22, 2, TEXT_COLOR_W, true);
    tft.print(termometrsNumeration[i]);
    
    prepareText(ARROW_X_POSITION, y+10, 4);
    //tft.print(char(0x1A));
    //tft.fillTriangle(ARROW_X_POSITION, y+2, ARROW_X_POSITION, y+28, ARROW_X_POSITION+20, y+15, TEXT_COLOR_W);
    tft.fillTriangle(ARROW_X_POSITION, y+2, ARROW_X_POSITION, y+28, ARROW_X_POSITION+20, y+15, TEXT_COLOR_W);

    if(temp[3][i] == 0) {
      displayTemperature(i, y);
    } else {
      tft.fillRect(ARROW_X_POSITION, y, 120, 30, BACKGROUND_COLOR);
      prepareText(ARROW_X_POSITION-5, y+15, 1, TEXT_COLOR_W, true);
      tft.print("WYLACZONY");
      temp[3][i] = 2;      
    }

    prepareText(155, y+15, 1, TEXT_COLOR_W, true);
    tft.print(char(DEGREE));
    prepareText(170, y+27, 2, TEXT_COLOR_W, true);
    tft.print(temperatureUnit);
    tft.setFont();
    
    prepareText(202, y, 2, ILI9341_RED);
    tft.print(convertTempUnit(temperature[2][i]));

    if(temp[4][i] == 1){
       prepareText(202, y+18, 2);
       tft.print("*");  
    }

    if(temp[5][i] == 1){
       prepareText(202+12, y+18, 2);
       tft.print("P");  
    }

    if(global_alarm_set == 1){
       prepareText(202+24, y+18, 2);
       tft.print("A");  
    }   
  }
}

void printTemperatureUpdate(char temperatureUnit){
  for(int i=0 ; i<SENSORS_NUMBER ; i++){
    y = calculateChannelPositionY(i);  //calculate sensor y position on display to update
    
    if(temp[3][i] == 0){
      if(temp[0][i] >= temp[2][i] && temp[4][i] == 1){ //set sound alarm flag on if condition true
        temp[6][i] = 1;
      } else if(temp[0][i] < (temp[2][i]-HistIntervals[global_hist]) && temp[4][i] == 1) {
        temp[6][i] = 0;
      }

      if(temp[4][i] == 1){
        if(temp[0][i] >= temp[2][i]){ //set sound alarm flag on if condition true
          temp[6][i] = 1;
        } else if(temp[0][i] < (temp[2][i]-HistIntervals[global_hist])) {
          temp[6][i] = 0;
        }
      } else {
        temp[6][i] = 0;
      }

      if(temp[5][i] == 1){
        if(temp[0][i] >= temp[2][i]){//set relay alarm flag on if condition true
          temp[7][i] = 1;
        } else if(temp[0][i] < (temp[2][i]-HistIntervals[global_hist])) {
          temp[7][i] = 0;
        }
      } else {
        temp[7][i] = 0;
      }
    } else {
      if(temp[3][i] == 1){
        temp[6][i] = 0;
        temp[7][i] = 0;
        tft.fillRect(ARROW_X_POSITION, y, 120, 30, BACKGROUND_COLOR);
       // Serial.println(y);
        prepareText(ARROW_X_POSITION-5, y+22, 1, TEXT_COLOR_W, true);
        tft.print("WYLACZONY");
        temp[3][i] = 2;         
      }
      continue;
    }
    
//Serial.println(temp[1][i]);
 prepareText(ARROW_X_POSITION, y, 5);
   
    if(temp[10][i] > temp[11][i] && abs(temp[10][i] - temp[11][i]) >= 2 && temp[12][i] != 1){
      temp[12][i] = 1;
      tft.fillRect(ARROW_X_POSITION, y, 42, 40, BACKGROUND_COLOR);
      prepareText(ARROW_X_POSITION, y+5, 4, ILI9341_RED);
     // tft.print(char(0x18));
      tft.fillTriangle(ARROW_X_POSITION+10, y+2, ARROW_X_POSITION, y+28, ARROW_X_POSITION+20, y+28, ILI9341_RED);
    }else if(temp[10][i] < temp[11][i] && abs(temp[10][i] - temp[11][i]) >= 2 && temp[12][i] != 2){
      temp[12][i] = 2;
      tft.fillRect(ARROW_X_POSITION, y, 42, 40, BACKGROUND_COLOR);
      prepareText(ARROW_X_POSITION, y+5, 4, ILI9341_GREEN);
     // tft.print(char(0x19));
     tft.fillTriangle(ARROW_X_POSITION+10, y+28, ARROW_X_POSITION, y+2, ARROW_X_POSITION+20, y+2, ILI9341_GREEN);
    } else if(abs(temp[10][i] - temp[11][i]) < 2 && temp[12][i] != 0){
      temp[12][i] = 0;
      tft.fillRect(ARROW_X_POSITION, y, 42, 40, BACKGROUND_COLOR);
      prepareText(ARROW_X_POSITION, y+5, 4, TEXT_COLOR_W);
     // tft.print(char(0x1A));

      tft.fillTriangle(ARROW_X_POSITION, y+2, ARROW_X_POSITION, y+28, ARROW_X_POSITION+20, y+15, TEXT_COLOR_W);
    }
      
    tft.setFont();

    if((temp[6][i] == 1 || temp[7][i] == 1) && global_alarm_set == 1){
      tft.fillCircle(ARROW_X_POSITION+35, y+18, 9, ILI9341_RED);
    } else {
      tft.fillRect(ARROW_X_POSITION+25, y, 20, 40, BACKGROUND_COLOR);
    }
    
    //&& abs(temp[8][i] - temp[11][i]) > 1 
    //Serial.println(temp[0][i]);
    if(temp[0][i] != temp[1][i]) displayTemperature(i, y);
  }
}

void printUpdateSetAlarm(int port){
    tmpRelay = temp[5][port]; 
    tmpSound = temp[4][port]; 
    tmpAlarm = temp[3][port]; 
    tmpTempAlarm = temp[2][port];
    
    TS_Point action;
    int Sx, Sy;
    while(true){
    if (ts.touched()) {
      action = getPoint24();
      if(action.y > 280 && action.x < 115){ //Anuluj
        soundClic();
        printTemperature(temp, temperatureUnit[global_temp_unit_set]);
        break; 
      } else if (action.y > 280 && action.x > 115){ //OK
        soundClic();
        savePort(port, tmpTempAlarm, tmpAlarm, tmpSound, tmpRelay);
        printTemperature(temp, temperatureUnit[global_temp_unit_set]);
        break; 
      } else if (action.y > 100 && action.y < 140 && action.x > 80){  // Pomiar
        soundClic();
        if(tmpAlarm == 0){
          tmpAlarm = 1;
          printButton(80, 100, 150, 40, ILI9341_RED, TXT_POMIAR_OFF, 90, 110, 2);
        } else {
          tmpAlarm = 0;
          printButton(80, 100, 150, 40, ILI9341_GREEN, TXT_POMIAR_ON, 90, 110, 2, TEXT_COLOR_B);
        }
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 155 && action.y < 195 && action.x > 80){
        soundClic();
        if(tmpSound == 1){
          tmpSound = 0;
          printButton(80, 155, 150, 40, ILI9341_RED, TXT_DZWIEK_OFF, 90, 165, 2);
        } else {
          tmpSound = 1;
          printButton(80, 155, 150, 40, ILI9341_GREEN, TXT_DZWIEK_ON, 90, 165, 2, TEXT_COLOR_B);
        }
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 210 && action.y < 250 && action.x > 80){
        soundClic();
        if(tmpRelay == 1){
          tmpRelay = 0;
          printButton(80, 210, 150, 40, ILI9341_RED, TXT_RELAY_OFF, 90, 220, 2);
        } else {
          tmpRelay = 1;
          printButton(80, 210, 150, 40, ILI9341_GREEN, TXT_RELAY_ON, 90, 220, 2, TEXT_COLOR_B);
        }
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 40 && action.y < 260 && action.x < 40 &&  action.x > 20){
        soundClic();
        Sy = action.y;
       // Serial.println(Sy);
        tmpTempAlarm=map(Sy, 260, 40, TEMP_MIN, TEMP_MAX);
        drawTemperatureAlarm(tmpTempAlarm);
        drawTempSlide(tmpTempAlarm, Sy);
      } else if(action.y > 50 && action.y < 70 && action.x < 70 &&  action.x > 50) {
        soundClic();
        if(tmpTempAlarm < TEMP_MAX){
          tmpTempAlarm += 1;
          drawTemperatureAlarm(tmpTempAlarm);
          Sy=map(tmpTempAlarm, TEMP_MIN, TEMP_MAX, 260, 40);
          drawTempSlide(tmpTempAlarm, Sy);                   
        }
        delay(SHORT_CLICK_DELAY);
      } else if(action.y > 240 && action.y < 260 && action.x < 70 &&  action.x > 50) {
        soundClic();
        if(tmpTempAlarm > TEMP_MIN){
          tmpTempAlarm -= 1;
          drawTemperatureAlarm(tmpTempAlarm);
          Sy=map(tmpTempAlarm, TEMP_MIN, TEMP_MAX, 260, 40);
          drawTempSlide(tmpTempAlarm, Sy);                   
        }
        delay(SHORT_CLICK_DELAY);        
      }
    }
  }
}

void printSetAlarm(TS_Point p){
  int port = map(p.y, 0, tft.height(), 0, sizeof(termometrsNumeration));
  tft.fillScreen(BACKGROUND_COLOR);
  prepareText(5, 10, 2, TEXT_COLOR_W);  
  tft.print("USTAWIENIE ALARMU ");
  tft.println(termometrsNumeration[port]);
  
  int Sy=map(temp[2][port], TEMP_MIN, TEMP_MAX, 260, 40);
  drawTempSlide(temp[2][port], Sy);
    drawTemperatureAlarm(temp[2][port]);
    prepareText(195, 55, 1, TEXT_COLOR_W, true);
    tft.print(char(DEGREE));
    prepareText(210, 72, 2, TEXT_COLOR_W, true);
    tft.print(temperatureUnit[global_temp_unit_set]);
    tft.setFont();
    
   tft.drawRect(50, 40, 20, 20, TEXT_COLOR_W);
   prepareText(53, 39, 3, ILI9341_RED);
   tft.print("+");
   tft.drawRect(50, 240, 20, 20, TEXT_COLOR_W);
   prepareText(53, 239, 3, ILI9341_GREEN);
   tft.print("-");   
      
  if(temp[3][port] == 0){
    printButton(80, 100, 150, 40, ILI9341_GREEN, TXT_POMIAR_ON, 90, 110, 2, TEXT_COLOR_B);
  } else {
    printButton(80, 100, 150, 40, ILI9341_RED, TXT_POMIAR_OFF, 90, 110, 2);
  }
  if(temp[4][port] == 1){
    printButton(80, 155, 150, 40, ILI9341_GREEN, TXT_DZWIEK_ON, 90, 165, 2, TEXT_COLOR_B);
  } else {
    printButton(80, 155, 150, 40, ILI9341_RED, TXT_DZWIEK_OFF, 90, 165, 2);
  }
  if(temp[5][port] == 1){
    printButton(80, 210, 150, 40, ILI9341_GREEN, TXT_RELAY_ON, 90, 220, 2, TEXT_COLOR_B);
  } else {
    printButton(80, 210, 150, 40, ILI9341_RED, TXT_RELAY_OFF, 90, 220, 2);
  }
  
  printButton(10, 280, 105, 35, ILI9341_RED, TXT_ANULUJ, 20, 290, 2);
  printButton(125, 280, 105, 35, ILI9341_GREEN, TXT_OK, 165, 290, 2, TEXT_COLOR_B);
  printUpdateSetAlarm(port);
}

void printUpdateMainMenu(){
  tmpUnit = global_temp_unit_set;
  tmpAlarm = global_alarm_set;
  tmpSound = global_sound_set;
  tmpColor = global_colors_set;
  tmpBacklight = global_bl_set;
  tmpBacklightVal = global_bl_val_set;
  tmpHist = global_hist;
  TS_Point action;
  
    while(true){
    if (ts.touched()) {
      action = getPoint24();
      if(menu2Access){
        menu2Access = false;
        menu2Timer = millis();
      }
      if(action.y > 280 && action.x < 115){ //Anuluj
        soundClic();
        printTemperature(temp, temperatureUnit[global_temp_unit_set]);
        break; 
      } else if (action.y > 280 && action.x > 115){ //OK
        soundClic();
        saveGlobal(tmpUnit, tmpSound, tmpAlarm, tmpColor, tmpBacklight, tmpBacklightVal, tmpHist);
        setColorMode();
        printTemperature(temp, temperatureUnit[global_temp_unit_set]);
        break; 
      } else if (action.y > 0 && action.y < 60){
       // soundClic();
        if((millis() - menu2Timer > FACTORY_DELAY) && menu2Access == false){
          soundClic(100);
          delay(200);
          soundClic(100);
          printMainMenu2();
          break;
        }
      } else if (action.y > 60 && action.y < 100 ){
        soundClic();
        if(tmpUnit < sizeof(temperatureUnit)-1){
          ++tmpUnit;
        } else {
          tmpUnit = 0;
        }
        prepareText(40, 55, 2, ILI9341_GREEN);
        //cleanSign(11);
        tft.fillRect(40, 55, 190, 30, ILI9341_GREEN);
        prepareText(40, 55, 2, TEXT_COLOR_B);
        tft.print("Jednostka ");
        tft.print(temperatureUnit[tmpUnit]);
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 100 && action.y < 140){
        soundClic();
        if(tmpSound == 1){
          tmpSound = 0;
          printButton(10, 100, 220, 40, ILI9341_RED, TXT_DZW_DOT_OFF, 30, 110, 2);
        } else {
          tmpSound = 1;
          printButton(10, 100, 220, 40, ILI9341_GREEN, TXT_DZW_DOT_ON, 30, 110, 2, TEXT_COLOR_B);
        }
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 155 && action.y < 195){
        soundClic();
        if(tmpAlarm == 1){
          tmpAlarm = 0;
          printButton(10, 155, 220, 40, ILI9341_RED, TXT_ALARM_OFF, 40, 165, 2);
        } else {
          tmpAlarm = 1;
          printButton(10, 155, 220, 40, ILI9341_GREEN, TXT_ALARM_ON, 40, 165, 2, TEXT_COLOR_B);
        }
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 210 && action.y < 265 && action.x > 10 && action.x < 59){
        soundClic();
        if(tmpBacklight == 0){
          tmpBacklight = 1;
          printButton(10, 210, 49, 50, ILI9341_GREEN, "Pod", 12, 215, 2, TEXT_COLOR_B);
          prepareText(12, 240, 2, TEXT_COLOR_B);
          tft.print(tmpBacklight*9);tft.print(" s");
        } else if(tmpBacklight == 1){
          tmpBacklight = 2;
          printButton(10, 210, 49, 50, ILI9341_GREEN, "Pod", 12, 215, 2, TEXT_COLOR_B);
          prepareText(12, 240, 2, TEXT_COLOR_B);
          tft.print(tmpBacklight*9);tft.print(" s");
        } else if(tmpBacklight == 2){
          tmpBacklight = 0;
          printButton(10, 210, 49, 50, ILI9341_GREEN, "Pod", 12, 215, 2, TEXT_COLOR_B);
          prepareText(12, 240, 2, TEXT_COLOR_B);
          tft.print(tmpBacklight*9);tft.print(" s");
        } 
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 210 && action.y < 260 && action.x > 67 && action.x < 116){
        soundClic();
        if(tmpBacklightVal == 1){
          tmpBacklightVal = 2;
          brightSet(tmpBacklightVal);
          printButton(67, 210, 49, 50, ILI9341_GREEN, "Jas", 69, 215, 2, TEXT_COLOR_B);
          prepareText(69, 240, 2, TEXT_COLOR_B);
          tft.print(tmpBacklightVal*20+40);tft.print("%");
        } else if(tmpBacklightVal == 2){
          tmpBacklightVal = 3;
          brightSet(tmpBacklightVal);
          printButton(67, 210, 49, 50, ILI9341_GREEN, "Jas", 69, 215, 2, TEXT_COLOR_B);
          prepareText(69, 240, 2, TEXT_COLOR_B);
          tft.print(tmpBacklightVal*20+40);tft.print("%");
        } else if(tmpBacklightVal == 3){
          tmpBacklightVal = 1;
          brightSet(tmpBacklightVal);
          printButton(67, 210, 49, 50, ILI9341_GREEN, "Jas", 69, 215, 2, TEXT_COLOR_B);
          prepareText(69, 240, 2, TEXT_COLOR_B);
          tft.print(tmpBacklightVal*20+40);tft.print("%");
        }      
        delay(SHORT_CLICK_DELAY);
      } else if (action.y > 210 && action.y < 260 && action.x > 124 && action.x < 173){
        soundClic();
        if(tmpHist < ((byte)sizeof(HistIntervals)-1)) { //16){
          tmpHist++;
          printButton(124, 210, 49, 50, ILI9341_GREEN, "His", 126, 215, 2, TEXT_COLOR_B);
          prepareText(126, 240, 2, TEXT_COLOR_B);
          tft.print(HistIntervals[tmpHist]);
          prepareText(150, 245, 1, TEXT_COLOR_B, true);tft.print(" ");tft.print(char(DEGREE));prepareText(126, 240, 2, TEXT_COLOR_B);
        } else if(tmpHist == ((byte)sizeof(HistIntervals)-1)) { //16){
          tmpHist = 0;
          printButton(124, 210, 49, 50, ILI9341_GREEN, "His", 126, 215, 2, TEXT_COLOR_B);
          prepareText(126, 240, 2, TEXT_COLOR_B);
          tft.print(HistIntervals[tmpHist]);
          prepareText(150, 245, 1, TEXT_COLOR_B, true);tft.print(" ");tft.print(char(DEGREE));prepareText(126, 240, 2, TEXT_COLOR_B);
        }      
        delay(SHORT_CLICK_DELAY);    
      } else if (action.y > 210 && action.y < 260 && action.x > 181 && action.x < 230){
        soundClic();
        if(tmpColor == 0){
          tmpColor = 1;
          printButton(181, 210, 49, 50, ILI9341_GREEN, "Den", 183, 215, 2, TEXT_COLOR_B);
          prepareText(183, 240, 2, TEXT_COLOR_B);
          tft.print("Kol");
        } else if(tmpColor == 1) {
          tmpColor = 2;
          printButton(181, 210, 49, 50, ILI9341_GREEN, "Den", 183, 215, 2, TEXT_COLOR_B);
          prepareText(183, 240, 2, TEXT_COLOR_B);
          tft.print("Mon");
        } else if(tmpColor == 2) {
          tmpColor = 3;
          printButton(181, 210, 49, 50, ILI9341_GREEN, "Noc", 183, 215, 2, TEXT_COLOR_B);
          prepareText(183, 240, 2, TEXT_COLOR_B);
          tft.print("Kol");
        }  else {
          tmpColor = 0;
          printButton(181, 210, 49, 50, ILI9341_GREEN, "Noc", 183, 215, 2, TEXT_COLOR_B);
          prepareText(183, 240, 2, TEXT_COLOR_B);
          tft.print("Mon");
        }
        delay(SHORT_CLICK_DELAY);
      }
    } else {
        menu2Access = true;
    }
  } 
}

void printMainMenu(){
  tft.fillScreen(BACKGROUND_COLOR);
  prepareText(20, 10, 2, TEXT_COLOR_W);  
  tft.print("USTAWIENIA OGOLNE");
  
  printButton(10, 45, 220, 40, ILI9341_GREEN, "Jednostka ", 40, 55, 2, TEXT_COLOR_B);
  tft.print(temperatureUnit[global_temp_unit_set]);
    
  if(global_sound_set == 1){
    printButton(10, 100, 220, 40, ILI9341_GREEN, TXT_DZW_DOT_ON, 30, 110, 2, TEXT_COLOR_B);
  } else {
    printButton(10, 100, 220, 40, ILI9341_RED, TXT_DZW_DOT_OFF, 30, 110, 2);
  }
  if(global_alarm_set == 1){
    printButton(10, 155, 220, 40, ILI9341_GREEN, TXT_ALARM_ON, 40, 165, 2, TEXT_COLOR_B);
  } else {
    printButton(10, 155, 220, 40, ILI9341_RED, TXT_ALARM_OFF, 40, 165, 2);
  }
  printButton(10, 210, 49, 50, ILI9341_GREEN, "Pod", 12, 215, 2, TEXT_COLOR_B);
  prepareText(12, 240, 2, TEXT_COLOR_B);
  tft.print(global_bl_set*9);tft.print(" s");

  printButton(67, 210, 49, 50, ILI9341_GREEN, "Jas", 69, 215, 2, TEXT_COLOR_B);
  prepareText(69, 240, 2, TEXT_COLOR_B);
  tft.print(global_bl_val_set*20+40);tft.print("%");
  
  printButton(124, 210, 49, 50, ILI9341_GREEN, "His", 126, 215, 2, TEXT_COLOR_B);
  prepareText(126, 240, 2, TEXT_COLOR_B);
  tft.print(HistIntervals[global_hist]);
  prepareText(150, 245, 1, TEXT_COLOR_B, true);tft.print(" ");tft.print(char(DEGREE));prepareText(126, 240, 2, TEXT_COLOR_B);

  if(global_colors_set == 1){
    printButton(181, 210, 49, 50, ILI9341_GREEN, "Den", 183, 215, 2, TEXT_COLOR_B);
    prepareText(183, 240, 2, TEXT_COLOR_B);
    tft.print("Kol");
  } else if(global_colors_set == 2) {
    printButton(181, 210, 49, 50, ILI9341_GREEN, "Den", 183, 215, 2, TEXT_COLOR_B);
    prepareText(183, 240, 2, TEXT_COLOR_B);
    tft.print("Mon");
  }  else if(global_colors_set == 3) {
    printButton(181, 210, 49, 50, ILI9341_GREEN, "Noc", 183, 215, 2, TEXT_COLOR_B);
    prepareText(183, 240, 2, TEXT_COLOR_B);
    tft.print("Kol");
  } else {
    printButton(181, 210, 49, 50, ILI9341_GREEN, "Noc", 183, 215, 2, TEXT_COLOR_B);
    prepareText(183, 240, 2, TEXT_COLOR_B); 
    tft.print("Mon");
  }
  
  printButton(10, 280, 105, 35, ILI9341_RED, TXT_ANULUJ, 20, 290, 2);
  printButton(125, 280, 105, 35, ILI9341_GREEN, TXT_OK, 165, 290, 2, TEXT_COLOR_B);
  printUpdateMainMenu();
}

void printUpdateMainMenu2(){
  TS_Point action; 
  while(true){
    if (ts.touched()) {
      action = getPoint24();
      if(action.y > 280 && action.x < 115){ //Save
        soundClic();
        saveTempCali(-1, 0);
        printButton(10, 280, 105, 35, ILI9341_GREEN, "Save", 20, 290, 2, TEXT_COLOR_B);
        delay(200);
        printButton(10, 280, 105, 35, ILI9341_YELLOW, "Save", 20, 290, 2, TEXT_COLOR_B);       
      } else if (action.y > 280 && action.x > 115){ //OK
        soundClic();
        saveTempCali(-1, 0);
        printTemperature(temp, temperatureUnit[global_temp_unit_set]);
        break; 
    } else if (action.y > 45 && action.y < 85) {
       _eepromReset();
       //tft.fillRect(10, 45, 220, 40, ILI9341_YELLOW);
       printButton(10, 45, 220, 40, ILI9341_YELLOW, "Resetowanie ...", 40, 57, 2, TEXT_COLOR_B);
       for(int i=10; i<220 ;i++){
         tft.fillRect(10, 80, i, 5, ILI9341_RED);
        delay(8);
       }
      delay(SHORT_CLICK_DELAY);
      _eepromReadConfig();
      setColorMode();
      printTemperature(temp, temperatureUnit[global_temp_unit_set]);
      break;
    } else {
      for(int port = 0 ; port < 8 ; port++){
        int xshift = 0, ydown = 0, corr = 0;
        if(port<4) {
          ydown = 40*port;
        } else {
          ydown = 40*(port-4);
          xshift = 120;
          corr = 15;
        }
        if(action.x > xshift+35-corr && action.x < xshift+55-corr && action.y > ydown+100 && action.y < ydown+120){
          (temp[13][port] < 99) ? temp[13][port]++ : temp[13][port] = 0;
          tft.fillRect(xshift+56, ydown+100, 26, 20, BACKGROUND_COLOR);
          prepareText(xshift+56+3, ydown+100+2, 2, TEXT_COLOR_W);
          tft.print(temp[13][port]);         
        } else if(action.x > xshift+35+50-corr && action.x < xshift+55+50-corr && action.y > ydown+100 && action.y < ydown+120){
          (temp[13][port] >= 0) ? temp[13][port]-- : temp[13][port] = 99;
          tft.fillRect(xshift+56, ydown+100, 26, 20, BACKGROUND_COLOR);
          prepareText(xshift+56+3, ydown+100+2, 2, TEXT_COLOR_W);
          tft.print(temp[13][port]);         
        }
        delay(SHORT_CLICK_DELAY/2);
        }
      }   
    }
  } 
}

void printMainMenu2(){
  tft.fillRect(0, 0, 240, 320, BACKGROUND_COLOR);
  prepareText(20, 10, 2, TEXT_COLOR_W);  
  tft.print(" OPCJE FABRYCZNE");

  printButton(10, 45, 220, 40, ILI9341_YELLOW, "Reset Pamieci", 40, 57, 2, TEXT_COLOR_B);

  plusMinusButtons(0, 35, 100); plusMinusButtons(4, 155, 100);
  plusMinusButtons(1, 35, 140); plusMinusButtons(5, 155, 140);
  plusMinusButtons(2, 35, 180); plusMinusButtons(6, 155, 180);
  plusMinusButtons(3, 35, 220); plusMinusButtons(7, 155, 220);

  printButton(10, 280, 105, 35, ILI9341_YELLOW, "Save", 20, 290, 2, TEXT_COLOR_B);
  printButton(125, 280, 105, 35, ILI9341_GREEN, TXT_OK, 165, 290, 2, TEXT_COLOR_B);
  printUpdateMainMenu2();
}

void printChannelChart(int channel){
  tft.drawRect(5, 5, 230, 25, TEXT_COLOR_W);

  tft.drawRect(5, 35, 230, 280, TEXT_COLOR_W); //wx 230, wy 280
}

