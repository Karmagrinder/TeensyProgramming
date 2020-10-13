#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <math.h>
#include <stdio.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
//AudioEffectDelay         delay1;
AudioEffectFreeverb      freeverb1;
AudioOutputI2S           i2s1;           //xy=345,389
AudioControlSGTL5000 sgtl5000_1;
AudioConnection          patchCord1(sine1, 0, mixer1, 0);
AudioConnection          patchCord2(saw1, 0, mixer1, 1);
AudioConnection          patchCord3(square1, 0, mixer1, 2);
AudioConnection          patchCord4(triangle1, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, envelope1);
AudioConnection          patchCord6(envelope1, freeverb1);
AudioConnection          patchCord7(freeverb1, 0, i2s1, 0);
AudioConnection          patchCord8(freeverb1, 0, i2s1, 1); //Delay is mono to save processing
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
bool isPrintEnabled = false;
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
int midiChannel;
int lastNote;
float FXVal1;

enum waveforms
  {
    sine = 0,
    saw = 1,
    square = 2,
    triangle = 3    
  };

////  Display stuff ///

char screen_line0[20] = "KonkaSynth v3"; 
char screen_line1[20] = "Mode: ";
char screen_line2[20] = "Wave:";
char screen_line3[20] = "Env: ";

//>>>>>>>>>>>>>>>>>>>>>>>SETUP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void setup() 
  {
    //Serial.begin (9600);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
      //Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.display();
    delay(2000);
    setTextOnOled(screen_line0, 0);
    delay(2000);
    
    
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
//    delay1.disable(0);
//    delay1.disable(1); // delay is off on restart. 
//    delay1.disable(2);
//    delay1.disable(3);
//    delay1.disable(4);
//    delay1.disable(5);
//    delay1.disable(6);
//    delay1.disable(7);  
    AudioMemory(70);
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.5);

    deviceMode = 1;
    midiChannel = 1;

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
    distance = (float)duration/58.2;
    return distance;
  }
//>>>>>>>>>>>>>>>>>>>>> Key Mapping <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int keyMap()
  {
    sensorVal = sensorReading();
     // Turn off the previous notes
    if (sensorVal > 67)  // ignore if distance is more than 67cm
      {
        //Serial.println("\n Distance Sensor: ignoring\n");
        key = 0;
        if(lastNote != 0 && key == 0)
        {
          usbMIDI.sendNoteOff(lastNote, 127, midiChannel);
        }        
        lastNote = 0;
        return key;
      }
          
    key = sensorVal + 21;
    if (lastNote != key)
    {
      usbMIDI.sendNoteOff(lastNote, 127, midiChannel);
      usbMIDI.sendNoteOn(key, 99, midiChannel);
      lastNote = key;
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
       amplitudeVal = (float)z/300;           
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

void setWaveFormAmplitude(int selectedWaveForm)
  {
    setMixerLevelsToNull();
    setAmplitudesToNull();
    setFrequenciesToNull();
    setNoteAndFrequencies();
    float level = 0.5;
    mixer1.gain(selectedWaveForm, level);
  }

void waveFormHandling()
  {
    int potVal1=analogRead(waveformPotPin);
    int waveFormSelect = potVal1/256;
    setWaveFormAmplitude(waveFormSelect);
    currentWaveForm = waveFormSelect;
  }
//>>>>>>>>>>>>>>>>>>>>Delay<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void delayHandler()
  {
//    int potVal2 = analogRead(FXPotPin);
//        FXVal = potVal2*0.28;  // If the FX value goes over 300/ or ~290 weird things start to happen, so let's keep it low.
//        //delay(20);
//        if(FXVal>1)
//        {  
//          
//          if(FXVal>140)
//            {
//              FXVal = 140; // Restricting the delay to some sane value, to prevent crash.
//            }
//          delay1.delay(0, FXVal);
//        
//        }
  }

void reverbHandler()
{
      FXVal =  analogRead(FXPotPin)/1023.0;
      FXVal1 = analogRead(pot4)/1023.0;
      
      freeverb1.roomsize(FXVal);
      freeverb1.damping(FXVal1);
  
}
 
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> generating the sound<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void soundGen()
  {
    keyMap();
    noteFrequency = frequencyCalculator();
    //noteAmplitude = amplitudeMapping();
    noteAmplitude = 0.5;    
    if (noteFrequency > 27.4)
    {     
      envelope1.noteOn();
      delay(2);
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
    {   
      vol = analogRead(volumePin);
      vol = (float)vol/1023.0;
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
        reverbHandler();
        
        break;
      
      case 2:
        mapPotsToEnvelope();
        break;
      
      case 3:
        mapPotsToWaveformMixer();
        break;
        
      case 4:
        midiControllerMode();
        break;
      
      default:
        break;
    }
  }

// CHECK POT STATE CHANGE
void isPotStateChanged()
  {
    if (pot1LastVal != analogRead(pot1)/120)
    {
      isPot1Changed = true;
    }
    else
    {
      isPot1Changed = false;
    }    
     
    if (pot2LastVal != analogRead(pot2)/120)
    {
      isPot2Changed = true;
    }
    else
    {
      isPot2Changed = false;
    }

    if (pot3LastVal != analogRead(pot3)/120)
    {
      isPot3Changed = true;
    }
    else
    {
      isPot3Changed = false;
    }

    if (pot4LastVal != analogRead(pot4)/120)
    {
      isPot4Changed = true;
    }
    else
    {
      isPot4Changed = false;
    }
  }

  // SAVE POT STATEs
void savePotStates()
  {
    pot1LastVal = analogRead(pot1)/120;
    pot2LastVal = analogRead(pot2)/120;
    pot3LastVal = analogRead(pot3)/120;
    pot4LastVal = analogRead(pot4)/120;
  }

// Envelope Mode
void mapPotsToEnvelope()
  {
    
    if(isPot1Changed)
    {
      attackVal = (float)analogRead(pot1)/100;
      updateEnvlope();
    }

    if(isPot2Changed)
    {
      decayVal = (float)analogRead(pot2)/50;
      updateEnvlope();
    }

    if(isPot3Changed)
    {
      sustainVal = (float)analogRead(pot3)/50;
      updateEnvlope();
    }

    if(isPot4Changed)
    {
      releaseVal = (float)analogRead(pot4)/50;
      updateEnvlope();
    }  
 }
  
void updateEnvlope()
{
  float defaultDelayVal = 0;
  float defaultHoldVal = 0.2;
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

// MIDI controller mode
void midiControllerMode()
  {
       isPrintEnabled = !isPrintEnabled;
   }

   
// Screen
void setTextOnOled(char text[], int lineNumber) {
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,lineNumber);             // Start at top-left corner
  display.println(F(text));
  display.display();  
}

/// Display params ////>>>>>>>>

void displayParams()
{ 
  display.clearDisplay();
  sprintf(screen_line0,"<< KonkaSynth v3 >>");
  setTextOnOled(screen_line0, 0);
  switch (deviceMode)
    {
      case 1:
        sprintf(screen_line1, "Mode: std");
        sprintf(screen_line2,"Waveform type: %d", currentWaveForm);
        //sprintf(screen_line2, "1=sin, 2=saw, 3=sqr, 4=trg");
        sprintf(screen_line3, "Vol:%.2f, Fx:%.2f", vol, FXVal);                
        
        setTextOnOled(screen_line1, 8);
        setTextOnOled(screen_line2, 16);
        setTextOnOled(screen_line3, 24);       
        
        break;
      
      case 2:
        sprintf(screen_line1, "Mode: env");
        sprintf(screen_line2, "A:%.2f, D:%2.f", attackVal, decayVal);
        sprintf(screen_line3, "S:%.2f, R: %.2f", sustainVal, releaseVal);
        setTextOnOled(screen_line1, 8);
        setTextOnOled(screen_line2, 16);
        setTextOnOled(screen_line3, 24);
        break;
      
      case 3:
        sprintf(screen_line1, "Mode: Wave-mixer");
        sprintf(screen_line2, "Sin:%.2f, Saw:%2.f", sineLevel, sawLevel);  //8
        sprintf(screen_line3, "Sqr:%.2f, Trg: %.2f", squareLevel, triangleLevel);
        setTextOnOled(screen_line1, 8);
        setTextOnOled(screen_line2, 16);
        setTextOnOled(screen_line3, 24);
        break;
        
      case 4:
        midiControllerMode();
        sprintf(screen_line1, "Mode: MIDI");
        setTextOnOled(screen_line1, 8);
        break;
      
      default:
        break;
    }
   
    
}

///Serial print ///

void printStuffToSerial()
{
   Serial.print("\nWaveForm:");
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
   Serial.print(vol);
   Serial.print("  Delay:");
   Serial.print(FXVal);
   
   Serial.print("\n");
   Serial.print("SineLevel:");
   Serial.print(sineLevel);
   Serial.print(" SawLevel:");
   Serial.print(sawLevel);
   Serial.print("Square:");
   Serial.print(squareLevel);
   Serial.print(" Triangle:");
   Serial.print(triangleLevel);
  Serial.print("\n");
  Serial.print("Attack:");
  Serial.print(attackVal);
  Serial.print(" Decay:");
  Serial.print(decayVal);
  Serial.print(" Sustain:");
  Serial.print(sustainVal);
  Serial.print(" Release:");
  Serial.print(releaseVal);  
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
    
    
    if(millis() - last_time >= 1000) 
    {
        displayParams();

      if (isPrintEnabled)
      {
        if(key!=0)
          {
            printStuffToSerial();
          }
      }
    last_time = millis();
  }

 }
