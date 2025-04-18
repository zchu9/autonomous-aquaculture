import * as React from "react";
import { useEffect } from "react";

import Button from "@mui/material/Button";
import Box from "@mui/material/Box";
import EditOutlinedIcon from "@mui/icons-material/EditOutlined";
import FormControlLabel from "@mui/material/FormControlLabel";
import Grid from "@mui/material/Grid2";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";

import IOSSwitch from "../comm/Switch";

import Paper from "@mui/material/Paper";
import { styled } from "@mui/material/styles";

import DeviceData from "../comm/DeviceDataInterface";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface DeviceInfoPaneProps {
  uuid: string;
}

const InfoPane = styled(Paper)(({ theme }) => ({
  padding: theme.spacing(2),
  height: "100%",
  display: "flex",
  flexDirection: "column",
}));

const InfoPaneRow = styled(Box)(({ theme }) => ({
  textAlign: "left",
}));

export default function DeviceInfoPane(props: DeviceInfoPaneProps) {
  const [objectData, setObjectData] = React.useState<DeviceData>();

  async function fetchFarmData() {
    console.log(`Fetching device data from ${base_url}...`);

    try {
      const response = await fetch(base_url + "/farm/" + props.uuid + "/info", {
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
    fetchFarmData();

    const intervalId = setInterval(fetchFarmData, 60000);

    return () => {
      clearInterval(intervalId);
    };
  }, []);

  return (
    <InfoPane>
      <Stack spacing={2}>
        <Grid container spacing={2}>
          <Grid size={6}>
            <InfoPaneRow>
              <Typography variant="h5" fontWeight="bold">
                {objectData?.farm_name}
              </Typography>
              <Typography variant="body3">{objectData?._id}</Typography>
            </InfoPaneRow>
          </Grid>
          <Grid size={6} sx={{ textAlign: "right" }}>
            <Button
              variant="contained"
              startIcon={<EditOutlinedIcon />}
              onClick={() => {
                console.log("Uh-oh");
              }}
            >
              Edit
            </Button>
          </Grid>
        </Grid>

        <InfoPaneRow>
          <Typography variant="subtitle2" fontWeight="bold">
            Created On:
          </Typography>{" "}
          <Typography variant="body1">
            {objectData?.created_at.toLocaleString()}
          </Typography>
        </InfoPaneRow>

        <Grid container spacing={2}>
          <Grid size={6}>
            <InfoPaneRow>
              <Typography variant="subtitle2" fontWeight="bold">
                Connection Status:
              </Typography>
              <Typography variant="body1">
                {objectData?.status ? "Connected" : "Disconnected"}
              </Typography>
            </InfoPaneRow>
          </Grid>
          <Grid size={6}>
            <Typography variant="subtitle2" fontWeight="bold">
              Received Status On:
            </Typography>
            <Typography variant="body1">—</Typography>
          </Grid>
          <Grid size={6}>
            <InfoPaneRow>
              <Typography variant="subtitle2" fontWeight="bold">
                Cage Status:
              </Typography>
              <Typography variant="body1">
                {objectData?.status
                  ? objectData?.cage_position
                    ? "Up"
                    : "Down"
                  : "—"}
              </Typography>
            </InfoPaneRow>
          </Grid>
          <Grid size={6}>
            <InfoPaneRow>
              <Typography variant="subtitle2" fontWeight="bold">
                Next Scheduled Operation:
              </Typography>
              <Typography variant="body1">—</Typography>
            </InfoPaneRow>
          </Grid>
        </Grid>

        <Grid container spacing={2}>
          <Grid size={2}>
            <Typography variant="subtitle2" fontWeight="bold">
              Active
            </Typography>
          </Grid>
          <Grid>
            <Box>
              <FormControlLabel
                control={<IOSSwitch sx={{ m: 1 }} />}
              ></FormControlLabel>
            </Box>
          </Grid>
        </Grid>
      </Stack>
    </InfoPane>
  );
}
