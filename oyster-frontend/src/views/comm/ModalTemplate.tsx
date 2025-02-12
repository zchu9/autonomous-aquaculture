import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';
import Box from '@mui/material/Box';
import { ReactElement } from 'react';


interface ModalTemplateProps {
    label: string;
    children: ReactElement[]
}

const style = {
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform: 'translate(-50%, -50%)',
    width: 400,
    bgcolor: 'background.paper',
    border: '2px solid #000',
    boxShadow: 24,
    p: 4,
};

export default function AddDeviceModal(props: ModalTemplateProps) {
    return (
        <>
            <Box sx={style}>
                <h2> {props.label}</h2>
                {props.children}
            </Box>
        </>
    )
}
