#include "FastLED.h"

#define NUM_LEDS 72
#define DATA_PIN 10

#define MAX_BRIGHTNESS     64
#define MED_BRIGHTNESS     16
#define MIN_BRIGHTNESS     8
#define FRAMES_PER_SECOND  120

#define RED_LEDS 24
#define AMBER_LEDS 48
#define GREEN_LEDS 72

#define STOP_DELAY 6000
#define AMBER_DELAY 2500
#define GO_DELAY 6000

int PHOTOCELL_PIN=1;
int PHOTOCELL_READING;

CRGB leds[NUM_LEDS];

void setup() { 
  Serial.begin(9600);
  
  delay(3000); // 3 second delay for recovery

  set_max_power_in_volts_and_milliamps(5, 500);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MIN_BRIGHTNESS); 
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

void loop() {


  /* Red to Green sequence */
  FastLED.clear();
  for(int led = 0; led < RED_LEDS; led++) { 
            leds[led] = CRGB::Red; 
        }
  setBrightness();
  FastLED.show();
  delay(STOP_DELAY);

  for(int led = RED_LEDS; led < AMBER_LEDS; led++) { 
            leds[led] = CRGB::DarkOrange; 
        }
  setBrightness();
  FastLED.show();
  delay(AMBER_DELAY);

  FastLED.clear();

  // Now set green
  for(int led = AMBER_LEDS; led < GREEN_LEDS; led++) { 
          leds[led] = CRGB::Green; 
      } 
  setBrightness();
  FastLED.show();
  delay(GO_DELAY);

  /* Green to Red sequence */
  FastLED.clear();
  for(int led = RED_LEDS; led < AMBER_LEDS; led++) { 
          leds[led] = CRGB::DarkOrange; 
      }
  setBrightness();
  FastLED.show();
  delay(AMBER_DELAY);

}
