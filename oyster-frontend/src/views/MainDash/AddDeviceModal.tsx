
import ModalTemplate from '../comm/ModalTemplate'
import Button from '@mui/material/Button';
import Stack from '@mui/material/Stack';
import Typography from '@mui/material/Typography';

import AddCircleOutlineOutlinedIcon from '@mui/icons-material/AddCircleOutlineOutlined';

const base_url = `${import.meta.env.VITE_API_URL}`

interface AddDeviceModalProps {
    closeFn: any
}

export default function AddDeviceModal(props: AddDeviceModalProps) {

    async function createDevice() {
        console.log("Adding new device");
        try{
            const response = await fetch(base_url + '/addFarm', {
                method: 'POST',
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    location: "somewhere",
                    cage_position: "up",
                    created_at: new Date(),
                    status: "Disconnected"
                })
            });
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }
        } catch (error) {
            console.error('Error fetching sensor data:', error);
        }
    }

    return (
        <>
            <ModalTemplate label="Add new Device">
                {/* <Typography id="modal-modal-title" variant="h6" component="h2">
                    Text in a modal
                </Typography>
                <Typography id="modal-modal-description" sx={{ mt: 2 }}>
                    Duis mollis, est non commodo luctus, nisi erat porttitor ligula.
                </Typography> */}
                <Stack spacing={2}>
                    <Button
                        variant="contained"
                        // disabled = {!((dateIsValid || doSendNow) && isValidOpSelected)}
                        onClick = {
                            () => {
                                createDevice()
                                // testAPI();
                                props.closeFn()
                            }
                        }>
                            Confirm
                    </Button>
                </Stack>
            </ModalTemplate>
        </>
    )
}