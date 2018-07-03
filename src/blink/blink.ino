/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.
*/

#define LOLIN_LED_PIN 2

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LOLIN_LED_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LOLIN_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                         // wait for a second
  digitalWrite(LOLIN_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                         // wait for a second
}
