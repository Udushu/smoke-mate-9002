import "./Temperature.css";

interface TemperatureProps {
  value: number;
  unit: "C" | "F";
  id: string;
  target?: number; // Optional target temperature
  label?: string; // Optional label for the temperature
}

export const Temperature = ({
  value,
  unit,
  id,
  target,
  label,
}: TemperatureProps) => {
  return (
    <div id={id} className="instrument-readout">
      {label && <div className="instrument-label">{label}</div>}
      <span
        className="dseg"
        style={{
          fontFamily: "'DSEG7ClassicMini', monospace",
          fontSize: "2.5rem",
        }}
      >
        {value}
      </span>
      <span className="temperature-unit" style={{ marginLeft: 8 }}>
        °{unit}
      </span>
      {typeof target === "number" && (
        <span
          className="temperature-target"
          style={{ marginLeft: 16, fontSize: "1.2rem", color: "#aaa" }}
        >
          / {target}°{unit}
        </span>
      )}
    </div>
  );
};

export default Temperature;
