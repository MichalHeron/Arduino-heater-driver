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


#define dryContactNC 1  //from pin x to ground
bool risePower = true;
double sensorValue = 0;
int sensorValueTenTimes;
double prevSensorValue = 0;
int prevSensorValueTenTimes;
int setVoltage = 0;

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

  // Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
  // 20K-ohm resistor is pulled to 5V. This configuration causes the input to read
  // HIGH when the switch is open, and LOW when it is closed.
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(dryContactNC, INPUT_PULLUP);
}

void loop() {
  lcd.setCursor(7, 0);
  lcd.print("set:");
  lcd.print(setVoltage);
  lcd.print(" ");
  lcd.print(digitalRead(dryContactNC));
  lcd.print("  ");

  //  if (sensorVal == HIGH) {
  // if NC C is true then do
  if ((sensorValue > 52)) {
    // if (sensorValue > 52) {
    if (risePower) setVoltage += 1;  //Podnieś  o 1%
    else setVoltage -= 1;            //obniż o 1%
    if (prevSensorValue - 0.15 < sensorValue) risePower = true;
    else risePower = false;
    prevSensorValue = sensorValue;
  }


  // if (digitalRead(dryContactNC)) { test X
  //   // if (sensorValue > 52) {
  //   if (risePower) setVoltage += 1;  //Podnieś  o 1%
  //   else setVoltage -= 1;            //obniż o 1%
  //   if (prevSensorValueTenTimes - 1 < sensorValueTenTimes) risePower = true;
  //   else risePower = false;
  //   prevSensorValue = sensorValue;
  // }

  //netstat –nr | grep defaultif (!digitalRead(dryContactNC)) setVoltage = 0;

  if (sensorValue < 53.5) setVoltage -= 5;  //jezeli spadnie ponizej napiecia floating (chyba)
                                            // if (sensorValue < 50) setVoltage = 0; //wyzerwoanie mocy po naglym spadku ponizej wartosci

  if (setVoltage > 100) setVoltage = 100;
  if (setVoltage < 0) setVoltage = 0;

  //Set the output value for DAC 10V channel 0, range 0-10000
  dac.setDACOutVoltage(convertPercentToDACRange(setVoltage), 0);
  delay(100);
  dac.store();

  sensorValue = convertToAkuVoltage(analogRead(A1));
  lcd.setCursor(0, 1);
  lcd.print(sensorValue);
  lcd.print("V");
  lcd.print("      ");
  delay(200);  //Czekaj 1s w sumie

  prevSensorValueTenTimes = prevSensorValue * 10; //test X
  sensorValueTenTimes = sensorValue * 10;

}