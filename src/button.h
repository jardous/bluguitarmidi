#include <AceButton.h>
//#include <ButtonConfig.h>
//#include <AdjustableButtonConfig.h>
using namespace ace_button;

// The pin number attached to the mode switching button.
const int  BUTTON_PIN = 2;


// One button wired to the pin at BUTTON_PIN. Automatically uses the default
// ButtonConfig. The alternative is to call the AceButton::init() method in
// setup() below.
AceButton button(BUTTON_PIN);


uint32_t getColor(uint8_t brightness) {

    uint32_t colorOut;
    uint8_t fill = 0;

    if (!potentiometer.isEnabled()) {
      fill = 0x66;
    }
    
    switch (mode) {
      case MODE_MASTER:
        colorOut = strip.Color(brightness, fill, fill);
        break;
      case MODE_GAIN:
        colorOut = strip.Color(fill, brightness, fill);
        break;
      case MODE_POWER:
        colorOut = strip.Color(fill, fill, brightness);
        break;
    }

    return colorOut;
}


void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  
  uint8_t brightness = 0xFF;

  switch (eventType) {
    //case AceButton::kEventPressed:
    
    case AceButton::kEventClicked:
      Serial.println("Button press");
      if (potentiometer.isEnabled()) {
        Serial.println("  - disable the pot");
        potentiometer.disable();
        Control_Surface.sendCC(potentiometer.getAddress(), 0x7f);  // send max (127)
        brightness = 0xFF;
        strip.setPixelColor(0, getColor(brightness));
        strip.show();
      } else {
        Serial.println("  - enable the pot");
        potentiometer.enable();
        potentiometer.forcedUpdate();
        brightness = 2*potentiometer.getValue();
        strip.setPixelColor(0, getColor(brightness));
        strip.show();
      }
      break;

    case AceButton::kEventLongPressed:
      Serial.println("Long press");
      switch (mode) {
        case MODE_POWER:
          Serial.println("  -- power -> master");
          mode = MODE_MASTER;
          potentiometer.setAddress(MIDI_CC::Channel_Volume);
          break;
        case MODE_MASTER:
          Serial.println("  -- master -> gain");
          mode = MODE_GAIN;
          potentiometer.setAddress(20);
          break;
        case MODE_GAIN:
          Serial.println("  -- gain -> power");
          mode = MODE_POWER;
          potentiometer.setAddress(30);
          break;
      }

      brightness = 0xFF;
      if (potentiometer.isEnabled()) {
        potentiometer.forcedUpdate();
        brightness = 2*potentiometer.getValue();
      } else {
        Control_Surface.sendCC(potentiometer.getAddress(), 0x7f);  // send max (127)
      }

      strip.setPixelColor(0, getColor(brightness));
      strip.show();

      break;
  }

}



void buttons_setup() {

  // Button uses the built-in pull up register.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Configure the ButtonConfig with the event handler, and enable all higher
  // level events.
  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setClickDelay(600);
  buttonConfig->setDoubleClickDelay(2000);
  
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterRepeatPress);
}

void buttons_loop() {
  button.check();
}