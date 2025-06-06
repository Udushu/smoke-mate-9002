#ifndef WEBSERVER_SMOKEMATE_H
#define WEBSERVER_SMOKEMATE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <functional>
#include "types.h"

#define STATIC_JSON_DOCUMENT_SIZE 2048

class WebServer
{
public:
    using StatusCallback = std::function<String()>;
    using ConfigGetCallback = std::function<String()>;
    using ConfigSetCallback = std::function<bool(const String &)>;

    WebServer(uint16_t port, ControllerStatus &status, Configuration &config);

    void begin();
    void end();
    bool isNVRAMSaveRequired();

private:
    AsyncWebServer m_server;
    StatusCallback m_statusCallback;
    ConfigGetCallback m_configGetCallback;
    ConfigSetCallback m_configSetCallback;

    ControllerStatus &m_status;
    Configuration &m_config;

    bool m_isNVRAMSaveRequired = false;

    void setStatusCallback(StatusCallback cb);
    void setConfigGetCallback(ConfigGetCallback cb);
    void setConfigSetCallback(ConfigSetCallback cb);

    void setupRoutes();
    void handleRoot(AsyncWebServerRequest *request);
    void handleApiStatus(AsyncWebServerRequest *request);
    void handleApiConfigGet(AsyncWebServerRequest *request);
    void handleApiConfigSet(AsyncWebServerRequest *request, uint8_t *data, size_t len);
    void handleApiControllerStart(AsyncWebServerRequest *request);
    void handleApiControllerStop(AsyncWebServerRequest *request);
};

#endif // WEBSERVER_SMOKEMATE_H