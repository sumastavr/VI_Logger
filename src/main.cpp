#include <Arduino.h>

#include <Wire.h>
#include <INA226.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

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

void displayResults(int index, float voltage, float power, float current){
  tft.setTextSize(2);
  switch(index){
    case 0: tft.setTextColor(ST77XX_YELLOW);
            tft.setCursor(0, 0); 
            tft.print(voltage);
            tft.print("V");
            tft.setCursor(80, 0);
            tft.print(current);
            tft.print("A");
            tft.setCursor(160, 0); 
            tft.print(power);
            tft.print("W");
            break;
    case 1: tft.setTextColor(ST77XX_GREEN);
            tft.setCursor(0, 20); 
            tft.print(voltage);
            tft.print("V");
            tft.setCursor(80, 20);
            tft.print(current);
            tft.print("A");
            tft.setCursor(160, 20); 
            tft.print(power);
            tft.print("W");
            break;
    case 2: tft.setTextColor(ST77XX_CYAN);
            tft.setCursor(0, 40); 
            tft.print(voltage);
            tft.print("V");
            tft.setCursor(80, 40);
            tft.print(current);
            tft.print("A");
            tft.setCursor(160, 40); 
            tft.print(power);
            tft.print("W");
            break;
  }

}

void setup() {

  Serial.begin(115200);

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

  displayResults(0,12.4,24.5,24.3);
  displayResults(1,23.4,4.5,8.3);
  displayResults(2,52.4,3.5,5.3);

  while(1){
    //tftPrintTest();
  }

  Serial.println("Initialize INA226");
  Serial.println("-----------------------------------------------");

  Wire.begin();

  bool success1, success2, success3;

  // Default INA226 address is 0x40
  success1 = ina1.begin(0x40);
  //success2 = ina1.begin(0x40);
  //success3 = ina1.begin(0x40);

  success2 = true;
  success3 = true;

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

  //while(1);
}

void loop()
{

  
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
  delay(1000);
  

 /*
 Serial.print(ina1.readBusVoltage(), 5);
 Serial.print(" ");
 Serial.println(ina1.readBusPower(), 5);

 delay(1000);
*/


}