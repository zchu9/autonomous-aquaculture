
import * as React from 'react';
import ModalTemplate from './ModalTemplate'
import Typography from '@mui/material/Typography';
import Box from '@mui/material/Box';
import { DateTimePicker } from '@mui/x-date-pickers';
import { AdapterDayjs } from '@mui/x-date-pickers/AdapterDayjs';
import { LocalizationProvider } from '@mui/x-date-pickers/LocalizationProvider';
import MenuItem from '@mui/material/MenuItem';
import FormControl from '@mui/material/FormControl';
import Select, { SelectChangeEvent } from '@mui/material/Select';
import InputLabel from '@mui/material/InputLabel';
import Button from '@mui/material/Button';
import SendIcon from '@mui/icons-material/Send';
import Checkbox from '@mui/material/Checkbox';
import Stack from '@mui/material/Stack';
import Chip from '@mui/material/Chip';
import { styled } from '@mui/material/styles';
import Paper from '@mui/material/Paper';
import DeviceData from '../MainDash/DeviceTable'

const base_url = `${import.meta.env.VITE_API_URL}`

interface ScheduleModalProps {
    closeFn: any;
    devices: DeviceData[]
}

const ListItem = styled('li')(({ theme }) => ({
    margin: theme.spacing(0.5),
}));

export default function ScheduleOpModal(props: ScheduleModalProps) {

    const [doSendNow, setDoSendNow] = React.useState<boolean>(false);
    const [dateIsValid, setDateIsValid] = React.useState<boolean>(false);
    const [isValidOpSelected, setIsValidOpSelected] = React.useState<boolean>(false);
    const [scheduleDate, setScheduleDate] = React.useState<Date>();
    const [operation, setOperation] = React.useState<boolean>();

    const [devices, setDevices] = React.useState<readonly DeviceData[]>(props.devices);

    const handleDelete = (chipToDelete: Devices) => () => {
        setDevices((chips) => chips.filter((chip) => chip.key !== chipToDelete.key));
    };



    async function handleConfirm() {
        const api_route = (operation) ? "/lowerCages" : "/liftCages"
        const ids = devices.map((device: DeviceData) => {
            return device._id
        });

        try{
            const response = await fetch(base_url + api_route, {
                method: 'POST',
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    ids: ids,
                    date: scheduleDate
                })
            });
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }
        } catch (error) {
            console.error('Error sending command:', error);
        }

        props.closeFn();
    }

    async function testAPI() {
        console.log(`Hitting ${base_url}/test_pub...`);
    
        try {
          const response = await fetch(base_url + '/test_pub', {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json'
            },
          });
          if (!response.ok) {
            const errorData = await response.json();
            throw new Error(errorData.message || "Something went wrong");
          }
    
          console.log("Successfully hit!");
        } catch (error) {
          console.error('Error fetching sensor data:', error);
        }
    }

    const handleSendNowClicked = (event: React.ChangeEvent<HTMLInputElement>) => {
        setDoSendNow(event.target.checked);
    };

    const handleDateTimeChosen = (value: TValue, context: FieldChangeHandlerContext) => {
        if (value != null) {
            var d = value["$d"];
            if (context.validationError == null) {
                setScheduleDate(d);
                setDateIsValid(true);
            } else {
                setDateIsValid(false);
            }
        }
    }

    const handleOperationChosen = (event: SelectChangeEvent, child?: object) => {
        var choice = event.target.value;
        if (choice != null) {
            setOperation(choice == 1);
            setIsValidOpSelected(true);
        } else {
            setIsValidOpSelected(false);
        }
        console.log(isValidOpSelected);
    }

    return (
        <LocalizationProvider dateAdapter={AdapterDayjs}>
            <ModalTemplate label="Schedule Bulk Operation">

                <Stack spacing={2}>
                    <h3>Selected Devices:</h3>

                    <Box
                        sx={{
                            display: 'flex',
                            justifyContent: 'center',
                            flexWrap: 'wrap',
                            listStyle: 'none',
                            p: 0.5,
                            m: 0,
                        }}
                        component="ul"
                    >
                        {devices.map((data, idx) => {
                            return (
                            <ListItem key={idx}>
                                <Chip
                                    label={data._id}
                                    onDelete={handleDelete(data)}
                                />
                            </ListItem>
                            );
                        })}
                    </Box>

                    <h3>Choose Date/Time:</h3>
                    
                    <DateTimePicker 
                        disablePast
                        disabled={doSendNow}
                        onAccept={handleDateTimeChosen}
                        />

                    <Box sx={{ minWidth: 120 }}>
                        <h3>Send Now</h3>
                        <Checkbox
                            color="primary"
                            onChange={handleSendNowClicked}
                            inputProps={{
                            'aria-label': 'schedule-now',
                            }}
                        />
                    </Box>
                    

                    <h3>Choose Operation:</h3>  
                    
                    <FormControl fullWidth>
                        <InputLabel id="operation-select-label">Operation</InputLabel>
                        <Select
                            labelId="operation-select-label"
                            id="operation-select"
                            label="Operation"
                            defaultValue={''}
                            onChange={handleOperationChosen}
                            >
                                <MenuItem value={0}>Up</MenuItem>
                                <MenuItem value={1}>Down</MenuItem>
                        </Select>
                    </FormControl>

                    

                    <Button
                        variant="contained"
                        startIcon={<SendIcon/>}
                        disabled = {!((dateIsValid || doSendNow) && isValidOpSelected)}
                        onClick = {handleConfirm}>
                            Send
                    </Button>
                </Stack>
            </ModalTemplate>
        </LocalizationProvider>
    )
}