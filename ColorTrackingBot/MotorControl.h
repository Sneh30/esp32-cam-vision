#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

// ======== MOTOR PIN ASSIGNMENT (MATCHING YOUR ESP32-CAM BOARD) ========
// IN1 = GPIO 12
// IN2 = GPIO 13
// IN3 = GPIO 14
// IN4 = GPIO 15

const int IN1 = 12;
const int IN2 = 13;
const int IN3 = 14;
const int IN4 = 15;

// ======== MOTOR STOP ========
void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ======== MOTOR INITIALIZATION ========
void motorInit() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();  // <-- now valid because stopMotors() is above
}

// ======== MOVE FORWARD ========
void moveForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// ======== TURN LEFT ========
void turnLeft(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// ======== TURN RIGHT ========
void turnRight(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

#endif
