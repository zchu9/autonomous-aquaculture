import Button from '@mui/material/Button';
import Box from '@mui/material/Box';
import EditOutlinedIcon from '@mui/icons-material/EditOutlined';
import FormControlLabel from '@mui/material/FormControlLabel';
import Grid from '@mui/material/Grid2';
import Stack from '@mui/material/Stack';

import IOSSwitch from '../comm/Switch';

import Paper from '@mui/material/Paper';
import { styled } from '@mui/material/styles';

interface ModalTemplateProps {
    uuid: string
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

export default function DeviceInfoPane(props: ModalTemplateProps) {
    return (
        <InfoPane>
            <Stack spacing={2}>
                <Grid container spacing={2}>
                    <Grid size={6}>
                        <InfoPaneRow>
                            <h2>Device Name</h2>
                            UUID
                        </InfoPaneRow>
                    </Grid>
                    <Grid size={6} sx={{textAlign: 'right'}}>
                        <Button
                            variant="contained"
                            startIcon={<EditOutlinedIcon/>}
                            onClick = { () => {
                                console.log("Uh-oh");
                            }
                            }>
                            Edit
                        </Button>
                    </Grid>
                </Grid>

                <InfoPaneRow>
                    Created On:
                </InfoPaneRow>

                <Box></Box>

                <Grid container spacing={2}>
                    <Grid size={6}>
                        <InfoPaneRow>
                            Device Status:
                        </InfoPaneRow>
                    </Grid>
                    <Grid size={6}>
                        <InfoPaneRow>
                            Received Status On: 
                        </InfoPaneRow>
                    </Grid> 
                    <Grid size={6}>
                        <InfoPaneRow>
                            Cage Status: 
                        </InfoPaneRow>
                    </Grid>
                    <Grid size={6}>
                        <InfoPaneRow>
                            Next Scheduled Operation: 
                        </InfoPaneRow>
                    </Grid>
                </Grid>

                <Grid container spacing={2}>
                    <Grid size={1}>
                        Active
                    </Grid>
                    <Grid>
                        <Box>
                            <FormControlLabel
                                control={<IOSSwitch sx={{ m : 1}}/>}>
                            </FormControlLabel>
                        </Box>
                    </Grid>
                
                </Grid>
            </Stack>
        </InfoPane>
        // // <div>

        //     {/* 

        //     <Box>
        //         
                

        //         <h3>UUID: {props.uuid}</h3>
        //         <h3>Created on</h3>
        //         <h3>Device Status (Last updated Updated mm/dd/yy)</h3>
        //         <h3>Cage Status (Last updated mm/dd/yy)</h3>

        //         <h3>Last Scheduled Operation (Sent mm/dd/yy)</h3>
        //         <h3>Next Scheduled Operation (Sent mm/dd/yy)</h3>

        //         <div>
        //             <h3>Active</h3>
        //             
        //         </div>
        //     </Box> */}
        // // </div>
    )
}