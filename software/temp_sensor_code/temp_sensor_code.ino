// Digital pins to dictate what sensor the multiplexer should get info from
int dig[4]={2,3,4,5};
// Pin that controls blinking LED
int D6 = 6;
// decides if the LED is lit or not
bool LED = false;
void setup() {
  pinMode(dig[0], OUTPUT);
  pinMode(dig[1], OUTPUT);
  pinMode(dig[2], OUTPUT);
  pinMode(dig[3], OUTPUT);
  // Pin that receives data from mutliplexer C I/O pin
  pinMode(A0, INPUT);
  pinMode(D6, OUTPUT);
}

void convert(double read){
  // calculates Vin
  double vin = (read* 5.0)/(1023.0);
  if (vin<=1.51){
    LED = true;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
