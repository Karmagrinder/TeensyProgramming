#include <Audio.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>


//SD card as input source
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


void setup()

   {      pinMode(switchPin, INPUT);
          Serial.begin(9600);
          Serial.print("SETUP");
          AudioMemory(12);
          Serial.print(">AudioMemoryAllocated=12 \n");
          audioShield.enable();
          Serial.print(">AudioShieldEnabled \n");
          //audioShield.volume(100);
          //To get sound :
          SPI.setMOSI(7);
          SPI.setSCK(14);
      
          SD.begin(10);
     }

void musicPlay()
  {
         sdCardSound.play("SDTEST2.WAV");
         // Some delay so that all the SD card related functions get updated with their respective information.  
         delay(2000); 
         Serial.print("\n Playing Sound: ");
         Serial.print(sdCardSound.isPlaying());
         Serial.print("\n");
         Serial.print(sdCardSound.positionMillis());
         Serial.print(" \n Total Track Length:");
         Serial.print(sdCardSound.lengthMillis());
         
         //This loop is required to make sure that program doesn't exits back to main loop, 
         //because that will cause the sdCardSound.play()function to get interupted.
         //Also to be able to stop the audio when ever desired. 
         while(sdCardSound.isPlaying())
           {
             if (digitalRead(switchPin)==HIGH)
              {
                currentState = !currentState;
                sdCardSound.stop();
                Serial.print("\n <<<<Stoping>>>>\n");
              }
              delay(500);
              Serial.print("\n<<<<Playing>>>\n");
           }
         AudioMemoryUsageMaxReset();
         playingState = true;     
                 
  }
  
void volumeControl()

{
  
    //Volume control
    float vol = analogRead(15);
    vol = vol/2048;
    audioShield.volume(vol);
    delay(20);
    Serial.print("\n Current Volume:");
    Serial.print(vol);

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
                
      }
    volumeControl();  
    
  }
       
       

