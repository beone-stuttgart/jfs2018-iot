#include <Arduino.h>
#include <Homie.h>

#define BLINK_DELAY 667


class LightStatus {
  public:
    HomieNode* node;
    const char* name;
    int pin;
    bool ledIsOn = false;
    int blinkCount = 0;
    unsigned long blinkStart = 0;

    LightStatus(const char* theName, int thePin): name(theName), pin(thePin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
    }

    void setLedState(bool on) {
      if (ledIsOn!=on) {
        digitalWrite(pin, on ? LOW : HIGH);
        ledIsOn = on;
        if (node!=NULL) {
          node->setProperty("on").send(on ? "true" : "false");
        }
        Homie.getLogger() << "Light " << name << " is " << (on ? "on" : "off") << endl;
      }
    }

    void startBlink(int count) {
      Homie.getLogger() << "Blinking " << name << " " << count << " times" << endl;
      blinkCount = count;
      blinkStart = millis();
    }

    void checkBlink() {
      if (blinkCount>0) {
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
};


class RelaisNode: public HomieNode {
  private:
    LightStatus& status;

  public:
    RelaisNode(LightStatus& theStatus): HomieNode(theStatus.name, "switch"), status(theStatus) {
      theStatus.node = this;
      advertise("on").settable(NULL);
      advertise("blink").settable(NULL);
    }

  protected:
    bool ledOnHandler(const String& value) {
      if (value != "true" && value != "false") return false;
      status.setLedState((value == "true"));
      return true;
    }

    bool ledBlinkHandler(const String& value) {
      int count = value.toInt();
      if (count<=0 || count>10) {
        count=3;
      }
      status.startBlink(count);
      return true;
    }

    virtual bool handleInput(const String& property, const HomieRange& range, const String& value) {
      if (property=="on") {
        return ledOnHandler(value);
      } else if (property=="blink") {
        return ledBlinkHandler(value);
      } else {
        Serial << "Unknown property " << property << endl;
        return false;
      }
    }
};


LightStatus rot("rot", D0);
LightStatus gelb("gelb", D1);
LightStatus gruen("gruen", D2);
RelaisNode relaisRot(rot);
RelaisNode relaisGelb(gelb);
RelaisNode relaisGruen(gruen);


void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  Homie_setFirmware("mqtt-ampel", "1.0.0");
  Homie.setup();
}


void loop() {
  Homie.loop();
  rot.checkBlink();
  gelb.checkBlink();
  gruen.checkBlink();
}