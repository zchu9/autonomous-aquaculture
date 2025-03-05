import { useState, useEffect } from 'react'
import { useSearchParams } from 'react-router-dom';

import Button from '@mui/material/Button';
import Container from '@mui/material/Container';
import Grid from '@mui/material/Grid2';
import Box from '@mui/material/Box';
import DeviceInfoPane from './DeviceInfoPane';
import {DeviceDataPane, IQueryResult} from './DeviceDataPane';
import ArrowBackOutlinedIcon from '@mui/icons-material/ArrowBackOutlined';
import { Link as RouterLink } from 'react-router-dom';


const base_url = `${import.meta.env.VITE_API_URL}`

const device_data: IQueryResult[] = [
  {date: "1/21/25", temperature_c: 37.01, height: 0.1},
  {date: "1/22/25", temperature_c: 35.6, height: 0.11},
  {date: "1/23/25", temperature_c: 36.8, height: 1.2},
  {date: "1/24/25", temperature_c: 40.1, height: 1.4},
  {date: "1/25/25", temperature_c: 39.2, height: 0.0},
  // {date: "HI", temperature_c: 0.01},
  // {date: "HELLO", temperature_c: 0.02},
  // {date: "HEY", temperature_c: 0.02},
  // {date: "YO", temperature_c: 0.02},
  // {date: "NI HAO MA", temperature_c: 0.02},
]

export default function DevicePage() {
    const [deviceUUID, setDeviceUUID] = useSearchParams();
    const uuid = deviceUUID.get('id');

    const paramsObject = Object.fromEntries(deviceUUID);

    async function fetchSensorData() {
        console.log(`Fetching device data from ${base_url}...`);
    
        try {
          const response = await fetch(base_url + '/sensor_data/' + uuid, {
            method: 'GET',
            headers: {
              'Content-Type': 'application/json'
            },
          });
          if (!response.ok) {
            const errorData = await response.json();
            throw new Error(errorData.message || "Something went wrong");
          }
    
          // const data = await response.json();
          // setResult(data.message);
        } catch (error) {
          console.error('Error fetching sensor data:', error);
        }
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
          // const data = await response.json();
          // setResult(data.message);
        } catch (error) {
          console.error('Error fetching sensor data:', error);
        }
    }
    
    
    useEffect(() => {
    
    fetchSensorData();

    const intervalId = setInterval(fetchSensorData, 60000);

    return () => clearInterval(intervalId);
    }, []);

    return (
        <Container maxWidth="false" sx={{
          flexGrow: 1
        }}>
          <Box sx={{
            p: 2,
            textAlign: 'left'
          }}>
            <Button 
              variant="contained"
              startIcon={<ArrowBackOutlinedIcon/>}
              component={RouterLink}
              to='/'>
                  Return to dashboard
            </Button>
          </Box>
          
          <Grid container spacing={4}>
            <Grid size={6}>
              <DeviceInfoPane uuid={deviceUUID}/>
            </Grid>
            <Grid size={6}>
              <DeviceDataPane data={device_data}/>
            </Grid>
          </Grid>
        </Container>
    )
}