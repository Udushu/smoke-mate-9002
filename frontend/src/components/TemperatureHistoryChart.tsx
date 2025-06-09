import React, { useState } from "react";
import { Line } from "react-chartjs-2";
import {
  Chart,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Legend,
  Tooltip,
  TimeScale,
} from "chart.js";
import "chartjs-adapter-date-fns";
import { DeviceStatus } from "../types/deviceStatus";
import "./Temperature.css";

// Register Chart.js components
Chart.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Legend,
  Tooltip,
  TimeScale
);

interface TemperatureHistoryChartProps {
  history: DeviceStatus[];
}

const TemperatureHistoryChart: React.FC<TemperatureHistoryChartProps> = ({
  history,
}) => {
  const [showFan, setShowFan] = useState(false);
  const [showError, setShowError] = useState(false);

  console.log(history);

  const data = {
    labels: history.map((item: DeviceStatus) => item.timeStamp),
    datasets: [
      {
        label: "Smoker Temp",
        data: history.map((item: DeviceStatus) => item.temperatureSmoker),
        borderColor: "#fd7e14",
        backgroundColor: "rgba(253,126,20,0.1)",
        tension: 0.2,
        yAxisID: "y",
      },
      {
        label: "Food Temp",
        data: history.map((item: DeviceStatus) => item.temperatureFood),
        borderColor: "#0d6efd",
        backgroundColor: "rgba(13,110,253,0.1)",
        tension: 0.2,
        yAxisID: "y",
      },
      {
        label: "Target Temp",
        data: history.map((item: DeviceStatus) => item.temperatureTarget),
        borderColor: "#198754",
        backgroundColor: "rgba(25,135,84,0.1)",
        borderDash: [5, 5],
        tension: 0.2,
        pointRadius: 0,
        pointHoverRadius: 0,
        yAxisID: "y",
      },
      ...(showFan
        ? [
            {
              label: "Fan Power",
              data: history.map((item: DeviceStatus) =>
                Math.round((item.fanPWM / 255) * 100)
              ),
              borderColor: "#ffc107",
              backgroundColor: "rgba(255,193,7,0.1)",
              tension: 0.2,
              pointRadius: 0,
              pointHoverRadius: 0,
              yAxisID: "y1",
              borderWidth: 1,
            },
          ]
        : []),
      ...(showError
        ? [
            {
              label: "Temperature Error",
              data: history.map((item: DeviceStatus) => item.temperatureError),
              borderColor: "#dc3545",
              backgroundColor: "rgba(220,53,69,0.1)",
              tension: 0.2,
              pointRadius: 0,
              pointHoverRadius: 0,
              yAxisID: "y2",
              borderWidth: 1,
            },
          ]
        : []),
    ],
  };

  const options = {
    responsive: true,
    plugins: {
      legend: { position: "top" as const },
      tooltip: { mode: "index" as const, intersect: false },
    },
    scales: {
      x: {
        type: "time" as const,
        time: { unit: "minute" as const, tooltipFormat: "HH:mm:ss" },
      },
      y: {
        title: { display: true, text: "Temperature (°F)" },
        beginAtZero: true,
        position: "left" as const,
      },
      y1: {
        title: {
          display: true,
          text: "Fan Power",
          color: "#ffc107",
        },
        ticks: {
          color: "#ffc107",
          callback: (tickValue: string | number) => `${tickValue}%`,
        },
        beginAtZero: true,
        max: 100,
        position: "right" as const,
        grid: { drawOnChartArea: false },
        display: showFan,
      },
      y2: {
        title: {
          display: true,
          text: "Temperature Error",
          color: "#dc3545",
        },
        ticks: {
          color: "#dc3545",
        },
        beginAtZero: true,
        position: "right" as const,
        grid: { drawOnChartArea: false },
        display: showError,
        offset: true, // offset so it doesn't overlap with y1
      },
    },
  };

  return (
    <div className="instrument-readout">
      <div className="instrument-label">Temperature History</div>
      <Line data={data} options={options} height={160} />
      <div className="d-flex justify-content-center mt-2 gap-2">
        <button
          className="btn btn-sm btn-outline-warning"
          onClick={() => setShowFan((v) => !v)}
        >
          {showFan ? "Hide Fan PWM" : "Show Fan PWM"}
        </button>
        <button
          className="btn btn-sm btn-outline-danger"
          onClick={() => setShowError((v) => !v)}
        >
          {showError ? "Hide Temp Error" : "Show Temp Error"}
        </button>
      </div>
    </div>
  );
};

export default TemperatureHistoryChart;
