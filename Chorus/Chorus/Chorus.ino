/*This program runs the chorus effect and uses a switch to toggle the
effect and bypass, and a potentiometer to change the chorus voicing
*/
/*
VERSION 2 - use modified library which has been changed to handle
            one channel instead of two
140529
Proc = 7 (7),  Mem = 4 (4)
  2a
  - default at startup is to have passthru ON and the button
    switches the chorus effect in.
  
previous performance measures were PROC/MEM 9/4

From: http://www.cs.cf.ac.uk/Dave/CM0268/PDF/10_CM0268_Audio_FX.pdf
about Comb filter effects
Effect      Delay range (ms)    Modulation
Resonator      0 - 20            None
Flanger        0 - 15            Sinusoidal (approx 1Hz)
Chorus        25 - 50            None
Echo            >50              None

FMI:
The audio board uses the following pins.
 6 - MEMCS
 7 - MOSI
 9 - BCLK
10 - SDCS
11 - MCLK
12 - MISO
13 - RX
14 - SCLK
15 - VOL
18 - SDA
19 - SCL
22 - TX
23 - LRCLK


AudioProcessorUsage()
AudioProcessorUsageMax()
AudioProcessorUsageMaxReset()
AudioMemoryUsage()
AudioMemoryUsageMax()
AudioMemoryUsageMaxReset()

The CPU usage is an integer from 0 to 100, and the memory is from 0 to however
many blocks you provided with AudioMemory().

*/

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

// Number of samples in each delay line
#define CHORUS_DELAY_LENGTH (200*AUDIO_BLOCK_SAMPLES) // 16*2.5 = 40ms 
// Allocate the delay lines for left and right channels
short l_delayline[CHORUS_DELAY_LENGTH];
short r_delayline[CHORUS_DELAY_LENGTH];

// Default is to just pass the audio through. 

//const int myInput = AUDIO_INPUT_MIC;
const int myInput = AUDIO_INPUT_LINEIN;

AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioEffectChorus   l_myEffect;
AudioEffectChorus   r_myEffect;
AudioOutputI2S      audioOutput;        // audio shield: headphones & line-out

// Create Audio connections between the components
// Both channels of the audio input go to the chorus effect
AudioConnection c1(audioInput, 0, l_myEffect, 0);
// If the input jack is MONO un-comment the following:
AudioConnection c2(audioInput, 0, r_myEffect, 0);  
// If the input jack is STEREO un-comment the following:
//AudioConnection c2(audioInput, 1, r_myEffect, 0);
// both channels chorus effects go to the audio output
AudioConnection c3(l_myEffect, 0, audioOutput, 0);
AudioConnection c4(r_myEffect, 0, audioOutput, 1);

AudioControlSGTL5000 audioShield;

// number of "voices" in the chorus which INCLUDES the original voice
int n_chorus = 2;

//Potentiometers 
//const int pot1Pin = 21;
const int pot2Pin = 20;
//float pot1Val;
float pot2Val;
// Defining a button to trigger effect
const int switchPin = 0;
boolean currentState = false;
boolean fxState = false;


// <<<<<<<<<<<<<<>>>>>>>>>>>>>>>>
void setup() {
  
  Serial.begin(9600);
  while (!Serial) ;
  delay(3000);

  pinMode(switchPin, INPUT);
  // pinMode(pot1Pin, INPUT);
  pinMode(pot2Pin, INPUT);

  // Maximum memory usage was reported as 4
  // Proc = 9 (9),  Mem = 4 (4)
  AudioMemory(4);

  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.65);
  
  // Initialize the effect - left channel
  // address of delayline
  // total number of samples in the delay line
  // number of voices in the chorus INCLUDING the original voice
  if(!l_myEffect.begin(l_delayline,CHORUS_DELAY_LENGTH,n_chorus)) {
    Serial.println("AudioEffectChorus - left channel begin failed");
    while(1);
  }

  // Initialize the effect - right channel
  // address of delayline
  // total number of samples in the delay line
  // number of voices in the chorus INCLUDING the original voice
  if(!r_myEffect.begin(r_delayline,CHORUS_DELAY_LENGTH,n_chorus)) {
    Serial.println("AudioEffectChorus - left channel begin failed");
    while(1);
  }
  // Initially the effect is off. It is switched on when the
  // PASSTHRU button is pushed.
  l_myEffect.voices(0);
  r_myEffect.voices(0);

  Serial.println("setup done");
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}


// audio volume

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

void potControl()
{
  //pot1Val = analogRead(pot1Pin);
  pot2Val = analogRead(pot2Pin);
  
  n_chorus = pot2Val/300;
  //s_depthFactor = pot1Val/100;
  
  Serial.print("\n n_chorus:");
  Serial.print(n_chorus);

    
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

  
void FX()
  {
    Serial.print("\n FX active ! \n");
    l_myEffect.voices(n_chorus);
    r_myEffect.voices(n_chorus);
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
    l_myEffect.voices(0);
    r_myEffect.voices(0);
    fxState = false;
    Serial.print("\n Audio Processor Usage:");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("\n Memory Usage:");
    Serial.print(AudioMemoryUsageMax());
    AudioMemoryUsageMaxReset();
    AudioProcessorUsageMaxReset();
  }


