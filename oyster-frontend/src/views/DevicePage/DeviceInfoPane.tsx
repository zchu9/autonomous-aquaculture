import * as React from "react";
import { useEffect } from "react";

import Button from "@mui/material/Button";
import Box from "@mui/material/Box";
import EditOutlinedIcon from "@mui/icons-material/EditOutlined";
import Grid from "@mui/material/Grid2";
import Stack from "@mui/material/Stack";
import Typography from "@mui/material/Typography";
import List from "@mui/material/List";
import ListItem from "@mui/material/ListItem";

import Modal from "@mui/joy/Modal";
import ModalClose from "@mui/joy/ModalClose";
import ModalDialog from "@mui/joy/ModalDialog";
import ScheduleOpModal from "../comm/ScheduleOpModal";
import EditCalendarOutlinedIcon from "@mui/icons-material/EditCalendarOutlined";

import Paper from "@mui/material/Paper";
import { styled } from "@mui/material/styles";

import DeviceData from "../comm/DeviceDataInterface";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface ScheduleList {
  command: Boolean;
  date: Date;
  duration: number;
  status: string;
}

interface DeviceInfoPaneProps {
  uuid: string;
}

const InfoPane = styled(Paper)(({ theme }) => ({
  padding: theme.spacing(2),
  height: "100%",
  display: "flex",
  flexDirection: "column",
}));

const InfoPaneRow = styled(Box)(() => ({
  textAlign: "left",
}));

export default function DeviceInfoPane(props: DeviceInfoPaneProps) {
  const [objectData, setObjectData] = React.useState<DeviceData>();
  const [scheduleList, setSchedules] = React.useState<ScheduleList[]>();

  const [scheduleModalopen, setScheduleModalOpen] = React.useState(false);
  const handleScheduleModalOpen = () => setScheduleModalOpen(true);
  const handleScheduleModalClose = () => setScheduleModalOpen(false);

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

  async function fetchSchedule() {
    console.log(`Fetching schedules from ${base_url}...`);

    try {
      const response = await fetch(
        base_url + "/farm/" + props.uuid + "/getAllActiveLiftSchedule",
        {
          method: "GET",
        }
      );
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const data = await response.json();

      const temp = data.map((item: any) => {
        const thing = item.schedule[0].dates.map(() => {
          return {
            date: new Date(item.created_at),
            command: item.schedule[0].command,
            duration: item.schedule[0].duration,
            status: item.schedule[0].status,
          };
        });
        return thing;
      });

      setSchedules(temp.flat());
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
  }

  useEffect(() => {
    fetchFarmData();
    fetchSchedule();

    const intervalId1 = setInterval(fetchFarmData, 60000);
    const intervalId2 = setInterval(fetchSchedule, 60000);

    return () => {
      clearInterval(intervalId1);
      clearInterval(intervalId2);
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
              <Typography variant="body1">{objectData?._id}</Typography>
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
                Next Scheduled Operation(s):
              </Typography>
              {scheduleList?.length === 0 ? (
                <Typography variant="body1">—</Typography>
              ) : (
                scheduleList?.map((schedule: ScheduleList, idx: number) => {
                  return (
                    <List>
                      <ListItem key={idx}>
                        <div>
                          {schedule.date.toLocaleString()} -{" "}
                          {schedule.command ? "Up" : "Down"} for{" "}
                          {schedule.duration} minutes
                        </div>
                      </ListItem>
                    </List>
                  );
                })
              )}
            </InfoPaneRow>
          </Grid>
          <Grid>
            <InfoPaneRow>
              <Typography variant="subtitle2" fontWeight="bold">
                Schedule an Operation:
              </Typography>

              <Button
                variant="contained"
                startIcon={<EditCalendarOutlinedIcon />}
                onClick={handleScheduleModalOpen}
              >
                Bulk Schedule
              </Button>

              <Modal
                open={scheduleModalopen}
                onClose={handleScheduleModalClose}
                aria-labelledby="modal-modal-title"
                aria-describedby="modal-modal-description"
              >
                <ModalDialog>
                  <ModalClose />
                  <ScheduleOpModal
                    closeFn={handleScheduleModalClose}
                    devices={[objectData!]}
                  />
                </ModalDialog>
              </Modal>
            </InfoPaneRow>
          </Grid>
        </Grid>

        {/* <Grid container spacing={2}>
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
        </Grid> */}
      </Stack>
    </InfoPane>
  );
}
