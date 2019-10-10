#define buttonPin 6
#include "jsbutton.h"

#include "FastLED.h"
#include "EEPROM.h"

#define NUM_LEDS 72
#define DATA_PIN 10

#define MAX_BRIGHTNESS     128
#define MED_BRIGHTNESS     16
#define MIN_BRIGHTNESS     8
#define FRAMES_PER_SECOND  120

//#define RED_LEDS 24
//#define AMBER_LEDS 48
//#define GREEN_LEDS 72

#define RED_LEDS 72
#define AMBER_LEDS 48
#define GREEN_LEDS 24

#define STOP_DELAY 6000
#define AMBER_DELAY 2500
#define GO_DELAY 6000

// Definition for the array of routines to display.
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//Mode and EEPROM variables
uint8_t maxMode = 6;                                          // Maximum number of display modes. Would prefer to get this another way, but whatever.
int eepaddress = 0;

uint8_t gCurrentPatternNumber = 0;                            // Index number of which pattern is current
uint8_t gHue = 0;                                             // rotating "base color" used by many of the patterns

typedef void (*SimplePatternList[])();                        // List of patterns to cycle through.  Each is defined as a separate function below.


int PHOTOCELL_PIN=1;
int PHOTOCELL_READING;

CRGB leds[NUM_LEDS];


//Set up our state change detection
String trafficLightState = "AMBER";      // state will go RED > REDAMBER > GREEN > AMBER > RED
unsigned long previousMillis = 0;        // will store last time LED was updated

void setup() { 
  Serial.begin(9600);
  
  delay(3000); // 3 second delay for recovery

  pinMode(buttonPin, INPUT);      // Set button input pin
  digitalWrite(buttonPin, HIGH );

  set_max_power_in_volts_and_milliamps(5, 750);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MIN_BRIGHTNESS); 

  gCurrentPatternNumber = EEPROM.read(eepaddress);
  //gCurrentPatternNumber = 0;

  if (gCurrentPatternNumber > maxMode) gCurrentPatternNumber = 0;   // A safety in case the EEPROM has an illegal value.

  Serial.print("Starting ledMode: ");
  Serial.println(gCurrentPatternNumber);
}


SimplePatternList gPatterns = {trafficLight, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };


void loop() {

  readbutton();
  
  EVERY_N_MILLISECONDS(50) {
     gPatterns[gCurrentPatternNumber]();                       // Call the current pattern function once, updating the 'leds' array
  }

  EVERY_N_MILLISECONDS(20) {                                  // slowly cycle the "base color" through the rainbow
    gHue++;
  }

  FastLED.show();
}

void setColour(int led_start, int led_end, struct CRGB colour) {
  for(int led = led_start; led < led_end; led++) { 
            leds[led] = colour; 
        }
  FastLED.show();
}


void setBrightness() {
  int photocellValue = analogRead(PHOTOCELL_PIN);
  int mappedValue;
  
  if (photocellValue < 400) {
    mappedValue = MIN_BRIGHTNESS;
  }
  else if ((400 <= photocellValue) &&  (photocellValue < 800) ) {
    mappedValue = MED_BRIGHTNESS;
  }
  else {
    mappedValue = MAX_BRIGHTNESS;
  }
   
  Serial.print("Analog value = ");
  Serial.println(photocellValue);
  Serial.print("Mapped value = ");
  Serial.println(mappedValue);
  FastLED.setBrightness(mappedValue);
}


void readbutton() {                                           // Read the button and increase the mode

  uint8_t b = checkButton();

  if (b == 1) {                                               // Just a click event to advance to next pattern
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
    Serial.println(gCurrentPatternNumber);
    FastLED.clear();
  }

  if (b == 2) {                                               // A double-click event to reset to 0 pattern
    gCurrentPatternNumber = 0;
    Serial.println(gCurrentPatternNumber);
    FastLED.clear();
  }

  if (b == 3) {                                               // A hold event to write current pattern to EEPROM
    EEPROM.write(eepaddress,gCurrentPatternNumber);
    Serial.print("Writing: ");
    Serial.println(gCurrentPatternNumber);
    FastLED.clear();
    leds[0] = CRGB::White;
    FastLED.delay(200);
    FastLED.show();
    FastLED.delay(200);
    FastLED.clear();
  }

} // readbutton()




//--------------------[ Effects are below here ]------------------------------------------------------------------------------

void trafficLight() {
  unsigned long currentMillis = millis();

  if((trafficLightState == "RED") && (currentMillis - previousMillis >= STOP_DELAY))
  {
    //Go to REDAMBER
    FastLED.clear();
    setColour(AMBER_LEDS, RED_LEDS, CRGB::Red);
    setColour(GREEN_LEDS, AMBER_LEDS, CRGB::DarkOrange);
    //Remember the time
    previousMillis = currentMillis;
    //Change the state to REDAMBER
    trafficLightState = "REDAMBER";
    Serial.println(trafficLightState);
  }
  
  if((trafficLightState == "REDAMBER") && (currentMillis - previousMillis >= AMBER_DELAY))
  {
    //Go to GREEN
    FastLED.clear();
    setColour(0, GREEN_LEDS, CRGB::Green);
    //Remember the time
    previousMillis = currentMillis;
    //Change the state to GREEN
    trafficLightState = "GREEN";
    Serial.println(trafficLightState);
  }
  
  if((trafficLightState == "GREEN") && (currentMillis - previousMillis >= GO_DELAY))
  {
    //Go to AMBER
    FastLED.clear();
    setColour(GREEN_LEDS, AMBER_LEDS, CRGB::DarkOrange);
    //Remember the time
    previousMillis = currentMillis;
    //Change the state to AMBER
    trafficLightState = "AMBER";
    Serial.println(trafficLightState);
  }

  if((trafficLightState == "AMBER") && (currentMillis - previousMillis >= AMBER_DELAY))
  {
    //Go to RED
    FastLED.clear();
    setColour(AMBER_LEDS, RED_LEDS, CRGB::Red);
    //Remember the time
    previousMillis = currentMillis; //Remember the time
    //Change the state to RED
    trafficLightState = "RED";
    Serial.println(trafficLightState);
  }
}


void rainbow() {

  fill_rainbow(leds, NUM_LEDS, gHue, 7);                      // FastLED's built-in rainbow generator.
  
} // rainbow()



void rainbowWithGlitter() {

  rainbow();                                                  // Built-in FastLED rainbow, plus some random sparkly glitter.
  addGlitter(70);
  
} // rainbowWithGlitter()



void addGlitter(fract8 chanceOfGlitter) {

  if(random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }

} // addGlitter()



void confetti() {                                             // Random colored speckles that blink in and fade smoothly.

  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
  
} // confetti()



void sinelon() {                                              // A colored dot sweeping back and forth, with fading trails.

  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS-1);
  leds[pos] += CHSV(gHue, 255, 192);
  
} // sinelon()



void bpm() {                                                  // Colored stripes pulsing at a defined Beats-Per-Minute.

  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  
  for(int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  
} // bpm()



void juggle() {                                               // Eight colored dots, weaving in and out of sync with each other.

  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  
  for(int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS-1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  
} // juggle()