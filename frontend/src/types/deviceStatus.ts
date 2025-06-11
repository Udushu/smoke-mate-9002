export interface DeviceStatus {
  timestamp: string | null;
  isConnected: boolean;
  isRunning: boolean;
  uuid: string;
  uptime: number;
  controllerStartMSec: number;
  temperatureSmoker: number;
  temperatureFood: number;
  temperatureTarget: number;
  fanPWM: number;
  doorPosition: number;
  RSSI: number;
  bars: number;
  ipAddress: string;
  isWiFiConnected: boolean;
  networkName: string;
  temperatureError: number;
  isProfileRunning: string;
  temperatureProfileStepIndex: number;
  temperatureProfileStartTimeMSec: number;
  temperatureProfileStepsCount: number;
  temperatureProfileStepType: string;
}

export const defaultDeviceStatus: DeviceStatus = {
  timestamp: null,
  isConnected: false,
  isRunning: false,
  uuid: '',
  uptime: 0,
  controllerStartMSec: 0,
  temperatureSmoker: 0,
  temperatureFood: 0,
  temperatureTarget: 0,
  fanPWM: 0,
  doorPosition: 0,
  RSSI: 0,
  bars: 0,
  ipAddress: '',
  isWiFiConnected: false,
  networkName: '',
  temperatureError: 0,
  isProfileRunning: 'UNKNOWN',
  temperatureProfileStepIndex: -1,
  temperatureProfileStartTimeMSec: 0,
  temperatureProfileStepsCount: 0,
  temperatureProfileStepType: 'UNKNOWN',
};