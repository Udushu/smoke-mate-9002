import React, { useState } from "react";

interface ConfigFieldProps {
  label: string;
  value: string | number | boolean;
  type?: "text" | "number" | "checkbox";
  units?: string;
  onChange: (value: string | number | boolean) => void;
}

const ConfigField: React.FC<ConfigFieldProps> = ({
  label,
  value,
  type = "text",
  units = "",
  onChange,
}) => {
  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const newValue = type === "checkbox" ? e.target.checked : e.target.value;
    onChange(type === "number" ? Number(newValue) : newValue);
  };

  return (
    <>
      <div className="input-group">
        <div className="input-group-text col-md-5">
          <span className="form-label">{label}</span>
        </div>

        {type === "checkbox" ? (
          <div className="form-check form-switch ms-3" style={{ zoom: 1.5 }}>
            <input
              type="checkbox"
              className="form-check-input"
              checked={Boolean(value)}
              onChange={handleChange}
            />
          </div>
        ) : (
          <input
            type={type}
            className="form-control"
            value={String(value)}
            onChange={handleChange}
          />
        )}

        {units === "" ? null : <div className="input-group-text">{units}</div>}
      </div>
      <p />
    </>
  );
};

export default ConfigField;
