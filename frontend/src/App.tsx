import React, { useEffect, useState } from "react";
import { DeviceStatus, defaultDeviceStatus } from "./types/deviceStatus";
import { DeviceConfig, defaultDeviceConfig } from "./types/deviceConfig";
import { StatusBar } from "./components/StatusBar";
import TemperatureBar from "./components/TemperatureBar";
import TemperatureHistoryChart from "./components/TemperatureHistoryChart";
import Button from "./components/Button";
import ConfigEditor from "./components/ConfigEditor";

const BACKEND_URL = "http://192.168.2.157:5000";
const BACKEND_POLLING_INTERVAL = 250; // 0.25 second

const App: React.FC = () => {
  const [status, setStatus] = useState<DeviceStatus>(defaultDeviceStatus);
  const [history, setHistory] = useState<DeviceStatus[]>([]);
  const [config, setConfig] = useState<DeviceConfig>(defaultDeviceConfig);
  const [showConfig, setShowConfig] = useState(false);

  // Fetch the device status every second to update the UI in real-time
  useEffect(() => {
    // Fetch the device status every second
    const interval = setInterval(() => {
      // Fetch the device status
      fetch(BACKEND_URL + "/status")
        .then((response) => {
          // Check if the response is OK
          if (!response.ok) {
            // If not, throw an error
            throw new Error(`HTTP error! status: ${response.status}`);
          }
          // If the response is OK, return the JSON
          return response.json();
        })
        .then((data) => {
          // Set the device status and update the state
          // Unpack the data.status into DeviceStatus object
          const deviceStatus: DeviceStatus = {
            timestamp: data.timestamp,
            isConnected: data.isConnected,
            isRunning: data.isRunning,
            uuid: data.uuid,
            uptime: data.uptime,
            controllerStartMSec: data.controllerStartMSec,
            temperatureSmoker: data.temperatureSmoker,
            temperatureFood: data.temperatureFood,
            temperatureTarget: data.temperatureTarget,
            fanPWM: data.fanPWM,
            doorPosition: data.doorPosition,
            RSSI: data.RSSI,
            bars: data.bars,
            ipAddress: data.ipAddress,
            isWiFiConnected: data.isWiFiConnected,
            networkName: data.networkName,
            temperatureError: data.temperatureError,
            isProfileRunning: data.isProfileRunning,
            temperatureProfileStepIndex: data.temperatureProfileStepIndex,
            temperatureProfileStartTimeMSec:
              data.temperatureProfileStartTimeMSec,
            temperatureProfileStepsCount: data.temperatureProfileStepsCount,
            temperatureProfileStepType: data.temperatureProfileStepType,
          };
          setStatus(deviceStatus);
          // console.log("Is connected: ", isConnected, "Is running: ", isRunning);
          // console.log(deviceStatus);
        })
        .catch((error) => console.log("Error:", error));
    }, BACKEND_POLLING_INTERVAL); // Fetch every 1 second

    // Cleanup function to clear the interval when the component is unmounted
    return () => clearInterval(interval);
  }, []);

  // Fetch the device config every second to update the UI in real-time
  useEffect(() => {
    // Fetch the device status every second
    const interval = setInterval(() => {
      // Fetch the device status
      fetch(BACKEND_URL + "/config")
        .then((response) => {
          // Check if the response is OK
          if (!response.ok) {
            // If not, throw an error
            throw new Error(`HTTP error! status: ${response.status}`);
          }
          // If the response is OK, return the JSON
          return response.json();
        })
        .then((data) => {
          // Unpack the data into DeviceConfig object
          const deviceConfig: DeviceConfig = {
            temperatureTarget: data.temperatureTarget,
            temperatureIntervalMSec: data.temperatureIntervalMSec,
            isTemperatureProfilingEnabled: data.isTemperatureProfilingEnabled,
            temperatureProfileStepsCount: data.temperatureProfileStepsCount,
            temperatureProfile: Array.isArray(data.temperatureProfile)
              ? data.temperatureProfile.map((step: any) => ({
                  temperatureStartF: step.temperatureStartF,
                  temperatureEndF: step.temperatureEndF,
                  timeMSec: step.timeMSec,
                  type: step.type,
                }))
              : [],
            isPIDEnabled: data.isPIDEnabled,
            kP: data.kP,
            kI: data.kI,
            kD: data.kD,
            bangBangLowThreshold: data.bangBangLowThreshold,
            bangBangHighThreshold: data.bangBangHighThreshold,
            bangBangHysteresis: data.bangBangHysteresis,
            bangBangFanSpeed: data.bangBangFanSpeed,
            doorOpenPosition: data.doorOpenPosition,
            doorClosePosition: data.doorClosePosition,
            themometerSmokerGain: data.themometerSmokerGain,
            themometerSmokerOffset: data.themometerSmokerOffset,
            themometerFoodGain: data.themometerFoodGain,
            themometerFoodOffset: data.themometerFoodOffset,
            isThemometerSimulated: data.isThemometerSimulated,
            isForcedFanPWM: data.isForcedFanPWM,
            forcedFanPWM: data.forcedFanPWM,
            isForcedDoorPosition: data.isForcedDoorPosition,
            forcedDoorPosition: data.forcedDoorPosition,
            isWiFiEnabled: data.isWiFiEnabled,
          };
          setConfig(deviceConfig);
        })
        .catch((error) => console.log("Error:", error));
    }, BACKEND_POLLING_INTERVAL); // Fetch every 1 second

    // Cleanup function to clear the interval when the component is unmounted
    return () => clearInterval(interval);
  }, []);

  // Fetch the device status every second to update the UI in real-time
  useEffect(() => {
    // Fetch the device status every second
    const interval = setInterval(() => {
      // Fetch the device status
      fetch(BACKEND_URL + "/run-status-history")
        .then((response) => {
          // Check if the response is OK
          if (!response.ok) {
            // If not, throw an error
            throw new Error(`HTTP error! status: ${response.status}`);
          }
          // If the response is OK, return the JSON
          return response.json();
        })
        .then((data) => {
          // Unpack the data into an array of DeviceStatus objects
          const runStatusHistory: DeviceStatus[] = data.map((item: any) => ({
            timeStamp: new Date(item.timestamp), // Convert timestamp to Date object
            isConnected: item.isConnected,
            RSSI: item.RSSI,
            bars: item.bars,
            controllerStartMSec: item.controllerStartMSec,
            doorPosition: item.doorPosition,
            fanPWM: item.fanPWM,
            ipAddress: item.ipAddress,
            isRunning: item.isRunning,
            isWiFiConnected: item.isWiFiConnected,
            temperatureFood: item.temperatureFood,
            temperatureSmoker: item.temperatureSmoker,
            temperatureTarget: item.temperatureTarget,
            uptime: item.uptime,
            uuid: item.uuid,
            networkName: item.networkName,
            temperatureError: item.temperatureError,
          }));
          setHistory(runStatusHistory);
        })
        .catch((error) => console.log("Error:", error));
    }, BACKEND_POLLING_INTERVAL); // Fetch every 1 second

    // Cleanup function to clear the interval when the component is unmounted
    return () => clearInterval(interval);
  }, []);

  const handleStart = async () => {
    try {
      await fetch(BACKEND_URL + "/start", { method: "POST" });
    } catch (error) {
      console.error("Failed to start controller:", error);
    }
  };

  const handleStop = async () => {
    try {
      await fetch(BACKEND_URL + "/stop", { method: "POST" });
    } catch (error) {
      console.error("Failed to stop controller:", error);
    }
  };

  const handleConfigSave = async (newConfig: DeviceConfig) => {
    try {
      await fetch(BACKEND_URL + "/config", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(newConfig),
      });
      // Optionally update local state
      setConfig(newConfig);
    } catch (error) {
      console.error("Failed to save config:", error);
    }
  };

  return (
    <div className="container mt-5">
      <div className="d-flex align-items-center justify-content-left">
        <img
          src="icon.svg"
          alt="Logo"
          className="mb-3"
          style={{ width: 80, height: 80 }}
        />
        <h1 className="text-primary">SmokeMate</h1>
      </div>
      <StatusBar status={status} config={config} />
      <hr />
      <div className="row justify-content-center">
        <TemperatureBar status={status} config={config} />
      </div>
      <hr />
      <div className="row justify-content-center">
        <TemperatureHistoryChart history={history} />
      </div>
      <div className="row justify-content-center">
        <p />
      </div>
      <div className="row justify-content-center">
        <Button
          active={status.isRunning}
          onClick={status.isRunning ? handleStop : handleStart}
        >
          {status.isRunning ? "Stop Controller" : "Start Controller"}
        </Button>
      </div>
      <div className="row justify-content-center mt-3">
        <Button active={showConfig} onClick={() => setShowConfig((v) => !v)}>
          {showConfig ? "Hide Configuration" : "Edit Configuration"}
        </Button>
      </div>
      {showConfig && (
        <div className="row justify-content-center mt-3">
          <ConfigEditor config={config} onSave={handleConfigSave} />
        </div>
      )}
    </div>
  );
};

export default App;
