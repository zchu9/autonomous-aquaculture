import { AppBar, Toolbar, Typography } from "@mui/material";

interface BarProps {
  title: string;
}

export default function Bar(props: BarProps) {
  return (
    <>
      <AppBar
        position="static"
        sx={{
          backgroundColor: "#1976d2",
          //   alignItems: "center",
          justifyContent: "center",
          display: "flex",
        }}
      >
        <Toolbar>
          <Typography variant="h6" component="div" sx={{ flexGrow: 1 }}>
            {props.title}
          </Typography>
          <img
            src="logo.png" // Replace with the actual logo path
            alt="Company Logo"
            style={{
              height: "80px",
              marginRight: "10px",
              float: "right",
              padding: 4,
            }}
          />
        </Toolbar>
      </AppBar>
    </>
  );
}
