#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <WiFiNINA.h>

bool is_failed = false;

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;

void setup() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);

  Serial.begin(9600);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    is_failed = true;
    return;
  }

  filter.begin(25);
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
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, heading;
  unsigned long microsNow;

  if (!IMU.gyroscopeAvailable() || !IMU.accelerationAvailable()) {
    return;
  }
  IMU.readGyroscope(gx, gy, gz);
  IMU.readAcceleration(ax, ay, az);

  // update the filter, which computes orientation
  filter.updateIMU(gx, gy, gz, ax, ay, az);

  pitch = filter.getPitch();
  if (abs(pitch) > 80) {
    digitalWrite(LEDR, HIGH);
  } else {
    digitalWrite(LEDR, LOW);
  }
  Serial.print("Pitch: ");
  Serial.println(pitch);
}
