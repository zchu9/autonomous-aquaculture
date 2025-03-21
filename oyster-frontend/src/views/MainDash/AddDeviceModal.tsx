import * as React from "react";
import ModalTemplate from "../comm/ModalTemplate";
import Button from "@mui/material/Button";
import Stack from "@mui/material/Stack";
import TextField from "@mui/material/TextField";
import Typography from "@mui/material/Typography";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface AddDeviceModalProps {
  closeFn: any;
}

export default function AddDeviceModal(props: AddDeviceModalProps) {
  const [deviceName, setDeviceName] = React.useState<string>("");

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
          status: "Disconnected",
        }),
      });
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  return (
    <>
      <ModalTemplate label="Add new Device">
        <div>Enter device details</div>
        <Stack spacing={2}>
          <h3>Device Name:</h3>
          <TextField
            id="outlined-basic"
            label="Outlined"
            variant="outlined"
            onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
              setDeviceName(event.target.value);
            }}
          />
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
      </ModalTemplate>
    </>
  );
}
