#include <QTRSensors.h>

// --- Motor Pin Definitions ---
#define AIN1 5
#define AIN2 6
#define BIN1 9
#define BIN2 10

// --- Speed Configuration ---
int MAX_SPEED = 210;  // PWM Limit (0-255)
int BASE_SPEED = 190; // Nominal cruising speed

QTRSensors qtr;

const uint8_t SensorCount = 5;
uint16_t sensorValues[SensorCount];
int threshold[SensorCount];

// --- PID Control Parameters ---
float Kp = 3.63; // Proportional: Corrects current error
float Ki = 0.0;  // Integral: Eliminates steady-state error (currently disabled)
float Kd = 6.77; // Derivative: Dampens oscillations by reacting to rate of change

uint8_t multiP = 1, multiI = 1, multiD = 1;
float Pvalue, Ivalue, Dvalue;

uint16_t position;
int P, D, I, previousError, PIDvalue, error;
int lsp, rsp;
uint16_t lastPosition = 0;
bool onLine = true;

void setup() {
  // Initialize Motor Control Pins
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  // Configure QTR Sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A1, A2, A3, A4, A5}, SensorCount);

  // Indicate calibration phase with built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // --- Calibration Loop ---
  // Rotate the robot over the line during this phase
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW); // Calibration finished

  // Calculate dynamic threshold values for line detection
  for (uint8_t i = 0; i < SensorCount; i++) {
    threshold[i] = (qtr.calibrationOn.minimum[i] + qtr.calibrationOn.maximum[i]) / 2;
  }

  delay(1000);
}

void loop() {
  robot_control();
}

/**
 * Main control logic: calculates track position and updates PID loop
 */
void robot_control() {
  position = readLine();
  error = 2000 - position; // Setpoint is 2000 (center of 5 sensors)
  PID_Linefollow(error);
}

/**
 * Reads the current line position and handles "Out-of-Bounds" scenarios
 * Returns a value from 0 to 4000
 */
uint16_t readLine() {
  uint16_t position = qtr.readLineBlack(sensorValues);
  onLine = false;
  bool allOnBlack = true;
  
  // Verify if the robot is still over the line
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] > threshold[i]) {
      onLine = true;
    } else {
      allOnBlack = false;
    }
  }

  // Recovery logic: if line is lost, assume it is at the last known extreme
  if (!onLine) {
    if (lastPosition == (SensorCount - 1) * 1000 / 2)
      return lastPosition;
    
    if (lastPosition < (SensorCount - 1) * 1000 / 2) {
      return 0; // Lost to the left
    } else {
      return (SensorCount - 1) * 1000; // Lost to the right
    }
  }

  // If all sensors see black (e.g., intersection), maintain current trajectory
  if (allOnBlack) {
    return lastPosition;
  }

  lastPosition = position;
  return position;
}

/**
 * Calculates the PID output and adjusts motor speeds
 */
void PID_Linefollow(int error) {
  P = error;
  I += error;
  D = error - previousError;

  // Scaling PID values
  Pvalue = (Kp / pow(10, multiP)) * P;
  Ivalue = (Ki / pow(10, multiI)) * I;
  Dvalue = (Kd / pow(10, multiD)) * D;

  PIDvalue = Pvalue + Ivalue + Dvalue;
  previousError = error;

  // Differential drive speed calculation
  lsp = BASE_SPEED - PIDvalue;
  rsp = BASE_SPEED + PIDvalue;

  // Fail-safe: move straight if line status is ambiguous
  if (!onLine) {
    lsp = BASE_SPEED;
    rsp = BASE_SPEED;
  }

  motor_drive(lsp, rsp);
}

/**
 * Direct motor control interface with speed constraint
 */
void motor_drive(int left, int right) {
  // Constrain output to safe PWM limits
  left = constrain(left, -MAX_SPEED, MAX_SPEED);
  right = constrain(right, -MAX_SPEED, MAX_SPEED);

  // Left Motor Control
  analogWrite(AIN1, left > 0 ? left : 0);
  analogWrite(AIN2, left < 0 ? -left : 0);

  // Right Motor Control
  analogWrite(BIN1, right > 0 ? right : 0);
  analogWrite(BIN2, right < 0 ? -right : 0);
}
