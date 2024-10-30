#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <WiFiNINA.h>
#include <PDM.h>

#define BLINK_INTERVAL 1000 // the inver
#define BLINK_LENGTH 10000
#define MAX_VOLUME 1000
#define MAX_PITCH 80

Madgwick imuFilter; // Umbenannter imuFilter für IMU-Daten
// PDM variables for microphone
static const char channels = 1;
static const int frequency = 16000;
short sampleBuffer[512];
volatile int samplesRead;

// Callback function to process the data from the PDM microphone
void onPDMdata()
{
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2; // 2 bytes per sample
}

class LED
{
  bool isEnable = false;
  NinaPin pin;

public:
  LED(NinaPin _pin) : pin(_pin)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  void enable()
  {
    isEnable = true;
    digitalWrite(pin, HIGH);
  }

  void disable()
  {
    isEnable = false;
    digitalWrite(pin, LOW);
  }

  void toggle()
  {
    isEnable = !isEnable;
    digitalWrite(pin, isEnable ? HIGH : LOW);
  }
};

class Blink
{
private:
  LED *led;
  long toggleTime = 0;
  long startTime = 0;
  bool isEnabled = false;

public:
  Blink(LED *_led) : led(_led) {}

  void update()
  {
    if (!isEnabled)
      return;
    long currentTime = millis();
    if (startTime + BLINK_LENGTH <= currentTime)
    {
      stop();
    }
    else if (toggleTime + BLINK_INTERVAL <= currentTime)
    {
      toggleTime = currentTime;
      led->toggle();
    }
  }

  void start()
  {
    if (!isEnabled) {
      isEnabled = true;
      toggleTime = millis();
    }
    startTime = millis();
  }

  void stop()
  {
    isEnabled = false;
    led->disable();
  }

  bool isBlinking()
  {
    return isEnabled;
  }
};

Blink *pitchStatus;
Blink *volumeStatus;
LED *green;

void setup()
{
  Serial.begin(9600);

  LED *red = new LED(LEDR);
  green = new LED(LEDG);
  LED *blue = new LED(LEDB);

  pitchStatus = new Blink(red);
  volumeStatus = new Blink(blue);

  // Start the IMU
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }
  imuFilter.begin(25); // Initialisierung des Madgwick-imuFilters

  // Initialize the PDM microphone
  if (!PDM.begin(channels, frequency))
  {
    Serial.println("Failed to start PDM!");
    while (1)
      ;
  }

  // Set PDM data receive callback
  PDM.onReceive(onPDMdata);
}

class DebugPrinter
{
private:
  float pitch;
  float volume;

public:
  DebugPrinter()
  {
    pitch = 0;
    volume = 0;
  }

  void setPitch(float _pitch)
  {
    pitch = _pitch;
  }

  void setVolume(float _volume)
  {
    volume = _volume;
  }

  void print()
  {
    Serial.print("Pitch: ");
    Serial.print(pitch);
    Serial.print(" Volume: ");
    Serial.println(volume);
  }
};

DebugPrinter *debugPrinter = new DebugPrinter();

void handlePitchStatus()
{
  // --- Neigungsüberwachung (Pitch) ---
  float ax, ay, az;
  float gx, gy, gz;
  float pitch;

  if (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable())
    return;

  if (!IMU.readAcceleration(ax, ay, az) || !IMU.readGyroscope(gx, gy, gz))
    return;

  imuFilter.updateIMU(gx, gy, gz, ax, ay, az);
  pitch = imuFilter.getPitch(); // Berechnung des Pitch-Winkels
  debugPrinter->setPitch(pitch);

  // Pitch überwachen und Red LED toggeln
  if (abs(pitch) > MAX_PITCH)
  {
    pitchStatus->start();
  }
}

void handleVolumeStatus()
{
  // --- Audioüberwachung (PDM Mikrofon) ---
  int maxVolume = 0; // Variable zur Speicherung des maximalen Lautstärkepegels
  if (!samplesRead)
    return;
  for (int i = 0; i < samplesRead; i++)
  {
    // Ermittlung des maximalen Lautstärkepegels im PDM-Buffer
    if (abs(sampleBuffer[i]) > maxVolume)
    {
      maxVolume = abs(sampleBuffer[i]);
    }
  }

  debugPrinter->setVolume(0);
  // LED basierend auf Lautstärke umschalten
  if (maxVolume > MAX_VOLUME)
  {
    debugPrinter->setVolume(maxVolume);
    volumeStatus->start();
  }
}

void loop()
{
  handlePitchStatus();
  handleVolumeStatus();

  pitchStatus->update();
  volumeStatus->update();

  if (pitchStatus->isBlinking() || volumeStatus->isBlinking())
  {
    green->disable();
  }
  else {
    green->enable();
  }

  debugPrinter->print();
}
