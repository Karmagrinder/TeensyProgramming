#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

const int switchPin = 0;
const int pot1Pin = 21;
const int pot2Pin = 20;
boolean currentState = false;
float pot1Val;
float pot2Val;

void setup()
{
  pinMode(switchPin, INPUT);
  pinMode(pot1Pin, INPUT);
  pinMode(pot2Pin, INPUT);
  Serial.begin(9600);
}  

void loop()
{
  if (digitalRead(switchPin)==HIGH)
      {
        Serial.print("\n Button Pressed \n");
      }
  
  pot1Val = analogRead(pot1Pin);
  pot2Val = analogRead(pot2Pin);
  
  Serial.print("\n Pot1:");
  Serial.print(pot1Val);
  Serial.print("  ");
  Serial.print("\n Pot2:");
  Serial.print(pot2Val);
  
  delay(1000);
  
}
