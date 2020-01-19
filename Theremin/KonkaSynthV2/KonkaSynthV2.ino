#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>
#include <stdio.h>

//Pins
#define trigPin 0 // For UltrasonicSensor HC-S04
#define echoPin 1 // For UltrasonicSensor HC-S04
#define switch1 2
#define switch2 3
#define switch3 4
#define switch4 5 
#define photoResistorPin 20 
#define led 13 

#define envelopePotPin 21
#define volumePin 15
#define waveformPotPin 17
#define FXPotPin 16

#define pot1 15
#define pot2 16
#define pot3 17
#define pot4 21



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
bool isPrintEnabled;
int deviceMode;
bool isPot1Changed;
bool isPot2Changed;
bool isPot3Changed;
bool isPot4Changed;
int pot1LastVal;
int pot2LastVal;
int pot3LastVal;
int pot4LastVal;
float attackVal;
float decayVal;
float sustainVal;
float releaseVal;
float sineLevel;
float sawLevel;
float squareLevel;
float triangleLevel;

enum waveforms
  {
    sine = 0,
    saw = 1,
    square = 2,
    triangle = 3    
  };

//>>>>>>>>>>>>>>>>>>>>>>>SETUP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void setup() 
  {
    //Serial.begin (9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(led, OUTPUT);

    pinMode(switch1, INPUT);
    pinMode(switch2, INPUT);
    pinMode(switch3, INPUT);
    pinMode(switch4, INPUT);

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

    isPrintEnabled = false;
    deviceMode = 1;

    //Init Envelope settings
    setEnvelopeValues(0, 0.5, 1.5, 15, 0.667, 30);
  }
//>>>>>>>>>>>>>> Detect Mode <<<<<<<<<<<<<<<<<<
void setDeviceMode()
  {
      int switch1Val = 0;
      int switch2Val = 0;
      int switch3Val = 0;
      int switch4Val = 0;

      switch1Val = digitalRead(switch1);
      switch2Val = digitalRead(switch2);
      switch3Val = digitalRead(switch3);
      switch4Val = digitalRead(switch4);

      if (switch1Val == HIGH)
      {
        deviceMode = 1;
      }
      else if (switch2Val == HIGH)
      {
        deviceMode = 2;
      }
      else if (switch3Val == HIGH)
      {
        deviceMode = 3;
      }
      else if (switch4Val == HIGH)
      {
        deviceMode  = 4;
      }     
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
      double y= pow(2, ((float)(x-49)/12)); // pow() takes only double as variables
      //noteFrequency = (2^((key-49)/12))*440;
      noteFrequency = y*440;
    }
    return noteFrequency;     
  }
// >>>>>>>>>>>>>>>>>>>>>> Envelope altering >>>>>>>>>>>>>>>>>>>>>>>>>

void setEnvelopeValues(float delay, float hold, float attack,  float decay, float sustain, float release)
  {
      envelope1.delay(delay);
      envelope1.attack(attack);
      envelope1.hold(hold);
      envelope1.decay(decay);
      envelope1.sustain(sustain);
      envelope1.release(release);
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
void setMixerLevelsToNull()
  {
      mixer1.gain(0, 0);
      mixer1.gain(1, 0);
      mixer1.gain(2, 0);
      mixer1.gain(3, 0);
    }

 void setAmplitudesToNull()
 {
    // reset amplitudes
      sine1.amplitude(0);
      saw1.amplitude(0);
      square1.amplitude(0);
      triangle1.amplitude(0);
  }

  void setFrequenciesToNull()
  {
      // reset frequency
      sine1.frequency(0);
      saw1.frequency(0);
      square1.frequency(0);
      triangle1.frequency(0);
    }
    
  void setNoteAndFrequencies()
  {
      sine1.amplitude(noteAmplitude);
      sine1.frequency(noteFrequency);
      saw1.amplitude(noteAmplitude);
      saw1.frequency(noteFrequency);
      square1.amplitude(noteAmplitude);
      square1.frequency(noteFrequency);
      triangle1.amplitude(noteAmplitude);
      triangle1.frequency(noteFrequency); 
    }

void setWaveFormAmplitube(int selectedWaveForm)
  {
    setMixerLevelsToNull();
    setAmplitudesToNull();
    setFrequenciesToNull();
    setNoteAndFrequencies();
    float level = 1.0;
    mixer1.gain(selectedWaveForm, level);
  }

void waveFormHandling()
  {
    int potVal1=analogRead(waveformPotPin);
    int waveFormSelect = potVal1/256;
    setWaveFormAmplitube(waveFormSelect);
    currentWaveForm = waveFormSelect;
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
    if (noteFrequency > 27.4)
    {     
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
// DEVICE MODE 
void handleDeviceMode()
  {
    switch (deviceMode)
    {
      case 1:
        volumeControl();
        waveFormHandling();
        setEnvelopeValues(0, 0.5, 1.5, 15, 0.667, 30);
        delayHandler();
        break;
      
      case 2:
        mapPotsToEnvelope();
        break;
      
      case 3:
        mapPotsToWaveformMixer();
        break;
      
      default:
        break;
    }
  }

// CHECK POT STATE CHANGE
void isPotStateChanged()
  {
    if (pot1LastVal != analogRead(pot1))
    {
      isPot1Changed = true;
    }
    else
    {
      isPot1Changed = false;
    }    
     
    if (pot2LastVal != analogRead(pot2))
    {
      isPot2Changed = true;
    }
    else
    {
      isPot2Changed = false;
    }

    if (pot3LastVal != analogRead(pot3))
    {
      isPot1Changed = true;
    }
    else
    {
      isPot3Changed = false;
    }

    if (pot4LastVal != analogRead(pot4))
    {
      isPot4Changed = true;
    }
    else
    {
      isPot1Changed = false;
    }
  }

  // SAVE POT STATEs
void savePotStates()
  {
    pot1LastVal = analogRead(pot1);
    pot2LastVal = analogRead(pot2);
    pot3LastVal = analogRead(pot3);
    pot4LastVal = analogRead(pot4);
  }



// Envelope Mode
void mapPotsToEnvelope()
  {
    
    if(isPot1Changed)
    {
      attackVal = (float)analogRead(pot1)/100;
    }

    if(isPot2Changed)
    {
      decayVal = (float)analogRead(pot1)/100;
    }

    if(isPot3Changed)
    {
      sustainVal = (float)analogRead(pot1)/100;
    }

    if(isPot4Changed)
    {
      releaseVal = (float)analogRead(pot1)/100;
    }
    
    float defaultDelayVal = 1;
    float defaultHoldVal = 1;

    setEnvelopeValues(defaultDelayVal, defaultHoldVal, attackVal, decayVal, sustainVal, releaseVal);    
  }

  // WaveForm Mode
void mapPotsToWaveformMixer()
  {
    if(isPot1Changed)
    {
      sineLevel = (float)analogRead(pot1)/1024;
    }

    if(isPot2Changed)
    {
      sawLevel = (float)analogRead(pot2)/1024;
    }

    if(isPot3Changed)
    {
      squareLevel = (float)analogRead(pot3)/1024;
    }

    if(isPot4Changed)
    {
      triangleLevel = (float)analogRead(pot4)/1024;
    }

    setAmplitudesToNull();
    setFrequenciesToNull();
    setNoteAndFrequencies();   
    
    mixer1.gain(0, sineLevel);
    mixer1.gain(1, sawLevel);
    mixer1.gain(2, squareLevel);
    mixer1.gain(3, triangleLevel);    
            
  }



// >>>>>>>>>>>>>>>>>>>>>>>>>>> Main Loop <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
unsigned long last_time = millis();

void loop()
  {
    isPotStateChanged();
    setDeviceMode();
    handleDeviceMode(); 
    soundGen();
    savePotStates();
    if (isPrintEnabled)
    {
      if(millis() - last_time >= 500) {
      if(key==0)
        {
          Serial.println("\nNot active \n");
          
        }
      
      else 
        {        
          // Serial.print("\nWaveForm:");
          // Serial.print(currentWaveForm);
          // Serial.print("  Key: ");
          // Serial.print(key); 
          // Serial.print("  Envelope:");
          // Serial.print(envelopeSelect);
          // Serial.print("  amplitudeVal: ");
          // Serial.print(amplitudeVal);
          // Serial.print("  Frequency: ");
          // Serial.print(noteFrequency);
          // Serial.print("  Current Volume:");
          // Serial.print(noteFrequency);
          // Serial.print("  Delay:");
          // Serial.print(FXVal);
          Serial.print("\n");
          Serial.print("SineLevel:");
          Serial.print(sineLevel);
          Serial.print(" SawLevel:");
          Serial.print(sawLevel);
          Serial.print("Square:");
          Serial.print(squareLevel);
          Serial.print(" Triangle:");
          Serial.print(triangleLevel);                    
        }
      last_time = millis();
    }
    delay(10);
  }
 


 }
