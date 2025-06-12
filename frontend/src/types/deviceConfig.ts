export interface TemperatureProfileStep {
  temperatureStartF: number;
  temperatureEndF: number;
  timeMSec: number;
  type: number | string; // Use number if backend sends enum as int, or string if as text
}

export interface DeviceConfig {
  temperatureTarget: number;
  temperatureIntervalMSec: number;
  isTemperatureProfilingEnabled: boolean;
  temperatureProfileStepsCount: number;
  temperatureProfile: TemperatureProfileStep[];
  isPIDEnabled: boolean;
  kP: number;
  kI: number;
  kD: number;
  bangBangLowThreshold: number;
  bangBangHighThreshold: number;
  bangBangHysteresis: number;
  bangBangFanSpeed: number;
  doorOpenPosition: number;
  doorClosePosition: number;
  themometerSmokerGain: number;
  themometerSmokerOffset: number;
  themometerFoodGain: number;
  themometerFoodOffset: number;
  isThemometerSimulated: boolean;
  isForcedFanPWM: boolean;
  forcedFanPWM: number;
  isForcedDoorPosition: boolean;
  forcedDoorPosition: number;
  isWiFiEnabled: boolean;

  // wifiPassword?: string; // Uncomment if you want to handle password in frontend
}

export const defaultDeviceConfig: DeviceConfig = {
  temperatureTarget: 0,
  temperatureIntervalMSec: 0,
  isTemperatureProfilingEnabled: false,
  temperatureProfileStepsCount: 0,
  temperatureProfile: [],
  isPIDEnabled: false,
  kP: 0,
  kI: 0,
  kD: 0,
  bangBangLowThreshold: 0,
  bangBangHighThreshold: 0,
  bangBangHysteresis: 0,
  bangBangFanSpeed: 0,
  doorOpenPosition: 0,
  doorClosePosition: 0,
  themometerSmokerGain: 0,
  themometerSmokerOffset: 0,
  themometerFoodGain: 0,
  themometerFoodOffset: 0,
  isThemometerSimulated: false,
  isForcedFanPWM: false,
  forcedFanPWM: 0,
  isForcedDoorPosition: false,
  forcedDoorPosition: 0,
  isWiFiEnabled: false,

  // wifiPassword: "", // Uncomment if you want to handle password in frontend
};