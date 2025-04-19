import * as React from "react";
import { useEffect } from "react";

import DeviceTable from "./DeviceTable";
import DeviceData from "../comm/DeviceDataInterface";
import { Box, Container } from "@mui/material";
import Bar from "../comm/Bar";

const base_url = `${import.meta.env.VITE_API_URL}`;

function MainDash() {
  const [data, setData] = React.useState<DeviceData[]>([]);

  async function fetchSensorData() {
    console.log(`Fetching device data from ${base_url}...`);

    try {
      const response = await fetch(base_url + "/farm", {
        method: "GET",
      });
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const data = await response.json();

      const formattedData: DeviceData[] = data.map(
        (device: any, index: number) => ({
          ...device,
          rowId: index,
          created_at: new Date(device.created_at),
        })
      );

      setData(formattedData);
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  useEffect(() => {
    fetchSensorData();

    const intervalId = setInterval(fetchSensorData, 60000);

    return () => clearInterval(intervalId);
  }, []);

  return (
    <>
      <Box
        sx={{
          flexGrow: 1,
          height: "100vh",
          display: "flex",
          flexDirection: "column",
        }}
      >
        <Bar title="Main Dashboard" />
        <Container
          sx={{
            marginTop: 4,
            flexGrow: 1,
            display: "flex",
            flexDirection: "column",
            justifyContent: "flex-start",
            alignItems: "stretch",
            padding: 0,
          }}
        >
          <DeviceTable rows={data} />
        </Container>
      </Box>
    </>
  );
}

export default MainDash;
