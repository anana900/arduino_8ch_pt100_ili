/*
 * You can copy and reuse this code but do NOT remove this header informatin.
 * Author: Rafal Fratczak, 2017
 * Contact: fratczak.karolew@gmail.com
 */
 
/*
 * Use global variables predefined in variables.h
 * Read configuration from EEPROM once and save it in appriopriate global variables during setup step.
 * Every changes of configuration confirmed by Save, OK or similar button are saved to EEPROM and global variables are updated.
 * Global variables:
int temp[13][SENSORS_NUMBER] = {{0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0}, 
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0},                                                    
                              {0, 0, 0, 0, 0, 0, 0, 0}}; 
                      // array to keep temperature parameters:
                      // [0][*] keep actual temperature read from sensor
                      // [1][*] temperature history average
                      // [2][*] keep alarm temperature
                      // [3][*] port active
                      // [4][*] sound alarm control
                      // [5][*] relay alarm control
                      // [6][*] activation of sound alarm flag
                      // [7][*] activation of relay alarm flag
                      // [8][*] temperature history 1
                      // [9][*] temperature history 2
                      // [10][*] temperature trend 3
                      // [11][*] temperature trend 4
                      // [12][*] arrows: 0 ->, 1 up, 2 down
int global_alarm_set,     //0 or 1 - relay - on/off
    global_sound_set,     //0 or 1 - sourd - on/off
    global_temp_unit_set, //store 3 values, each represent unit
    global_colors_set,    //store 4 values 0 - 3, each represents specific tft color layout
    global_bl_set,        //have 2 functioms: 1 - is used as a flag to set the back light on (1) off (0). 2 - is used as backlight time
    global_bl_val_set,    //is used as backlight bright control value
    global_hist,          //used to keep the histeresis alarm value: 0 - 10
    global_sd_set;        //keeps logger intervals minutes
*/

#include <EEPROM.h>
#define RANGE 8
#define _EEPROM_START 32
#define _EEPROM_AT_START 0      //(0*RANGE)
#define _EEPROM_A_START (1*RANGE)
#define _EEPROM_SA_START (2*RANGE)
#define _EEPROM_RA_START (3*RANGE)
#define _EEPROM_TEMP_CAL (4*RANGE)
#define _EEPROM_GLOBAL_START (5*RANGE)

void saveAlarmTemperature(int temperature, int port){
  if(temperature > 255){
    EEPROM.write(_EEPROM_START-8 + _EEPROM_AT_START + port, 255);
  } else {
    EEPROM.write(_EEPROM_START-8 + _EEPROM_AT_START + port, 0);
  }
  EEPROM.write(_EEPROM_START + _EEPROM_AT_START + port, temperature%255);
}

int readAlarmTemperature(int port){
  return EEPROM.read(_EEPROM_START-8 + _EEPROM_AT_START + port) + EEPROM.read(_EEPROM_START + _EEPROM_AT_START + port);
}

void _eepromReadConfig(){
  for(int port=0 ; port<SENSORS_NUMBER; port++){
    temp[2][port] = readAlarmTemperature(port);
    temp[3][port] = EEPROM.read(_EEPROM_START + _EEPROM_A_START + port);
    temp[4][port] = EEPROM.read(_EEPROM_START + _EEPROM_SA_START + port);
    temp[5][port] = EEPROM.read(_EEPROM_START + _EEPROM_RA_START + port);
    temp[13][port] = (int8_t)EEPROM.read(_EEPROM_START + _EEPROM_TEMP_CAL + port);
  }
  global_temp_unit_set  = EEPROM.read(_EEPROM_START + _EEPROM_GLOBAL_START + 0);
  global_sound_set      = EEPROM.read(_EEPROM_START + _EEPROM_GLOBAL_START + 1);
  global_alarm_set      = EEPROM.read(_EEPROM_START + _EEPROM_GLOBAL_START + 2);
  global_colors_set     = EEPROM.read(_EEPROM_START + _EEPROM_GLOBAL_START + 3);
  global_bl_set         = EEPROM.read(_EEPROM_START + _EEPROM_GLOBAL_START + 4);
  global_bl_val_set     = EEPROM.read(_EEPROM_START + _EEPROM_GLOBAL_START + 5);
  global_hist           = EEPROM.read(_EEPROM_START + _EEPROM_GLOBAL_START + 6);
}

void savePort(int port, int tempAlarm, int alarm, int soundAlarm, int relayAlarm){
  saveAlarmTemperature(tempAlarm, port);
  temp[2][port] = tempAlarm;
  
  EEPROM.write(_EEPROM_START + _EEPROM_A_START + port, alarm);
  temp[3][port] = alarm;
  
  EEPROM.write(_EEPROM_START + _EEPROM_SA_START + port, soundAlarm);
  temp[4][port] = soundAlarm;
  
  EEPROM.write(_EEPROM_START + _EEPROM_RA_START + port, relayAlarm);
  temp[5][port] = relayAlarm;
}

void saveTempCali(int port, int8_t calib){
  if(port<0){
    for(int i=0 ; i<SENSORS_NUMBER ; i++){
      EEPROM.write(_EEPROM_START + _EEPROM_TEMP_CAL + i, (int8_t)temp[13][i]);
    }
  } else {
      EEPROM.write(_EEPROM_START + _EEPROM_TEMP_CAL + port, calib);
      temp[13][port] = calib;
  }
}

void saveGlobal(int unit, int sound, int alarm, int color, int back, int bright, int hist){
  EEPROM.write(_EEPROM_START + _EEPROM_GLOBAL_START + 0, unit);
  global_temp_unit_set = unit;

  EEPROM.write(_EEPROM_START + _EEPROM_GLOBAL_START + 1, sound);
  global_sound_set = sound;

  EEPROM.write(_EEPROM_START + _EEPROM_GLOBAL_START + 2, alarm);
  global_alarm_set = alarm;

  EEPROM.write(_EEPROM_START + _EEPROM_GLOBAL_START + 3, color);
  global_colors_set = color;

  EEPROM.write(_EEPROM_START + _EEPROM_GLOBAL_START + 4, back);
  global_bl_set = back;
  
  EEPROM.write(_EEPROM_START + _EEPROM_GLOBAL_START + 5, bright);
  global_bl_val_set = bright;
  
  EEPROM.write(_EEPROM_START + _EEPROM_GLOBAL_START + 6, hist);
  global_hist = hist;
}

void _eepromReset(){
  for(int port=0;port<SENSORS_NUMBER;port++){
    savePort(port, TEMP_MAX-50, 1, 1, 1);
    saveTempCali(port, 50);
  }
  saveGlobal(0,0,0,1,0,3,0);
}
