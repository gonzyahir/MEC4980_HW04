#include <Arduino.h>
#include "Stepper.h"
#include <WiFi.h>

#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET     -6 * 3600
#define UTC_OFFSET_DST 0

int dirPin = 13;
int stepPin = 12;
int enablePin = 11;

int enableButton = 9;

int minutes;
int hours;

int prevHours = 0;
int prevMinutes = 0;
int prevSteps = 0;

float stepsPerRev = 48.0;
float stepsPerMin = stepsPerRev / 60.0;
float stepsPerHour = stepsPerRev / 12.0;

enum MachineStates {
    disabled,
    enabled
};

MachineStates currentState = disabled;

// Using 255 as dummy minSwitch pin, no max switch
Stepper motor(stepPin, dirPin, 255, 0, enablePin, true);

bool GetLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 5000)) {
    Serial.println("Failed to obtain time");
    return false;
  }
  hours = timeinfo.tm_hour % 12;
  minutes = timeinfo.tm_min;
  return true;
}

void setup() {
    delay(2000);
    Serial.begin(115200);

    WiFi.begin("Gonzalez WiFi", "Jando0573");
        while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

    motor.begin();
    //motor.enable();
    motor.setMoveSpeed(50);
    //motor.setStepPulseWidth(2);

    pinMode(enableButton, INPUT_PULLDOWN); 
}

void loop() {
    if (digitalRead(enableButton)) {
            currentState = (MachineStates)((currentState + 1) % 2);
            delay(250);
        }

    switch (currentState) {
        case disabled:
            motor.disable();
            Serial.println("Motor Disabled");
            delay(50);
        break;
        case enabled:
            motor.enable();
            if (GetLocalTime()) {
                Serial.printf("Time: %02d:%02d\n", hours, minutes);
                int total_minutes = ((hours) * 60 + (minutes));
                int steps = (int)(total_minutes * stepsPerMin);
                Serial.println(steps);
                motor.moveToBlocking(steps);
                while (!digitalRead(enableButton)) {
                    motor.setVelocity(.0133);
                    motor.update();
                }
                currentState = disabled;
            }

                /*
                if ((minutes-prevMinutes) >= 5) {
                    Serial.printf("Time: %02d:%02d\n", hours, minutes);
                    int steps = floor((((float)(hours-prevHours) * 60.0) + (float)(minutes-prevMinutes)) * stepsPerMin);
                    Serial.println(steps);
                    motor.moveToBlocking(steps);
                    prevMinutes = minutes;
                    prevHours = hours;
                }
                */
            delay(250);
        break;
    }
}