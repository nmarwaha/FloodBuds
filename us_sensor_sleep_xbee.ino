//SOURCE:https://forum.arduino.cc/index.php?topic=173850.0
// Example of sleeping and saving power
// 
// Author: Nick Gammon
// Date:   25 May 2011

#include <avr/sleep.h>
#include <avr/wdt.h>

#define LED 13 // LED on when system on

#define trigPin 10
#define echoPin 13
// vcc to 3.3v, gnd to gnd

// We'll use SoftwareSerial to communicate with the XBee:
#include <SoftwareSerial.h>
// XBee's DOUT (TX) is connected to pin 2 (Arduino's Software RX)
// XBee's DIN (RX) is connected to pin 3 (Arduino's Software TX)
SoftwareSerial XBee(2, 3); // RX, TX


// watchdog interrupt
ISR(WDT_vect) 
  {
  wdt_disable();  // disable watchdog
  }

void myWatchdogEnable(const byte interval) 
  {  
  MCUSR = 0;                          // reset various flags
  WDTCSR |= 0b00011000;               // see docs, set WDCE, WDE
  WDTCSR =  0b01000000 | interval;    // set WDIE, and appropriate delay

  wdt_reset();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_mode();            // now goes to Sleep and waits for the interrupt
  } 

void setup()
{
pinMode (LED, OUTPUT);
Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  XBee.begin(9600);
}  // end of setup

void loop()
{
  digitalWrite (LED, HIGH);  // awake
  delay (2000);    // ie. do stuff here

  float duration, distance, height, averageHeight, cumulativeHeight, maxiter;
  maxiter=10;
  for(int i = 0; i < maxiter; i++)
  {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  
  duration = pulseIn(echoPin, HIGH);
  distance=((duration / 2) * 0.0344);
  //height = 59.5-distance;             //47.625 w/ stool
  height = distance;             //47.625 w/ stool
  
  if (distance >= 400) {
    //Serial.print("Distance = ");
    Serial.println("Out of range");
  }
  else {
    //Serial.print("Height = ");
    Serial.print(height);
    Serial.print('\n');
    //Serial.println(" cm");
    delay(50);
    
    cumulativeHeight += height;
    
  }
  delay(100);
  }
    averageHeight=cumulativeHeight/maxiter;
    Serial.print(averageHeight);
    Serial.print('\n');
    XBee.print(averageHeight);
    //XBee.print(" cm");
    //XBee.print('\n');
delay(2000);

  
  digitalWrite (LED, LOW);  // asleep

  // sleep for a total of 30 minutes
  int k;
  for (k = 0; k < 225; k++)
  {
  myWatchdogEnable (0b100001);  // 8 seconds
  }  }  // end ofloop



// sleep bit patterns:
//  1 second:  0b000110
//  2 seconds: 0b000111
//  4 seconds: 0b100000
//  8 seconds: 0b100001



