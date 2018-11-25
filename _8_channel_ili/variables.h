/*
 * You can copy and reuse this code but do NOT remove this header informatin.
 * Author: Rafal Fratczak, 2017
 * Contact: fratczak.karolew@gmail.com
 */
 
#define STMPE_CS 8 // CS Touch SPI
#define SD_CS_PIN 7 //CS SD card
#define TFT_DC 9  //CD LCD SPI
#define TFT_CS 10 //CS LCD SPI
#define SOUND 2 //zamiana sound z cs tftlcd 10 z 2
#define RELAY 6
#define LCD_LIGHT 3
#define LONG_CLICK_DELAY 1500
#define SHORT_CLICK_DELAY 200
#define ARROW_DISPLAY_DELAY 8000
#define FACTORY_DELAY 1000
#define TEMP_MIN 0 //-10
#define TEMP_MAX 400 //400
#define AC_RESOLUTION 1024 //750
#define SENSORS_NUMBER 8  //number of sensors supported by device
#define ARROW_X_POSITION 40 //main temperature panel x position of arrow
#define TEMPERATURE_X_POSITION 90 //main temperature panel x position of temperature
#define CHAR_CLEANER 0xDA // special 0xFFFF character for clear function
#define DEGREE 0x21 //degree character, 0xDE - TODO implement 8bit font with full characters set
#define TXT_DZWIEK_ON "Dzwiek ON"
#define TXT_DZWIEK_OFF "Dzwiek OFF"
#define TXT_DZW_DOT_ON "Dzwiek Dotyk ON"
#define TXT_DZW_DOT_OFF "Dzwiek Dotyk OFF"
#define TXT_POMIAR_ON "Pomiar ON"
#define TXT_POMIAR_OFF "Pomiar OFF"
#define TXT_ALARM_ON "Alarm ON"
#define TXT_ALARM_OFF "Alarm OFF"
#define TXT_RELAY_ON "Wyjscie ON"
#define TXT_RELAY_OFF "Wyjscie OFF"
#define TXT_OK "OK"
#define TXT_ANULUJ "ANULUJ"

int averageTemp[SENSORS_NUMBER] = {0};
const char temperatureUnit[] = {'C','F','K'};   //temperature units
const byte HistIntervals[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 25, 30, 40, 50, 70, 90}; //histeresis values for alarm turn off level

int chart[8][100];

int y, 
    tmpRelay = 0,           // keep global var
    tmpSound = 0,           // keep global var
    tmpAlarm = 0,           // keep global var
    tmpTempAlarm = 0,       // keep global var
    tmpUnit = 0,            // keep global var
    tmpColor = 0,           // keep global var
    tmpBacklight = 0,       // temporary variable used to store global variable global_bl_set in settings menu - bl on off
    tmpBacklightVal = 0,    // temporary variable used to store global variable global_bl_val_set in settings menu - bl level
    tmpBacklightStatus = 0, // used to used to control the backlight function - if 0 then skip turn off if 1 then skip turn on
    tmpBacklightFirstT = 0, // used to control the first touch on tft. if 1 wait a while then change to 0. if 0 then normal work
    tmpHist = 0;            // keep global var

boolean pressedActive = false,    //var to handle touchscreen short and long press
        longPressActive = false,
        tmpArrowChange = true,    //var to handle arrow displaying
        menu2Access = true;
        
unsigned long pressedTimer = 0;
unsigned long arrowUpddateTimer = 0;
unsigned long backlightTimer = 0;
unsigned long menu2Timer = 0;

uint8_t analog_ports[] = {A0, A1, A2, A3, A4, A5, A6, A7}; //array of analog input to read temperature from sensors
const char termometrsNumeration[]={'A','B','C','D','E','F','G','H'}; //sensors description
int temp[14][SENSORS_NUMBER] = {{0, 0, 0, 0, 0, 0, 0, 0}, 
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
                      // [13][*] temperature results calibration
int global_alarm_set,     //0 or 1 - relay - on/off
    global_sound_set,     //0 or 1 - sourd - on/off
    global_temp_unit_set, //store 3 values, each represent unit
    global_colors_set,    //store 4 values 0 - 3, each represents specific tft color layout
    global_bl_set,        //have 2 functioms: 1 - is used as a flag to set the back light on (1) off (0). 2 - is used as backlight time
    global_bl_val_set,    //is used as backlight bright control value
    global_hist;          //used to keep the histeresis alarm value: 0 - 10

uint16_t BACKGROUND_COLOR;    // keeps backgroud color val
uint16_t TEXT_COLOR_W;        // keeps font color val
uint16_t TEXT_COLOR_B = 0x0000;
