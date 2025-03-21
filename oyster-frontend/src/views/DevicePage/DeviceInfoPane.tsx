import Button from "@mui/material/Button";
import Box from "@mui/material/Box";
import EditOutlinedIcon from "@mui/icons-material/EditOutlined";
import FormControlLabel from "@mui/material/FormControlLabel";
import Grid from "@mui/material/Grid2";
import Stack from "@mui/material/Stack";

import IOSSwitch from "../comm/Switch";

import Paper from "@mui/material/Paper";
import { styled } from "@mui/material/styles";

import DeviceData from "../comm/DeviceDataInterface";

interface DeviceInfoPaneProps {
  deviceInfo: DeviceData;
}

const InfoPane = styled(Paper)(({ theme }) => ({
  // width: 120,
  // height: 120,
  padding: theme.spacing(2),
  // ...theme.typography.body2,
}));

const InfoPaneRow = styled(Box)(({ theme }) => ({
  textAlign: "left",
}));

export function DeviceInfoPane(props: DeviceInfoPaneProps) {
  if (!props.deviceInfo) {
    return (
      <InfoPane>
        <Stack spacing={2}>
          <h2>Loading Device Info...</h2>
        </Stack>
      </InfoPane>
    );
  }

  const { _id, status, cage_position, created_at } = props.deviceInfo || {};
  const formattedDate = created_at ? new Date(created_at) : null;
  const createdAtString =
    formattedDate && !isNaN(formattedDate.getTime())
      ? formattedDate.toLocaleString()
      : "Invalid Date";

  return (
    <InfoPane>
      <Stack spacing={2}>
        <Grid container spacing={2}>
          <Grid size={6}>
            <InfoPaneRow>
              <h2>Device Name</h2>
              {_id}
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

        <InfoPaneRow>Created On: {createdAtString}</InfoPaneRow>

        <Box></Box>

        <Grid container spacing={2}>
          <Grid size={6}>
            <InfoPaneRow>
              Connection Status: {status ? "Connected" : "Disconnected"}
            </InfoPaneRow>
          </Grid>
          <Grid size={6}>
            <InfoPaneRow>Received Status On:</InfoPaneRow>
          </Grid>
          <Grid size={6}>
            <InfoPaneRow>
              Cage Status: {status ? (cage_position ? "Up" : "Down") : "—"}
            </InfoPaneRow>
          </Grid>
          <Grid size={6}>
            <InfoPaneRow>Next Scheduled Operation:</InfoPaneRow>
          </Grid>
        </Grid>

        <Grid container spacing={2}>
          <Grid size={2}>Active</Grid>
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
