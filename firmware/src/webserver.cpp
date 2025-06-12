#include "webserver.h"

WebServer::WebServer(uint16_t port, ControllerStatus &status, Configuration &config)
    : m_server(port), m_status(status), m_config(config)

{
}

void WebServer::setStatusCallback(StatusCallback cb)
{
    m_statusCallback = cb;
}

void WebServer::setConfigGetCallback(ConfigGetCallback cb)
{
    m_configGetCallback = cb;
}

void WebServer::setConfigSetCallback(ConfigSetCallback cb)
{
    m_configSetCallback = cb;
}

void WebServer::begin()
{
    setupRoutes();
    m_server.begin();
}

void WebServer::end()
{
    m_server.end();
}

bool WebServer::isNVRAMSaveRequired()
{
    if (m_isNVRAMSaveRequired)
    {
        m_isNVRAMSaveRequired = false; // Reset the flag after checking
        return true;                   // Indicate that NVRAM save is required
    }
    else
    {
        return false; // No NVRAM save required
    }
}

void WebServer::setupRoutes()
{
    m_server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
                { handleRoot(request); });

    m_server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request)
                { handleApiStatus(request); });

    m_server.on("/config", HTTP_GET, [this](AsyncWebServerRequest *request)
                { handleApiConfigGet(request); });

    m_server.on("/config", HTTP_POST, [this](AsyncWebServerRequest *request)
                {
                    // This will be handled in the onRequestBody callback
                },
                NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t)
                { handleApiConfigSet(request, data, len); });

    // --- New endpoints ---
    m_server.on("/start", HTTP_POST, [this](AsyncWebServerRequest *request)
                { handleApiControllerStart(request); });

    m_server.on("/stop", HTTP_POST, [this](AsyncWebServerRequest *request)
                { handleApiControllerStop(request); });

    // REST API endpoints will be added here
}

void WebServer::handleRoot(AsyncWebServerRequest *request)
{
    const ControllerStatus &s = m_status;
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>SmokeMATE Status</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            body { background: #000; color: #fff; font-family: 'Segoe UI', Arial, sans-serif; margin: 0; }
            .container { max-width: 420px; margin: 40px auto; background: #222; border-radius: 16px; box-shadow: 0 4px 16px #0008; padding: 24px; }
            h1 { text-align: center; color: #ff6600; margin-bottom: 24px; }
            .status-table { width: 100%; border-collapse: separate; border-spacing: 0 8px; }
            .status-table td { padding: 8px 12px; border-radius: 8px; }
            .label { color: #aaa; text-align: left; }
            .value { color: #fff; font-weight: bold; text-align: right; }
            .accent { color: #0083FE; }
            .footer { text-align: center; margin-top: 24px; color: #666; font-size: 0.9em; }
            .wifi { color: #00FFD0; }
            .running { color: #008610; }
            .stopped { color: #ff6600; }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>SmokeMATE</h1>
            <table class="status-table">
                <tr><td class="label">Status</td><td class="value )rawliteral";
    html += s.isRunning ? "running" : "stopped";
    html += "\">";
    html += s.isRunning ? "RUNNING" : "STOPPED";
    html += "</td></tr>";

    html += "<tr><td class=\"label\">Uptime</td><td class=\"value\">" + String(s.uptime / 1000) + " s</td></tr>";
    html += "<tr><td class=\"label\">Smoker Temp</td><td class=\"value accent\">" + String(s.temperatureSmoker) + " &deg;F</td></tr>";
    html += "<tr><td class=\"label\">Food Temp</td><td class=\"value accent\">" + String(s.temperatureFood) + " &deg;F</td></tr>";
    html += "<tr><td class=\"label\">Target Temp</td><td class=\"value accent\">" + String(s.temperatureTarget) + " &deg;F</td></tr>";
    html += "<tr><td class=\"label\">Fan PWM</td><td class=\"value\">" + String(s.fanPWM) + " / 255</td></tr>";
    html += "<tr><td class=\"label\">Door Position</td><td class=\"value\">" + String(s.doorPosition) + " &deg;</td></tr>";
    html += "<tr><td class=\"label\">WiFi</td><td class=\"value wifi\">" + String(s.isWiFiConnected ? "Connected" : "Disconnected") + "</td></tr>";
    html += "<tr><td class=\"label\">IP Address</td><td class=\"value wifi\">" + s.ipAddress + "</td></tr>";
    html += "<tr><td class=\"label\">RSSI</td><td class=\"value wifi\">" + String(s.RSSI) + " dBm (bars: " + String(s.bars) + ")</td></tr>";

    html += R"rawliteral(
            </table>
            <div class="footer">SmokeMATE &copy; 2025</div>
        </div>
    </body>
    </html>
    )rawliteral";

    request->send(200, "text/html", html);
}

void WebServer::handleApiStatus(AsyncWebServerRequest *request)
{
    const ControllerStatus &s = m_status;
    StaticJsonDocument<STATIC_JSON_DOCUMENT_SIZE> doc;

    doc["isRunning"] = s.isRunning;
    doc["uuid"] = s.uuid;
    doc["uptime"] = s.uptime;
    doc["controllerStartMSec"] = s.controllerStartMSec;
    doc["temperatureSmoker"] = s.temperatureSmoker;
    doc["temperatureFood"] = s.temperatureFood;
    doc["temperatureTarget"] = s.temperatureTarget;
    doc["fanPWM"] = s.fanPWM;
    doc["doorPosition"] = s.doorPosition;
    doc["RSSI"] = s.RSSI;
    doc["bars"] = s.bars;
    doc["ipAddress"] = s.ipAddress;
    doc["isWiFiConnected"] = s.isWiFiConnected;
    doc["networkName"] = s.networkName;
    doc["temperatureError"] = s.temperatureError; // Add temperature error if available
    doc["isProfileRunning"] = s.isProfileRunning;
    doc["temperatureProfileStepIndex"] = s.temperatureProfileStepIndex;
    doc["temperatureProfileStartTimeMSec"] = s.temperatureProfileStartTimeMSec;
    doc["temperatureProfileStepsCount"] = s.temperatureProfileStepsCount;
    doc["temperatureProfileStepType"] = static_cast<int>(s.temperatureProfileStepType); // Convert enum to int

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
}

void WebServer::handleApiConfigGet(AsyncWebServerRequest *request)
{
    const Configuration &c = m_config;
    StaticJsonDocument<STATIC_JSON_DOCUMENT_SIZE> doc;

    doc["temperatureTarget"] = c.temperatureTarget;
    doc["temperatureIntervalMSec"] = c.temperatureIntervalMSec;

    doc["isTemperatureProfilingEnabled"] = c.isTemperatureProfilingEnabled;
    doc["temperatureProfileStepsCount"] = c.temperatureProfileStepsCount;
    JsonArray profileArray = doc.createNestedArray("temperatureProfile");
    for (int i = 0; i < c.temperatureProfileStepsCount; ++i)
    {
        JsonObject step = profileArray.createNestedObject();
        step["temperatureStartF"] = c.temperatureProfile[i].temperatureStartF;
        step["temperatureEndF"] = c.temperatureProfile[i].temperatureEndF;
        step["timeMSec"] = c.temperatureProfile[i].timeMSec;
        step["type"] = static_cast<int>(c.temperatureProfile[i].type); // Convert enum to int
    }

    doc["isPIDEnabled"] = c.isPIDEnabled;
    doc["kP"] = c.kP;
    doc["kI"] = c.kI;
    doc["kD"] = c.kD;
    doc["bangBangLowThreshold"] = c.bangBangLowThreshold;
    doc["bangBangHighThreshold"] = c.bangBangHighThreshold;
    doc["bangBangHysteresis"] = c.bangBangHysteresis;
    doc["bangBangFanSpeed"] = c.bangBangFanSpeed;
    doc["doorOpenPosition"] = c.doorOpenPosition;
    doc["doorClosePosition"] = c.doorClosePosition;
    doc["themometerSmokerGain"] = c.themometerSmokerGain;
    doc["themometerSmokerOffset"] = c.themometerSmokerOffset;
    doc["themometerFoodGain"] = c.themometerFoodGain;
    doc["themometerFoodOffset"] = c.themometerFoodOffset;
    doc["isThemometerSimulated"] = c.isThemometerSimulated;
    doc["isForcedFanPWM"] = c.isForcedFanPWM;
    doc["forcedFanPWM"] = c.forcedFanPWM;
    doc["isForcedDoorPosition"] = c.isForcedDoorPosition;
    doc["forcedDoorPosition"] = c.forcedDoorPosition;
    doc["isWiFiEnabled"] = c.isWiFiEnabled;
    doc["wifiSSID"] = String(c.wifiSSID);

    // Do not include wifiPassword for security, or include if needed:
    // doc["wifiPassword"] = String(c.wifiPassword);

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
}

void WebServer::handleApiConfigSet(AsyncWebServerRequest *request, uint8_t *data, size_t len)
{
    StaticJsonDocument<STATIC_JSON_DOCUMENT_SIZE> doc;
    DeserializationError error = deserializeJson(doc, data, len);

    if (error)
    {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    // Update all Configuration fields if present in JSON
    if (doc.containsKey("temperatureTarget"))
        m_config.temperatureTarget = doc["temperatureTarget"];
    if (doc.containsKey("temperatureIntervalMSec"))
        m_config.temperatureIntervalMSec = doc["temperatureIntervalMSec"];

    if (doc.containsKey("isTemperatureProfilingEnabled"))
        m_config.isTemperatureProfilingEnabled = doc["isTemperatureProfilingEnabled"];
    if (doc.containsKey("temperatureProfileStepsCount"))
        m_config.temperatureProfileStepsCount = doc["temperatureProfileStepsCount"];
    if (doc.containsKey("temperatureProfile"))
    {
        JsonArray arr = doc["temperatureProfile"].as<JsonArray>();
        int count = min((int)arr.size(), MAX_PROFILE_STEPS);
        for (int i = 0; i < count; ++i)
        {
            JsonObject step = arr[i];
            if (step.containsKey("timeMSec"))
                m_config.temperatureProfile[i].timeMSec = step["timeMSec"];
            if (step.containsKey("type"))
                m_config.temperatureProfile[i].type = (TempProfileType)((int)step["type"]);
            if (step.containsKey("temperatureStartF"))
                m_config.temperatureProfile[i].temperatureStartF = step["temperatureStartF"];
            if (step.containsKey("temperatureEndF"))
                m_config.temperatureProfile[i].temperatureEndF = step["temperatureEndF"];
        }
    }

    if (doc.containsKey("isPIDEnabled"))
        m_config.isPIDEnabled = doc["isPIDEnabled"];
    if (doc.containsKey("kP"))
        m_config.kP = doc["kP"];
    if (doc.containsKey("kI"))
        m_config.kI = doc["kI"];
    if (doc.containsKey("kD"))
        m_config.kD = doc["kD"];

    if (doc.containsKey("bangBangLowThreshold"))
        m_config.bangBangLowThreshold = doc["bangBangLowThreshold"];
    if (doc.containsKey("bangBangHighThreshold"))
        m_config.bangBangHighThreshold = doc["bangBangHighThreshold"];
    if (doc.containsKey("bangBangHysteresis"))
        m_config.bangBangHysteresis = doc["bangBangHysteresis"];
    if (doc.containsKey("bangBangFanSpeed"))
        m_config.bangBangFanSpeed = doc["bangBangFanSpeed"];

    if (doc.containsKey("doorOpenPosition"))
        m_config.doorOpenPosition = doc["doorOpenPosition"];
    if (doc.containsKey("doorClosePosition"))
        m_config.doorClosePosition = doc["doorClosePosition"];

    if (doc.containsKey("themometerSmokerGain"))
        m_config.themometerSmokerGain = doc["themometerSmokerGain"];
    if (doc.containsKey("themometerSmokerOffset"))
        m_config.themometerSmokerOffset = doc["themometerSmokerOffset"];
    if (doc.containsKey("themometerFoodGain"))
        m_config.themometerFoodGain = doc["themometerFoodGain"];
    if (doc.containsKey("themometerFoodOffset"))
        m_config.themometerFoodOffset = doc["themometerFoodOffset"];

    if (doc.containsKey("isThemometerSimulated"))
        m_config.isThemometerSimulated = doc["isThemometerSimulated"];

    if (doc.containsKey("isForcedFanPWM"))
        m_config.isForcedFanPWM = doc["isForcedFanPWM"];
    if (doc.containsKey("forcedFanPWM"))
        m_config.forcedFanPWM = doc["forcedFanPWM"];

    if (doc.containsKey("isForcedDoorPosition"))
        m_config.isForcedDoorPosition = doc["isForcedDoorPosition"];
    if (doc.containsKey("forcedDoorPosition"))
        m_config.forcedDoorPosition = doc["forcedDoorPosition"];

    if (doc.containsKey("isWiFiEnabled"))
        m_config.isWiFiEnabled = doc["isWiFiEnabled"];
    if (doc.containsKey("wifiSSID"))
        strncpy(m_config.wifiSSID, doc["wifiSSID"], sizeof(m_config.wifiSSID));
    if (doc.containsKey("wifiPassword"))
        strncpy(m_config.wifiPassword, doc["wifiPassword"], sizeof(m_config.wifiPassword));

    // Optionally call the callback
    if (m_configSetCallback)
    {
        String json;
        serializeJson(doc, json);
        if (!m_configSetCallback(json))
        {
            request->send(500, "application/json", "{\"error\":\"Config callback failed\"}");
            return;
        }
    }

    request->send(200, "application/json", "{\"success\":true}");

    m_isNVRAMSaveRequired = true; // Set flag to indicate NVRAM save is required
}

void WebServer::handleApiControllerStart(AsyncWebServerRequest *request)
{
    m_status.isRunning = true;
    m_status.controllerStartMSec = millis();
    request->send(200, "application/json", "{\"success\":true,\"message\":\"Controller started\"}");
}

void WebServer::handleApiControllerStop(AsyncWebServerRequest *request)
{
    m_status.isRunning = false;
    request->send(200, "application/json", "{\"success\":true,\"message\":\"Controller stopped\"}");
}