import * as React from 'react';
import { alpha } from '@mui/material/styles';
import Box from '@mui/material/Box';
import Table from '@mui/material/Table';
import TableBody from '@mui/material/TableBody';
import TableCell from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import TableHead from '@mui/material/TableHead';
import TablePagination from '@mui/material/TablePagination';
import TableRow from '@mui/material/TableRow';
import TableSortLabel from '@mui/material/TableSortLabel';
import Toolbar from '@mui/material/Toolbar';
import Typography from '@mui/material/Typography';
import Paper from '@mui/material/Paper';
import Checkbox from '@mui/material/Checkbox';
import IconButton from '@mui/material/IconButton';
import Tooltip from '@mui/material/Tooltip';
import { Link as RouterLink } from 'react-router-dom';
import FilterListIcon from '@mui/icons-material/FilterList';
import { visuallyHidden } from '@mui/utils';
import { Link } from '@mui/material';


import Modal from '@mui/material/Modal';


import Button from '@mui/material/Button';
import TextField from '@mui/material/TextField';
import SearchIcon from '@mui/icons-material/Search';
import EditCalendarOutlinedIcon from '@mui/icons-material/EditCalendarOutlined';
import AddCircleOutlineOutlinedIcon from '@mui/icons-material/AddCircleOutlineOutlined';
import AddDeviceModal from './AddDeviceModal';
import ScheduleOpModal from '../comm/ScheduleOpModal';

enum CagePosition {
    Up = 1,
    Down = 2
}
  

interface DeviceData {
    name: string,
    uuid: string,
    rowId: number,
    created_on_date: Date,
    is_connected: boolean,
    cage_status: CagePosition,
    last_update_date: Date
}

function createData(
    name: string,
    uuid: string,
    rowId: number,
    created_on_date: Date,
    is_connected: boolean,
    cage_status: CagePosition,
    last_update_date: Date
  ): DeviceData {
    return { name, uuid, rowId, created_on_date, is_connected, cage_status, last_update_date };
  }

interface HeadCell {
    id: keyof DeviceData;
    numeric: boolean;
    disablePadding: boolean;
    label: string;
}

const headCells: readonly HeadCell[] = [
  {
    id: 'name',
    numeric: false,
    disablePadding: true,
    label: 'Device Name',
  },
  {
    id: 'uuid',
    numeric: false,
    disablePadding: false,
    label: 'UUID',
  },
  {
    id: 'created_on_date',
    numeric: false,
    disablePadding: false,
    label: 'Created On',
  },
  {
    id: 'is_connected',
    numeric: false,
    disablePadding: false,
    label: 'Device Status',
  },
  {
    id: 'cage_status',
    numeric: false,
    disablePadding: false,
    label: 'Cage Status',
  },
  {
    id: 'last_update_date',
    numeric: false,
    disablePadding: false,
    label: 'Status Last Updated',
  },
];

const rows = [
  createData('Device 1', 'aaaaaaaa', 1, new Date(Date.UTC(2025, 0, 20, 11, 52, 0)), true, CagePosition.Down, new Date()),
  createData('Device 2', 'bbbbbbbb', 2, new Date(Date.UTC(2025, 0, 21, 11, 52, 0)), true, CagePosition.Up, new Date()),
  createData('Device 3', 'cccccccc', 3, new Date(Date.UTC(2025, 0, 22, 11, 52, 0)), false, CagePosition.Down, new Date()),
  createData('Device 4', 'dddddddd', 4, new Date(Date.UTC(2025, 0, 23, 11, 52, 0)), true, CagePosition.Down, new Date()),
  createData('Device 5', 'eeeeeeee', 5, new Date(Date.UTC(2025, 0, 24, 11, 52, 0)), false, CagePosition.Down, new Date()),
];

function descendingComparator<T>(a: T, b: T, orderBy: keyof T) {
    if (b[orderBy] < a[orderBy]) {
      return -1;
    }
    if (b[orderBy] > a[orderBy]) {
      return 1;
    }
    return 0;
  }
  
  type Order = 'asc' | 'desc';
  
  function getComparator<Key extends keyof any>(
    order: Order,
    orderBy: Key,
  ): (
    a: { [key in Key]: number | string },
    b: { [key in Key]: number | string },
  ) => number {
    return order === 'desc'
      ? (a, b) => descendingComparator(a, b, orderBy)
      : (a, b) => -descendingComparator(a, b, orderBy);
  }

interface EnhancedTableProps {
    numSelected: number;
    onRequestSort: (event: React.MouseEvent<unknown>, property: keyof DeviceData) => void;
    onSelectAllClick: (event: React.ChangeEvent<HTMLInputElement>) => void;
    order: Order;
    orderBy: string;
    rowCount: number;
  }
  
  function EnhancedTableHead(props: EnhancedTableProps) {
    const { onSelectAllClick, order, orderBy, numSelected, rowCount, onRequestSort } =
      props;
    const createSortHandler =
      (property: keyof DeviceData) => (event: React.MouseEvent<unknown>) => {
        onRequestSort(event, property);
      };
  
    return (
      <TableHead>
        <TableRow>
          <TableCell padding="checkbox">
            <Checkbox
              color="primary"
              indeterminate={numSelected > 0 && numSelected < rowCount}
              checked={rowCount > 0 && numSelected === rowCount}
              onChange={onSelectAllClick}
              inputProps={{
                'aria-label': 'select all desserts',
              }}
            />
          </TableCell>
          {headCells.map((headCell) => (
            <TableCell
              key={headCell.id}
              align={headCell.numeric ? 'right' : 'left'}
              padding={headCell.disablePadding ? 'none' : 'normal'}
              sortDirection={orderBy === headCell.id ? order : false}
            >
              <TableSortLabel
                active={orderBy === headCell.id}
                direction={orderBy === headCell.id ? order : 'asc'}
                onClick={createSortHandler(headCell.id)}
              >
                {headCell.label}
                {orderBy === headCell.id ? (
                  <Box component="span" sx={visuallyHidden}>
                    {order === 'desc' ? 'sorted descending' : 'sorted ascending'}
                  </Box>
                ) : null}
              </TableSortLabel>
            </TableCell>
          ))}
        </TableRow>
      </TableHead>
    );
  }
  interface EnhancedTableToolbarProps {
    numSelected: number;
  }
  function EnhancedTableToolbar(props: EnhancedTableToolbarProps) {
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
          }
        ]}
      >

        <Box sx={{display: 'flex', alignItems:'flex-end'}}>
            <SearchIcon sx={{color: 'action.active', mr: 1, my: 0.5}}/>
            <TextField id="device-search-bar" label="Search for a device" variant="standard" />
        </Box>

        <Box>
          <Tooltip title="Filter list">
              <IconButton>
                  <FilterListIcon />
              </IconButton>
          </Tooltip>
        </Box>

        <Box sx={{flexGrow: 1}}/>

        <Box>
          <Button 
              variant="contained"
              startIcon={<AddCircleOutlineOutlinedIcon/>}
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
            <div><AddDeviceModal/></div>
        </Modal>

        <Box sx={{p: 0.5}}/>
        
        <Button 
            variant="contained"
            startIcon={<EditCalendarOutlinedIcon/>}
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
            <div><ScheduleOpModal/></div>
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
  export default function DeviceTable() {
    const [order, setOrder] = React.useState<Order>('asc');
    const [orderBy, setOrderBy] = React.useState<keyof DeviceData>('name');
    const [selected, setSelected] = React.useState<readonly number[]>([]);
    const [page, setPage] = React.useState(0);
    const [rowsPerPage, setRowsPerPage] = React.useState(5);
  
    const handleRequestSort = (
      event: React.MouseEvent<unknown>,
      property: keyof DeviceData,
    ) => {
      const isAsc = orderBy === property && order === 'asc';
      setOrder(isAsc ? 'desc' : 'asc');
      setOrderBy(property);
    };
  
    const handleSelectAllClick = (event: React.ChangeEvent<HTMLInputElement>) => {
      if (event.target.checked) {
        const newSelected = rows.map((n) => n.rowId);
        setSelected(newSelected);
        return;
      }
      setSelected([]);
    };
  
    const handleClick = (event: React.MouseEvent<unknown>, id: number) => {
      const selectedIndex = selected.indexOf(id);
      let newSelected: readonly number[] = [];
  
      if (selectedIndex === -1) {
        newSelected = newSelected.concat(selected, id);
      } else if (selectedIndex === 0) {
        newSelected = newSelected.concat(selected.slice(1));
      } else if (selectedIndex === selected.length - 1) {
        newSelected = newSelected.concat(selected.slice(0, -1));
      } else if (selectedIndex > 0) {
        newSelected = newSelected.concat(
          selected.slice(0, selectedIndex),
          selected.slice(selectedIndex + 1),
        );
      }
      setSelected(newSelected);
    };
  
    const handleChangePage = (event: unknown, newPage: number) => {
      setPage(newPage);
    };
  
    const handleChangeRowsPerPage = (event: React.ChangeEvent<HTMLInputElement>) => {
      setRowsPerPage(parseInt(event.target.value, 10));
      setPage(0);
    };
  
    // Avoid a layout jump when reaching the last page with empty rows.
    const emptyRows =
      page > 0 ? Math.max(0, (1 + page) * rowsPerPage - rows.length) : 0;
  
    const visibleRows = React.useMemo(
      () =>
        [...rows]
          .sort(getComparator(order, orderBy))
          .slice(page * rowsPerPage, page * rowsPerPage + rowsPerPage),
      [order, orderBy, page, rowsPerPage],
    );
  
    return (
      <Box sx={{ width: '100%' }}>
        <Paper sx={{ width: '100%', mb: 2 }}>
          <EnhancedTableToolbar numSelected={selected.length} />
          <TableContainer>
            <Table
              sx={{ minWidth: 750 }}
              aria-labelledby="tableTitle"
            >
              <EnhancedTableHead
                numSelected={selected.length}
                order={order}
                orderBy={orderBy}
                onSelectAllClick={handleSelectAllClick}
                onRequestSort={handleRequestSort}
                rowCount={rows.length}
              />
              <TableBody>
                {visibleRows.map((row, index) => {
                  const isItemSelected = selected.includes(row.rowId);
                  const labelId = `enhanced-table-checkbox-${index}`;
  
                  return (
                    <TableRow
                      hover
                      onClick={(event) => handleClick(event, row.rowId)}
                      role="checkbox"
                      aria-checked={isItemSelected}
                      tabIndex={-1}
                      key={row.uuid}
                      selected={isItemSelected}
                      sx={{ cursor: 'pointer' }}
                    >
                      <TableCell padding="checkbox">
                        <Checkbox
                          color="primary"
                          checked={isItemSelected}
                          inputProps={{
                            'aria-labelledby': labelId,
                          }}
                        />
                      </TableCell>
                      <TableCell
                        component="th"
                        id={labelId}
                        scope="row"
                        padding="none"
                      >
                        <Link
                          component={RouterLink}
                          to={{
                            pathname: `/farm`,
                            search: `?id=${row.uuid}`
                          }}>
                          {row.name}
                        </Link>
                      </TableCell>
                      <TableCell align="right">{row.uuid}</TableCell>
                      <TableCell align="right">{row.created_on_date.toLocaleString()}</TableCell>
                      <TableCell align="right">{row.is_connected ? "Connected" : "Disconnected"}</TableCell>
                      <TableCell align="right">{row.is_connected ? (row.cage_status == 1 ? "Up" : "Down") : "Disconnected"}</TableCell>
                      <TableCell align="right">{row.last_update_date.toLocaleString()}</TableCell>
                    </TableRow>
                  );
                })}
                {emptyRows > 0 && (
                  <TableRow
                  >
                    <TableCell colSpan={6} />
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
          <TablePagination
            rowsPerPageOptions={[5, 10, 25]}
            component="div"
            count={rows.length}
            rowsPerPage={rowsPerPage}
            page={page}
            onPageChange={handleChangePage}
            onRowsPerPageChange={handleChangeRowsPerPage}
          />
        </Paper>
      </Box>
    );
}