/*
 * You can copy and reuse this code but do NOT remove this header informatin.
 * Author: Rafal Fratczak, 2017
 * Contact: fratczak.karolew@gmail.com
 */
 
//#include "PetitFS.h"
//#include "PetitSerial.h"
//PetitSerial PS;
//#define Serial PS
//
//FATFS fs;
//FRESULT res; 
//uint8_t buf[12];
//UINT br;
//char block[10]={"poczta"};
//char temps[3];
// unsigned int sdcursor = 0;
// static int temperature = 0;
//
//void errorHalt(char* msg) {
//  Serial.print("Error: ");
//  Serial.println(msg);
//  while(1);
//}
//
//void test() {
////1
//    sdcursor = sdcursor +335;
//   pf_lseek(sdcursor);
// //     pf_lseek(0);
//// pf_lseek(180);
//
// Serial.println("SD recording:");
//  Serial.println(sdcursor);
//String text="123456789458";
//for(int i=0 ; i<12 ;i++) buf[i]=text[i];
//    pf_write("\n", 1, &br);
//for(int i=0; i<sizeof(termometrsNumeration);i++){
//  sprintf (temps, "%03i", temp[0][i]);
//   Serial.println(temps);
//     pf_write(buf, 12, &br);
//      pf_write(" ", 1, &br);
//    pf_write(&termometrsNumeration[i], 1, &br);
//    pf_write(" ", 1, &br);
//   pf_write(temps, strlen(temps), &br);
//   pf_write(" ", 1, &br);
//    pf_write("C", 1, &br);
//      pf_write("\n", 1, &br);
//}
////3
//  pf_write(0, 0, &br);
//}


///-------------------------------------------------------

//#include "/home/rfratcza/sketchbook/libraries/PetitFS/src/PetitFS.h"
//#include "/home/rfratcza/sketchbook/libraries/PetitFS/src/PetitSerial.h"
#include "PetitFS.h"
#include "PetitSerial.h"
PetitSerial PS;
#define Serial PS

FATFS fs;     /* File system object */
FRESULT res; 
uint8_t readbuf[32];
uint8_t buf[32]={'1','6','1','1','2','3','2','1','0','2','3','4'};
UINT br;
char block[10]={"poczta"};
char temps[3];
static unsigned int sdcursor = 0;
 static int temperature = 0;

void errorHalt(char* msg) {
  Serial.print("Error: ");
  Serial.println(msg);
  //while(1);
}

void test() {
 // if (pf_open("TEST.TXT")) errorHalt("pf_open");
 
String text="123456789458";
for(int i=0 ; i<12 ;i++) buf[i]=text[i];
temperature++;
sprintf (temps, "%03i", temperature);
//sprintf (buf, "123456789458");

//1
    sdcursor = sdcursor +32;
   pf_lseek(sdcursor);
 //     pf_lseek(0);
// pf_lseek(180);
//  pf_lseek(0);
//2 
  Serial.println(sdcursor);
    res = pf_write("\n", 1, &br);
      Serial.println(res);
    res = pf_write("C", 1, &br);
      Serial.println(res);
    res =  pf_write("\n", 1, &br);
  Serial.println(res);
//3
  res = pf_write(0, 0, &br);
    Serial.println(res);
      Serial.println("---------------------");
}

void printsd(){
    // Dump test file to Serial.
 // pf_lseek(0);
 pf_lseek(0);
  while (1) {
 //   UINT nr;
    if (pf_read(readbuf, sizeof(readbuf), &br)) errorHalt("pf_read");
    if (br == 0) break;
    Serial.write(readbuf, br);
  }  
}

//----------------------------------------------------------
void SDInitialization() {
  Serial.begin(9600);
  if (pf_mount(&fs)) errorHalt("pf_mount init");
  if (pf_open("TEMP.TXT")) errorHalt("pf_open init");
}


