#include <Wire.h>
// Digital pins to dictate what sensor the multiplexer should get info from
int dig[3]={2,3,4};
// Pin that controls blinking LED
int D6 = 6;
// decides if the LED is lit or not
bool LED = false;
// Helps store the highest sensor data.
double maxTemp = 5;
double intTemp = 0;

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
  if (vSensor<=1.51){
    LED = true;
  }
  return vSensor;
}

double VtoT(double V){
  // Takes how each decrease in 5mV is equal to 1 degree increase of C
  return ((-1.0/0.005)*(V-1.51) + 60.0);
}

// Sends the highest tempature found via I2C,  thanks to well documented adruino code: https://docs.arduino.cc/learn/communication/wire/ and https://www.instructables.com/Arduino-and-I2C/
void onReq(){
  String reqMessage = "Max temp: " + String(VtoT(maxTemp), 2) + "C";
  Wire.print(reqMessage);
}

void loop() {
  // sets LED to off at the start of every loop
  LED = false;
  // In order to store the highest tempature per illiteration through the sensors, 
   maxTemp = 5;
  // reads through all sensors
  for(int i = 0; i<8; i++){
    digitalWrite(dig[0], bitRead(i, 0));
    digitalWrite(dig[1], bitRead(i, 1));
    digitalWrite(dig[2], bitRead(i, 2));
    // Allows the mux to safely change values without much noise
    delayMicroseconds(150);
    // Sends the read signal to be converted to see if tempature is above threashold, and if it is, the LED STAYS LIT till the next pass of that battery full loop() cycle
    // Also lower voltage = higher tempature, so thats why intTemp becomes maxTemp when its lower
    intTemp = convert(analogRead(A0));
    if(intTemp < maxTemp){
      maxTemp = intTemp;
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
