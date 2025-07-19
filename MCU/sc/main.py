import serial
import time
from rich.logging import RichHandler
import logging

def connectLoop(ser:serial.Serial):
    noConnection = True
    attempts = 0
    while(noConnection):
        ser.write(bytes("sclink\n", 'utf-8'))
        response = ser.readline()
        if response == b"connected":
            noConnection = False
        else:
            attempts += 1
            log.info(f"{response}...")
            time.sleep(attempts % 5)

    print("connected")




def commandLoop(ser:serial.Serial):
    while 1:
        command = input()
        if command == "q":
            quit("Exiting..")
        ser.write(bytes(command, "utf-8"))
        log.info(f"Sent command: {command}")
        while command == "r":
            response = ser.readline()
            if response != b"":
                log.debug(f"Response: {response.decode()[:-1]}")
                ser.reset_input_buffer()
                break


FORMAT = "%(message)s"
logging.basicConfig(
    level="NOTSET", format=FORMAT, datefmt="[%X]", handlers=[RichHandler()]
)
log = logging.getLogger(__name__)
def main():

    ser = serial.Serial(port="/dev/ttyACM0", baudrate=115200, timeout=0.2)
    connectLoop(ser)
    commandLoop(ser)


if __name__ == "__main__":
    main()
