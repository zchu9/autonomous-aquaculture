import * as React from "react";
import ModalTemplate from "../comm/ModalTemplate";
import Button from "@mui/material/Button";
import Stack from "@mui/material/Stack";
import TextField from "@mui/material/TextField";
import Radio from "@mui/material/Radio";
import RadioGroup from "@mui/material/RadioGroup";
import FormControlLabel from "@mui/material/FormControlLabel";
import FormControl from "@mui/material/FormControl";
import FormLabel from "@mui/material/FormLabel";
import Divider from "@mui/material/Divider";
import Grid from "@mui/material/Grid";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";

import Typography from "@mui/material/Typography";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface AddDeviceModalProps {
  closeFn: any;
}

export default function AddDeviceModal(props: AddDeviceModalProps) {
  const [deviceName, setDeviceName] = React.useState<string>("");
  const [devicePasswd, setDevicePasswd] = React.useState<string>("");
  const [commType, setCommType] = React.useState<string>("");

  async function createDevice() {
    console.log("Adding new device");
    try {
      const response = await fetch(base_url + "/addFarm", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          farm_name: deviceName,
          location: "somewhere",
          lora_passwd: devicePasswd,
          comm_type: commType,
        }),
      });
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const blob = await response.blob();
      const url = window.URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = "config.h"; // Replace with the desired file name
      document.body.appendChild(a);
      a.click();
      a.remove();
      window.URL.revokeObjectURL(url);
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  const handleRadioChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    setCommType(event.target.value);
  };

  return (
    <>
      <ModalTemplate label="Add new Device">
        <Grid container spacing={2}>
          <Grid item xs={5}>
            <Stack spacing={2}>
              <FormLabel>Device Name</FormLabel>
              <TextField
                id="outlined-basic"
                label="Device Name"
                variant="outlined"
                onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
                  setDeviceName(event.target.value);
                }}
              />

              <FormLabel>LoRA Password</FormLabel>
              <TextField
                id="outlined-basic"
                label="LoRA Password"
                variant="outlined"
                onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
                  setDevicePasswd(event.target.value);
                }}
              />

              <Divider variant="inset" />

              <FormControl>
                <FormLabel>Communication Type</FormLabel>
                <RadioGroup
                  aria-labelledby="demo-radio-buttons-group-label"
                  defaultValue="female"
                  name="radio-buttons-group"
                  onChange={handleRadioChange}
                >
                  <FormControlLabel
                    value="USE_CELLULAR"
                    control={<Radio />}
                    label="Cellular"
                  />
                  <FormControlLabel
                    value="USE_LORA"
                    control={<Radio />}
                    label="LoRA"
                  />
                </RadioGroup>
              </FormControl>

              <Button
                variant="contained"
                onClick={() => {
                  createDevice();
                  props.closeFn();
                }}
                disabled={
                  deviceName.length < 1 ||
                  devicePasswd.length < 1 ||
                  commType.length < 1
                }
              >
                Confirm
              </Button>
            </Stack>
          </Grid>
          <Grid item xs={1}>
            <Divider orientation="vertical" variant="middle" />
          </Grid>
          <Grid item xs={6}>
            <List sx={{ width: "100%" }}>
              <ListItem disablePadding>
                1. Use the form on the left to enter your device's details.
              </ListItem>
              <ListItem disablePadding>
                2. Click confirm to register the device and generate a
                configuration file. The configuration file will be downloaded
                automatically.
              </ListItem>
              <ListItem disablePadding>
                3. Copy the config file to the microcontroller code's include
                directory.
              </ListItem>
              <ListItem disablePadding>
                4. Upload the microcontroller code to the device.
              </ListItem>
            </List>
          </Grid>
        </Grid>
      </ModalTemplate>
    </>
  );
}
