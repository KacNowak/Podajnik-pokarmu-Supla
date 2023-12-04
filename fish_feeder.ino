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
#include <supla/control/button.h>
#include <supla/device/status_led.h>
#include <supla/storage/littlefs_config.h>
#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/status_led_parameters.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/device/supla_ca_cert.h>
#include <supla/events.h>
#include <supla/storage/eeprom.h>
#include <supla/sensor/virtual_binary.h>
#include <supla/sensor/virtual_hygromometer.h>

#include "StepperMotor.h"
#include "FeedMeasure.h"

Supla::Eeprom eeprom;

Supla::ESPWifi wifi;
Supla::LittleFsConfig configSupla;

Supla::Device::StatusLed statusLed(STATUS_LED_GPIO, true); // inverted state
Supla::EspWebServer suplaServer;

// HTML www component (they appear in sections according to creation
// sequence)
Supla::Html::DeviceInfo htmlDeviceInfo(&SuplaDevice);
Supla::Html::WifiParameters htmlWifi;
Supla::Html::ProtocolParameters htmlProto;
Supla::Html::StatusLedParameters htmlStatusLed;

Supla::Control::VirtualRelay *virtualRelay = nullptr;
Supla::Sensor::VirtualBinary *virtualSensor = nullptr;
Supla::Sensor::VirtualHygroMeter *virtualSensor2 = nullptr;

StepperMotor stepper(Pin1, Pin2, Pin3, Pin4);
FeedMeasure feed;

bool networkMode=true;

void setup() {
  Serial.begin(115200);

  // Channels configuration
  // CH 0 - virtual relay (P)
  virtualRelay = new Supla::Control::VirtualRelay();
  virtualSensor = new Supla::Sensor::VirtualBinary();
  virtualSensor2 = new Supla::Sensor::VirtualHygroMeter();

  // Buttons configuration
  auto buttonCfg = new Supla::Control::Button(BUTTON_CFG_GPIO, true, true);

  buttonCfg->configureAsConfigButton(&SuplaDevice);

  // Configure defualt Supla CA certificate
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);

  SuplaDevice.setName("SUPLA-PODAJNIK");
  SuplaDevice.begin();

  stepper.setDirection(true);
  stepper.setSpinTime(15000);
  stepper.setSpeed(2);

  feed.setMeasureDelayTime(30000);
  feed.setInputHigh(400);
  feed.measureStart();
}

void updateSensor(){
  virtualSensor2->setValue(feed.getPercentValue());
  if(feed.getPercentValue()<10) virtualSensor->set();
  else virtualSensor->clear();

}

bool ButtonPressed(){
  if(!stepper.isRolling()){
    if(digitalRead(BUTTON_CFG_GPIO)==LOW){
      delay(20);
      return true;
    }else return false;
  }else return false;
}

void loop() {
  if(networkMode){
    //Serial.print("SUPLA STATUS: ");
    //Serial.println(SuplaDevice.getCurrentStatus());
    SuplaDevice.iterate();
    stepper.iterate();
    if(virtualRelay->isOn() || ButtonPressed()){
      virtualRelay->turnOff();
      stepper.startSpin();
    }
    updateSensor();
    feed.iterate();
  }else{
    
  }
}
