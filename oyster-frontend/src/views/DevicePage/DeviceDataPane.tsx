import * as React from 'react';
import Box from '@mui/material/Box';;
import { LineChart, XAxis, YAxis, Tooltip, CartesianGrid, Line, Legend } from 'recharts';
import Paper from '@mui/material/Paper';
import { styled } from '@mui/material/styles';

export interface IQueryResult {
    date: string;
    temperature_c: number;
    height: number;
  }

interface DeviceDataPaneProps {
    data: IQueryResult[]
}

const InfoPane = styled(Paper)(({ theme }) => ({
    // width: 120,
    // height: 120,
    padding: theme.spacing(2),
    // ...theme.typography.body2,
  }));

const InfoPaneRow = styled(Box)(({ them }) => ({
    textAlign: 'left',
}))

export function DeviceDataPane(props: DeviceDataPaneProps) {
    const [id, setId] = React.useState(0);

    return (
        <InfoPane>
            <h2>Sensor Data</h2>
            <LineChart width={500} height={400} data={props.data} margin={{ top: 5, right: 5, left: 5, bottom: 100 }}>
                <CartesianGrid stroke="#f5f5f5" />
                <XAxis dataKey="date" angle={0} interval={"preserveStartEnd"} position="bottom"/>
                <YAxis yAxisId={0} hide={id != 0} label={{value: "Temperature (C)", angle:-90, position:"insideLeft"}}/>
                <YAxis yAxisId={1} hide={id != 1} label={{value: "Height (m)", angle:-90, position:"insideLeft"}}/>
                <Tooltip offset={-100}/>
                <Legend verticalAlign="top" height={36}/>
                <Line type="monotone" dataKey="temperature_c" stroke="#8884d8" yAxisId={0} onMouseOver={()=>{setId(0)}}/>
                <Line type="monotone" dataKey="height" stroke="#82ca9d" yAxisId={1} onMouseOver={()=>{setId(1)}}/>
            </LineChart>
        </InfoPane>
    )
}