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
        value={form.temperatureTarget}
        type="number"
        units="°F"
        onChange={(value) =>
          setForm({ ...form, temperatureTarget: Number(value) })
        }
      />
      <ConfigField
        label="Temperature Interval"
        value={form.temperatureIntervalMSec / 1000}
        type="number"
        units="seconds"
        onChange={(value) =>
          setForm({ ...form, temperatureIntervalMSec: Number(value) * 1000 })
        }
      />
      <ConfigField
        label="Enable Temperature Profile"
        value={form.isTemperatureProfilingEnabled}
        type="checkbox"
        units=""
        onChange={(value) =>
          setForm({ ...form, isTemperatureProfilingEnabled: Boolean(value) })
        }
      />

      <details open={form.isTemperatureProfilingEnabled}>
        <summary className="mb-2">Temperature Profile Settings</summary>
        <div
          style={{
            border: "2px solid #007bff",
            borderRadius: "8px",
            padding: "16px",
            marginBottom: "16px",
          }}
        >
          <ConfigField
            label="Temperature Profile Steps"
            value={form.temperatureProfileStepsCount}
            type="number"
            units=""
            onChange={(value) =>
              setForm({ ...form, temperatureProfileStepsCount: Number(value) })
            }
          />

          {form.temperatureProfileStepsCount > 0 &&
            form.temperatureProfile.map((step, idx) => (
              <TemperatureProfileStepField
                key={idx}
                index={idx}
                duration={step.timeMSec}
                type={step.type === 0 ? "DWELL" : "RAMP"}
                temperatureStart={step.temperatureStartF}
                temperatureEnd={step.temperatureEndF}
                onChange={() => {}}
              />
            ))}
        </div>
      </details>

      <ConfigField
        label="Enable PID Control"
        value={form.isPIDEnabled}
        type="checkbox"
        onChange={(value) => setForm({ ...form, isPIDEnabled: Boolean(value) })}
      />

      <ConfigField
        label="PID Kp"
        value={form.kP}
        type="number"
        onChange={(value) => setForm({ ...form, kP: Number(value) })}
      />
      <ConfigField
        label="PID Ki"
        value={form.kI}
        type="number"
        onChange={(value) => setForm({ ...form, kI: Number(value) })}
      />
      <ConfigField
        label="PID Kd"
        value={form.kD}
        type="number"
        onChange={(value) => setForm({ ...form, kD: Number(value) })}
      />

      <ConfigField
        label="Bang-Bang Band"
        value={2 * (form.temperatureTarget - form.bangBangLowThreshold)}
        type="number"
        units="°F"
        onChange={(value) => {
          const band = Math.floor(Number(value) / 2);
          setForm({
            ...form,
            bangBangLowThreshold: form.temperatureTarget - band,
            bangBangHighThreshold: form.temperatureTarget + band,
          });
        }}
      />

      <ConfigField
        label="Bang-Bang Hysteresis"
        value={form.bangBangHysteresis}
        type="number"
        units="°F"
        onChange={(value) =>
          setForm({ ...form, bangBangHysteresis: Number(value) })
        }
      />

      <ConfigField
        label="Bang-Bang Fan Speed"
        value={form.bangBangFanSpeed}
        type="number"
        units="%"
        onChange={(value) =>
          setForm({ ...form, bangBangFanSpeed: Number(value) })
        }
      />
      <ConfigField
        label="Door Open Position"
        value={form.doorOpenPosition}
        type="number"
        units="degrees"
        onChange={(value) =>
          setForm({ ...form, doorOpenPosition: Number(value) })
        }
      />

      <ConfigField
        label="Door Close Position"
        value={form.doorClosePosition}
        type="number"
        units="degrees"
        onChange={(value) =>
          setForm({ ...form, doorClosePosition: Number(value) })
        }
      />
      <ConfigField
        label="Thermometer Smoker Gain"
        value={form.themometerSmokerGain}
        type="number"
        units="--"
        onChange={(value) =>
          setForm({ ...form, themometerSmokerGain: Number(value) })
        }
      />
      <ConfigField
        label="Thermometer Smoker Offset"
        value={form.themometerSmokerOffset}
        type="number"
        units="°C"
        onChange={(value) =>
          setForm({ ...form, themometerSmokerOffset: Number(value) })
        }
      />
      <ConfigField
        label="Thermometer Food Gain"
        value={form.themometerFoodGain}
        type="number"
        units="--"
        onChange={(value) =>
          setForm({ ...form, themometerFoodGain: Number(value) })
        }
      />
      <ConfigField
        label="Thermometer Food Offset"
        value={form.themometerFoodOffset}
        type="number"
        units="°C"
        onChange={(value) =>
          setForm({ ...form, themometerFoodOffset: Number(value) })
        }
      />
      <ConfigField
        label="Simulate Thermometer"
        value={form.isThemometerSimulated}
        type="checkbox"
        onChange={(value) =>
          setForm({ ...form, isThemometerSimulated: Boolean(value) })
        }
      />
      <ConfigField
        label="Force Fan PWM"
        value={form.isForcedFanPWM}
        type="checkbox"
        onChange={(value) =>
          setForm({ ...form, isForcedFanPWM: Boolean(value) })
        }
      />
      <ConfigField
        label="Forced Fan PWM"
        value={form.forcedFanPWM}
        type="number"
        units="PWM"
        onChange={(value) => setForm({ ...form, forcedFanPWM: Number(value) })}
      />
      <ConfigField
        label="Force Door Position"
        value={form.isForcedDoorPosition}
        type="checkbox"
        onChange={(value) =>
          setForm({ ...form, isForcedDoorPosition: Boolean(value) })
        }
      />
      <ConfigField
        label="Forced Door Position"
        value={form.forcedDoorPosition}
        type="number"
        units="degrees"
        onChange={(value) =>
          setForm({ ...form, forcedDoorPosition: Number(value) })
        }
      />

      {/* Add more fields as needed */}
      <button type="submit" className="btn btn-primary mt-2">
        Save
      </button>
    </form>
  );
};

export default ConfigEditor;
