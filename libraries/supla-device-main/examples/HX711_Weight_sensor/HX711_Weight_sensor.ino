/*
Copyright (C) AC SOFTWARE SP. Z O.O.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <SuplaDevice.h>
/*
 * This example requires HX711_ADC library installed. 
 * https://github.com/olkal/HX711_ADC
 */
 
// Add include to HX711 sensor
#include <supla/sensor/HX711.h>
#include <supla/control/button.h>

#define SDA_GPIO 21
#define SCL_GPIO 22
#define BUTTON_GPIO 4

// Choose proper network interface for your card:
#ifdef ARDUINO_ARCH_AVR
  // Arduino Mega with EthernetShield W5100:
  #include <supla/network/ethernet_shield.h>
  // Ethernet MAC address
  uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  Supla::EthernetShield ethernet(mac);

  // Arduino Mega with ENC28J60:
  // #include <supla/network/ENC28J60.h>
  // Supla::ENC28J60 ethernet(mac);
#elif defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  // ESP8266 and ESP32 based board:
  #include <supla/network/esp_wifi.h>
  Supla::ESPWifi wifi("your_wifi_ssid", "your_wifi_password");
#endif

// Choose where Supla should store HX711 configuration data in persistent memory
// We recommend to use external FRAM memory
#include <supla/storage/eeprom.h>
Supla::Eeprom eeprom;
// #include <supla/storage/fram_spi.h>
// Supla::FramSpi fram(STORAGE_OFFSET);

void setup() {

  Serial.begin(115200);

  // Replace the falowing GUID with value that you can retrieve from https://www.supla.org/arduino/get-guid
  char GUID[SUPLA_GUID_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  // Replace the following AUTHKEY with value that you can retrieve from: https://www.supla.org/arduino/get-authkey
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  /*
   * Having your device already registered at cloud.supla.org,
   * you want to change CHANNEL sequence or remove any of them,
   * then you must also remove the device itself from cloud.supla.org.
   * Otherwise you will get "Channel conflict!" error.
   */


  auto scales = new Supla::Sensor::HX711(SDA_GPIO, SCL_GPIO);
  // Optionally, enter the calibration value as the third parameter (SDA_GPIO, SCL_GPIO, 696.0)
  auto button = new Supla::Control::Button(BUTTON_GPIO);
  button->setSwNoiseFilterDelay(50);
  button->addAction(Supla::TARE_SCALES, scales, Supla::ON_PRESS);

  /*
   * SuplaDevice Initialization.
   * Server address is available at https://cloud.supla.org 
   * If you do not have an account, you can create it at https://cloud.supla.org/account/create
   * SUPLA and SUPLA CLOUD are free of charge
   * 
   */

  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    "svr1.supla.org",  // SUPLA server address
                    "email@address",   // Email address used to login to Supla Cloud
                    AUTHKEY);          // Authorization key
    
}

void loop() {
  SuplaDevice.iterate();
}

