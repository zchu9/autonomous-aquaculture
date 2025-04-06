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
import ListItemButton from "@mui/material/ListItemButton";
import ListItemIcon from "@mui/material/ListItemIcon";
import ListItemText from "@mui/material/ListItemText";
import Box from "@mui/material/Box";

import Typography from "@mui/material/Typography";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface AddDeviceModalProps {
  closeFn: any;
}

export default function AddDeviceModal(props: AddDeviceModalProps) {
  const [deviceName, setDeviceName] = React.useState<string>("");
  const [devicePasswd, setDevicePasswd] = React.useState<string>("");

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
          cage_position: "up",
          created_at: new Date(),
          lora_passwd: devicePasswd,
          status: "Disconnected",
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

  return (
    <>
      <ModalTemplate label="Add new Device">
        <Grid container>
          <Grid size={5}>
            <Stack spacing={2}>
              <div>Enter device details</div>
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
                >
                  <FormControlLabel
                    value="Cellular"
                    control={<Radio />}
                    label="Cellular"
                  />
                  <FormControlLabel
                    value="LoRA"
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
              >
                Confirm
              </Button>
            </Stack>
          </Grid>
          <Grid size={1}>
            <Divider orientation="vertical" variant="middle" />
          </Grid>
          <Grid size={5}>
            <List>
              <ListItem disablePadding>
                1. Use the form to the left to enter your device's details.
              </ListItem>
              <ListItem disablePadding>
                2. Click confirm to register the device and generate a config
                file. The config file will be dowwnloaded automatically.
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
