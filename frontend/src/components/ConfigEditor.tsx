import React, { useState } from "react";
import { DeviceConfig } from "../types/deviceConfig";
import ConfigField from "./ConfigField";
import TemperatureProfileStepField from "./TemperatureProfileStepField";

interface ConfigEditorProps {
  config: DeviceConfig;
  onSave: (newConfig: DeviceConfig) => void;
}

const ConfigEditor: React.FC<ConfigEditorProps> = ({ config, onSave }) => {
  const [form, setForm] = useState<DeviceConfig>(config);

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value, type, checked } = e.target;
    setForm((prev) => ({
      ...prev,
      [name]:
        type === "checkbox"
          ? checked
          : isNaN(Number(value))
          ? value
          : Number(value),
    }));
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    onSave(form);
  };

  return (
    <form onSubmit={handleSubmit} className="p-3 border rounded">
      <ConfigField
        label="Temperature Target"
        value={config.temperatureTarget}
        type="number"
        units="°F"
        onChange={(value) =>
          setForm({ ...form, temperatureTarget: Number(value) })
        }
      />
      <ConfigField
        label="Temperature Interval"
        value={config.temperatureIntervalMSec / 1000}
        type="number"
        units="seconds"
        onChange={(value) =>
          setForm({ ...form, temperatureIntervalMSec: Number(value) * 1000 })
        }
      />
      <ConfigField
        label="Enable Temperature Profile"
        value={config.isTemperatureProfilingEnabled}
        type="checkbox"
        units=""
        onChange={(value) =>
          setForm({ ...form, isTemperatureProfilingEnabled: Boolean(value) })
        }
      />
      <ConfigField
        label="Temperature Profile Steps"
        value={config.temperatureProfileStepsCount}
        type="number"
        units=""
        onChange={(value) =>
          setForm({ ...form, temperatureProfileStepsCount: Number(value) })
        }
      />

      {config.temperatureProfileStepsCount > 0 &&
        config.temperatureProfile.map((step, idx) => (
          <TemperatureProfileStepField
            index={idx}
            duration={step.timeMSec}
            type={step.type === 0 ? "DWELL" : "RAMP"}
            temperatureStart={step.temperatureStartF}
            temperatureEnd={step.temperatureEndF}
            onChange={() => {}}
          />
        ))}

      {/* Add more fields as needed */}
      <button type="submit" className="btn btn-primary mt-2">
        Save
      </button>
    </form>
  );
};

export default ConfigEditor;
