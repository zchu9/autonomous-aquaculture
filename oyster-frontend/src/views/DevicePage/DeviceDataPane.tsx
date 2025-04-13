import * as React from "react";
import { useEffect } from "react";
import Stack from "@mui/material/Stack";
import Box from "@mui/material/Box";
import {
  ResponsiveContainer,
  LineChart,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
  Line,
  Legend,
} from "recharts";
import Paper from "@mui/material/Paper";
import { styled } from "@mui/material/styles";
import Typography from "@mui/material/Typography";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface DeviceDataPaneProps {
  uuid: string;
}

interface SensorData {
  created_at: Date;
  height: number;
  temperature: number;
}

interface SystemLevels {
  battery_temp: number;
  battery_time: Date;
  battery_voltage: number;
  solar_panel_power: number;
}

const DataPane = styled(Paper)(({ theme }) => ({
  padding: theme.spacing(2),
  display: "flex",
  flexDirection: "column",
  overflow: "auto",
  flexGrow: 1,
}));

export function DeviceDataPane(props: DeviceDataPaneProps) {
  const [objectData, setObjectData] = React.useState<SensorData>();
  const [systemLevels, setSystemLevels] = React.useState<SystemLevels>();
  const [cameraStr, setCameraStr] = React.useState<string>("");

  async function fetchSensorData() {
    console.log(`Fetching device data from ${base_url}...`);

    try {
      const response = await fetch(
        base_url + "/farm/" + props.uuid + "/getAllSensorData",
        {
          method: "GET",
        }
      );
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const data = await response.json();

      setObjectData(
        data.map((item: any) => {
          return {
            created_at: new Date(item.created_at),
            height: item.height,
            temperature: item.temperature,
          };
        })
      );
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  async function fetchSystemLevels() {
    console.log(`Fetching system levels from ${base_url}...`);

    try {
      const response = await fetch(
        base_url + "/farm/" + props.uuid + "/getAllSystemLevels",
        {
          method: "GET",
        }
      );
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const data = await response.json();

      console.log(data);

      setSystemLevels(
        data.map((item: any) => {
          return {
            created_at: new Date(item.created_at),
            battery_temp: item.battery_temp,
            battery_time: new Date(item.battery_time),
            battery_voltage: item.battery_voltage,
            solar_panel_power: item.solar_panel_power,
          };
        })
      );
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  async function fetchImage() {
    console.log(`Fetching image data from ${base_url}...`);

    try {
      const response = await fetch(
        base_url + "/farm/" + props.uuid + "/sensorImage",
        {
          method: "GET",
        }
      );
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const data = await response.json();

      setCameraStr(data.camera);
    } catch (error) {
      console.error("Error fetching image:", error);
    }
  }

  useEffect(() => {
    fetchSensorData();

    const sensorDataIntervalId = setInterval(fetchSensorData, 60000);

    return () => {
      clearInterval(sensorDataIntervalId);
    };
  }, []);

  useEffect(() => {
    fetchImage();

    const intervalId = setInterval(fetchImage, 60000);

    return () => {
      clearInterval(intervalId);
    };
  }, []);

  useEffect(() => {
    fetchSystemLevels();

    const intervalId = setInterval(fetchSystemLevels, 60000);

    return () => {
      clearInterval(intervalId);
    };
  }, []);

  return (
    <DataPane>
      <Stack>
        <Typography variant="h5" fontWeight="bold">
          Sensor Data
        </Typography>
        <ResponsiveContainer width="100%" height={300}>
          <LineChart data={objectData} margin={{ top: 5, right: 5, left: 5 }}>
            <CartesianGrid stroke="#f5f5f5" />
            <XAxis
              dataKey="date"
              angle={0}
              interval={"preserveStartEnd"}
              position="bottom"
            />
            <YAxis yAxisId={0} />
            <Tooltip offset={-100} />
            <Legend verticalAlign="top" height={36} />
            <Line type="monotone" dataKey="temperature" stroke="#8884d8" />
            <Line type="monotone" dataKey="height" stroke="#82ca9d" />
          </LineChart>
        </ResponsiveContainer>

        <ResponsiveContainer width="100%" height={300}>
          <LineChart data={systemLevels} margin={{ top: 5, right: 5, left: 5 }}>
            <CartesianGrid stroke="#f5f5f5" />
            <XAxis
              dataKey="date"
              angle={0}
              interval={"preserveStartEnd"}
              position="bottom"
            />
            <YAxis yAxisId={0} />
            <Tooltip offset={-100} />
            <Legend verticalAlign="top" height={36} />
            <Line type="monotone" dataKey="battery_temp" stroke="#8884d8" />
            <Line type="monotone" dataKey="battery_voltage" stroke="#82ca9d" />
          </LineChart>
        </ResponsiveContainer>

        <Typography variant="h5" fontWeight="bold">
          Image
        </Typography>

        <Box sx={{ width: "100%", justify: "center" }}>
          <img src={cameraStr} width={"100%"} />
        </Box>
      </Stack>
    </DataPane>
  );
}
