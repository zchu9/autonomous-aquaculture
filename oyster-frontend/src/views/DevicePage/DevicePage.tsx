import { useSearchParams } from "react-router-dom";

import Button from "@mui/material/Button";
import Container from "@mui/material/Container";
import Grid from "@mui/material/Grid2";
import Box from "@mui/material/Box";
import DeviceInfoPane from "./DeviceInfoPane";
import { DeviceDataPane, IQueryResult } from "./DeviceDataPane";
import ArrowBackOutlinedIcon from "@mui/icons-material/ArrowBackOutlined";
import { Link as RouterLink } from "react-router-dom";
import { AppBar, Toolbar, Typography } from "@mui/material";
import Bar from "../comm/Bar";

export default function DevicePage() {
  const [deviceUUID, setDeviceUUID] = useSearchParams();
  const uuid = deviceUUID.get("id");
  const paramsObject = Object.fromEntries(deviceUUID);

  return (
    <Box
      sx={{
        flexGrow: 1,
        height: "100vh",
        display: "flex",
        flexDirection: "column",
      }}
    >
      <Bar title="" />

      <Box
        sx={{
          flexGrow: 1,
          p: 4,
          textAlign: "left",
          display: "flex",
          flexDirection: "column",
          gap: 2,
        }}
      >
        <Button
          variant="contained"
          startIcon={<ArrowBackOutlinedIcon />}
          component={RouterLink}
          to="/"
          sx={{
            maxWidth: "15%",
          }}
        >
          Return to dashboard
        </Button>

        <Grid
          container
          spacing={4}
          sx={{ marginTop: 2, flexGrow: 1, display: "flex" }}
        >
          <Grid
            item
            size={6}
            sx={{ display: "flex", flexDirection: "column", flexGrow: 1 }}
          >
            <DeviceInfoPane uuid={uuid} />
          </Grid>
          <Grid
            item
            size={6}
            sx={{ display: "flex", flexDirection: "column", flexGrow: 1 }}
          >
            <DeviceDataPane uuid={uuid} />
          </Grid>
        </Grid>
      </Box>
    </Box>
  );
}
