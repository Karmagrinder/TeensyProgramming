#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>

//Ultrasonic sensor HC-S04 stuff
#define trigPin 0
#define echoPin 1
#define trigPin1 2
#define echoPin1 3
#define led 13
#define potPin 21
long distance, duration, distance1, duration1;


//Audio stuff
AudioSynthWaveformSine sine1;
AudioEffectEnvelope envelope1;
AudioOutputI2S i2s1;
AudioControlSGTL5000 sgtl5000_1;
AudioConnection con1(sine1, envelope1);
AudioConnection con2(envelope1, 0, i2s1, 0);
AudioConnection con3(envelope1, 1, i2s1, 1);

float noteFrequency;
float noteAmplitude;
float amplitudeVal; 
long sensorVal;
long sensor1Val;
int key;
/* Envelope default values
		state = 0;
		delay(0.0);  // default values...
		attack(1.5);
		hold(0.5);
		decay(15.0);
		sustain(0.667);
		release(30.0);

*/

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
   
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
 
}

//>>>>>>>>>>>>>> Reading the ultra sonic sensor<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
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
  
 long sensor1Reading()
  {
    digitalWrite(trigPin1, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    distance1 = duration/58.2;
    return distance1;
  } 
//>>>>>>>>>>>>>>>>>>>>> Key Mapping <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int keyMap()
    {
      sensorVal = sensorReading();
      if (sensorVal >50)  // ignore if distance is more than 50cm
        {
          Serial.println("\nSensor: ignoring\n");
          key = 0;
        }
      else{
             key = 1.78*sensorVal;
          Serial.print("\n Key: ");
          Serial.print(key);   
           }
      return key;
    }
  
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Translating sensor reading to frequency>>>>>>>>>>>>>>>>>>>>>>
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
// >>>>>>>>>>>>>>>>>>>>>> Envelope altering >>>>>>>>>>>>>>>>>>>>>>>>>
void alterEnvelope()
    { 
      int envelopeSelect;
      int potVal;
       potVal = analogRead(potPin);
       envelopeSelect = potVal/256;
      
      switch (envelopeSelect){
         
           case 0:
                  //envelope1.state = 0;
  		envelope1.delay(0.0);  // default values...
  		envelope1.attack(1.5);
  		envelope1.hold(0.5);
  		envelope1.decay(15.0);
  		envelope1.sustain(0.667);
  		envelope1.release(30.0);
                break;
             case 1:
            //envelope1.state = 0;
		envelope1.delay(20.0);  // default values...
		envelope1.attack(5.0);
		envelope1.hold(1);
		envelope1.decay(20.0);
		envelope1.sustain(0.800);
		envelope1.release(30.0);
                break;
            case 2:
                  //envelope1.state = 0;
  		envelope1.delay(30.0);  // default values...
  		envelope1.attack(10);
  		envelope1.hold(0.5);
  		envelope1.decay(30.0);
  		envelope1.sustain(1);
  		envelope1.release(50.0);
                break;
             case 3:
            //envelope1.state = 0;
		envelope1.delay(50);  // default values...
		envelope1.attack(15);
		envelope1.hold(1);
		envelope1.decay(40.0);
		envelope1.sustain(1);
		envelope1.release(40.0);
                break;        
          
      }
       Serial.print("  Envelope:");
       Serial.print(envelopeSelect);      
      
    }

  
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> generating the sound<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void soundGen()
  {
    keyMap();
    noteFrequency = frequencyCalculator();
    //noteAmplitude = amplitudeMapping();
    noteAmplitude = 0.5;
    alterEnvelope();
    if (noteFrequency > 27.4)
    {  
          sine1.frequency(noteFrequency);
          sine1.amplitude(noteAmplitude);
          envelope1.noteOn();
          //delay(36);
          delay(20);
          envelope1.noteOff();
       }
       
     if(noteFrequency == 0)
      {
        Serial.print("\nDoing nothing \n"); 
        envelope1.noteOff();
      }
  }
  
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Volume Control <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void volumeControl()
    {   float vol = analogRead(15);
        vol = vol/1048;
        sgtl5000_1.volume(vol);
        //delay(20);
        Serial.print("  Current Volume:");
        Serial.print(vol);
     }


// >>>>>>>>>>>>>>>>>>>>>>>>>>> Main Loop <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void loop()
{
 volumeControl(); 
 soundGen();
 Serial.print("  Frequency: ");
 Serial.print(noteFrequency);
 //delay(1000);
 }
