#include <Wire.h>
// Digital pins to dictate what sensor the multiplexer should get info from
int dig[3]={2,3,4};
// Pin that controls blinking LED
int D6 = 6;
// decides if the LED is lit or not
bool LED = false;
// Helps store the highest sensor data.
double maxTemp;
double TTemp;
// Matches voltage to temp across sensors from datasheet
double volts[33] = {1.30, 1.31, 1.32, 1.33, 1.34, 1.35, 1.37, 1.38, 1.40, 1.43, 1.45, 1.48, 1.51, 1.55, 1.59, 1.63, 1.68, 1.74, 1.80, 1.86, 1.92, 1.99, 2.05, 2.11, 2.17, 2.23, 2.27, 2.32, 2.35, 2.38, 2.40, 2.42, 2.44};
double temps[33] = {120.0, 115.0, 110.0, 105.0, 100.0, 95.0, 90.0, 85.0, 80.0, 75.0, 70.0, 65.0, 60.0, 55.0, 50.0, 45.0, 40.0, 35.0, 30.0, 25.0, 20.0, 15.0, 10.0, 5.0, 0.0, -5.0, -10.0, -15.0, -20.0, -25.0, -30.0, -35.0, -40.0};

void setup() {
  pinMode(dig[0], OUTPUT);
  pinMode(dig[1], OUTPUT);
  pinMode(dig[2], OUTPUT);
  // Pin that receives data from mutliplexer C I/O pin
  pinMode(A0, INPUT);
  pinMode(D6, OUTPUT);
  // Initialized I2C
  Wire.begin(0x08);
  Wire.onRequest(onReq);
}

// method to return the voltage red as a DC signal using the threashold of 1.51V being 60C
double convert(double read){
  // calculates voltage at A0 pin
  double vA0 = (read* 5.0)/(1023.0);
  // subtracts the voltage drop due to the internal resistance of mux as the tempature changes (We get voltage across 3300 resistor from 5-vAo and find the current in the branch, 
  // then find the voltage drop across internal mux, and then subtract that form the vA0 reading)
  double vSensor =  vA0 - (470.0 * (5-vA0) / 3300.0);
  
  // Sets a pre-assumption that the tempature across the sensors is the lowest possible incase its somehow greater than 2.44V
  double foundTemp = -40;
  // loops through known values of tempature valeus until it lands between two different tempatures, then it assigns the higher tempature to the sensor, to be safe
  for(int i = 0; i < 33; i++){
    if(vSensor <= volts[i]){
      foundTemp = temps[i];
      break;
    }
  }

  if (foundTemp >= 60.0){
    LED = true;
  }
  return foundTemp;
}

// Sends the highest tempature found via I2C,  thanks to well documented adruino code: https://docs.arduino.cc/learn/communication/wire/ and https://www.instructables.com/Arduino-and-I2C/
void onReq(){
  String reqMessage = "Max temp: " + String(maxTemp, 2) + "C";
  Wire.print(reqMessage);
}

void loop() {
  // sets LED to off at the start of every loop
  LED = false;
  // In order to store the highest tempature per illiteration through the sensors, we set a baseline to make sure the value is then at least changed once
   maxTemp = -100;
  // reads through all sensors
  for(int i = 0; i<8; i++){
    digitalWrite(dig[0], bitRead(i, 0));
    digitalWrite(dig[1], bitRead(i, 1));
    digitalWrite(dig[2], bitRead(i, 2));
    // Allows the mux to safely change values without much noise
    delayMicroseconds(150);
    // Sends the read signal to be converted to see if tempature is above threashold, and if it is, the LED STAYS LIT till the next pass of that battery full loop() cycle
    // Also lower voltage = higher tempature, so thats why intTemp becomes maxTemp when its lower
    TTemp = convert(analogRead(A0));
    if(TTemp > maxTemp){
      maxTemp = TTemp;
    }
  }
  // For the LED to blink when a sensor has been detected over the max threashold, and allow us to visually see it
  for(int k = 0; k <10 && LED; k++){
      digitalWrite(D6, LED);
      delay(40);
      digitalWrite(D6, !LED);
      delay(40);
  }
}
