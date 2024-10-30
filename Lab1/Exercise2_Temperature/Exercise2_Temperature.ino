#include <Arduino_LSM6DSOX.h>
#include <WiFiNINA.h>

bool is_failed = false;

void setup() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    is_failed = true;
  }
}

void loop() {
  if (is_failed) {
    Serial.println("Failed to initialize IMU!");
    digitalWrite(LEDR, HIGH);
    delay(50);
    digitalWrite(LEDR, LOW);
    delay(50);
    return;
  }
  float x, y, z;

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
  }

  if (IMU.temperatureAvailable())
  {
    int temperature_deg = 0;
    IMU.readTemperature(temperature_deg);
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, LOW);

    if (temperature_deg < 26) {
      digitalWrite(LEDB, HIGH);
    }
    if (19 < temperature_deg && temperature_deg < 36) {
      digitalWrite(LEDG, HIGH);
    }
    if (32 < temperature_deg) {
      digitalWrite(LEDR, HIGH);
    }

    Serial.print("LSM6DSOX Temperature = ");
    Serial.print(temperature_deg);
    Serial.println(" °C");
  }
}
