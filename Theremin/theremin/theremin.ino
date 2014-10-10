#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

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

float wavFrequency;
float wavAmplitude = 0.5;
long sensorVal;

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
    //Serial.print("\n");
    //Serial.print(distance);
  }
  
  // Translating sensor reading to frequency
float frequencyCalculator()
  {
    // Highest sensor reading is about 123, sending out notes from mid range frequency
    sensorVal = sensorReading();
    Serial.print("\n Sensor Reading: ");
    Serial.print(sensorVal);  
            if(sensorVal<=4)
           {
            return 246.94; // B
            
           }
           
           else if ((sensorVal>=4) &&(sensorVal<=10))
           { return 261.63; // C
             
           }
           else if ((sensorVal>=10) &&(sensorVal<=15))
           { return 293.66; //D
              
           }
           else if((sensorVal>=15) &&(sensorVal<=20))
           {
             return 329.63; //E
             
           }
           else if ((sensorVal>=20) &&(sensorVal<=25))
          { 
            return 349.23; //F
                       
          }
           
          else if ((sensorVal>=25) &&(sensorVal<=30))
          { 
            return 392; //G
            
          }  
          else if ((sensorVal>=30) &&(sensorVal<=35))
         {  return 440; //A
            
         } 
         
         else
         {
         return 146.83; // D from mid-low   
          }
 
     }
    
  
  // generating the sound
void soundGen()
  {
    wavFrequency = frequencyCalculator();
    sine1.frequency(wavFrequency);
    sine1.amplitude(wavAmplitude);
    envelope1.noteOn();
    delay(36);
    envelope1.noteOff();
    
  }
  

void loop()
{
 soundGen();
 Serial.print("  Frequency: ");
 Serial.print(wavFrequency);
 delay(1000);
 }
