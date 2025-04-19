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

interface SolarPanelData {
  created_at: Date;
  controller_temp: number;
  load_voltage: number;
  solar_panel_current: number;
  solar_panel_power: number;
  solar_panel_voltage: number;
}

interface BatteryData {
  created_at: Date;
  battery_current: number;
  battery_temp: number;
  battery_voltage: number;
  power: number;
  state_of_charge: number;
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
  const [solarPanelData, setSolarPanelData] = React.useState<SolarPanelData>();
  const [batteryData, setBatteryData] = React.useState<BatteryData>();
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

      await setSolarPanelData(
        data.map((item: any) => {
          return {
            created_at: new Date(item.created_at),
            controller_temp: item.renogy_mppt[0].controller_temp,
            load_voltage: item.renogy_mppt[0].load_voltage,
            solar_panel_current: item.renogy_mppt[0].solar_panel_current,
            solar_panel_power: item.renogy_mppt[0].solar_panel_power,
            solar_panel_voltage: item.renogy_mppt[0].solar_panel_voltage,
          };
        })
      );

      await setBatteryData(
        data.map((item: any) => {
          var thing = {
            created_at: new Date(item.created_at),
            battery_current: item.smart_shunt[0].battery_current,
            battery_temp: item.smart_shunt[0].battery_temp,
            battery_voltage: item.smart_shunt[0].battery_voltage,
            power: item.smart_shunt[0].power,
            state_of_charge: item.smart_shunt[0].state_of_charge,
          };
          console.log(thing);
          return thing;
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
            <XAxis dataKey="created_at" hide={true} />
            <YAxis yAxisId={0} />
            <Tooltip />
            <Legend verticalAlign="top" height={36} />
            <Line type="monotone" dataKey="temperature" stroke="#8884d8" />
            <Line type="monotone" dataKey="height" stroke="#82ca9d" />
          </LineChart>
        </ResponsiveContainer>

        <Typography variant="h5" fontWeight="bold">
          Solar Panel Data
        </Typography>
        <ResponsiveContainer width="100%" height={300}>
          <LineChart
            data={solarPanelData}
            margin={{ top: 5, right: 5, left: 5 }}
          >
            <CartesianGrid stroke="#f5f5f5" />
            <XAxis dataKey="created_at" hide={true} />
            <YAxis yAxisId={0} />
            <Tooltip />
            <Legend verticalAlign="top" height={36} />
            <Line type="monotone" dataKey="controller_temp" stroke="#8884d8" />
            <Line type="monotone" dataKey="load_voltage" stroke="#82ca9d" />
            <Line
              type="monotone"
              dataKey="solar_panel_current"
              stroke="#ff7300"
            />
            <Line
              type="monotone"
              dataKey="solar_panel_power"
              stroke="#387908"
            />
            <Line
              type="monotone"
              dataKey="solar_panel_voltage"
              stroke="#df1849"
            />
          </LineChart>
        </ResponsiveContainer>

        <Typography variant="h5" fontWeight="bold">
          Battery Data
        </Typography>
        <ResponsiveContainer width="100%" height={300}>
          <LineChart data={batteryData} margin={{ top: 5, right: 5, left: 5 }}>
            <CartesianGrid stroke="#f5f5f5" />
            <XAxis dataKey="created_at" hide={true} />
            <YAxis yAxisId={0} />
            <Tooltip offset={-100} />
            <Legend verticalAlign="top" height={36} />
            <Line type="monotone" dataKey="battery_current" stroke="#8884d8" />
            <Line type="monotone" dataKey="battery_temp" stroke="#82ca9d" />
            <Line type="monotone" dataKey="battery_voltage" stroke="#ff7300" />
            <Line type="monotone" dataKey="power" stroke="#387908" />
            <Line type="monotone" dataKey="state_of_charge" stroke="#df1849" />
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
