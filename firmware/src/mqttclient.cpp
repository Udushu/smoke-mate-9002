#include "mqttclient.h"
#include <ArduinoJson.h>

SmokeMatesMqttClient::SmokeMatesMqttClient(Configuration &config, ControllerStatus &status)
    : m_config(config), m_controllerStatus(status)
{
    m_lastPublishTime = 0;
    m_isConnected = false;
    m_shouldReconnect = false;
    m_reconnectTimer = 0;
}

void SmokeMatesMqttClient::begin()
{
    if (!m_config.isMqttEnabled)
    {
        Serial.println("MQTT: Disabled in configuration");
        return;
    }

    Serial.println("MQTT: Initializing client...");

    // Build topic strings
    buildTopics();

    // Set up MQTT client callbacks using lambda functions
    m_mqttClient.onConnect([this](bool sessionPresent)
                           { this->onConnect(sessionPresent); });

    m_mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason)
                              { this->onDisconnect(reason); });

    m_mqttClient.onMessage([this](char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
                           { this->onMessage(topic, payload, properties, len, index, total); });

    // Configure connection parameters
    m_mqttClient.setServer(m_config.mqttBroker, m_config.mqttPort);
    m_mqttClient.setClientId(m_config.mqttClientId);

    // Set credentials if provided
    if (strlen(m_config.mqttUsername) > 0)
    {
        m_mqttClient.setCredentials(m_config.mqttUsername, m_config.mqttPassword);
        Serial.printf("MQTT: Using authentication for user: %s\n", m_config.mqttUsername);
    }

    // Set keep alive and clean session
    m_mqttClient.setKeepAlive(60);
    m_mqttClient.setCleanSession(true);

    Serial.printf("MQTT: Connecting to %s:%d as %s\n",
                  m_config.mqttBroker, m_config.mqttPort, m_config.mqttClientId);

    // Initial connection attempt
    m_mqttClient.connect();
}

void SmokeMatesMqttClient::service(ulong currentTimeMSec)
{
    if (!m_config.isMqttEnabled)
    {
        return;
    }

    // Handle reconnection if needed
    if (m_shouldReconnect && (currentTimeMSec - m_reconnectTimer >= 5000))
    {
        Serial.println("MQTT: Attempting reconnection...");
        m_mqttClient.connect();
        m_shouldReconnect = false;
        m_reconnectTimer = currentTimeMSec;
    }

    // Publish data at configured interval
    if (m_isConnected &&
        (currentTimeMSec - m_lastPublishTime >= (m_config.mqttPublishInterval * 1000)))
    {

        publishStatus();
        publishTemperatures();

        m_lastPublishTime = currentTimeMSec;
    }
}

void SmokeMatesMqttClient::buildTopics()
{
    m_topicPrefix = String(m_config.mqttTopicPrefix);
    m_topicStatus = m_topicPrefix + "/status";
    m_topicTemperatures = m_topicPrefix + "/temperatures";
    m_topicControl = m_topicPrefix + "/control";
    m_topicAvailability = m_topicPrefix + "/availability";

    Serial.printf("MQTT: Topic prefix: %s\n", m_topicPrefix.c_str());
}

void SmokeMatesMqttClient::publishStatus()
{
    if (!m_isConnected)
        return;

    // Create JSON status payload
    StaticJsonDocument<512> doc;

    doc["running"] = m_controllerStatus.isRunning;
    doc["uuid"] = m_controllerStatus.uuid;
    doc["uptime"] = m_controllerStatus.uptime;
    doc["controller_start"] = m_controllerStatus.controllerStartMSec;
    doc["fan_pwm"] = m_controllerStatus.fanPWM;
    doc["door_position"] = m_controllerStatus.doorPosition;

    // Network status
    JsonObject network = doc.createNestedObject("network");
    network["wifi_connected"] = m_controllerStatus.isWiFiConnected;
    network["rssi"] = m_controllerStatus.RSSI;
    network["bars"] = m_controllerStatus.bars;
    network["ip_address"] = m_controllerStatus.ipAddress;
    network["network_name"] = m_controllerStatus.networkName;

    // Profile status
    JsonObject profile = doc.createNestedObject("profile");
    profile["running"] = m_controllerStatus.isProfileRunning;
    profile["step_index"] = m_controllerStatus.temperatureProfileStepIndex;
    profile["start_time"] = m_controllerStatus.temperatureProfileStartTimeMSec;
    profile["steps_count"] = m_controllerStatus.temperatureProfileStepsCount;
    profile["step_type"] = (m_controllerStatus.temperatureProfileStepType == TEMP_PROFILE_TYPE_DWELL) ? "DWELL" : "RAMP";

    String payload;
    serializeJson(doc, payload);

    String topic = m_topicStatus + "/state";
    m_mqttClient.publish(topic.c_str(), 0, m_config.isMqttRetain, payload.c_str());

    // Individual status topics for easier consumption
    m_mqttClient.publish((m_topicStatus + "/running").c_str(), 0, m_config.isMqttRetain,
                         m_controllerStatus.isRunning ? "true" : "false");

    m_mqttClient.publish((m_topicStatus + "/uptime").c_str(), 0, false,
                         String(m_controllerStatus.uptime).c_str());
}

void SmokeMatesMqttClient::publishTemperatures()
{
    if (!m_isConnected)
        return;

    // Create JSON temperature payload
    StaticJsonDocument<256> doc;

    doc["smoker"] = m_controllerStatus.temperatureSmoker;
    doc["food"] = m_controllerStatus.temperatureFood;
    doc["target"] = m_controllerStatus.temperatureTarget;
    doc["error"] = m_controllerStatus.temperatureError;
    doc["timestamp"] = millis();

    String payload;
    serializeJson(doc, payload);

    String topic = m_topicTemperatures + "/state";
    m_mqttClient.publish(topic.c_str(), 0, false, payload.c_str());

    // Individual temperature topics
    m_mqttClient.publish((m_topicTemperatures + "/smoker").c_str(), 0, false,
                         String(m_controllerStatus.temperatureSmoker).c_str());

    m_mqttClient.publish((m_topicTemperatures + "/food").c_str(), 0, false,
                         String(m_controllerStatus.temperatureFood).c_str());

    m_mqttClient.publish((m_topicTemperatures + "/target").c_str(), 0, m_config.isMqttRetain,
                         String(m_controllerStatus.temperatureTarget).c_str());
}

void SmokeMatesMqttClient::publishAvailability(bool online)
{
    if (!m_mqttClient.connected())
        return;

    String payload = online ? "online" : "offline";
    m_mqttClient.publish(m_topicAvailability.c_str(), 0, true, payload.c_str());

    Serial.printf("MQTT: Published availability: %s\n", payload.c_str());
}

void SmokeMatesMqttClient::reconnect()
{
    if (!m_config.isMqttEnabled)
        return;

    Serial.println("MQTT: Manual reconnection requested");
    m_mqttClient.disconnect(true);
    delay(1000);
    m_mqttClient.connect();
}

void SmokeMatesMqttClient::disconnect()
{
    if (m_isConnected)
    {
        publishAvailability(false);
        m_mqttClient.disconnect();
    }
}

void SmokeMatesMqttClient::onConnect(bool sessionPresent)
{
    Serial.println("MQTT: Connected to broker");
    Serial.printf("MQTT: Session present: %s\n", sessionPresent ? "true" : "false");

    m_isConnected = true;
    m_shouldReconnect = false;

    // Subscribe to control topics
    String targetTopic = m_topicControl + "/target/set";
    String runningTopic = m_topicControl + "/running/set";
    String fanTopic = m_topicControl + "/fan/set";
    String doorTopic = m_topicControl + "/door/set";

    m_mqttClient.subscribe(targetTopic.c_str(), 0);
    m_mqttClient.subscribe(runningTopic.c_str(), 0);
    m_mqttClient.subscribe(fanTopic.c_str(), 0);
    m_mqttClient.subscribe(doorTopic.c_str(), 0);

    Serial.printf("MQTT: Subscribed to control topics:\n");
    Serial.printf("  - %s\n", targetTopic.c_str());
    Serial.printf("  - %s\n", runningTopic.c_str());
    Serial.printf("  - %s\n", fanTopic.c_str());
    Serial.printf("  - %s\n", doorTopic.c_str());

    // Publish availability and initial state
    publishAvailability(true);
    publishStatus();
    publishTemperatures();
}

void SmokeMatesMqttClient::onDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Serial.printf("MQTT: Disconnected from broker, reason: %d\n", (int)reason);

    m_isConnected = false;

    // Schedule reconnection attempt
    m_shouldReconnect = true;
    m_reconnectTimer = millis();

    // Print reason for debugging
    switch (reason)
    {
    case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
        Serial.println("MQTT: TCP connection lost");
        break;
    case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
        Serial.println("MQTT: Unacceptable protocol version");
        break;
    case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
        Serial.println("MQTT: Client identifier rejected");
        break;
    case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
        Serial.println("MQTT: Server unavailable");
        break;
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
        Serial.println("MQTT: Malformed credentials");
        break;
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
        Serial.println("MQTT: Not authorized");
        break;
    default:
        Serial.println("MQTT: Unknown disconnect reason");
        break;
    }
}

void SmokeMatesMqttClient::onMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    // Create null-terminated payload string
    String payloadStr = "";
    for (size_t i = 0; i < len; i++)
    {
        payloadStr += (char)payload[i];
    }

    String topicStr = String(topic);

    Serial.printf("MQTT: Received [%s] %s\n", topic, payloadStr.c_str());

    // Handle control commands
    if (topicStr.endsWith("/target/set"))
    {
        handleTargetTemperatureCommand(payloadStr);
    }
    else if (topicStr.endsWith("/running/set"))
    {
        handleRunningCommand(payloadStr);
    }
    else if (topicStr.endsWith("/fan/set"))
    {
        handleFanCommand(payloadStr);
    }
    else if (topicStr.endsWith("/door/set"))
    {
        handleDoorCommand(payloadStr);
    }
    else
    {
        Serial.printf("MQTT: Unknown command topic: %s\n", topic);
    }
}

void SmokeMatesMqttClient::handleTargetTemperatureCommand(const String &payload)
{
    int newTarget = payload.toInt();

    // Validate temperature range
    if (newTarget >= 80 && newTarget <= 500)
    {
        m_controllerStatus.temperatureTarget = newTarget;
        Serial.printf("MQTT: Target temperature set to %d°F\n", newTarget);

        // Publish confirmation to the control topic (for legacy/UI compatibility)
        String controlTopic = m_topicControl + "/target";
        m_mqttClient.publish(controlTopic.c_str(), 0, m_config.isMqttRetain, String(newTarget).c_str());

        // Publish state to the temperature topic used by Home Assistant's mqtt.number
        String stateTopic = m_topicTemperatures + "/target";
        m_mqttClient.publish(stateTopic.c_str(), 0, m_config.isMqttRetain, String(newTarget).c_str());
    }
    else
    {
        Serial.printf("MQTT: Invalid target temperature: %d°F (range: 80-500°F)\n", newTarget);
    }
}

void SmokeMatesMqttClient::handleRunningCommand(const String &payload)
{
    String lowerPayload = payload;
    lowerPayload.toLowerCase();

    bool newState = false;

    if (lowerPayload == "true" || lowerPayload == "1" || lowerPayload == "on" || lowerPayload == "start")
    {
        newState = true;
    }
    else if (lowerPayload == "false" || lowerPayload == "0" || lowerPayload == "off" || lowerPayload == "stop")
    {
        newState = false;
    }
    else
    {
        Serial.printf("MQTT: Invalid running command: %s\n", payload.c_str());
        return;
    }

    if (newState != m_controllerStatus.isRunning)
    {
        m_controllerStatus.isRunning = newState;
        if (newState)
        {
            m_controllerStatus.controllerStartMSec = millis();
            Serial.println("MQTT: Controller started");
        }
        else
        {
            Serial.println("MQTT: Controller stopped");
        }

        // Publish confirmation
        String topic = m_topicControl + "/running";
        m_mqttClient.publish(topic.c_str(), 0, m_config.isMqttRetain, newState ? "true" : "false");
    }
}

void SmokeMatesMqttClient::handleFanCommand(const String &payload)
{
    int fanPWM = payload.toInt();

    // Validate PWM range
    if (fanPWM >= 0 && fanPWM <= 255)
    {
        m_controllerStatus.fanPWM = fanPWM;
        Serial.printf("MQTT: Fan PWM set to %d\n", fanPWM);

        // Publish confirmation
        String topic = m_topicControl + "/fan";
        m_mqttClient.publish(topic.c_str(), 0, false, String(fanPWM).c_str());
    }
    else
    {
        Serial.printf("MQTT: Invalid fan PWM: %d (range: 0-255)\n", fanPWM);
    }
}

void SmokeMatesMqttClient::handleDoorCommand(const String &payload)
{
    int doorPosition = payload.toInt();

    // Validate door position range
    if (doorPosition >= 0 && doorPosition <= 180)
    {
        m_controllerStatus.doorPosition = doorPosition;
        Serial.printf("MQTT: Door position set to %d degrees\n", doorPosition);

        // Publish confirmation
        String topic = m_topicControl + "/door";
        m_mqttClient.publish(topic.c_str(), 0, false, String(doorPosition).c_str());
    }
    else
    {
        Serial.printf("MQTT: Invalid door position: %d (range: 0-180 degrees)\n", doorPosition);
    }
}