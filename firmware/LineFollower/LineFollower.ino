#include <Arduino.h>
#include <QTRSensors.h>
#include "Control.h"

// Preserve the pin assignment from the original uploaded firmware.
constexpr uint8_t kLeftForward = 5;
constexpr uint8_t kLeftReverse = 6;
constexpr uint8_t kRightForward = 9;
constexpr uint8_t kRightReverse = 10;
constexpr uint8_t kSensorCount = 5;
const uint8_t kSensorPins[kSensorCount] = {A1, A2, A3, A4, A5};
constexpr uint16_t kCalibrationSamples = 400;

QTRSensors qtr;
LineFollower::Controller controller;
uint16_t sensorValues[kSensorCount];

void driveMotor(uint8_t forward, uint8_t reverse, int speed) {
    // Deactivate the opposite direction before applying PWM.
    if (speed >= 0) {
        analogWrite(reverse, 0);
        analogWrite(forward, speed);
    } else {
        analogWrite(forward, 0);
        analogWrite(reverse, -speed);
    }
}

void setup() {
    pinMode(kLeftForward, OUTPUT);
    pinMode(kLeftReverse, OUTPUT);
    pinMode(kRightForward, OUTPUT);
    pinMode(kRightReverse, OUTPUT);
    driveMotor(kLeftForward, kLeftReverse, 0);
    driveMotor(kRightForward, kRightReverse, 0);
    qtr.setTypeAnalog();
    qtr.setSensorPins(kSensorPins, kSensorCount);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    // Manually sweep every sensor over both the line and the background.
    for (uint16_t i = 0; i < kCalibrationSamples; ++i) qtr.calibrate();
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}

void loop() {
    const uint16_t position = qtr.readLineBlack(sensorValues);
    bool onLine = false;
    bool allBlack = true;
    for (uint8_t i = 0; i < kSensorCount; ++i) {
        const bool black = sensorValues[i] > LineFollower::kBlackThreshold;
        onLine |= black;
        allBlack &= black;
    }
    const auto command = controller.update(position, onLine, allBlack, millis());
    driveMotor(kLeftForward, kLeftReverse, command.left);
    driveMotor(kRightForward, kRightReverse, command.right);
}
