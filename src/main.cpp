#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Control_Surface.h>

// Select the serial port to use.
auto &serial = Serial;
// Instantiate a Serial MIDI interface at the default MIDI baud rate.
SerialMIDI_Interface<decltype(serial)> midi {serial, MIDI_BAUD};
//SerialDebugMIDI_Interface<decltype(serial)> midi {serial, 115200};

CCPotentiometer potentiometer { A2, MIDI_CC::Channel_Volume };

// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 1
Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);

typedef enum {
    MODE_MASTER,
    MODE_GAIN,
    MODE_POWER
} t_mode;

t_mode mode = MODE_MASTER;

#include "button.h"

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, 0xFF);

    buttons_setup();

    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

    Control_Surface.begin();
}



uint8_t pot_prev_val = 0;
void loop() {
    Control_Surface.loop();

    buttons_loop();

    uint8_t val = potentiometer.getValue();
    if (val != pot_prev_val) {
        
        strip.setPixelColor(0, getColor(2*val));
        strip.show();
        pot_prev_val = val;
    }
}