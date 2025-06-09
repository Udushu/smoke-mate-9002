export interface DeviceStatus {
    timeStamp: Date;
    isConnected: boolean;
    isRunning: boolean;
    controllerStartMSec: number;
    uptime: number;
    temperatureFood: number;
    temperatureSmoker: number;
    temperatureTarget: number;
    doorPosition: number;
    fanPWM: number;
    isWiFiConnected: boolean;
    ipAddress: string;
    RSSI: number;
    bars: number;
    uuid: string;
    networkName: string;
    temperatureError: number;
}

export const defaultDeviceStatus: DeviceStatus = {
    timeStamp: new Date(),
    isConnected: false,
    isRunning: false,
    controllerStartMSec: 0,
    uptime: 0,
    temperatureFood: 0,
    temperatureSmoker: 0,
    temperatureTarget: 0,
    doorPosition: 0,
    fanPWM: 0,
    isWiFiConnected: false,
    ipAddress: "",
    RSSI: 0,
    bars: 0,
    uuid: "",
    networkName: "",
    temperatureError: 0,
};