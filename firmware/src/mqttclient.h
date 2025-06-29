#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <AsyncMqttClient.h>
#include <WiFi.h>
#include "types.h"

class SmokeMatesMqttClient
{
private:
    AsyncMqttClient m_mqttClient;
    Configuration &m_config;
    ControllerStatus &m_controllerStatus;

    ulong m_lastPublishTime;
    bool m_isConnected;
    bool m_shouldReconnect;
    ulong m_reconnectTimer;

    // Topic strings
    String m_topicPrefix;
    String m_topicStatus;
    String m_topicTemperatures;
    String m_topicControl;
    String m_topicAvailability;

    void buildTopics();
    void publishStatus();
    void publishTemperatures();
    void publishAvailability(bool online);

    // Command handlers
    void handleTargetTemperatureCommand(const String &payload);
    void handleRunningCommand(const String &payload);
    void handleFanCommand(const String &payload);
    void handleDoorCommand(const String &payload);

public:
    SmokeMatesMqttClient(Configuration &config, ControllerStatus &status);

    void begin();
    void service(ulong currentTimeMSec);
    void reconnect();
    void disconnect();

    bool isConnected() const { return m_isConnected; }

    // Callback handlers
    void onConnect(bool sessionPresent);
    void onDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
};

#endif // MQTTCLIENT_H