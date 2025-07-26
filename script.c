#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// DHT Sensor setup
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Relay
#define RELAY_PIN 16

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables
float temperature = 0.0;
bool heaterState = false;

enum SystemState { IDLE, HEATING, STABILIZING, TARGET_REACHED, OVERHEAT };
SystemState currentState = IDLE;

// Function to update state
void updateState(float temp) {
  if (temp < 24.0 && !heaterState) {
    currentState = HEATING;
    heaterState = true;
    digitalWrite(RELAY_PIN, HIGH);
  } else if (temp >= 24.0 && temp <= 26.0 && heaterState) {
    currentState = STABILIZING;
  } else if (temp > 26.0 && heaterState) {
    heaterState = false;
    digitalWrite(RELAY_PIN, LOW);
    currentState = TARGET_REACHED;
  }

  if (temp > 30.0) {
    currentState = OVERHEAT;
    heaterState = false;
    digitalWrite(RELAY_PIN, LOW);
  }

  if (!heaterState && temp < 24.0) {
    currentState = HEATING;
    digitalWrite(RELAY_PIN, HIGH);
    heaterState = true;
  }

  if (!heaterState && temp >= 24.0 && temp <= 26.0) {
    currentState = IDLE;
  }
}

// Display task
void displayTask(void *parameter) {
  for (;;) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");

    display.setCursor(0, 16);
    display.print("Heater: ");
    display.println(heaterState ? "ON" : "OFF");

    display.setCursor(0, 32);
    display.print("State: ");
    switch (currentState) {
      case IDLE: display.println("IDLE"); break;
      case HEATING: display.println("HEATING"); break;
      case STABILIZING: display.println("STABILIZING"); break;
      case TARGET_REACHED: display.println("TARGET OK"); break;
      case OVERHEAT: display.println("OVERHEAT!"); break;
    }

    display.display();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Temperature reading and logic
void sensorTask(void *parameter) {
  for (;;) {
    float temp = dht.readTemperature();

    if (!isnan(temp)) {
      temperature = temp;
      updateState(temp);

      // Log to Serial
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" C | Heater: ");
      Serial.print(heaterState ? "ON" : "OFF");
      Serial.print(" | State: ");
      switch (currentState) {
        case IDLE: Serial.println("IDLE"); break;
        case HEATING: Serial.println("HEATING"); break;
        case STABILIZING: Serial.println("STABILIZING"); break;
        case TARGET_REACHED: Serial.println("TARGET REACHED"); break;
        case OVERHEAT: Serial.println("OVERHEAT"); break;
      }
    } else {
      Serial.println("Sensor error");
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // OLED Init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("System Booting...");
  display.display();

  delay(1000);

  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(sensorTask, "Sensor Task", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(displayTask, "Display Task", 2048, NULL, 1, NULL, 1);
}

void loop() {
  // Nothing here; tasks handle everything
}
