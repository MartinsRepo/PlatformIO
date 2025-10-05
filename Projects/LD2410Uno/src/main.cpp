/*
   HLK-LD2410C Radar-Sensor mit Arduino Uno
   Kommunikation über SoftwareSerial @ 115200 Baud
   Ausgabe der Sensordaten im Serial Monitor
   Library: MyLD2410
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "MyLD2410.h"

#define RX_PIN 2
#define TX_PIN 3

// SoftwareSerial anlegen
SoftwareSerial sensorSerial(RX_PIN, TX_PIN);  // RX, TX

// Konfiguration
#define SERIAL_BAUD_RATE 115200   // PC Serial Monitor
#define LD2410_BAUD_RATE 19200   // Sensor auf 115200 konfigurieren
#define ENHANCED_MODE             // Erweiterte Daten aktivieren

// Objekt für Sensor
#ifdef DEBUG_MODE
MyLD2410 sensor(sensorSerial, true);
#else
MyLD2410 sensor(sensorSerial);
#endif

unsigned long nextPrint = 0, printEvery = 100;  // jede Sekunde ausgeben

// -------------------------
// Pin-Belegung für Uno
// -------------------------
// Arduino D2 (RX)  <--  TX vom Sensor
// Arduino D3 (TX)  -->  RX vom Sensor
// Arduino GND      --   GND vom Sensor
// Arduino 5V (oder externe 5–12V, ca. 200mA!) --> Vcc Sensor

void printValue(const byte &val) {
  Serial.print(' ');
  Serial.print(val);
}

void printData() {
  Serial.print(sensor.statusString());
  if (sensor.presenceDetected()) {
    Serial.print(", distance: ");
    Serial.print(sensor.detectedDistance());
    Serial.print("cm");
  }
  Serial.println();

  if (sensor.movingTargetDetected()) {
    Serial.print(" MOVING    = ");
    Serial.print(sensor.movingTargetSignal());
    Serial.print("@");
    Serial.print(sensor.movingTargetDistance());
    Serial.print("cm ");
    if (sensor.inEnhancedMode()) {
      Serial.print("\n signals->[");
      sensor.getMovingSignals().forEach(printValue);
      Serial.print(" ] thresholds:[");
      sensor.getMovingThresholds().forEach(printValue);
      Serial.print(" ]");
    }
    Serial.println();
  }

  if (sensor.stationaryTargetDetected()) {
    Serial.print(" STATIONARY= ");
    Serial.print(sensor.stationaryTargetSignal());
    Serial.print("@");
    Serial.print(sensor.stationaryTargetDistance());
    Serial.print("cm ");
    if (sensor.inEnhancedMode()) {
      Serial.print("\n signals->[");
      sensor.getStationarySignals().forEach(printValue);
      Serial.print(" ] thresholds:[");
      sensor.getStationaryThresholds().forEach(printValue);
      Serial.print(" ]");
    }
    Serial.println();
  }

  if (sensor.inEnhancedMode() && (sensor.getFirmwareMajor() > 1)) { 
    Serial.print("Light level: ");
    Serial.println(sensor.getLightLevel());
    Serial.print("Output level: ");
    Serial.println((sensor.getOutLevel()) ? "HIGH" : "LOW");
  }

  Serial.println();
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);        // PC Verbindung
  sensorSerial.begin(LD2410_BAUD_RATE);  // Verbindung zum Radar-Sensor
  delay(2000);

  Serial.println(__FILE__);

  if (!sensor.begin()) {
    Serial.println("⚠️ Sensor evtl. noch nicht sofort bereit, versuche weiter...");
  }

#ifdef ENHANCED_MODE
  sensor.enhancedMode();
#else
  sensor.enhancedMode(false);
#endif
}

void loop() {
  //printData();
  //delay(500);
  while (sensor.check() != MyLD2410::Response::DATA) {
    Serial.print("*");
    delay(100);
  }

  //if ((sensor.check() == MyLD2410::Response::DATA) && (millis() > nextPrint)) {
  if (millis() > nextPrint) {
    nextPrint = millis() + printEvery;
    printData();
  }
}

