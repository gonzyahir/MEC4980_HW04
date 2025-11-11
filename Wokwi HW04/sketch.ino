#include <WiFi.h>
#include "time.h"

#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET     -6 * 3600
#define UTC_OFFSET_DST 0

int hours;
int minutes;
int steps;

float stepsPerHour = 16.666;
float stepsPerMinutes = 3.333;

int hourMotorEnablePin = 4;
int hourMotorStepPin = 6;   // changed from 6
int hourMotorDirPin = 15;

int minuteMotorEnablePin = 5;
int minuteMotorStepPin = 7; // changed from 7
int minuteMotorDirPin = 16;

int prevHours = 0;
int prevMinutes = 0;

bool GetLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 5000)) {
    Serial.println("Failed to obtain time");
    return false;
  }
  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  return true;
}

void moveMotor(int stepPin, int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delay(10);
    digitalWrite(stepPin, LOW);
    delay(10);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(hourMotorEnablePin, OUTPUT);
  pinMode(hourMotorStepPin, OUTPUT);
  pinMode(hourMotorDirPin, OUTPUT);

  pinMode(minuteMotorEnablePin, OUTPUT);
  pinMode(minuteMotorStepPin, OUTPUT);
  pinMode(minuteMotorDirPin, OUTPUT);

  digitalWrite(hourMotorEnablePin, LOW);
  digitalWrite(hourMotorDirPin, HIGH);
  digitalWrite(minuteMotorEnablePin, LOW);
  digitalWrite(minuteMotorDirPin, HIGH);

  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  if (GetLocalTime()) {
    if (hours != prevHours || minutes != prevMinutes) {
      Serial.printf("Time: %02d:%02d\n", hours, minutes);

      int hours_steps = floor((hours-prevHours) * stepsPerHour);
      int minutes_steps = floor((minutes-prevMinutes) * stepsPerMinutes);

      moveMotor(hourMotorStepPin, hours_steps);
      moveMotor(minuteMotorStepPin, minutes_steps);

      prevHours = hours;
      prevMinutes = minutes;
    }
  }
  delay(1000);
}
