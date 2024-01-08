#ifndef WebServer_h
#define WebServer_h

#include <ESP8266WebServer.h>
#include "FeedMeasure.h"

const char htmlRestart[] PROGMEM = R"=====(
  <!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>SUPLA - PODAJNIK</title><style> *,::after,::before{box-sizing: border-box;}body{margin: 0;font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans';font-size: 1rem;font-weight: 400;line-height: 1.5;color: #01774c;background-color: #1f1f1f;}.form-control{display: block;width: 100%; height: calc(1.5em + .75rem + 2px);border: 1px solid #ced4da;}button {border: 1px solid transparent;color: #fff;background-color: #007bff;border-color: #007bff;padding: .5rem 1rem;font-size: 1.25rem;line-height: 1.5;border-radius: .3rem;width: 100%} .form-signin {width: 100%;max-width: 400px;padding: 15px;margin: auto;}h1 {text-align: center;}h2,p {text-align: center;color: white;}</style></head><body><main class='form-signin'><h1>SUPLA - PODAJNIK</h1><p>Urządzenie zrestartuje się automatycznie za <span id='timer'>5</span></p></main></body><script>var t = document.getElementById('timer'); var i = 5; function odliczanie() { if (i >= 0) { t.innerHTML = i; i--; } if (i == -1) { document.getElementsByTagName('p')[0].innerHTML = 'Trwa restart urządzenia.<br />'; i--; } } setInterval('odliczanie()', 1000);</script></html>
)=====";

const char htmlNetwork[] PROGMEM = R"=====(
  <!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><meta name='author' content='Kacper Nowak'><title>SUPLA - PODAJNIK</title><style> *,::after,::before {box-sizing: border-box;}body {margin: 0;font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans';font-size: 1rem;font-weight: 400;line-height: 1.5;color: #1cc88a;background-color: #1f1f1f;}.form-control {display: block;width: 100%; height: calc(1.5em + .75rem + 2px);border: 1px solid #01774c;background-color: #cfcfcf;letter-spacing: 2px;font-weight: bold;}button {border: 1px solid transparent;color: #fff;background-color: #01774c;border-color: #01774c;padding: .5rem 1rem;font-size: 1.2rem;line-height: 1.5;border-radius: .3rem;width: 100%}.form-signin {width: 100%;max-width: 400px;padding: 15px;margin: auto;}h1 {text-align: center;}h2,p {text-align: center;color: white;}.navbar {display: flex;justify-content: center;margin-bottom: 20px;}.nav-block {background-color: #01774c;padding: .5rem .7rem;text-align: center;border-radius: .3rem;margin-left: 1rem;margin-right: 1rem;color: white;font-size: 1.2rem;line-height: 1.5;}a {font-style: normal;text-decoration: none;}.device-name {background-color: #01774c;text-align: center;margin-top: -15px;margin-bottom: 20px;border-bottom-left-radius: 30px;border-bottom-right-radius: 30px;color: white;font-size: 1.2rem;line-height: 1.5;font-weight: bold;padding: 7px;}.device-info button {cursor: pointer;background-color: #871010;border-color: #871010;margin-top: 40px;}.active {font-weight: bold;}#btn-motor{background-color: #1cc88a;border-color: #1cc88a;color: black;}input{margin-bottom: 20px;}button {cursor: pointer;}</style></head><body><main class='form-signin'><form action='/network' method='post'><div class='device-name'>SUPLA-PODAJNIK</div><div class='navbar'><a href='/'><div class='nav-block'>Informacje</div></a><a href='/network'><div class='nav-block active'>Konfiguracja</div></a></div><hr><div class='navbar'><a href='/network'><div class='nav-block active'>Sieć/SUPLA</div></a><a href='/parameters'><div class='nav-block'>Parametry</div></a></div><h1>Ustawienia wifi</h1><div class='form-floating'><label>Nazwa sieci</label><input type='text' class='form-control' name='ssid'></div><div class='form-floating'><label>Hasło</label><input type='password' class='form-control' name='password'></div><hr><h1>Ustawienia SUPLA</h1><div class='form-floating'><label>Serwer</label><input type='text' class='form-control' name='server'></div><div class='form-floating'><label>Konto (E-mail)</label><input type='text' class='form-control' name='email'></div><button type='submit'>Zapisz</button><div class='device-info'><a href='/restart'><button type='button'>ZRESTARTUJ</button></a></div></form></main></body></html>
)=====";
/*
const char htmlParameters[] PROGMEM = R"=====(
  <!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><meta name='author' content='Kacper Nowak'><title>SUPLA - PODAJNIK</title><style>*,::after,::before {box-sizing: border-box;}body {margin: 0;font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans';font-size: 1rem;font-weight: 400;line-height: 1.5;color: #1cc88a;background-color: #1f1f1f;}.form-control {display: block;width: 100%; height: calc(1.5em + .75rem + 2px);border: 1px solid #01774c;background-color: #cfcfcf;letter-spacing: 2px;font-weight: bold;}button {border: 1px solid transparent;color: #fff;background-color: #01774c;border-color: #01774c;padding: .5rem 1rem;font-size: 1.2rem;line-height: 1.5;border-radius: .3rem;width: 100%}.form-signin {width: 100%;max-width: 400px;padding: 15px;margin: auto;}h1 {text-align: center;}h2,p {text-align: center;color: white;}.navbar {display: flex;justify-content: center;margin-bottom: 20px;}.nav-block {background-color: #01774c;padding: .5rem .7rem;text-align: center;border-radius: .3rem;margin-left: 1rem;margin-right: 1rem;color: white;font-size: 1.2rem;line-height: 1.5;}a {font-style: normal;text-decoration: none;}.device-name {background-color: #01774c;text-align: center;margin-top: -15px;margin-bottom: 20px;border-bottom-left-radius: 30px;border-bottom-right-radius: 30px;color: white;font-size: 1.2rem;line-height: 1.5;font-weight: bold;padding: 7px;}.device-info button {cursor: pointer;background-color: #a77823;border-color: #a77823;margin-top: 40px;}.active {font-weight: bold;}#btn-motor {background-color: #1cc88a;border-color: #1cc88a;color: black;}input {margin-bottom: 20px;}button{cursor: pointer;} .form-radio {width: 30px;height: 30px;margin-top: 50px;}.form-floating-radio{display: flex;cursor: pointer;}.label-radio{margin-top: 50px;}</style></head><body><main class='form-signin'><form action='/parameters' method='post'> <div class='device-name'>SUPLA-PODAJNIK</div><div class='navbar'><a href='/'><div class='nav-block'>Informacje</div></a><a href='/network'><div class='nav-block active'>Konfiguracja</div></a></div><hr><div class='navbar'><a href='/network'><div class='nav-block'>Sieć/SUPLA</div></a><a href='/parameters'><div class='nav-block active'>Parametry</div></a></div><h1>Ustawienia Ślimaka</h1><div class='form-floating'><label>Czas pracy [ms]</label><input type='number' class='form-control' name='spinTime'></div><div class='form-floating'><label>Prędkość [2-8] (2-najszybszy, 8-najwolniejszy)</label><input type='number' class='form-control' name='spinSpeed'></div><div class='form-floating-radio'><label>Kierunek obrotu:</label><br><div class='label-radio'><label>Lewo </label></div><input type='radio' value='left' class='form-radio' id='left' name='direction' checked/><input type='radio' value='right' class='form-radio' id='right' name='direction'/><div class='label-radio'>Prawo</div></div><hr><h1>Kalibracja Czujnika</h1><div class='form-floating'><label>Czas odpytywania [ms] (zalecane: 20s-60s)</label><input type='number' class='form-control' name='measureTime'></div><div class='form-floating'><label>Wartość 0% (Pusty zbiornik)</label><input type='number' class='form-control' name='empty'></div><div class='form-floating'><label>Wartość 100% (Pełny zbiornik)</label><input type='number' class='form-control' name='full'></div><button type='submit'>Zapisz i zrestartuj</button><div class='device-info'><a href='/parameters'><button type='button'>Odpytaj czujnik</button></a></div></form></main></body></html>
)=====";*/ //To dobre działa

const char htmlParameters1[] PROGMEM = R"=====(
  <!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><meta name='author' content='Kacper Nowak'><title>SUPLA - PODAJNIK</title><style>*,::after,::before {box-sizing: border-box;}body {margin: 0;font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans';font-size: 1rem;font-weight: 400;line-height: 1.5;color: #1cc88a;background-color: #1f1f1f;}.form-control {display: block;width: 100%; height: calc(1.5em + .75rem + 2px);border: 1px solid #01774c;background-color: #cfcfcf;letter-spacing: 2px;font-weight: bold;}button {border: 1px solid transparent;color: #fff;background-color: #01774c;border-color: #01774c;padding: .5rem 1rem;font-size: 1.2rem;line-height: 1.5;border-radius: .3rem;width: 100%}.form-signin {width: 100%;max-width: 400px;padding: 15px;margin: auto;}h1 {text-align: center;}h2,p {text-align: center;color: white;}.navbar {display: flex;justify-content: center;margin-bottom: 20px;}.nav-block {background-color: #01774c;padding: .5rem .7rem;text-align: center;border-radius: .3rem;margin-left: 1rem;margin-right: 1rem;color: white;font-size: 1.2rem;line-height: 1.5;}a {font-style: normal;text-decoration: none;}.device-name {background-color: #01774c;text-align: center;margin-top: -15px;margin-bottom: 20px;border-bottom-left-radius: 30px;border-bottom-right-radius: 30px;color: white;font-size: 1.2rem;line-height: 1.5;font-weight: bold;padding: 7px;}.device-info button {cursor: pointer;background-color: #a77823;border-color: #a77823;margin-top: 40px;}.active {font-weight: bold;}#btn-motor {background-color: #1cc88a;border-color: #1cc88a;color: black;}input {margin-bottom: 20px;}button{cursor: pointer;} .form-radio {width: 30px;height: 30px;margin-top: 50px;}.form-floating-radio{display: flex;cursor: pointer;}.label-radio{margin-top: 50px;}</style></head><body><main class='form-signin'><form action='/parameters' method='post'> <div class='device-name'>SUPLA-PODAJNIK</div><div class='navbar'><a href='/'><div class='nav-block'>Informacje</div></a><a href='/network'><div class='nav-block active'>Konfiguracja</div></a></div><hr><div class='navbar'><a href='/network'><div class='nav-block'>Sieć/SUPLA</div></a><a href='/parameters'><div class='nav-block active'>Parametry</div></a></div><h1>Ustawienia Ślimaka</h1><div class='form-floating'><label>Czas pracy [ms]</label><input type='number' class='form-control' name='spinTime'></div><div class='form-floating'><label>Prędkość [2-8] (2-najszybszy, 8-najwolniejszy)</label><input type='number' class='form-control' name='spinSpeed'></div><div class='form-floating-radio'><label>Kierunek obrotu:</label><br><div class='label-radio'><label>Lewo </label></div><input type='radio' value='left' class='form-radio' id='left' name='direction' checked/><input type='radio' value='right' class='form-radio' id='right' name='direction'/><div class='label-radio'>Prawo</div></div><hr><h1>Kalibracja Czujnika</h1><div class='form-floating'><label>Czas odpytywania [ms] (zalecane: 20s-60s)</label><input type='number' class='form-control' name='measureTime'></div><div class='form-floating'><label>Wartość 0% (Pusty zbiornik)</label><input type='number' class='form-control' name='empty'></div><button type='submit'>Zapisz i zrestartuj</button><div class='device-info'></form><div class='form-floating' id='measure' style="display: none;"><hr><h2 style='color: #a77823;'>Wartość odczytana z czujnika:</h2><h3 style='text-align: center;'>
)=====";

const char htmlParameters2[] PROGMEM = R"=====(
  </h3></div><div class='device-info'><a href='/parameters?measure=on'><button type='button'>Odpytaj czujnik</button></a></div></main></body><script>const measure = document.getElementById('measure'); function updateMeasure() {const queryString = window.location.search;const urlParams = new URLSearchParams(queryString);measureStatus = urlParams.get('measure');if (measureStatus == 'on') {measure.style.display = ''} else {measure.style.display = 'none';}} setInterval('updateMeasure()', 500);</script></html>
)=====";

class WebServer {
protected:
  int port;
  bool networkMode;
  bool initialized;
  int webMotorStatus;
  ESP8266WebServer server;
  bool rewriteNetData;
  bool rewriteParametersData;
  bool STA;
  FeedMeasure* feed;

public:
  WebServer();
  WebServer(int value);

  struct Settings
  {
    int motorSpeed;
    unsigned long motorSpinTime;
    String motorStatus;
    unsigned long measureTime;
    int measureEmpty;
    int measureFull;
    bool direction;
    String suplaStatus;
    String suplaServer;
    String suplaAccount;
  };

  struct Network
  {
    String ssid;
    String password;
    String server;
    String email;
  };
  
  void toggleNetworkMode();
  void setStatus(int value);
  void init(int motorSpeed, unsigned long motorSpinTime, int suplaStatus, String suplaServer, String suplaAccount, FeedMeasure* feedPtr);
  bool isInit();
  void startAP();
  void stop();
  void startSTA();
  void iterate();
  bool isRunning();
  bool isRunningSTA();
  void runPage();
  void restartPage();
  void configNetworkPage();
  void configParametersPage();
  String webDeviceDescription();
  void setWebMotorStatus(int value);

  //GET
  int getPort();
  int getWebMotorStatus();
  String getSSID();
  String getPassword();
  String getServer();
  String getEmail();
  unsigned long getSpinTime();
  int getSpinSpeed();
  bool getDirection();
  unsigned long getMeasureTime();
  int getMeasureEmpty();
  int getMeasureFull();
 

  //Save
  int accessToUpdate();
  void commit();

private:
  Settings deviceSettings;
  Network network;
};

#endif
