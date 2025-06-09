import React from "react";
import { DeviceStatus } from "../types/deviceStatus";
import { DeviceConfig } from "../types/deviceConfig";
import { StatusBanner } from "./StatusBanner";
import "./StatusBanner.css";
import "bootstrap-icons/font/bootstrap-icons.css";

interface StatusBarProps {
  status: DeviceStatus;
  config: DeviceConfig;
}

function getWifiIconClass(bars: number) {
  // Map bars (0-4) to Bootstrap Icons
  switch (bars) {
    case 4:
      return "bi-wifi";
    case 3:
      return "bi-wifi-2";
    case 2:
      return "bi-wifi-1";
    case 1:
      return "bi-wifi-off";
    default:
      return "bi-wifi-off";
  }
}

function getDoorPositionPercent(
  position: number,
  minPosition: number,
  maxPosition: number
) {
  // Calculate the percentage of the door position
  if (maxPosition - minPosition === 0) return 0; // Avoid division by zero
  return Math.round(
    ((position - minPosition) / (maxPosition - minPosition)) * 100
  );
}

function getFanPWMPercent(pwm: number) {
  // Calculate the percentage of the fan PWM
  return Math.round((pwm / 255) * 100); // Assuming PWM is from 0 to 255
}

export const StatusBar = ({ status, config }: StatusBarProps) => {
  return (
    <div className="row">
      <div className="col-md-3">
        <StatusBanner
          className="status-banner-fixed-height"
          color={status.isConnected ? "primary" : "secondary"}
          id="status-banner-is-connected"
        >
          {status.isConnected ? "CONNECTED" : "NOT CONNECTED"}
        </StatusBanner>
      </div>
      <div className="col-md-3">
        <StatusBanner
          className="status-banner-fixed-height"
          color={status.isRunning ? "primary" : "secondary"}
          id="status-banner-is-running"
        >
          {status.isRunning ? "CONTROLLER RUNNING" : "CONTROLLER OFF"}
        </StatusBanner>
      </div>
      <div className="col-md-3">
        <StatusBanner
          className="status-banner-fixed-height"
          color="secondary"
          id="status-banner-wifi-connected"
        >
          {status.networkName}
          <span
            className={`bi ${getWifiIconClass(status.bars)}`}
            style={{ fontSize: "1.5rem" }}
          />
        </StatusBanner>
      </div>
      <div className="col-md-3">
        <StatusBanner
          className="status-banner-fixed-height"
          color="secondary"
          id="status-banner-wifi-connected"
        >
          <div className="row">
            <div className="row">
              Door:{" "}
              {getDoorPositionPercent(
                status.doorPosition,
                config.doorClosePosition,
                config.doorOpenPosition
              )}
              %
            </div>
            <div className="row">Fan: {getFanPWMPercent(status.fanPWM)}%</div>
          </div>
        </StatusBanner>
      </div>
    </div>
  );
};
export default StatusBar;
