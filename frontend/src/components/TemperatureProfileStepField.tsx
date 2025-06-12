export interface TemperatureProfileStepFieldProps {
  index: number;
  duration: number;
  type: "DWELL" | "RAMP";
  temperatureStart: number;
  temperatureEnd: number;
  onChange: (change: { field: string; value: number | string }) => void;
}

const TemperatureProfileStepField = ({
  index,
  duration,
  type,
  temperatureStart,
  temperatureEnd,
  onChange,
}: TemperatureProfileStepFieldProps) => {
  return (
    <div className="input-group mb-2 align-items-center">
      <span className="input-group-text">{index + 1}</span>
      <select
        className="form-select"
        value={type}
        onChange={(e) => onChange({ field: "type", value: e.target.value })}
        aria-label="Temperature step type"
      >
        <option value="DWELL">Dwell</option>
        <option value="RAMP">Ramp</option>
      </select>
      <input
        type="number"
        className="form-control"
        value={Math.round(duration / 60000)}
        onChange={(e) =>
          onChange({
            field: "duration",
            value: Number(e.target.value) * 60000,
          })
        }
        aria-label="Duration (minutes)"
        placeholder="Duration (min)"
        min={0}
      />
      <span className="input-group-text">min</span>
      <input
        type="number"
        className="form-control"
        value={temperatureStart}
        onChange={(e) =>
          onChange({
            field: "temperatureStart",
            value: Number(e.target.value),
          })
        }
        aria-label="Temperature start (°C)"
        placeholder="Start °C"
      />
      <span className="input-group-text">°C</span>
      <input
        type="number"
        className="form-control col-md-2"
        value={type === "DWELL" ? temperatureStart : temperatureEnd}
        onChange={(e) =>
          onChange({
            field: "temperatureEnd",
            value: Number(e.target.value),
          })
        }
        aria-label="Temperature end (°C)"
        placeholder="End °C"
        disabled={type === "DWELL"}
      />
      <span className="input-group-text">°C</span>
    </div>
  );
};

export default TemperatureProfileStepField;
