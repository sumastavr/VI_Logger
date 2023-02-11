#include <Arduino.h>

#include <Wire.h>
#include <INA226.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "SPI.h"

#include "RTClib.h"

#include "FS.h"
#include "SD.h"

RTC_PCF8563 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define ADC_EN              14 
#define ADC_PIN             34
#define BUTTON_1            35
#define BUTTON_2            0

INA226 ina1(Wire);
INA226 ina2(Wire);
INA226 ina3(Wire);

#define TFT_CS        5
#define TFT_RST       23 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        16
#define TFT_MOSI 19  // Data out
#define TFT_SCLK 18  // Clock out

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

float p = 3.1415926;

//Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int ADC_DC = 27;

SPIClass spiSD = SPIClass(HSPI);
SPIClass spiTFT = SPIClass(VSPI);

void checkConfig(){
  Serial.print("Mode:                  ");
  switch (ina1.getMode())
  {
    case INA226_MODE_POWER_DOWN:      Serial.println("Power-Down"); break;
    case INA226_MODE_SHUNT_TRIG:      Serial.println("Shunt Voltage, Triggered"); break;
    case INA226_MODE_BUS_TRIG:        Serial.println("Bus Voltage, Triggered"); break;
    case INA226_MODE_SHUNT_BUS_TRIG:  Serial.println("Shunt and Bus, Triggered"); break;
    case INA226_MODE_ADC_OFF:         Serial.println("ADC Off"); break;
    case INA226_MODE_SHUNT_CONT:      Serial.println("Shunt Voltage, Continuous"); break;
    case INA226_MODE_BUS_CONT:        Serial.println("Bus Voltage, Continuous"); break;
    case INA226_MODE_SHUNT_BUS_CONT:  Serial.println("Shunt and Bus, Continuous"); break;
    default: Serial.println("unknown");
  }
  
  Serial.print("Samples average:       ");
  switch (ina1.getAverages())
  {
    case INA226_AVERAGES_1:           Serial.println("1 sample"); break;
    case INA226_AVERAGES_4:           Serial.println("4 samples"); break;
    case INA226_AVERAGES_16:          Serial.println("16 samples"); break;
    case INA226_AVERAGES_64:          Serial.println("64 samples"); break;
    case INA226_AVERAGES_128:         Serial.println("128 samples"); break;
    case INA226_AVERAGES_256:         Serial.println("256 samples"); break;
    case INA226_AVERAGES_512:         Serial.println("512 samples"); break;
    case INA226_AVERAGES_1024:        Serial.println("1024 samples"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Bus conversion time:   ");
  switch (ina1.getBusConversionTime())
  {
    case INA226_BUS_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_BUS_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_BUS_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_BUS_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_BUS_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_BUS_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_BUS_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_BUS_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }

  Serial.print("Shunt conversion time: ");
  switch (ina1.getShuntConversionTime())
  {
    case INA226_SHUNT_CONV_TIME_140US:  Serial.println("140uS"); break;
    case INA226_SHUNT_CONV_TIME_204US:  Serial.println("204uS"); break;
    case INA226_SHUNT_CONV_TIME_332US:  Serial.println("332uS"); break;
    case INA226_SHUNT_CONV_TIME_588US:  Serial.println("558uS"); break;
    case INA226_SHUNT_CONV_TIME_1100US: Serial.println("1.100ms"); break;
    case INA226_SHUNT_CONV_TIME_2116US: Serial.println("2.116ms"); break;
    case INA226_SHUNT_CONV_TIME_4156US: Serial.println("4.156ms"); break;
    case INA226_SHUNT_CONV_TIME_8244US: Serial.println("8.244ms"); break;
    default: Serial.println("unknown");
  }
  
  Serial.print("Max possible current:  ");
  Serial.print(ina1.getMaxPossibleCurrent());
  Serial.println(" A");

  Serial.print("Max current:           ");
  Serial.print(ina1.getMaxCurrent());
  Serial.println(" A");

  Serial.print("Max shunt voltage:     ");
  Serial.print(ina1.getMaxShuntVoltage());
  Serial.println(" V");

  Serial.print("Max power:             ");
  Serial.print(ina1.getMaxPower());
  Serial.println(" W");
}

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
  delay(1500);
}

void displayDateTime(int year, int month, int date, int hour, int minute, int second){
  tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setCursor(0, 0); 
  tft.print(hour);
  tft.print(":");
  tft.print(minute);
  tft.print(":");
  tft.print(second);
  tft.print("  ");

  tft.print(date);
  tft.print("-");
  tft.print(month);
  tft.print("-");
  tft.print(year);
  tft.print("     ");
  
}

void displayResults(int index, float voltage, float power, float current){
  tft.setTextSize(1);
  uint8_t precision=2;
  uint8_t startHeight=35;
  uint8_t deltaHeight=30;
  switch(index){
    case 0: tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);

            tft.setCursor(0, startHeight-10);
            tft.print("Sensor 1:");
            tft.setTextSize(2);
            tft.setCursor(0, startHeight); 
            tft.print("                    ");
            tft.setCursor(0, startHeight); 
            tft.print(voltage,precision);
            tft.print("V");
            tft.setCursor(80, startHeight);
            tft.print(current,precision);
            tft.print("A");
            tft.setCursor(160, startHeight); 
            tft.print(power,precision);
            tft.print("W");
            break;
    case 1: tft.setTextColor(ST77XX_GREEN,ST77XX_BLACK);
            tft.setCursor(0, startHeight+deltaHeight-10);
            tft.print("Sensor 2:");
            tft.setTextSize(2);
            tft.setCursor(0, startHeight+deltaHeight);
            tft.print("                     ");
            tft.setCursor(0, startHeight+deltaHeight);  
            tft.print(voltage,precision);
            tft.print("V");
            tft.setCursor(80, startHeight+deltaHeight);
            tft.print(current,precision);
            tft.print("A");
            tft.setCursor(160, startHeight+deltaHeight); 
            tft.print(power,precision);
            tft.print("W");
            break;
    case 2: tft.setTextColor(ST77XX_CYAN,ST77XX_BLACK);
            tft.setCursor(0, startHeight+deltaHeight+deltaHeight-10);
            tft.print("Sensor 3:");
            tft.setTextSize(2);
            tft.setCursor(0, startHeight+deltaHeight+deltaHeight); 
            tft.print("                     ");
            tft.setCursor(0, startHeight+deltaHeight+deltaHeight); 
            tft.print(voltage,precision);
            tft.print("V");
            tft.setCursor(80, startHeight+deltaHeight+deltaHeight);
            tft.print(current,precision);
            tft.print("A");
            tft.setCursor(160, startHeight+deltaHeight+deltaHeight); 
            tft.print(power,precision);
            tft.print("W");
            break;
  }

}

void dumpSD(){
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

char* sdFileName = "/v.ea";

#define DEBUG 1

bool appendFile(fs::FS &fs, const char * path, const char * message) {

  bool appendSuccess = false;
  if (DEBUG == 1) {
    Serial.print("Appending to file: ");
    Serial.println(path);
  }
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    appendSuccess = false;
    if (DEBUG == 1) {
      Serial.println("Failed opening the file.");
    }
  }
  if (file.print(message)) {
    appendSuccess = true;
    if (DEBUG == 1) {
      Serial.println("Appended successfully.");
    }
  } else {
    appendSuccess = false;
    if (DEBUG == 1) {
      Serial.println("Appending failed.");
    }
  }
  file.close();

  if (appendSuccess) {
    return true;
  } else {
    return false;
  }
}

void setup() {

  Serial.begin(115200);

  //#define SCK  25
  //#define MISO  27
  //#define MOSI  26
  #define CS_SD  33

  spiSD.begin(25, 27, 26);

  if (!SD.begin(CS_SD,spiSD)) {
    Serial.println("Card Mount Failed");
    //return;
    while(1);
  }else{
    dumpSD();
  }


  pinMode(4, OUTPUT);
  digitalWrite(4,1);

  pinMode(ADC_EN, OUTPUT);
  digitalWrite(ADC_EN, HIGH);

  tft.init(135, 240);           // Init ST7789 240x135
  tft.setRotation(3);
  Serial.println(F("Initialized"));

  tft.fillScreen(ST77XX_BLACK);
  //testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  delay(1000);

  tft.drawFastHLine(0,27,240,ST77XX_YELLOW);
  tft.drawFastHLine(0,27+30,240,ST77XX_GREEN);
  tft.drawFastHLine(0,27+60,240,ST77XX_CYAN);

  //tft.drawFastHLine(0,113,240,ST77XX_MAGENTA);

  Serial.println("Initialize INA226");
  Serial.println("-----------------------------------------------");

  Wire.begin();

  bool success1, success2, success3;

  // Default INA226 address is 0x40
  success1 = ina1.begin(0b1000000);
  success2 = ina2.begin(0b1000001);
  success3 = ina3.begin(0b1000010);

  //success2 = true;
  //success3 = true;

  // Check if the connection was successful, stop if not
  if(!success1){
    Serial.println("Connection error at Sensor INA 1");
    while(1);
  }
  if(!success2){
    Serial.println("Connection error at Sensor INA 3");
    while(1);
  }
  if(!success3){
    Serial.println("Connection error at Sensor INA 3");
    while(1);
  }

  // Configure INA226
  ina1.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);

  // Calibrate INA226. Rshunt = 0.002 ohm, Max excepted current = 4A
  ina1.calibrate(0.002, 2);

  // Display configuration
  checkConfig();

  Serial.println("-----------------------------------------------");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  rtc.start();


}

void loop()
{

  if (appendFile(SD, sdFileName, "Heyy")) {
  //success
  } else { 
  //failed
  }
  

  //spiTFT.begin();
  //tft.init(135, 240);           // Init ST7789 240x135
  
  #ifdef DEBUG

    DateTime now = rtc.now();

    displayDateTime(now.year(),now.month(),now.day(),now.hour(),now.minute(),now.second());

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.print("Sensor 1:");
    Serial.print('\t');
    Serial.print(ina1.readBusVoltage(), 5);
    Serial.print(" V");
    Serial.print('\t');
    Serial.print(ina1.readShuntCurrent(), 5);
    Serial.print(" mA");
    Serial.print('\t');
    Serial.print(ina1.readBusPower(), 5);
    Serial.println(" W");
    displayResults(0,ina1.readBusVoltage(),ina1.readShuntCurrent(),ina1.readBusPower());

    Serial.print("Sensor 2:");
    Serial.print('\t');
    Serial.print(ina2.readBusVoltage(), 5);
    Serial.print(" V");
    Serial.print('\t');
    Serial.print(ina2.readShuntCurrent()/3, 5);
    Serial.print(" mA");
    Serial.print('\t');
    Serial.print(ina2.readBusPower(), 5);
    Serial.println(" W");    
    displayResults(1,ina2.readBusVoltage(),ina2.readShuntCurrent(),ina2.readBusPower());

    Serial.print("Sensor 3:");
    Serial.print('\t');
    Serial.print(ina3.readBusVoltage(), 5);
    Serial.print(" V");
    Serial.print('\t');
    Serial.print(ina3.readShuntCurrent()/3, 5);
    Serial.print(" mA");
    Serial.print('\t');
    Serial.print(ina3.readBusPower(), 5);
    Serial.println(" W");    
    displayResults(2,ina3.readBusVoltage(),ina3.readShuntCurrent(),ina3.readBusPower());

    Serial.println();
  #else

  Serial.print("Bus voltage:   ");
  Serial.print(ina1.readBusVoltage(), 5);
  Serial.println(" V");

  Serial.print("Bus power:     ");
  Serial.print(ina1.readBusPower(), 5);
  Serial.println(" W");

  Serial.print("Shunt voltage: ");
  Serial.print(ina1.readShuntVoltage(), 5);
  Serial.println(" V");

  Serial.print("Shunt current: ");
  Serial.print(ina1.readShuntCurrent(), 5);
  Serial.println(" A");

  Serial.println("");
  #endif

  delay(1000);
  

 /*
 Serial.print(ina1.readBusVoltage(), 5);
 Serial.print(" ");
 Serial.println(ina1.readBusPower(), 5);

 delay(1000);
*/


}