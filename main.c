//Libraries required for Sensor and Oled
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === CONFIGURATION ===
#define SENSOR_PIN     12
#define RELAY_PIN      5
#define OLED_ADDRESS   0x3C
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64

#define MAX_SAFE_TEMP  30.0
#define SENSOR_ERROR   -127.0

#define TEMP_INTERVAL  3600000UL // 1 hour
#define MAX_HEAT_TIME  7UL * 60UL * 60UL * 1000UL // 7 hours

// Different modes that user can configure
enum Mode {
  ECO,          //When user want machine to handle heater smartly
  COMFORT,      //When user wants to switch on heater at regular timings
  MANUAL        //When user wants to manually switch on and off heater
};

//Different heater state as mentioned in assignment
enum HeaterState {
  IDLE,
  HEATING,
  STABILIZING,
  OVERHEAT
};

// === GLOBALS VARIABLES ===
Mode currentMode = ECO;
HeaterState currentState = IDLE;

float target_temp = 24.0;
float current_temp = 0.0;
bool heater_on = false;

unsigned long heating_start_time = 0;
unsigned long last_sensor_read = 0;

// === HARDWARE OBJECTS ===
OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void displayMessage(const String& msg);
void updateSensor();
void controlTask();
void sensorHealthCheck();

void setup() {
  Serial.begin(115200);

  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("OLED init failed!"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  displayMessage("Hello! User");

  // Init sensor
  sensors.begin();

  // Init relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // heater OFF
}

// === MAIN LOOP ===
void loop() {
  unsigned long now = millis();

  // Periodic sensor read
  if (now - last_sensor_read >= TEMP_INTERVAL || last_sensor_read == 0) {
    updateSensor();
    last_sensor_read = now;
  }

  sensorHealthCheck();      
  controlTask();

  delay(1000);
}

// Displaying Sensor Readings on OLed and Serial Monitor
//As mentioned in Assignment added Visual Feedback
void updateSensor() {
  sensors.requestTemperatures();
  current_temp = sensors.getTempCByIndex(0);
  Serial.print("[Sensor] Temp: ");
  Serial.println(current_temp);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(current_temp);
  display.println(" C");
  display.display();
}

// To ensure that sensor is working fine 
void sensorHealthCheck() {
  if (current_temp == SENSOR_ERROR || isnan(current_temp)) {
    currentState = IDLE;
    digitalWrite(RELAY_PIN, LOW);
    heater_on = false;
    displayMessage("Sensor Error");
    Serial.println("[ERROR] Sensor reading failed.");
  }
}

// This would work if user wants to automatically handle heater
void controlTask() {
  if (currentMode == ECO) {   //If user selected ECO mode
    switch (currentState) {
      case IDLE:      //initial state
        if (current_temp < 22.0) { // Comfort threshold
          currentState = HEATING;
          heating_start_time = millis();
          Serial.println("[State] Switching to HEATING");
        } else {
          heater_on = false;
          digitalWrite(RELAY_PIN, LOW);
          displayMessage("IDLE");
        }
        break;

      case HEATING:
        heater_on = true;
        digitalWrite(RELAY_PIN, HIGH);
        displayMessage("HEATING");
        if (current_temp >= target_temp - 0.5) {
          currentState = STABILIZING;
          Serial.println("[State] Switching to STABILIZING");
        } else if (current_temp >= MAX_SAFE_TEMP) {
          currentState = OVERHEAT;
          Serial.println("[State] Switching to OVERHEAT");
        }
        break;

      case STABILIZING:
        displayMessage("STABILIZING");
        if ((millis() - heating_start_time) > MAX_HEAT_TIME) {
          currentState = IDLE;
          heater_on = false;
          digitalWrite(RELAY_PIN, LOW);
          Serial.println("[State] Heating duration exceeded. Switching to IDLE.");
        }
        break;

      case OVERHEAT:
        heater_on = false;
        digitalWrite(RELAY_PIN, LOW);
        displayMessage("OVERHEAT");
        if (current_temp < (target_temp - 2.0)) {
          currentState = IDLE;
          Serial.println("[State] Cooling down. Switching to IDLE.");
        }
        break;
    }
  }

}

// === OLED DISPLAY ===
void displayMessage(const String& msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}
