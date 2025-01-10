#define BUTTON_PIN 2
#define LED_PIN 13

int oldButtonState;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN , INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int buttonState = !digitalRead(BUTTON_PIN);
  
  if(buttonState != oldButtonState) {
    oldButtonState = buttonState;
    Serial.println(buttonState);
  }


  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      digitalWrite(LED_PIN, HIGH);
    } else if (command == '0') {
      digitalWrite(LED_PIN, LOW);
    }
  }

  delay(100);
}
