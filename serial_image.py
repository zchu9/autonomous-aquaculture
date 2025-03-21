import serial

# --- Config ---
PORT = 'COM14'
BAUD = 115200
START_MARKER = b'IMG_START'
END_MARKER = b'IMG_END'
OUTPUT_FILE = 'captured_image.jpg'

# Open serial connection
ser = serial.Serial(PORT, BAUD, timeout=5)
ser.flush()

# Send 'Capture' command
ser.write(b'Capture\n')
print("[Python] Sent 'Capture' command, waiting for response...")

buffer = b''
in_image = False

while True:
    chunk = ser.read(512)

    if not chunk:
        print("[Python] Timeout or no data.")
        break

    buffer += chunk

    if not in_image:
        start_index = buffer.find(START_MARKER)
        if start_index != -1:
            print("[Python] Found IMG_START")
            buffer = buffer[start_index + len(START_MARKER):]
            in_image = True
        else:
            buffer = buffer[-len(START_MARKER):]

    if in_image:
        end_index = buffer.find(END_MARKER)
        if end_index != -1:
            image_data = buffer[:end_index]
            print(f"[Python] Found IMG_END — saving image ({len(image_data)} bytes)...")

            with open(OUTPUT_FILE, 'wb') as f:
                f.write(image_data)

            print(f"[Python] ✅ Image saved to {OUTPUT_FILE}")
            break
