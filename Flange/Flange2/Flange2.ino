
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

// Number of samples in each delay line
#define FLANGE_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)
//AUDIO_BLOCK_SAMPLES is defined in AudioStream.h (cores/teensy3), 
//1 AUDIO_BLOCK_SAMPLE =128 (approx 2.9ms),
//read more here: https://www.pjrc.com/teensy/td_libs_AudioNewObjects.html

// Allocate the delay lines for left and right channels
// Refer to effects_info.h for definitions of varios DELAY_LENGTHS
short l_delayline[FLANGE_DELAY_LENGTH];
short r_delayline[FLANGE_DELAY_LENGTH];

// Defining a button to trigger effect
const int switchPin = 0;
boolean currentState = false;
boolean fxState = false;

//Selecting LINE in MIC input is not that good, it get's distorted quite easily.
const int myInput = AUDIO_INPUT_LINEIN;


AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioEffectFlange   l_myEffect;
AudioEffectFlange   r_myEffect;
AudioOutputI2S      audioOutput;        // audio shield: headphones & line-out


// Create Audio connections between the components
// Both channels of the audio input go to the flange effect
AudioConnection c1(audioInput, 0, l_myEffect, 0);
// If the input jack is MONO un-comment the following:
AudioConnection c2(audioInput, 0, r_myEffect, 0);  
// If the input jack is STEREO un-comment the following:
//AudioConnection c2(audioInput, 1, r_myEffect, 0);  

// both channels from the flange effect go to the audio output
AudioConnection c3(l_myEffect, 0, audioOutput, 0);
AudioConnection c4(r_myEffect, 0, audioOutput, 1);

AudioControlSGTL5000 audioShield;

//FlangeResult = sig + sig(s_idx + s_depth*sin(2*pi*s_freq))

// editing the values of s_idx and s_depth doesn't works out that well, yet to figure out this.
int s_idx = FLANGE_DELAY_LENGTH/4;
int s_depth = FLANGE_DELAY_LENGTH/4;

//s_freq value is controlled by potentiometers the values 0,0.5,1 will get same value for sine function i.e 0,
//to get maximum depth effect 0.25 for +Depth and 0.75 for -Depth
double s_freq = .3; 

//potentiometers 
//const int pot1Pin = 21;
const int pot2Pin = 20;
//float pot1Val;
float pot2Val;

void setup()

{
    pinMode(switchPin, INPUT);
   // pinMode(pot1Pin, INPUT);
    pinMode(pot2Pin, INPUT);
    Serial.begin(9600);
    //while (!Serial) ;
    delay(3000); 
    AudioMemory(8);
    audioShield.enable();
    audioShield.inputSelect(myInput);
    audioShield.volume(0.5);
    
 // Set up the flange effect:
      // address of delayline
      // total number of samples in the delay line
      // Index (in samples) into the delay line for the added voice
      // Depth of the flange effect
      // frequency of the flange effect
      l_myEffect.begin(l_delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
      r_myEffect.begin(r_delayline,FLANGE_DELAY_LENGTH,s_idx,s_depth,s_freq);
      // Initially the effect is off. It is switched on when the
      // PASSTHRU button is pushed.
      l_myEffect.voices(FLANGE_DELAY_PASSTHRU,0,0);
      r_myEffect.voices(FLANGE_DELAY_PASSTHRU,0,0);
      Serial.println("setup done");
      AudioProcessorUsageMaxReset();
      AudioMemoryUsageMaxReset();
    
}

void volumeControl()

{
  
    //Volume control
    float vol = analogRead(15);
    vol = vol/1048;
    audioShield.volume(vol);
    delay(20);
    Serial.print("\n Current Volume:");
    Serial.print(vol);

}


void FX()
{  
    Serial.print("\n FX active ! \n");
    l_myEffect.voices(s_idx,s_depth,s_freq);
    r_myEffect.voices(s_idx,s_depth,s_freq);
    fxState = true;
    Serial.print("\n Audio Processor Usage(in FX):");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("\n Memory Usage:");
    Serial.print(AudioMemoryUsageMax());
    AudioMemoryUsageMaxReset();
    AudioProcessorUsageMaxReset();
          
        
}

void bypass()
{
    Serial.print("\n Bypass Mode Active ! \n");
    l_myEffect.voices(FLANGE_DELAY_PASSTHRU,0,0);
    r_myEffect.voices(FLANGE_DELAY_PASSTHRU,0,0);
    fxState = false;
    Serial.print("\n Audio Processor Usage:");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("\n Memory Usage:");
    Serial.print(AudioMemoryUsageMax());
    AudioMemoryUsageMaxReset();
    AudioProcessorUsageMaxReset();
}


void potControl()
{
  //pot1Val = analogRead(pot1Pin);
  pot2Val = analogRead(pot2Pin);
  
  s_freq = pot2Val/1024;
  //s_depthFactor = pot1Val/100;
  
  Serial.print("\n s_freq:");
  Serial.print(s_freq);

    
}

void loop()
{  
  delay(1000);
    if (digitalRead(switchPin)==HIGH)
      {

        FX();
        potControl();
        
      }
    
    else 
      {
        bypass();
      }
    volumeControl(); 
}

