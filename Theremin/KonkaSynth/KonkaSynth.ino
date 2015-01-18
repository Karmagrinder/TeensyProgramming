#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>

//Pins
#define trigPin 0 // For UltrasonicSensor HC-S04
#define echoPin 1 // For UltrasonicSensor HC-S04 
#define photoResistorPin 20 
#define led 13 
#define envelopePotPin 21
#define volumePin 15
#define waveformPotPin 17
#define FXPotPin 16


// GUItool: begin automatically generated code
AudioSynthWaveform       sine1;      //xy=65,77
AudioSynthWaveform       saw1;      //xy=65,134
AudioSynthWaveform       square1;      //xy=66,183
AudioSynthWaveform       triangle1;      //xy=69,231
AudioMixer4              mixer1;         //xy=224,128
AudioEffectEnvelope      envelope1;      //xy=269,293
AudioEffectDelay         delay1;
AudioOutputI2S           i2s1;           //xy=345,389
AudioControlSGTL5000 sgtl5000_1;
AudioConnection          patchCord1(sine1, 0, mixer1, 0);
AudioConnection          patchCord2(saw1, 0, mixer1, 1);
AudioConnection          patchCord3(square1, 0, mixer1, 2);
AudioConnection          patchCord4(triangle1, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, envelope1);
AudioConnection          patchCord6(envelope1, delay1);
AudioConnection          patchCord7(delay1, 0, i2s1, 0);
AudioConnection          patchCord8(delay1, 0, i2s1, 1); //Delay is mono to save processing
// GUItool: end automatically generated code

float noteFrequency;
float noteAmplitude;
double amplitudeVal; 
long sensorVal;
long sensor1Val;
float vol;
long distance, duration;
int envelopeSelect;
int currentWaveForm;
int key;
float FXVal;

//>>>>>>>>>>>>>>>>>>>>>>>SETUP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void setup() {
  //Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(volumePin, INPUT);
  pinMode(photoResistorPin, INPUT);
  pinMode(envelopePotPin, INPUT);
  pinMode(waveformPotPin, INPUT);
  pinMode(FXPotPin, INPUT);
  //Initializing Waveforms
  sine1.begin(0, noteFrequency, WAVEFORM_SINE);
  saw1.begin(0, noteFrequency, WAVEFORM_SAWTOOTH);
  square1.begin(0, noteFrequency, WAVEFORM_SQUARE);
  triangle1.begin(0, noteFrequency, WAVEFORM_TRIANGLE);
  //disabling delay on unused channels
  delay1.disable(0);
  delay1.disable(1); // delay is off on restart. 
  delay1.disable(2);
  delay1.disable(3);
  delay1.disable(4);
  delay1.disable(5);
  delay1.disable(6);
  delay1.disable(7);  
  AudioMemory(30);
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
    return distance;
  }
//>>>>>>>>>>>>>>>>>>>>> Key Mapping <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int keyMap()
    {
      sensorVal = sensorReading();
      if (sensorVal >50)  // ignore if distance is more than 50cm
        {
          //Serial.println("\n Distance Sensor: ignoring\n");
          key = 0;
        }
      else{
             key = 1.78*sensorVal;
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
       int potVal;
       potVal = analogRead(envelopePotPin);
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
		//envelope1.delay(20.0);  // default values...
		envelope1.attack(9.2);
		envelope1.hold(2.1);
		envelope1.decay(31.4);
		envelope1.sustain(0.6);
		envelope1.release(84.6);
                break;
            case 2:
                  //envelope1.state = 0;
  		envelope1.delay(30.0);  // default values...
		envelope1.attack(9.2);
		envelope1.hold(2.1);
		envelope1.decay(31.4);
		envelope1.sustain(0.6);
		envelope1.release(84.6);
                break;
             case 3:
            //envelope1.state = 0;
		envelope1.delay(50);  // default values...
		envelope1.attack(50);
		envelope1.hold(1);
		envelope1.decay(50);
		envelope1.sustain(1);
		envelope1.release(250);
                break;        
          }            
    }
// >>>>>>>>>>>>>>>>>>>>>> Amplitude maping >>>>>>>>>>>>>>>>>>>>>>>>>
float amplitudeMapping()
    {  
       double z;     
       z = analogRead(photoResistorPin);
       amplitudeVal = z/300;           
      return amplitudeVal;
    }
  
//>>>>>>>>>>>>>>>>>>>>>>> WaveForm handling >>>>>>>>>>>>>>>>>>>>>>>>>>
int waveFormHandling()
    {
      float level = 1.0;
      int potVal1=analogRead(waveformPotPin);
      int waveFormSelect = potVal1/256;
      
      switch (waveFormSelect){
         
           case 0:
                //Mixer 
                mixer1.gain(0,level);
                mixer1.gain(1,0);
                mixer1.gain(2,0);
                mixer1.gain(3,0);
                //Amplitude
                sine1.amplitude(noteAmplitude);
                saw1.amplitude(0);
                square1.amplitude(0);
                triangle1.amplitude(0);
                //Frequency
                sine1.frequency(noteFrequency);
                saw1.frequency(0);
                square1.frequency(0);
                triangle1.frequency(0);
                return(0);
                break;
           case 1:
                //Mixer
                mixer1.gain(0,0);
                mixer1.gain(1,level);
                mixer1.gain(2,0);
                mixer1.gain(3,0);
                //Amplitude
                sine1.amplitude(0);
                saw1.amplitude(noteAmplitude);
                square1.amplitude(0);
                triangle1.amplitude(0);
                //Frequency
                sine1.frequency(0);
                saw1.frequency(noteFrequency);
                square1.frequency(0);
                triangle1.frequency(0);
                
                return(1);
                break;
           case 2:
                //Mixer
                mixer1.gain(0,0);
                mixer1.gain(1,0);
                mixer1.gain(2,level);
                mixer1.gain(3,0);
                //Amplitude
                sine1.amplitude(0);
                saw1.amplitude(0);
                square1.amplitude(noteAmplitude);
                triangle1.amplitude(0);
                //Frequency
                sine1.frequency(0);
                saw1.frequency(0);
                square1.frequency(noteFrequency);
                triangle1.frequency(0);
                return(2);
                break;
           case 3:
                //Mixer
                mixer1.gain(0,0);
                mixer1.gain(1,0);
                mixer1.gain(2,0);
                mixer1.gain(3,level);
                //Amplitude
                sine1.amplitude(0);
                saw1.amplitude(0);
                square1.amplitude(0);
                triangle1.amplitude(noteAmplitude);
                //Frequency
                sine1.frequency(0);
                saw1.frequency(0);
                square1.frequency(0);
                triangle1.frequency(noteFrequency);
                return(3);
                break;        
          }
    }
//>>>>>>>>>>>>>>>>>>>>Delay<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void delayHandler()
    {
      int potVal2 = analogRead(FXPotPin);
         FXVal = potVal2*0.28;  // If the FX value goes over 300/ or ~290 weird things start to happen, so let's keep it low.
         delay(20);
         if(FXVal>1)
         {  
           
           if(FXVal>140)
             {
               FXVal = 140; // Restricting the delay to some sane value, to prevent crash.
             }
           delay1.delay(0, FXVal);
          
         }
    }
 
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> generating the sound<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void soundGen()
  {
    keyMap();
    noteFrequency = frequencyCalculator();
    noteAmplitude = amplitudeMapping();
    //noteAmplitude = 0.5;
    alterEnvelope();
    if (noteFrequency > 27.4)
    {     
          currentWaveForm = waveFormHandling();
          delayHandler();
          envelope1.noteOn();
          delay(20);
          envelope1.noteOff();
          digitalWrite(led,HIGH);
       }
       
     if(noteFrequency == 0)
      {
        envelope1.noteOff();
        digitalWrite(led,LOW);
      }
  }
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Volume Control <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void volumeControl()
    {   vol = analogRead(volumePin);
        vol = vol/1048;
        sgtl5000_1.volume(vol);
        
     }
// >>>>>>>>>>>>>>>>>>>>>>>>>>> Main Loop <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
unsigned long last_time = millis();

void loop()
{
 volumeControl(); 
 soundGen();
  if(millis() - last_time >= 500) {
      if(key==0)
    {
      //Serial.println("\nNot active \n");
      
    }
    
  else {  
   /*
    Serial.print("\n WaveForm: ");
    Serial.print(currentWaveForm);
    Serial.print("  Key: ");
    Serial.print(key); 
    Serial.print("  Envelope:");
    Serial.print(envelopeSelect);
    Serial.print("  amplitudeVal: ");
    Serial.print(amplitudeVal);
    Serial.print("  Frequency: ");
    Serial.print(noteFrequency);
    Serial.print("  Current Volume:");
    Serial.print(noteFrequency);
    Serial.print("  Delay:");
    Serial.print(FXVal);
    Serial.print("\n");
    */
  }
  last_time = millis();
 }
 


 }
