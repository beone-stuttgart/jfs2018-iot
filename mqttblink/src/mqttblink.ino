#include <Homie.h>

#define LOLIN_LED_PIN 2
#define BLINK_DELAY 100

HomieNode lightNode("led", "switch");

bool ledIsOn = false;
int blinkCount = 0;
unsigned long blinkStart = 0;

void setLedState(bool on) {
  if (ledIsOn!=on) {
    digitalWrite(LOLIN_LED_PIN, on ? LOW : HIGH);
    ledIsOn = on;
    lightNode.setProperty("on").send(on ? "true" : "false");
    Homie.getLogger() << "Light is " << (on ? "on" : "off") << endl;
  }
}

bool ledOnHandler(const HomieRange& range, const String& value) {
  if (value != "true" && value != "false") return false;
  setLedState((value == "true"));
  return true;
}

bool ledBlinkHandler(const HomieRange& range, const String& value) {
  int count = value.toInt();
  if (count<=0 || count>10) {
    count=3;
  }
  Homie.getLogger() << "Blinking " << count << " times" << endl;
  blinkCount = count;
  blinkStart = millis();
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  pinMode(LOLIN_LED_PIN, OUTPUT);
  digitalWrite(LOLIN_LED_PIN, HIGH);

  Homie_setFirmware("mqtt-blink", "1.0.0");

  lightNode.advertise("on").settable(ledOnHandler);
  lightNode.advertise("blink").settable(ledBlinkHandler);

  Homie.setup();
}

void loop() {
  Homie.loop();
  if (blinkCount>0) {
    // Don't use delay since it would block all other operations
    unsigned long currentMillis = millis();
    if (currentMillis-blinkStart < BLINK_DELAY) {
      setLedState(true);
    } else if (currentMillis-blinkStart < 2*BLINK_DELAY) {
      setLedState(false);
    } else {
      blinkStart = currentMillis;
      blinkCount--;
    }
  }
}
