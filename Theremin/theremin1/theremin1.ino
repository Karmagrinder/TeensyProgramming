#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>

//Ultrasonic sensor HC-S04 stuff
#define trigPin 0
#define echoPin 1
#define led 13
long distance, duration;


//Audio stuff
AudioSynthWaveformSine sine1;
AudioEffectEnvelope envelope1;
AudioOutputI2S i2s1;
AudioControlSGTL5000 sgtl5000_1;
AudioConnection con1(sine1, envelope1);
AudioConnection con2(envelope1, 0, i2s1, 0);
AudioConnection con3(envelope1, 1, i2s1, 1);

float noteFrequency;
float noteAmplitude = 0.5;
long sensorVal;
int key;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
   
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
 
}

// Reading the ultra sonic sensor
long sensorReading()
  {
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration/58.2;
    if (distance < 4) {  // This is where the LED On/Off happens
      digitalWrite(led,HIGH);
      }
    else
    digitalWrite(led,LOW);
    return distance;

  }
  
int keyMap()
    {
      sensorVal = sensorReading();
      if (sensorVal >=100)
        {
          Serial.println("Distance more than 1 meter,ignoring \n");
          key = 0;
        }
      else{
             key = 0.89*sensorVal;
          Serial.print("\n Key: ");
          Serial.print(key);   
           }
      return key;
    }
  
  // Translating sensor reading to frequency
float frequencyCalculator()
  {
    
    if(key == 0)
    {
      noteFrequency = 0;
    }
    
    else 
    {
      // formula taken from : http://en.wikipedia.org/wiki/Piano_key_frequencies
      double x = key;
      double y= pow(2, ((x-49)/12)); // pow() takes only double as variables
      //noteFrequency = (2^((key-49)/12))*440;
      noteFrequency = y*440;
  
  }
    return noteFrequency;  
   
   }
    
  
  // generating the sound
void soundGen()
  {
    keyMap();
    noteFrequency = frequencyCalculator();
    sine1.frequency(noteFrequency);
    sine1.amplitude(noteAmplitude);
    envelope1.noteOn();
    delay(36);
    envelope1.noteOff();
    
  }
  

void loop()
{
 soundGen();
 Serial.print("  Frequency: ");
 Serial.print(noteFrequency);
 //delay(1000);
 }
