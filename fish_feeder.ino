#define STATUS_LED_GPIO 2
#define BUTTON_CFG_GPIO 0

// ULN2003 Motor Driver Pins
#define Pin1 15
#define Pin2 13
#define Pin3 12
#define Pin4 14

#include <SuplaDevice.h>
#include <supla/network/esp_wifi.h>
#include <supla/control/virtual_relay.h>
#include <supla/device/status_led.h>
#include <supla/storage/littlefs_config.h>
#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/status_led_parameters.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/device/supla_ca_cert.h>
//#include <supla/events.h>
#include <supla/storage/eeprom.h>
#include <supla/sensor/virtual_binary.h>
#include <supla/sensor/virtual_hygromometer.h>
#include <supla/storage/config.h>
#include <supla/control/button.h>
#include <EEPROM.h> //ZAPISYWAĆ TYLKO WOLNO OD 50 ADRESU!!!

#include "StepperMotor.h"
#include "FeedMeasure.h"
#include "WebServer.h"
#include "Button.h"

Supla::Eeprom eeprom;

Supla::ESPWifi wifi;
Supla::LittleFsConfig configSupla;

Supla::Device::StatusLed statusLed(STATUS_LED_GPIO, true); // inverted state
//Supla::EspWebServer suplaServer;

Supla::Html::DeviceInfo htmlDeviceInfo(&SuplaDevice);
Supla::Html::WifiParameters htmlWifi;
Supla::Html::ProtocolParameters htmlProto;
Supla::Html::StatusLedParameters htmlStatusLed;

Supla::Control::VirtualRelay *virtualRelay = nullptr;
Supla::Sensor::VirtualBinary *virtualSensor = nullptr;
Supla::Sensor::VirtualHygroMeter *virtualSensor2 = nullptr;

StepperMotor stepper(Pin1, Pin2, Pin3, Pin4);
FeedMeasure feed;
WebServer *server = nullptr;
Button button(BUTTON_CFG_GPIO);

bool webSerwerStart = true;
unsigned long switchModeTime = 60000; //- 60sekund, 300000 - 5 minut czas na przejscie w tryb AP (Offline) jezeli nie ma sieci
unsigned long previousMillisStatus = 0;
int k=0;

struct parameters{
  bool motorDirection;
  unsigned long motorSpinTime;
  int motorSpeed;
  unsigned long measureDelayTime;
  int empty;
} parameter = {};

void setup() {
  Serial.begin(115200);

  virtualRelay = new Supla::Control::VirtualRelay();
  virtualSensor = new Supla::Sensor::VirtualBinary();
  virtualSensor2 = new Supla::Sensor::VirtualHygroMeter();
  server = new WebServer();

  // Configure defualt Supla CA certificate
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);

  SuplaDevice.setName("SUPLA-PODAJNIK");
  SuplaDevice.enterNormalMode();
  SuplaDevice.begin();

  applyParameters();

  feed.measureStart();
  button.init(clickHandler, longPressHandler);
}

void applyParameters(){
  EEPROM.begin(sizeof(struct parameters)+50);
  EEPROM.get(50, parameter);
  Serial.println("Odczytane dane z EEPROM:");
  Serial.print("motorDirection: ");
  Serial.println(parameter.motorDirection);
  Serial.print("motorSpinTime: ");
  Serial.println(parameter.motorSpinTime);
  Serial.print("motorSpeed: ");
  Serial.println(parameter.motorSpeed);
  Serial.print("measureDelayTime: ");
  Serial.println(parameter.measureDelayTime);
  Serial.print("empty: ");
  Serial.println(parameter.empty);

  stepper.setDirection(parameter.motorDirection);
  if(parameter.motorSpinTime>500 && parameter.motorSpinTime<120000){
    stepper.setSpinTime(parameter.motorSpinTime);
  }else{
    stepper.setSpinTime(15000);
  }
  if(parameter.motorSpeed>1 && parameter.motorSpeed<9){
    stepper.setSpeed(parameter.motorSpeed);
  }else{
    stepper.setSpeed(2);
  }
  if(parameter.measureDelayTime>1000 && parameter.measureDelayTime<300000){
    feed.setMeasureDelayTime(parameter.measureDelayTime);
  }else{
    feed.setMeasureDelayTime(30000);
  }
  if(parameter.empty>100 && parameter.empty<20000){
    feed.setInputHigh(parameter.empty);
  }else{
    feed.setInputHigh(500);
  }
}

void clickHandler() {
  if (!stepper.isRolling()) {
    stepper.startSpin();
    server->setWebMotorStatus(1);
  }
}

void longPressHandler() {
  modeAP();
}

void updateSettings() {
  if (server->accessToUpdate()==0) {
    auto cfg = Supla::Storage::ConfigInstance();

    cfg->setWiFiPassword(server->getPassword().c_str());
    cfg->setWiFiSSID(server->getSSID().c_str());
    cfg->setSuplaServer(server->getServer().c_str());
    cfg->setEmail(server->getEmail().c_str());
    cfg->setSuplaCommProtocolEnabled(true);
    cfg->setDeviceMode(Supla::DEVICE_MODE_NORMAL);

    configSupla.commit();
    server->commit();
  }else if (server->accessToUpdate()==1){
    parameter.motorDirection = server->getDirection();
    parameter.motorSpinTime = server->getSpinTime();
    parameter.motorSpeed = server->getSpinSpeed();
    parameter.measureDelayTime = server->getMeasureTime();
    parameter.empty = server->getMeasureEmpty();
    EEPROM.put(50, parameter);
    EEPROM.commit();
    server->commit();
  }
}

void updateSensor() {
  virtualSensor2->setValue(feed.getPercentValue());
  if (feed.getPercentValue() < 10) virtualSensor->set();
  else virtualSensor->clear();

}

void switchMode() {
  if (SuplaDevice.getCurrentStatus() == 8 && !server->isRunning()) {
    unsigned long currentMillisStatus = millis();
    if (currentMillisStatus - previousMillisStatus >= switchModeTime) {
      modeAP();
      Serial.println("The mode has been switched to: Offline");
    }
  } else if (SuplaDevice.getCurrentStatus() == 17 && !server->isRunningSTA()) {
    modeSTA();
    Serial.println("The mode has been switched to: Online");
  } else if (SuplaDevice.getCurrentStatus() == 40 && !server->isRunning()) {
    modeAP();
    Serial.println("The mode has been switched to: Custom Config Mode");
  }
}

void suplaReset() {
  SuplaDevice.resetToFactorySettings();
}

void loop() {
  stepper.iterate();
  button.read();

  if (!stepper.isRolling()) {
    switchMode();
    feed.iterate();
    if (virtualRelay->isOn()) {
      virtualRelay->turnOff();
      stepper.startSpin();
    }
  }
  
  if (server->isRunning()) {
    previousMillisStatus = millis();
    updateSettings();
  }else if (server->isRunningSTA()){
    updateSettings();
    if (server->getWebMotorStatus() == 1) {
      stepper.startSpin();
      server->setWebMotorStatus(-1);
    }
    if (stepper.isRolling()) {
      if (server->getWebMotorStatus() == 0) {
        stepper.stopSpin();
        server->setWebMotorStatus(-1);
      }
    }
    SuplaDevice.iterate();
  }else{
    SuplaDevice.iterate();
  }
  server->iterate();
}

void modeAP() {
  server->init(stepper.getSpeed(), stepper.getSpinTime(), SuplaDevice.getCurrentStatus(), String(Supla::Channel::reg_dev.ServerName),  String(Supla::Channel::reg_dev.Email), &feed);
  server->startAP();
}

void modeSTA() {
  server->init(stepper.getSpeed(), stepper.getSpinTime(), SuplaDevice.getCurrentStatus(), Supla::Channel::reg_dev.ServerName,  Supla::Channel::reg_dev.Email, &feed);
  server->startSTA();
}
