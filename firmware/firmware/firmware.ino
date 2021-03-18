#include "FastLED.h"

//button pins
#define POWER_SW_PIN 8
#define COLOR_SW_PIN 9
#define BRIGHTNESS_SW_PIN 10

//led constants
#define LED_TYPE    WS2812B
#define LED_PIN     3
#define NUM_LEDS    15


enum AnimationTypesEnum{
  AnimationNone,
  AnimationRainbow,
  AnimationFade
};

struct ColorAnimationStruct{
  CRGB color; //NULL for animations
  AnimationTypesEnum animationType; //animationType==AnimationNone for static colors
  int animationDelayMs; //NULL for animationType==AnimationNone
};

CRGB leds[NUM_LEDS];

//color struct - http://fastled.io/docs/3.1/struct_c_r_g_b.html
ColorAnimationStruct displayOptions[]={
  {NULL, AnimationRainbow, 5},
  {NULL, AnimationRainbow, 1},
  {NULL, AnimationFade, 10},
  {NULL, AnimationFade, 5},
  {CRGB::Red, AnimationNone,  NULL},
  {CRGB::OrangeRed, AnimationNone, NULL},
  {CRGB::Yellow, AnimationNone, NULL},
  {CRGB::GreenYellow, AnimationNone, NULL}, // light greenish
  {CRGB::Lime, AnimationNone, NULL},
  {CRGB::Blue, AnimationNone, NULL},
  {CRGB::DeepSkyBlue, AnimationNone, NULL},
  {CRGB::White, AnimationNone, NULL},
  {CRGB::DarkViolet, AnimationNone,  NULL},
  {CRGB::DeepPink, AnimationNone,  NULL}
};

//set the index to -1 for starters. Setup will call cycleToNextColor(), causing it to go to zero
int crtOptionIndex=-1;
int animationCurrentHue=1;

bool displayIsOn=true;

void reloadCurrentColor()
{
  if (displayOptions[crtOptionIndex].animationType==AnimationNone)
  {
    fill_solid(leds,NUM_LEDS,displayOptions[crtOptionIndex].color);
    FastLED.show(); 
  }
  else
  {
    updateAnimation();
  }
}

void cycleToNextOption()
{
  crtOptionIndex++;
  if (crtOptionIndex>=sizeof(displayOptions)/sizeof(displayOptions[0]))
    crtOptionIndex=0;

  reloadCurrentColor();
}

void updateAnimation()
{
  if (!displayIsOn)
    return;
  
  if (displayOptions[crtOptionIndex].animationType==AnimationRainbow)
  {
    //crtLedHue is used to advance the hue from one LED to the next
    uint8_t crtLedHue=animationCurrentHue; 
    
    for (int i=0; i<NUM_LEDS; i++)
    {
      leds[i]=CHSV(crtLedHue, 255, 255);
      crtLedHue+=(255/NUM_LEDS);
    }
  }
  else if (displayOptions[crtOptionIndex].animationType==AnimationFade)
  {
    fill_solid(leds,NUM_LEDS,CHSV(animationCurrentHue, 255, 255));
  }
  else
  {
    return; //not an animation
  }
  
  FastLED.show();  
  animationCurrentHue++;
  delay(displayOptions[crtOptionIndex].animationDelayMs);
}

void setup() {   
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS); 
  
  pinMode(POWER_SW_PIN, INPUT_PULLUP);
  pinMode(COLOR_SW_PIN, INPUT_PULLUP);
  pinMode(BRIGHTNESS_SW_PIN, INPUT_PULLUP);

  cycleToNextOption();
}

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
    cycleToNextOption();
    WaitUntilBtnReleased(COLOR_SW_PIN);
  }

  updateAnimation();
}
