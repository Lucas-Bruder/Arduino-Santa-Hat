/*
    Author: Lucas Bruder <LBruder@me.com>
    Date created: 12/6/2016

    Arduino Based Santa Hat!
*/
#include <FastLED.h>
#include <EEPROM.h>

#define SERIAL_BAUD_RATE (115200)

#define DEFAULT_BRIGHTNESS (255U)

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

  ANIMATION_MAX
};

/*
 * Speed at which the santa hat will display the animations
 */
enum speed_state_t {
  SPEED_SLOW = 0,
  SPEED_MEDIUM,
  SPEED_FAST,

  SPEED_COUNT
};

CRGB leds[LED_STRIP_LENGTH];
enum animation_state_t animation;
enum speed_state_t speed_state;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Starting santa hat");
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_STRIP_LENGTH);

  // load state and speed from EEPROM
  enum animation_state_t last_animation_state = (enum animation_state_t)EEPROM.read(EEPROM_ANIMATION_STATE_ADDR);
  enum speed_state_t last_speed_state =  (enum speed_state_t)(EEPROM.read(EEPROM_SPEED_STATE_ADDR) + 1);

  EEPROM.write(EEPROM_ANIMATION_STATE_ADDR, last_animation_state);
  EEPROM.write(EEPROM_SPEED_STATE_ADDR, last_speed_state);

  animation = last_animation_state;
  speed_state = last_speed_state;

  Serial.println(animation);
  Serial.println(speed_state);
}

void loop() {
  for (uint8_t led_index = 0; led_index < LED_STRIP_LENGTH; led_index++) {
      if (led_index % 2 == 0) {
        leds[led_index] = CRGB::Green; 
      } else {
        leds[led_index] = CRGB::Red; 
      }
  }
  FastLED.show(); 
}
