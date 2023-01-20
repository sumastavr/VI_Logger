#include <Arduino.h>

#include <Wire.h>
#include <INA226.h>

INA226 ina1(Wire);
INA226 ina2(Wire);
INA226 ina3(Wire);

void checkConfig()
{
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

void setup() 
{
  Serial.begin(115200);

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