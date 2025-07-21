import serial
# import time

import serial.tools
import serial.tools.list_ports
from rich.logging import RichHandler
# from rich import print
import logging

FORMAT = "%(message)s"
logging.basicConfig(
    level="NOTSET", format=FORMAT, datefmt="[%X]", handlers=[RichHandler()]
)
log = logging.getLogger(__name__)
def main():
    while 1:
        ports = serial.tools.list_ports.grep("Arduino|Feather", include_links=True)
        for port in ports:
            try:
                ser = serial.Serial(port=port[0], baudrate=115200, timeout=0.2)
                if ser.is_open:
                    log.info(f"Connected to {port}")
                    while 1:
                        message = ser.read_until(b'\n')
                        if message != b"":
                            log.debug(message.decode()[:-1])

                    
            except serial.SerialException as e:
                ser.close()
                log.error(e)

if __name__ == "__main__":
    main()
