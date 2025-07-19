import serial
import time

def stamp() -> str:
    return time.strftime("%H:%M:%S",time.localtime())

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
            print(f"{response}...")
            time.sleep(attempts % 5)

    print("connected")




def commandLoop(ser:serial.Serial):
    while 1:
        command = input("Enter command: ")
        if command == "q":
            quit("Exiting..")
        ser.write(bytes(command, "utf-8"))
        print(f"{stamp()}: Sent command: {command}")
        while command == "r":
            response = ser.readline()
            if response != b"":
                print(f"{stamp()}: {response.decode()[:-1]}")
                ser.reset_input_buffer()
                break


def main():
    ser = serial.Serial(port="/dev/ttyACM0", baudrate=115200, timeout=0.2)
    connectLoop(ser)
    commandLoop(ser)


if __name__ == "__main__":
    main()
