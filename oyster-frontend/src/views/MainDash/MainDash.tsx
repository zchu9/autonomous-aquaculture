import { useState, useEffect } from 'react'

import DeviceTable from './DeviceTable';

const base_url = `${import.meta.env.VITE_API_URL}`


function MainDash() {

  async function fetchSensorData() {
    console.log(`Fetching device data from ${base_url}...`);

    try {
      const response = await fetch(base_url + '/device_data', {
        method: 'GET',
        headers: {
          
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

  useEffect(() => {
    
    fetchSensorData();

    const intervalId = setInterval(fetchSensorData, 60000);

    return () => clearInterval(intervalId);
  }, []);

  return (
    <>
    <h1>Main Dashboard</h1>

      <div className="card">
        <DeviceTable/>
      </div> 
    </>
  )
}

export default MainDash

