#ifndef StepperMotor_h
#define StepperMotor_h

#include <Arduino.h>

class StepperMotor {
protected:
  int pin1, pin2, pin3, pin4;
  unsigned long previousMillisStepper;
  bool spin;
  int speed; // prędkość w ms (2-najszybszy, 8-najwolniejszy)
  int direction;
  unsigned long spinTime;
  bool restart;

public:
  StepperMotor(int p1, int p2, int p3, int p4);
  
  void iterate();
  bool isRolling();
  void setDirection(int d);
  void roll();
  void rightRoll();
  void leftRoll();
  void stopSpin();
  void setSpinTime(unsigned long sT);
  void setSpeed(int newSpeed);
  void startSpin();
  unsigned long getSpinTime();
  int getSpeed();
};

#endif
