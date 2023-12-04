#ifndef FeedMeasure_h
#define FeedMeasure_h

#include <Arduino.h>
#include "Adafruit_VL53L0X.h"
#include <Wire.h>

class FeedMeasure {
protected:
  double percentValue;
  int value;
  int inputLow;
  int inputHigh;
  unsigned long previousMillisMeasure;
  unsigned long measureDelayTime;
  bool state;
  bool restart;
  int deviceAddress;
  Adafruit_VL53L0X lox = Adafruit_VL53L0X();

public:
  FeedMeasure();
  void measure();
  void iterate();
  bool isOn();
  void measureStart();
  void measureStop();
  void setMeasureDelayTime(unsigned long val);
  int getValue();
  double getPercentValue();
  void setInputLow(int val);
  void setInputHigh(int val);
  void scannerI2C();
  void setDeviceAddress(int val);
};

#endif
