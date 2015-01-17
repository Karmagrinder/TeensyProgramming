#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>

//Pins
#define trigPin 0 // For UltrasonicSensor HC-S04
#define echoPin 1 // For UltrasonicSensor HC-S04 
#define LDR 20 
#define led 13 
#define pot1 21
#define pot2 17
#define pot3 16
#define pot4 15

long distance, duration;

void setup(){
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //pinMode(LDR, INPUT);
  //pinMode(pot1, INPUT);
  //pinMode(pot2, INPUT);
  //pinMode(pot3, INPUT);
  //pinMode(pot4, INPUT);
}

void sensorReading()
  {
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration/58.2;
  }

void loop(){
  
    sensorReading();
    Serial.print("\n Pot1: ");
    Serial.print(analogRead(pot1));
    Serial.print("  Pot2: ");
    Serial.print(analogRead(pot2)); 
    Serial.print("  Pot3:");
    Serial.print(analogRead(pot3));
    Serial.print("  Pot4: ");
    Serial.print(analogRead(pot4));
    Serial.print("  LDR: ");
    Serial.print(analogRead(LDR));
    Serial.print("  HC-SR04:");
    Serial.print(distance);
    Serial.print("\n");


}
        


