
import ModalTemplate from '../comm/ModalTemplate'
import Typography from '@mui/material/Typography';


export default function AddDeviceModal() {
    return (
        <>
            <ModalTemplate label="Add new Device">
                <Typography id="modal-modal-title" variant="h6" component="h2">
                    Text in a modal
                </Typography>
                <Typography id="modal-modal-description" sx={{ mt: 2 }}>
                    Duis mollis, est non commodo luctus, nisi erat porttitor ligula.
                </Typography>
            </ModalTemplate>
        </>
    )
}