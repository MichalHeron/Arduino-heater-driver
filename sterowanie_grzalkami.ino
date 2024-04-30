/*!
 * @file outputData.ino
 * @brief A use example for DAC, execute it to output different values from different channels.
 * @copyright  Copyright (c) 2021 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version V1.0
 * @date 2022-03-07
 * @url https://github.com/DFRobot/DFRobot_GP8403
 */
#include <LiquidCrystal.h>
#include <DFRobot_GP8403.h>
DFRobot_GP8403 dac(&Wire, 0x58);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // select the pins used on the LCD panel
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

bool risePower = true;
double sensorValue = 0;
int sensorValueTenTimes;
double prevSensorValue = 0;
int prevSensorValueTenTimes;
int setVoltage = 0;
int AKUPin = A1;
const int ACPin = A2;  //set arduino signal current read pin  !!!!!!!do skorygowania@@@@@.   CURR
int gridNoCurrentValue = 0.3;
#define ACTectionRange 20;  //set Non-invasive AC Current Sensor tection range (5A,10A,20A)       CURR
#define VREF 5.0            // check with mutlimetr and calibrate                                 CURR

float readACCurrentValue()  //CURR
{
  float ACCurrtntValue = 0;
  float peakVoltage = 0;
  float voltageVirtualValue = 0;  //Vrms
  for (int i = 0; i < 5; i++) {
    peakVoltage += analogRead(ACPin);  //read peak voltage
    delay(1);
  }
  peakVoltage = peakVoltage / 5;
  voltageVirtualValue = peakVoltage * 0.707;  //change the peak voltage to the Virtual Value of voltage

  /*The circuit is amplified by 2 times, so it is divided by 2.*/
  voltageVirtualValue = (voltageVirtualValue / 1024 * VREF) / 2;

  ACCurrtntValue = voltageVirtualValue * ACTectionRange;

  return ACCurrtntValue;
}

double convertToAkuVoltage(double a) {
  double akuValueVolt;
  //   4 - 58.  1024 *4/5 8/10.  819.2
  // 3 - 44    1024 *3/5 6/10    614.4
  //8000 - 58
  //a - x
  return akuValueVolt = (58 * a) / 820;  //wartosc 826 wyznaczono iteracyjnie
}

double convertPercentToDACRange(int a) {
  return 100 * a;
}

void setup() {
  lcd.begin(16, 2);     // start the library
  lcd.setCursor(0, 0);  // set the LCD cursor   position
  lcd.clear();
  delay(200);

  while (dac.begin() != 0) {
    lcd.print("init error");
    lcd.setCursor(0, 1);
    lcd.print(dac.begin());
    delay(1000);
  }

  lcd.print("succed ");
  //Set DAC output range
  dac.setDACOutRange(dac.eOutputRange10V);
}

void loop() {
  float ACCurrentValue = readACCurrentValue();  //read AC Current Value
  lcd.setCursor(7, 0);
  lcd.print("set:");
  lcd.print(setVoltage);
  lcd.print("  ");
  lcd.print(ACCurrentValue);
  lcd.print("A  ");

  if ((sensorValue > 52)) {
    if (risePower) setVoltage += 1;  //rise by 1%
    else setVoltage -= 1;            //low by 1%
    if (prevSensorValue - 0.15 < sensorValue) risePower = true;
    else risePower = false;
    prevSensorValue = sensorValue;
  }

  if (sensorValue < 53.5) setVoltage -= 5;                  //jezeli spadnie ponizej napiecia floating (chyba) if drop below floating value
                                                            // if (sensorValue < 50) setVoltage = 0; //wyzerwoanie mocy po naglym spadku ponizej wartosci
  if (ACCurrentValue > gridNoCurrentValue) setVoltage = 0;  //if true it means that inverter uses grid power
  if (setVoltage > 100) setVoltage = 100;
  if (setVoltage < 0) setVoltage = 0;

  //Set the output value for DAC 10V channel 0, range 0-10000
  dac.setDACOutVoltage(convertPercentToDACRange(setVoltage), 0);
  delay(100);
  dac.store();

  sensorValue = convertToAkuVoltage(analogRead(AKUPin));
  lcd.setCursor(0, 1);
  lcd.print(sensorValue);
  lcd.print("V");
  lcd.print("      ");
  delay(200);  //Czekaj 1s w sumie
}


// example

// /*!
//  * @file readACCurrent.
//  * @n This example reads Analog AC Current Sensor.

//  * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (https://www.dfrobot.com)
//  * @licence     The MIT License (MIT)
//  * @get from https://www.dfrobot.com

//  Created 2016-3-10
//  By berinie Chen <bernie.chen@dfrobot.com>

//  Revised 2019-8-6
//  By Henry Zhao<henry.zhao@dfrobot.com>
// */

// const int ACPin = A2;         //set arduino signal read pin
// #define ACTectionRange 20;    //set Non-invasive AC Current Sensor tection range (5A,10A,20A)

// // VREF: Analog reference
// // For Arduino UNO, Leonardo and mega2560, etc. change VREF to 5
// // For Arduino Zero, Due, MKR Family, ESP32, etc. 3V3 controllers, change VREF to 3.3
// #define VREF 5.0

// float readACCurrentValue()
// {
//   float ACCurrtntValue = 0;
//   float peakVoltage = 0;
//   float voltageVirtualValue = 0;  //Vrms
//   for (int i = 0; i < 5; i++)
//   {
//     peakVoltage += analogRead(ACPin);   //read peak voltage
//     delay(1);
//   }
//   peakVoltage = peakVoltage / 5;
//   voltageVirtualValue = peakVoltage * 0.707;    //change the peak voltage to the Virtual Value of voltage

//   /*The circuit is amplified by 2 times, so it is divided by 2.*/
//   voltageVirtualValue = (voltageVirtualValue / 1024 * VREF ) / 2;

//   ACCurrtntValue = voltageVirtualValue * ACTectionRange;

//   return ACCurrtntValue;
// }

// void setup()
// {
//   Serial.begin(115200);
//   pinMode(13, OUTPUT);

// }

// void loop()
// {
//   float ACCurrentValue = readACCurrentValue(); //read AC Current Value
//   Serial.print(ACCurrentValue);
//   Serial.println(" A");
//   digitalWrite(13, HIGH);
//   delay(500);
//   digitalWrite(13, LOW);
//   delay(500);
// }





// Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
// 20K-ohm resistor is pulled to 5V. This configuration causes the input to read
// HIGH when the switch is open, and LOW when it is closed.
//configure pin 2 as an input and enable the internal pull-up resistor
//pinMode(dryContactNC, INPUT_PULLUP);