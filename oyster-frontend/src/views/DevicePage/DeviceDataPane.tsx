import * as React from "react";
import { useEffect } from "react";
import Stack from "@mui/material/Stack";
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
  camera: string;
  created_at: Date;
  height: number;
  temperature: number;
}

const DataPane = styled(Paper)(({ theme }) => ({
  padding: theme.spacing(2),
  height: "100%",
  display: "flex",
  flexDirection: "column",
}));

export function DeviceDataPane(props: DeviceDataPaneProps) {
  const [id, setId] = React.useState(0);
  const [objectData, setObjectData] = React.useState<SensorData>();

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
            camera: item.camera,
            created_at: new Date(item.created_at),
            height: item.height,
            temperature: item.temperature,
          };
        })
      );

      console.log(data);
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  async function fetchImage() {
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
            camera: item.camera,
            created_at: new Date(item.created_at),
            height: item.height,
            temperature: item.temperature,
          };
        })
      );

      console.log(data);
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  useEffect(() => {
    fetchSensorData();

    const intervalId = setInterval(fetchSensorData, 60000);

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
            <Line
              type="monotone"
              dataKey="temperature"
              stroke="#8884d8"
              onMouseOver={() => {
                setId(0);
              }}
            />
            <Line
              type="monotone"
              dataKey="height"
              stroke="#82ca9d"
              onMouseOver={() => {
                setId(1);
              }}
            />
          </LineChart>
        </ResponsiveContainer>

        <Typography variant="h5" fontWeight="bold">
          Image
        </Typography>
      </Stack>
    </DataPane>
  );
}
