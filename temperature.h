/*
 * You can copy and reuse this code but do NOT remove this header informatin.
 * Author: Rafal Fratczak, 2017
 * Contact: fratczak.karolew@gmail.com
 */

void soundClic(int timer = 10){
  if(global_sound_set == 1){
    digitalWrite(SOUND, 1);
    delay(timer);
    digitalWrite(SOUND, 0);
  }
}

void brightSet(int val){
   analogWrite(LCD_LIGHT, 85 * val);
}

void backLight(){
   if((millis() - backlightTimer > 9000*global_bl_set) && global_bl_set > 0){
      if(tmpBacklightStatus == 1 ){
        tmpBacklightStatus = 0;
        tmpBacklightFirstT = 0;
        brightSet(0);
      }
    } else {
      if( tmpBacklightStatus == 0){
        tmpBacklightStatus = 1;
        brightSet(global_bl_val_set);
        return;
      }
    }
}

void relayAndSoundAlarmsControl(){
    if(temp[6][0]*!temp[3][0]+temp[6][1]*!temp[3][1]+temp[6][2]*!temp[3][2]+temp[6][3]*!temp[3][3]+temp[6][4]*!temp[3][4]+temp[6][5]*!temp[3][5]+temp[6][6]*!temp[3][6]+temp[6][7]*!temp[3][7] > 0
                                                                        && global_alarm_set == 1){  //sound per port
      digitalWrite(SOUND, 1);
    } else {
      digitalWrite(SOUND, 0);
    }
    if(temp[7][0]*!temp[3][0]+temp[7][1]*!temp[3][1]+temp[7][2]*!temp[3][2]+temp[7][3]*!temp[3][3]+temp[7][4]*!temp[3][4]+temp[7][5]*!temp[3][5]+temp[7][6]*!temp[3][6]+temp[7][7]*!temp[3][7] > 0 
                                                                         && global_alarm_set == 1){ //alarm per port - relay
      digitalWrite(RELAY, 1);
    } else {
      digitalWrite(RELAY, 0);
  }
}

int convertTempUnit(int tempCelsius){
  if(global_temp_unit_set == 2){
    return tempCelsius + 273;
  } else if (global_temp_unit_set == 1){
    return (int)(tempCelsius * 1.8) + 32;
  } else {
    return tempCelsius;
  }
}

void calculateTemperature(){
   for(int port =0 ; port < SENSORS_NUMBER ; port++){
    //Read actual temperature from analog ports
       // temp[0][port] = analogRead(analog_ports[port]);
        temp[9][port] = temp[8][port];
        //temp[8][port] = map(analogRead(analog_ports[port]) - 50 + temp[13][port], 0, AC_RESOLUTION, TEMP_MIN, TEMP_MAX); //+ temp[13][port]
        temp[8][port] = map(analogRead(analog_ports[port]), 0, AC_RESOLUTION, TEMP_MIN, TEMP_MAX - 50 + temp[13][port]); //+ temp[13][port]
        temp[1][port] = temp[0][port];
        temp[0][port] = (temp[8][port] + temp[9][port])/2; 

        Serial.println(analogRead(analog_ports[port]));
  }
   Serial.println("--------------------------------");   
    //Calculate the average history temperature, and determine the trend
    if(millis() - arrowUpddateTimer > ARROW_DISPLAY_DELAY){
      for(int port =0 ; port < SENSORS_NUMBER ; port++){   
        temp[11][port] = temp[10][port];
        temp[10][port] = temp[0][port];
      }
      arrowUpddateTimer = millis();
    }
}

