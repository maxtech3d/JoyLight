#include "FastLED.h"

//button pins
#define POWER_SW_PIN 8
#define COLOR_SW_PIN 9
#define BRIGHTNESS_SW_PIN 10

//led constants
#define LED_TYPE    WS2812B
#define LED_PIN     3
#define NUM_LEDS    15

CRGB leds[NUM_LEDS];

//color struct - http://fastled.io/docs/3.1/struct_c_r_g_b.html
CRGB colorsList[]={
  CRGB::Red,
  CRGB::DarkGreen,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Magenta,
  CRGB::White,
  CRGB::OrangeRed,
  CRGB::Yellow,
  CRGB::Cyan,
  CRGB::Lime
  };

//set the index to -1 for starters. Setup will call cycleToNextColor(), causing it to go to zero
int crtColorIndex=-1;

void reloadCurrentColor()
{
  fill_solid(leds,NUM_LEDS,colorsList[crtColorIndex]);
  FastLED.show(); 
}

void cycleToNextColor()
{
  crtColorIndex++;
  if (crtColorIndex>=sizeof(colorsList)/sizeof(colorsList[0]))
    crtColorIndex=0;

  reloadCurrentColor();
}

void setup() {   
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS); 
  
  pinMode(POWER_SW_PIN, INPUT_PULLUP);
  pinMode(COLOR_SW_PIN, INPUT_PULLUP);
  pinMode(BRIGHTNESS_SW_PIN, INPUT_PULLUP);

  cycleToNextColor();
}

bool displayIsOn=true;

bool IsBtnPressed(int pinNr)
{
  return !digitalRead(pinNr); //the button logic is reversed in hardware
}

void WaitUntilBtnReleased(int pinNr)
{
  //implement some primitive debouncing
  delay(5);
  while (IsBtnPressed(pinNr));
  delay(5);
}

void loop() { 
  //see if any buttons are pressed
  if (IsBtnPressed(POWER_SW_PIN))
  {
    if (displayIsOn)
    {
      displayIsOn=false;
      fill_solid(leds,NUM_LEDS,CRGB::Black);
      FastLED.show(); 
    }
    else
    {
      displayIsOn=true;
      reloadCurrentColor();
    }
    WaitUntilBtnReleased(POWER_SW_PIN);
  }

  if (IsBtnPressed(BRIGHTNESS_SW_PIN))
  {
    int brightness=FastLED.getBrightness();
    brightness+=32;
    if (brightness>=256)
      brightness-=256;

    FastLED.setBrightness(brightness);
    FastLED.show();
    WaitUntilBtnReleased(BRIGHTNESS_SW_PIN);
  }

  if (IsBtnPressed(COLOR_SW_PIN))
  {
    cycleToNextColor();
    WaitUntilBtnReleased(COLOR_SW_PIN);
  }
}
