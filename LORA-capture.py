import serial
import time

def capture_image(port='/dev/ttyACM0', baudrate=115200, output_file='captured_image.jpg'):
    # Open the serial connection.
    ser = serial.Serial(port, baudrate, timeout=3)
    time.sleep(2)  # Give some time for the port to initialize.
    
    # Flush any existing data and send the CAPTURE command.
    ser.reset_input_buffer()
    ser.write(b'CAPTURE\n')
    print("Sent CAPTURE command...")

    # Wait until we see the "IMG_START" marker.
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line == "IMG_START":
            print("Found IMG_START marker.")
            break

    # Read image data until we detect the "IMG_END" marker.
    image_data = bytearray()
    while True:
        chunk = ser.read(1024)  # Read in 1024-byte chunks.
        if not chunk:
            continue  # If the read times out, try again.
        if b"IMG_END" in chunk:
            # If the end marker is in this chunk, capture data up to that marker.
            end_index = chunk.find(b"IMG_END")
            image_data.extend(chunk[:end_index])
            print("Found IMG_END marker.")
            break
        else:
            image_data.extend(chunk)
    
    # Save the received image data to a JPEG file.
    with open(output_file, 'wb') as f:
        f.write(image_data)
        print(f"Image saved as {output_file} ({len(image_data)} bytes).")
    
    ser.close()

if __name__ == '__main__':
    # Adjust the port if needed
    capture_image(port='/dev/ttyACM0', baudrate=9600, output_file='captured_image.jpg')
