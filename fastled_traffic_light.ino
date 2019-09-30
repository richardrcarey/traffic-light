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

//Set up our state change detection
String trafficLightState = "AMBER";                 // state will go RED > REDAMBER > GREEN > AMBER > RED
unsigned long previousMillis = 0;        // will store last time LED was updated

void setup() { 
  Serial.begin(9600);
  
  delay(3000); // 3 second delay for recovery

  set_max_power_in_volts_and_milliamps(5, 500);
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MIN_BRIGHTNESS); 
}


void loop() {
  
  EVERY_N_MILLISECONDS(50) {
    trafficLight(); 
  }
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


void trafficLight() {
  unsigned long currentMillis = millis();

  if((trafficLightState == "RED") && (currentMillis - previousMillis >= STOP_DELAY))
  {
    //Go to REDAMBER
    FastLED.clear();
    setColour(0, RED_LEDS, CRGB::Red);
    setColour(RED_LEDS, AMBER_LEDS, CRGB::DarkOrange);
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
    setColour(AMBER_LEDS, GREEN_LEDS, CRGB::Green);
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
    setColour(RED_LEDS, AMBER_LEDS, CRGB::DarkOrange);
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
    setColour(0, RED_LEDS, CRGB::Red);
    //Remember the time
    previousMillis = currentMillis; //Remember the time
    //Change the state to RED
    trafficLightState = "RED";
    Serial.println(trafficLightState);
  }
}
