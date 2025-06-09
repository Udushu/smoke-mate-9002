export interface DeviceConfig {
  temperatureTarget: number;
  temperatureIntervalMSec: number;
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
  wifiSSID: string;
}

export const defaultDeviceConfig: DeviceConfig = {
  temperatureTarget: 0,
  temperatureIntervalMSec: 0,
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
  wifiSSID: "",
};