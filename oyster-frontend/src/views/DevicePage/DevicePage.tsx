import * as React from "react";
import { useState, useEffect } from "react";
import { useSearchParams } from "react-router-dom";

import Button from "@mui/material/Button";
import Container from "@mui/material/Container";
import Grid from "@mui/material/Grid2";
import Box from "@mui/material/Box";
import { DeviceInfoPane, DeviceInfo } from "./DeviceInfoPane";
import { DeviceDataPane, IQueryResult } from "./DeviceDataPane";
import ArrowBackOutlinedIcon from "@mui/icons-material/ArrowBackOutlined";
import { Link as RouterLink } from "react-router-dom";

const base_url = `${import.meta.env.VITE_API_URL}`;

const device_data: IQueryResult[] = [
  { date: "1/21/25", temperature_c: 37.01, height: 0.1 },
  { date: "1/22/25", temperature_c: 35.6, height: 0.11 },
  { date: "1/23/25", temperature_c: 36.8, height: 1.2 },
  { date: "1/24/25", temperature_c: 40.1, height: 1.4 },
  { date: "1/25/25", temperature_c: 39.2, height: 0.0 },
];

export default function DevicePage() {
  const [deviceUUID, setDeviceUUID] = useSearchParams();
  const uuid = deviceUUID.get("id");

  const [objectData, setObjectData] = React.useState<DeviceInfo>({});

  const paramsObject = Object.fromEntries(deviceUUID);

  async function fetchSensorData() {
    console.log(`Fetching device data from ${base_url}...`);

    try {
      const response = await fetch(base_url + "/farm/" + uuid + "/info", {
        method: "GET",
      });
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const data = await response.json();
      data.created_at = new Date(data.created_at);

      setObjectData(data);
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
    <Container
      maxWidth="false"
      sx={{
        flexGrow: 1,
      }}
    >
      <Box
        sx={{
          p: 2,
          textAlign: "left",
        }}
      >
        <Button
          variant="contained"
          startIcon={<ArrowBackOutlinedIcon />}
          component={RouterLink}
          to="/"
        >
          Return to dashboard
        </Button>
      </Box>

      <Grid container spacing={4}>
        <Grid size={6}>
          <DeviceInfoPane deviceInfo={objectData} />
        </Grid>
        <Grid size={6}>
          <DeviceDataPane data={device_data} />
        </Grid>
      </Grid>
    </Container>
  );
}
