// Digital pins to dictate what sensor the multiplexer should get info from
int dig[4]={2,3,4,5};
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
  pinMode(dig[3], OUTPUT);
  // Pin that receives data from mutliplexer C I/O pin
  pinMode(A0, INPUT);
  pinMode(D6, OUTPUT);
}

// method to return the voltage red as a DC signal using the threashold of 1.51V being 60C
double convert(double read){
  // calculates Vin
  double vin = (read* 5.0)/(1023.0);
  if (vin<=1.51){
    LED = true;
  }
  return vin;
}

void loop() {
  // sets LED to off at the start of every loop
  LED = false;
  // In order to store the highest tempature per illiteration through the sensors, 
   maxTemp = 5;
   intTemp = 0;
  // reads through all odd sensors first (since according to datasheet for the sensor, we should illiterate Cn, Cn+2, Cn+4, etc)
  for(int i = 1; i<16; i+=2){
    digitalWrite(dig[0], bitRead(i, 0));
    digitalWrite(dig[1], bitRead(i, 1));
    digitalWrite(dig[2], bitRead(i, 2));
    digitalWrite(dig[3], bitRead(i, 3));
    // Sends the read signal to be converted to see if tempature is above threashold, and if it is, the LED STAYS LIT till the next pass of that battery full loop() cycle
    intTemp = convert(analogRead(A0));
    if(intTemp < maxTemp){
      maxTemp = intTemp;
    }

  }
  // short delay, a lot longer than the max of 1000 ns, but just to keep data transmission safe and smooth
  delay(100);
  // reads through all the even sensors
    for(int j = 0; j<16; j+=2){
    digitalWrite(dig[0], bitRead(j, 0));
    digitalWrite(dig[1], bitRead(j, 1));
    digitalWrite(dig[2], bitRead(j, 2));
    digitalWrite(dig[3], bitRead(j, 3));
    // same thing as the earlier test.
    intTemp = convert(analogRead(A0));
    if(intTemp < maxTemp){
      maxTemp = intTemp;
    }
  }
  // For the LED to blink when a sensor has been detected over the max threashold
  for(int k = 0; k <10 && LED; k++){
      digitalWrite(D6, LED);
      delay(10);
      digitalWrite(D6, !LED);
  }
  
  // short delay, a lot longer than the max of 1000 ns, but just to keep data transmission safe and smooth
  delay(100);
}