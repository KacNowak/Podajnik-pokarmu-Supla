#include "StepperMotor.h"

StepperMotor::StepperMotor(int p1, int p2, int p3, int p4) 
  : pin1(p1), pin2(p2), pin3(p3), pin4(p4), previousMillisStepper(0), spin(false), spinTime(0), speed(5), restart(true) {
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(pin4, OUTPUT);
}

void StepperMotor::iterate() {
  if (spin) {
    if(restart){
      previousMillisStepper = millis();
      restart=false;
    }
    roll();
    unsigned long currentMillisStepper = millis();
    if (currentMillisStepper - previousMillisStepper >= spinTime) {
      restart=true;
      stopSpin();
    }
  }
}

bool StepperMotor::isRolling() {
  return spin;
}

void StepperMotor::setDirection(int d) {
  direction = d;
  if(d) Serial.println("Stepper direction channed to: right");
  else Serial.println("Stepper direction channed to: left");
}

void StepperMotor::roll() {
  if (direction) rightRoll();
  else leftRoll();
}

void StepperMotor::rightRoll() {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(speed);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(speed);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  delay(speed);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  delay(speed);
}

void StepperMotor::leftRoll() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  delay(speed);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  delay(speed);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(speed);
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(speed);
}

void StepperMotor::stopSpin() {
  spin = false;
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  Serial.print("Stepper value channed to: ");
  Serial.println(isRolling());
}

void StepperMotor::setSpinTime(unsigned long sT) {
  spinTime = sT;
  Serial.print("Stepper spin time channed to: ");
  Serial.print(spinTime);
  Serial.println(" ms");
}

void StepperMotor::setSpeed(int newSpeed) {
  speed = newSpeed;
  Serial.print("Stepper speed (2-8) channed to: ");
  Serial.println(speed);
}

void StepperMotor::startSpin() {
  spin = true;
  Serial.print("Stepper value channed to: ");
  Serial.println(isRolling());
}
