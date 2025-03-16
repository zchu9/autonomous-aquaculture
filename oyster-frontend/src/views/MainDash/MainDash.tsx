import * as React from 'react';
import { useState, useEffect } from 'react'

import DeviceTable from './DeviceTable';
import DeviceData from '../comm/DeviceDataInterface';

const base_url = `${import.meta.env.VITE_API_URL}`


function MainDash() {

  const [data, setData] = React.useState<DeviceData[]>([]);

  async function fetchSensorData() {
    console.log(`Fetching device data from ${base_url}...`);

    try {
      const response = await fetch(base_url + '/farm', {
        method: 'GET'
      });
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const data = await response.json();

      const formattedData: DeviceData[] = data.map((device: any, index: number) => ({
        ...device,
        rowId: index,
        created_at: new Date(device.created_at)
      }));

      setData(formattedData);
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
    <>
    <h1>Main Dashboard</h1>

      <div className="card">
        <DeviceTable rows={data}/>
      </div> 
    </>
  )
}

export default MainDash

