#include "FeedMeasure.h"

FeedMeasure::FeedMeasure() 
  : previousMillisMeasure(0), deviceAddress(0x29), state(false), inputLow(0), inputHigh(100), measureDelayTime(0), restart(true), value(0), percentValue(-1) {
}

void FeedMeasure::iterate() {
  if (isOn()) {
    if(restart){
      previousMillisMeasure = millis();
      restart=false;
    }
    unsigned long currentMillisMeasure = millis();
    if (currentMillisMeasure - previousMillisMeasure >= measureDelayTime) {
      measure();
      restart=true;
    }
  }
}

void FeedMeasure::measure(){
  lox.begin(deviceAddress);
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    value=measure.RangeMilliMeter;
    Serial.print("Measure data: ");
    Serial.println(value);
    if(value < inputHigh && value > inputLow){
      percentValue = map(value, inputLow, inputHigh, 100, 0);
      Serial.print("Measure [%]: ");
      Serial.println(percentValue);
      sensorPercent->setValue(percentValue);
      if (percentValue < 10) {
        empty->set();
      }else empty->clear();
    }
  } else {
    Serial.println("Incorrect data");
  }
}

bool FeedMeasure::isOn() {
  return state;
}

void FeedMeasure::measureStart(Supla::Sensor::VirtualHygroMeter *vs, Supla::Sensor::VirtualBinary *bs) {
  sensorPercent = vs;
  empty = bs;
  state=true;
  Serial.println("Feed measure: Start");
}

void FeedMeasure::measureStop() {
  state=false;
  Serial.println("Feed measure: Stop");
}

void FeedMeasure::setMeasureDelayTime(unsigned long val) {
  measureDelayTime=val;
  Serial.print("Feed measure delay time channed to: ");
  Serial.print(measureDelayTime);
  Serial.println(" ms");
}

int FeedMeasure::getValue(){
  return value;
}

double FeedMeasure::getPercentValue(){
  return percentValue;
}

void FeedMeasure::setInputLow(int val){
  inputLow=val;
}

void FeedMeasure::setInputHigh(int val){
  inputHigh=val;
}

void FeedMeasure::scannerI2C(){
  Wire.begin();
  byte error, address;
  int nDevices;

  Serial.println("\nI2C Scanner");
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void FeedMeasure::setDeviceAddress(int val){
  deviceAddress = val;
  Serial.print("Feed measure I2C address channed to: ");
  Serial.println(deviceAddress);
}
