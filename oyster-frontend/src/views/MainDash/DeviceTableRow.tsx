import * as React from "react";
import TableRow from "@mui/material/TableRow";
import TableCell from "@mui/material/TableCell";
import Checkbox from "@mui/material/Checkbox";

import { Link as RouterLink } from "react-router-dom";
import { Link } from "@mui/material";

import DeleteIcon from "@mui/icons-material/Delete";
import ListItemIcon from "@mui/material/ListItemIcon";
import MoreVertIcon from "@mui/icons-material/MoreVert";

import Button from "@mui/material/Button";
import Menu from "@mui/material/Menu";
import MenuItem from "@mui/material/MenuItem";

import DeviceData from "../comm/DeviceDataInterface";

const base_url = `${import.meta.env.VITE_API_URL}`;

interface DeviceTableRowProps {
  device: DeviceData;
  isItemSelected: boolean;
  handleClick: any;
  labelId: string;
}

export default function DeviceTableRow(props: DeviceTableRowProps) {
  const [anchorEl, setAnchorEl] = React.useState<null | HTMLElement>(null);
  const open = Boolean(anchorEl);

  const handleMoreClicked = (event: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(event.currentTarget);
  };

  const handleMoreClosed = () => {
    setAnchorEl(null);
  };

  async function handleDelete() {
    console.log("Deleting device");
    try {
      const response = await fetch(
        base_url + "/farm/" + props.device._id + "/delete",
        {
          method: "DELETE",
          headers: {
            "Content-Type": "application/json",
          },
        }
      );
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }
    } catch (error) {
      console.error("Error fetching sensor data:", error);
    }
    handleMoreClosed();
  }

  return (
    <TableRow
      hover
      onClick={(event: any) => props.handleClick(event, props.device.rowId)}
      role="checkbox"
      aria-checked={props.isItemSelected}
      tabIndex={-1}
      key={props.device._id}
      selected={props.isItemSelected}
      sx={{ cursor: "pointer" }}
    >
      <TableCell padding="checkbox">
        <Checkbox
          color="primary"
          checked={props.isItemSelected}
          inputProps={{
            "aria-labelledby": props.labelId,
          }}
        />
      </TableCell>
      <TableCell component="th" id={props.labelId} scope="row" padding="none">
        <Link
          component={RouterLink}
          to={{
            pathname: `/farm`,
            search: `?id=${props.device._id}`,
          }}
        >
          {props.device._id}
        </Link>
      </TableCell>
      <TableCell align="center">
        {props.device.status ? "Connected" : "Disconnected"}
      </TableCell>
      <TableCell align="right">{props.device.location}</TableCell>
      <TableCell align="center">
        {props.device.status
          ? props.device.cage_position
            ? "Up"
            : "Down"
          : "—"}
      </TableCell>
      <TableCell align="right">
        {props.device.created_at.toLocaleString()}
      </TableCell>
      <TableCell padding="checkbox">
        <Button
          id="basic-button"
          aria-controls={open ? "basic-menu" : undefined}
          aria-haspopup="true"
          aria-expanded={open ? "true" : undefined}
          onClick={handleMoreClicked}
          startIcon={<MoreVertIcon />}
        />
      </TableCell>

      <Menu
        id="basic-menu"
        anchorEl={anchorEl}
        open={open}
        onClose={handleMoreClosed}
        MenuListProps={{
          "aria-labelledby": "basic-button",
        }}
      >
        <MenuItem onClick={handleDelete}>
          Delete
          <ListItemIcon>
            <DeleteIcon />
          </ListItemIcon>
        </MenuItem>
      </Menu>
    </TableRow>
  );
}
