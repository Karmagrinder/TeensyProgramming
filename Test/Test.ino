#include <Audio.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>



// Sheild input:
//const int micInput = AUDIO_INPUT_MIC;
//Input sources: 
//AudioInputI2S src;
//SD card
AudioPlaySdWav sdCardSound;
//Output source:
AudioOutputI2S dest;
//Object to control audio shield. 
AudioControlSGTL5000 audioShield;

//Switch
const int switchPin = 0;
boolean currentState = false;
boolean playingState = false;
//connections
AudioConnection connection1(sdCardSound, 0, dest,0);
AudioConnection connection2(sdCardSound, 1, dest, 1);


//AudioMixer4 myMix;


void setup()

  { pinMode(switchPin, INPUT);
    Serial.begin(9600);
    Serial.print("SETUP");
    AudioMemory(12);
    Serial.print(">AudioMemoryAllocated=12 \n");
    audioShield.enable();
    Serial.print(">AudioShieldEnabled \n");
    //audioShield.inputSelect(micInput);
    audioShield.volume(100);
    //To get sound :
    SPI.setMOSI(7);
    SPI.setSCK(14);

    SD.begin(10);
     }

void musicPlay()
  {
         sdCardSound.play("SDTEST2.WAV");
         Serial.print("\n Playing Sound: ");
         Serial.print(sdCardSound.isPlaying());
         Serial.print("\n");
         Serial.print(sdCardSound.positionMillis());
         Serial.print(" \n Total Track Length:");
         Serial.print(sdCardSound.lengthMillis());
         AudioMemoryUsageMaxReset();
         
       
          
  }
  
void loop()
  {      
    delay(1000);    
    // Toggling Programe state with Switch 
    if (digitalRead(switchPin)==HIGH)
      {
        currentState = !currentState;
      }
    if ((currentState == true) &&(sdCardSound.isPlaying()==false))
      { 
         musicPlay(); 
         
      }
     
    else 
      {
        sdCardSound.stop();
        Serial.print("\n<<< Stopped !>>> \n");
        Serial.print("\n Audio Processor Usage:");
        Serial.print(AudioProcessorUsageMax());
        playingState = false;
        
      }
    
        float vol = analogRead(15);
        vol = vol/1024;
        audioShield.volume(vol);
        delay(20);
        Serial.print("\n Current Volume:");
        Serial.print(vol);
        
       }
       
       

