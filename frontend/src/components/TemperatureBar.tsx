import React from "react";
import Temperature from "./Temperature";
import { DeviceStatus } from "../types/deviceStatus";
import { DeviceConfig } from "../types/deviceConfig";

interface TemperatureBarProps {
  status: DeviceStatus;
  config: DeviceConfig;
}

const TemperatureBar: React.FC<TemperatureBarProps> = ({ status, config }) => (
  <div className="row justify-content-center">
    <div className="col-md-6 d-flex justify-content-center">
      <Temperature
        value={status.temperatureSmoker}
        unit="F"
        id="temperature-smoker"
        target={config.temperatureTarget}
        label="Smoker Temperature"
      />
    </div>
    <div className="col-md-6 d-flex justify-content-center">
      <Temperature
        value={status.temperatureFood}
        unit="F"
        id="temperature-temperature-food"
        label="Food Temperature"
      />
    </div>
  </div>
);

export default TemperatureBar;
