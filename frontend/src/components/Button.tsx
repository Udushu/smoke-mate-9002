import React from "react";
import "./Button.css"; // Import the CSS for styling

interface InstrumentButtonProps
  extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  active?: boolean;
  children: React.ReactNode;
}

const InstrumentButton: React.FC<InstrumentButtonProps> = ({
  active,
  children,
  ...props
}) => (
  <button
    className={`instrument-button${active ? " instrument-button-active" : ""}`}
    {...props}
  >
    {children}
  </button>
);

export default InstrumentButton;
