/*
    Author: Lucas Bruder <LBruder@me.com>
    Date created: 12/6/2016

    Arduino Based Santa Hat!
*/
#include <FastLED.h>
#include <EEPROM.h>

#define SERIAL_BAUD_RATE (115200)

#define LED_DEFAULT_BRIGHTNESS (20U)

#define EEPROM_ANIMATION_STATE_ADDR (0)
#define EEPROM_SPEED_STATE_ADDR     (10)

#define LED_PIN 12
#define LED_STRIP_LENGTH 18

/*
 * Different animations the santa hat will show
 */
enum animation_state_t {
  // Red and green lights constantly circle around the hat
  ANIMATION_CIRCLING = 0,
  // Red and green lights stay stationary, fade in and out
  ANIMATION_FADING,
  // Red bounces around the hat and once it reaches the beginning
  // green starts again
  ANIMATION_BOUNCING,
  // Circling the hat and fading in and out
  ANIMATION_CIRCLING_AND_FADING,
};

/*
 * Speed at which the santa hat will display the animations
 */
enum speed_state_t {
  SPEED_SLOW = 0,
  SPEED_MEDIUM,
  SPEED_FAST
};

CRGB leds[LED_STRIP_LENGTH];
enum animation_state_t animation;
enum speed_state_t speed_state;

// If red index is even, led_index % 2 = red
bool even_index_red = true;

uint8_t led_brightness = LED_DEFAULT_BRIGHTNESS;
bool brightness_increasing = true;

/*
 * Set the current speed state based on the last one loaded
 * from EEPROM.
 */
enum speed_state_t get_speed_state(enum speed_state_t last_speed_state)
{
    enum speed_state_t new_speed_state;
    switch(last_speed_state) 
    {
        case SPEED_SLOW:
            new_speed_state = SPEED_MEDIUM;
            break;
        case SPEED_MEDIUM:
            new_speed_state = SPEED_FAST;
            break;
        case SPEED_FAST:
            new_speed_state = SPEED_SLOW;
            break;
        default:
          while(1) {
              Serial.println("== Error setting speed state ==");
          }
    }
    return new_speed_state;
}

/*
 * Set the current animation state based on the last one loaded
 * from EEPROM.
 */
enum animation_state_t new_animation_state(enum animation_state_t last_animation_state)
{
    enum animation_state_t new_animation_state;
    switch(last_animation_state) 
    {
        case ANIMATION_CIRCLING:
            new_animation_state = ANIMATION_FADING;
            break;
        case ANIMATION_FADING:
            new_animation_state = ANIMATION_BOUNCING;
            break;
        case ANIMATION_BOUNCING:
            new_animation_state = ANIMATION_CIRCLING_AND_FADING;
            break;
        case ANIMATION_CIRCLING_AND_FADING:
            new_animation_state = ANIMATION_CIRCLING;
            break;
        default:
            while(1) {
                Serial.println("== Error setting animation ==");
            }
    }
    return new_animation_state;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Starting santa hat");
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_STRIP_LENGTH);
  FastLED.setBrightness(led_brightness);

  // load state and speed from EEPROM
  enum animation_state_t last_animation_state = (enum animation_state_t)EEPROM.read(EEPROM_ANIMATION_STATE_ADDR);
  enum speed_state_t last_speed_state =  (enum speed_state_t)(EEPROM.read(EEPROM_SPEED_STATE_ADDR));

  speed_state = get_speed_state(last_speed_state);

  if ((last_speed_state == SPEED_FAST) && (speed_state == SPEED_SLOW)) {
      animation = new_animation_state(last_animation_state);
  } else {
      animation = last_animation_state;
  }
  
  EEPROM.write(EEPROM_ANIMATION_STATE_ADDR, animation);
  EEPROM.write(EEPROM_SPEED_STATE_ADDR, speed_state);
}

void show_animation_circling(void)
{
    for (uint8_t led_index = 0; led_index < LED_STRIP_LENGTH; led_index++) {
        if (even_index_red) {
            if (led_index % 2 == 0) {
                leds[led_index] = CRGB::Red;
            } else {
                leds[led_index] = CRGB::Green;
            }
        } else {
            if (led_index % 2 == 0) {
                leds[led_index] = CRGB::Green;
            } else {
                leds[led_index] = CRGB::Red;
            }
        }

    }
    even_index_red = !even_index_red;
}

void show_animation_fading(void)
{
    uint8_t new_brightness = (brightness_increasing) ? ++led_brightness : --led_brightness;
    for (uint8_t led_index = 0; led_index < LED_STRIP_LENGTH; led_index++) {
        if(led_index % 2 == 0) {
            leds[led_index] = CRGB::Red;
        } else {
            leds[led_index] = CRGB::Green;
        }
    } 

    Serial.println(new_brightness);
    FastLED.setBrightness(new_brightness);
    
    if (new_brightness == UINT8_MAX || new_brightness == 0) {
      brightness_increasing = !brightness_increasing;
    }
}

void loop() {
     show_animation_fading();
//     switch(animation) {
//        case ANIMATION_CIRCLING:
////            show_animation_circling();
//            break;
//        case ANIMATION_FADING:
////            show_animation_fading();
//            break;
//        case ANIMATION_BOUNCING:
//
//            break;
//        case ANIMATION_CIRCLING_AND_FADING:
//
//            break;
//    }
    
    FastLED.show(); 
  
    switch(speed_state) {
        case SPEED_SLOW:
            FastLED.delay(500);
            break;
        case SPEED_MEDIUM:
            FastLED.delay(250);
            break;
        case SPEED_FAST:
            FastLED.delay(100);
            break;
        default:
            FastLED.delay(50000);
            break;
    }
}
