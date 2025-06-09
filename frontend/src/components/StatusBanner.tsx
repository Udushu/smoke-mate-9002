import React from "react";

interface StatusProps {
  children?: React.ReactNode;
  color?:
    | "primary"
    | "secondary"
    | "success"
    | "danger"
    | "warning"
    | "info"
    | "light"
    | "dark"
    | "link";
  id: string;
  className?: string;
}

export const StatusBanner = ({
  children,
  color,
  id = "primary",
  className = "",
}: StatusProps) => {
  return (
    <div
      className={`alert alert-${color} ${className ? className : ""}`}
      id={id}
      style={{ fontSize: "12px" }}
    >
      {children}
    </div>
  );
};

export default StatusBanner;
