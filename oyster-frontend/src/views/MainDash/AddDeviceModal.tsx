import * as React from "react";
import ModalTemplate from "../comm/ModalTemplate";
import Button from "@mui/material/Button";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface AddDeviceModalProps {
  closeFn: any;
}

export default function AddDeviceModal(props: AddDeviceModalProps) {
  async function createDevice() {
    console.log("Adding new device");
    try {
      const response = await fetch(base_url + "/addFarm", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
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
        Enter your Device Details:
        <Stack spacing={2}>
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
