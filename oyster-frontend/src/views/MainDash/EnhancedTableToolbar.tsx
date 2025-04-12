import * as React from "react";

import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import Toolbar from "@mui/material/Toolbar";
import IconButton from "@mui/material/IconButton";
import Tooltip from "@mui/material/Tooltip";
import FilterListIcon from "@mui/icons-material/FilterList";
import Modal from "@mui/joy/Modal";
import ModalClose from "@mui/joy/ModalClose";
import ModalDialog from "@mui/joy/ModalDialog";
import TextField from "@mui/material/TextField";
import SearchIcon from "@mui/icons-material/Search";
import EditCalendarOutlinedIcon from "@mui/icons-material/EditCalendarOutlined";
import AddCircleOutlineOutlinedIcon from "@mui/icons-material/AddCircleOutlineOutlined";
import AddDeviceModal from "./AddDeviceModal";
import ScheduleOpModal from "../comm/ScheduleOpModal";

import DeviceData from "../comm/DeviceDataInterface";

interface EnhancedTableToolbarProps {
  numSelected: number;
  selectedDevices: DeviceData[];
}

export default function EnhancedTableToolbar(props: EnhancedTableToolbarProps) {
  const { numSelected } = props;

  const [addDeviceModalopen, setAddDeviceModalOpen] = React.useState(false);
  const handleDeviceModalOpen = () => setAddDeviceModalOpen(true);
  const handleDeviceModalClose = () => setAddDeviceModalOpen(false);

  const [scheduleModalopen, setScheduleModalOpen] = React.useState(false);
  const handleScheduleModalOpen = () => setScheduleModalOpen(true);
  const handleScheduleModalClose = () => setScheduleModalOpen(false);

  return (
    <Toolbar
      sx={[
        {
          pl: { sm: 2 },
          pr: { xs: 1, sm: 1 },
        },
      ]}
    >
      <Box sx={{ display: "flex", alignItems: "flex-end" }}>
        <SearchIcon sx={{ color: "action.active", mr: 1, my: 0.5 }} />
        <TextField
          id="device-search-bar"
          label="Search for a device"
          variant="standard"
        />
      </Box>

      <Box>
        <Tooltip title="Filter list">
          <IconButton>
            <FilterListIcon />
          </IconButton>
        </Tooltip>
      </Box>

      <Box sx={{ flexGrow: 1 }} />

      <Box>
        <Button
          variant="contained"
          startIcon={<AddCircleOutlineOutlinedIcon />}
          onClick={handleDeviceModalOpen}
        >
          Add Device
        </Button>
      </Box>

      <Modal
        open={addDeviceModalopen}
        onClose={handleDeviceModalClose}
        aria-labelledby="modal-modal-title"
        aria-describedby="modal-modal-description"
      >
        <ModalDialog>
          <ModalClose />
          <AddDeviceModal closeFn={handleDeviceModalClose} />
        </ModalDialog>
      </Modal>

      <Box sx={{ p: 0.5 }} />

      <Button
        variant="contained"
        startIcon={<EditCalendarOutlinedIcon />}
        disabled={numSelected == 0}
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
            devices={props.selectedDevices}
          />
        </ModalDialog>
      </Modal>

      {/* {numSelected > 0 ? (
        <Typography
        sx={{ flex: '1 1 100%' }}
        color="inherit"
        variant="subtitle1"
        component="div"
        >
        {numSelected} selected
        </Typography>
    ) : (
        <Typography
        sx={{ flex: '1 1 100%' }}
        variant="h6"
        id="tableTitle"
        component="div"
        >
        Nutrition
        </Typography>
    )} */}
      {/* {numSelected > 0 ? (
        <Tooltip title="Delete">
        <IconButton>
            <DeleteIcon />
        </IconButton>
        </Tooltip>
    ) : (
    )} */}
    </Toolbar>
  );
}
