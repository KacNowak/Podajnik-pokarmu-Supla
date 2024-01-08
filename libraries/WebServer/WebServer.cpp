#include "WebServer.h"

WebServer::WebServer(int value)
    : port(value), networkMode(true), initialized(false), webMotorStatus(-1),
      rewriteNetData(false), STA(false), rewriteParametersData(false)
{
    ESP8266WebServer server(port);
}

WebServer::WebServer()
    : networkMode(true), initialized(false), port(80), webMotorStatus(-1),
      rewriteNetData(false), STA(false), rewriteParametersData(false)
{
    ESP8266WebServer server(port);
}

void WebServer::toggleNetworkMode()
{
    networkMode = !networkMode;
}

void WebServer::setStatus(int value)
{
    switch (value)
    {
    case 3:
        deviceSettings.suplaStatus = "Nieznany adres serwera";
        break;

    case 6:
        deviceSettings.suplaStatus = "Rozłączono z serwera";
        break;

    case 8:
        deviceSettings.suplaStatus = "Rozłączono z sieci/Sieć niedostępna";
        break;

    case 17:
        deviceSettings.suplaStatus = "Połączono";
        break;

    default:
        deviceSettings.suplaStatus = "Nieznany status urządzenia";
        break;
    }

    Serial.print("The status has been changed to: ");
    Serial.println(deviceSettings.suplaStatus);
}

void WebServer::init(int motorSpeed, unsigned long motorSpinTime, int suplaStatus, String suplaServer, String suplaAccount, FeedMeasure* feedPtr)
{
    feed = feedPtr;
    setStatus(suplaStatus);
    deviceSettings.motorSpeed = motorSpeed;
    deviceSettings.motorSpinTime = motorSpinTime;
    deviceSettings.suplaServer = suplaServer;
    deviceSettings.suplaAccount = suplaAccount;
    initialized = true;
}

bool WebServer::isInit()
{
    if (initialized)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void WebServer::startAP()
{
    if (isInit())
    {
        STA = false;
        WiFi.mode(WIFI_AP);
        WiFi.softAP("SUPLA-PODAJNIK", "");
        server.on("/", HTTP_GET, std::bind(&WebServer::runPage, this));
        server.on("/restart", HTTP_GET, std::bind(&WebServer::restartPage, this));
        server.on("/network", std::bind(&WebServer::configNetworkPage, this));
        server.on("/parameters", std::bind(&WebServer::configParametersPage, this));
        server.on("/?motor=on", HTTP_GET, std::bind(&WebServer::runPage, this));
        server.on("/?motor=off", HTTP_GET, std::bind(&WebServer::runPage, this));
        server.begin();
        toggleNetworkMode();
        Serial.println("AP Web server is running");
    }
    else
    {
        Serial.println("AP Web server is not initialized");
    }
}

void WebServer::startSTA()
{
    if (isInit())
    {
        server.on("/", HTTP_GET, std::bind(&WebServer::runPage, this));
        server.on("/restart", HTTP_GET, std::bind(&WebServer::restartPage, this));
        server.on("/network", std::bind(&WebServer::configNetworkPage, this));
        server.on("/parameters", std::bind(&WebServer::configParametersPage, this));
        server.on("/?motor=on", HTTP_GET, std::bind(&WebServer::runPage, this));
        server.on("/?motor=off", HTTP_GET, std::bind(&WebServer::runPage, this));
        server.begin();
        Serial.println("STA Web server is running");
        STA = true;
    }

    else
    {
        Serial.println("STA Web server is not initialized");
    }
}

void WebServer::stop()
{
    if (isRunning())
    {
        WiFi.softAPdisconnect(true);
        server.close();
        toggleNetworkMode();
        Serial.println("The web serwer has been disabled");
    }
}

void WebServer::iterate()
{
    if (isRunning() || isRunningSTA())
    {
        server.handleClient();
    }
}

bool WebServer::isRunning()
{
    if (networkMode)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool WebServer::isRunningSTA()
{
    return STA;
}

String WebServer::webDeviceDescription()
{
    String s1 = "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><meta name='author' content='Kacper Nowak'><title>SUPLA - PODAJNIK</title>    <style>*,::after,::before {box-sizing: border-box;}body {margin: 0;font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans';font-size: 1rem;font-weight: 400;line-height: 1.5;color: #1cc88a;background-color: #1f1f1f;}.form-control {display: block;width: 100%;height: calc(1.5em + .75rem + 2px);border: 1px solid #01774c;background-color: #cfcfcf;letter-spacing: 2px;font-weight: bold;}button {border: 1px solid transparent;color: #fff;background-color: #01774c;border-color: #01774c;padding: .5rem 1rem;font-size: 1.2rem;line-height: 1.5;border-radius: .3rem;width: 100%}.form-signin {width: 100%;max-width: 400px;padding: 15px;margin: auto;}h1 {text-align: center;}h2,p {text-align: center;color: white;}.navbar {display: flex;justify-content: center;margin-bottom: 20px;}.nav-block {background-color: #01774c;padding: .5rem .7rem;text-align: center;border-radius: .3rem;margin-left: 1rem;margin-right: 1rem;color: white;font-size: 1.2rem;line-height: 1.5;}a {font-style: normal;text-decoration: none;}.device-name {background-color: #01774c;text-align: center;margin-top: -15px;margin-bottom: 20px;border-bottom-left-radius: 30px;border-bottom-right-radius: 30px;color: white;font-size: 1.2rem;line-height: 1.5;font-weight: bold;padding: 7px;}.device-info {display: block;justify-content: center;font-size: 1.2rem;}.device-info p {color: #1cc88a;}.device-info span {color: white;}.device-info button {cursor: pointer;background-color: #871010;border-color: #871010;margin-top: 40px;}.active{font-weight: bold;}  #btn-motor{ background-color: #1cc88a; border-color: #1cc88a; color: #1f1f1f; }</style></head><body><main class='form-signin'><div class='device-name'>SUPLA-PODAJNIK</div><div class='navbar'><a href='/'><div class='nav-block active'>Informacje</div></a><a href='/network'><div class='nav-block'>Konfiguracja</div></a></div><div class='device-info'><hr><p>";
    String s2 = "";
    if (STA)
    {
        s2 = "Status Supla: <span>" + deviceSettings.suplaStatus + "</span><br>Tryb pracy: <span>" + "OFFLINE" + "</span><br>Adres IP: <span>" + WiFi.localIP().toString() + "</span><br>Adres MAC: <span>" + WiFi.macAddress() + "</span><br>Serwer Supla: <span>" + deviceSettings.suplaServer + "</span><br>Konto Supla: <span>" + deviceSettings.suplaAccount + "</span><br></p><hr><p>" + "Status ślimaka: <span id='status'>" + deviceSettings.motorStatus + "</span></br> Czas pracy ślimaka: <span>" + deviceSettings.motorSpinTime + "</span> ms</br> Prędkość pracy (2-8): <span>" + deviceSettings.motorSpeed + "</span></br></p><a href='#' id='btn-href'><button type='button' id='btn-motor'></button></a><a href='/restart'><button type='button'>ZRESTARTUJ</button></a></div></main></body><script>const btnMotor = document.getElementById('btn-motor');const btnHref = document.getElementById('btn-href');    const status = document.getElementById('status');    function updateState() {const queryString = window.location.search;const urlParams = new URLSearchParams(queryString);motorStatus = urlParams.get('motor');if (status.innerHTML=='Załączony') {btnHref.setAttribute('href', '?motor=off');btnMotor.innerHTML='ZATRZYMAJ ŚLIMAK';}else{btnHref.setAttribute('href', '?motor=on');btnMotor.innerHTML='URUCHOM ŚLIMAK';}} setInterval('updateState()', 500);</script></html>";
    }
    else
    {
        s2 = "Status Supla: <span>" + deviceSettings.suplaStatus + "</span><br>Tryb pracy: <span>" + "OFFLINE" + "</span><br>Adres IP: <span>" + WiFi.softAPIP().toString() + "</span><br>Adres MAC: <span>" + WiFi.macAddress() + "</span><br>Serwer Supla: <span>" + deviceSettings.suplaServer + "</span><br>Konto Supla: <span>" + deviceSettings.suplaAccount + "</span><br></p><hr><p>" + "Status ślimaka: <span id='status'>" + deviceSettings.motorStatus + "</span></br> Czas pracy ślimaka: <span>" + deviceSettings.motorSpinTime + "</span> ms</br> Prędkość pracy (2-8): <span>" + deviceSettings.motorSpeed + "</span></br></p><a href='#' id='btn-href'><button type='button' id='btn-motor'></button></a><a href='/restart'><button type='button'>ZRESTARTUJ</button></a></div></main></body><script>const btnMotor = document.getElementById('btn-motor');const btnHref = document.getElementById('btn-href');    const status = document.getElementById('status');    function updateState() {const queryString = window.location.search;const urlParams = new URLSearchParams(queryString);motorStatus = urlParams.get('motor');if (status.innerHTML=='Załączony') {btnHref.setAttribute('href', '?motor=off');btnMotor.innerHTML='ZATRZYMAJ ŚLIMAK';}else{btnHref.setAttribute('href', '?motor=on');btnMotor.innerHTML='URUCHOM ŚLIMAK';}} setInterval('updateState()', 500);</script></html>";
    }
    return s1 + s2;
}

void WebServer::runPage()
{
    if (server.method() == HTTP_GET)
    {
        if (server.arg("motor") == "on")
        {
            setWebMotorStatus(1);
        }
        else if (server.arg("motor") == "off")
        {
            setWebMotorStatus(0);
        }
        else
        {
            setWebMotorStatus(-1);
        }
    }
    server.send(200, "text/html", webDeviceDescription());
}

void WebServer::restartPage()
{
    server.send(200, "text/html", htmlRestart);
    delay(5000);
    ESP.restart();
}

void WebServer::configNetworkPage()
{
    if (server.method() == HTTP_POST)
    {
        network.ssid = server.arg("ssid");
        network.password = server.arg("password");
        network.server = server.arg("server");
        network.email = server.arg("email");
        rewriteNetData = true;
        server.send(200, "text/html", htmlRestart);
    }
    else
    {
        server.send(200, "text/html", htmlNetwork);
    }
}

void WebServer::configParametersPage()
{
    if (server.method() == HTTP_POST)
    {
        deviceSettings.motorSpinTime = server.arg("spinTime").toInt();
        deviceSettings.motorSpeed = server.arg("spinSpeed").toInt();
        deviceSettings.measureTime = server.arg("measureTime").toInt();
        deviceSettings.measureEmpty = server.arg("empty").toInt();
        deviceSettings.measureFull = server.arg("full").toInt();

        if (server.arg("direction") == "right")
        {
            deviceSettings.direction = true;
        }
        else
        {
            deviceSettings.direction = false;
        }
        rewriteParametersData = true;
        server.send(200, "text/html", htmlRestart);
    }
    else
    {
        feed->measure();
        String value = String(feed->getValue());
        String combinedHtml = String(htmlParameters1)+ value + String(htmlParameters2);
        server.send(200, "text/html", combinedHtml);
    }
}

int WebServer::getPort()
{
    return port;
}

void WebServer::setWebMotorStatus(int value)
{
    if (value == 0)
    {
        webMotorStatus = 0;
        deviceSettings.motorStatus = "Wyłączony";
    }
    else if (value == 1)
    {
        webMotorStatus = 1;
        deviceSettings.motorStatus = "Załączony";
    }
    else
    {
        webMotorStatus = -1;
        deviceSettings.motorStatus = "Wyłączony/Nieznany";
    }
}

int WebServer::getWebMotorStatus()
{
    return webMotorStatus;
}

String WebServer::getSSID()
{
    return network.ssid;
}
String WebServer::getPassword()
{
    return network.password;
}
String WebServer::getServer()
{
    return network.server;
}
String WebServer::getEmail()
{
    return network.email;
}
int WebServer::accessToUpdate()
{
    if (rewriteNetData)
    {
        return 0;
    }
    else if (rewriteParametersData)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}
void WebServer::commit()
{
    rewriteNetData = false;
    rewriteParametersData = false;
    Serial.println("Settings updated successfully");
    delay(5000);
    ESP.restart();
}
unsigned long WebServer::getSpinTime()
{
    return deviceSettings.motorSpinTime;
}

int WebServer::getSpinSpeed()
{
    return deviceSettings.motorSpeed;
}

unsigned long WebServer::getMeasureTime()
{
    return deviceSettings.measureTime;
}

int WebServer::getMeasureEmpty()
{
    return deviceSettings.measureEmpty;
}

int WebServer::getMeasureFull()
{
    return deviceSettings.measureFull;
}

bool WebServer::getDirection()
{
    return deviceSettings.direction;
}
