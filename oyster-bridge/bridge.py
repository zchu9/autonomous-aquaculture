import paho.mqtt.client as mqtt
import serial
import time
import json
import base64
import logging
from io import BytesIO
from PIL import Image, ImageFile
import os

MQTT_BROKER = "mqtt" #fixed assuming you're running this on the same device as the server
MQTT_PORT = 1883 #fixed port for MQTT with TLS
LORA_PORT = '/dev/serial0' #DON'T MODIFY; it's the name docker expects rather than the actual port name
SAVE_FILE = "data/address_lookup.json" #the file name for where we're storing the address map
BAUD_RATE = 9600 #customizeable, but I'd suggest you keep it low
LORA_PASSWORD = "A3F7B9C2" # 00000000 to FFFFFFFF; must match whatever you're using as the password during the GUI configs
PACKET_SIZE = 100 # relative max when accounting for the header space
MAX_PACKETS = 500 # possibly increase; size doesn't really matter on the Pi side
BUFFER_SIZE = 260 # max incomming packet can only be 256; extra bytes appended to buffer for safety
#(Timeout*Retries) MUST BE LARGER THAN THE ARDUINO SIDE (Timeout*Retries) FOR RELIABLE CAGE LIFTS DUE TO LORA BEING HALF DUPLEX
ACK_TIMEOUT = 3 #modify as needed
RETRY_LIMIT = 100 #modify as needed

# v debug line v
#ImageFile.LOAD_TRUNCATED_IMAGES = True
# ^ debug line ^

#logging object
logging.basicConfig(level=logging.DEBUG)

total_packets = -1
received_packets = ["" for _ in range(MAX_PACKETS)]
mqtt_task_list = []
image_data_base64 = ""
current_address = -1
address_lookup = {}

#serial object
lora = serial.Serial(LORA_PORT, BAUD_RATE, timeout=1)

def add_address(id, address):
    """Add an ID-address pair to the lookup table."""
    address_lookup[id] = address
    save_addresses()

def get_address(id):
    """Fetch the address based on the given ID."""
    return address_lookup.get(id, None)

def get_id_by_address(address):
    """Fetch the ID based on the given address."""
    for id, addr in address_lookup.items():
        if int(addr) == int(address):
            return id
    return None

def save_addresses():
    """Save the address lookup table to a JSON file."""
    with open(SAVE_FILE, "w") as f:
        json.dump(address_lookup, f)

def load_addresses():
    """Load the address lookup table from a JSON file."""
    global address_lookup
    if os.path.exists(SAVE_FILE):
        with open(SAVE_FILE, "r") as f:
            address_lookup = json.load(f)

#helper function for communicating with the LoRa module
def send_command(command):
    lora.write((command + "\r\n").encode('utf-8'))
    time.sleep(0.1)
    line = b""
    while True:
        char = lora.read(1)
        if not char:
            break
        line += char
        if char == b'\n':
            break
    logging.info(f"Flushing LoRa setup buffer: {(line.decode('utf-8').strip())}")
    lora.flushInput()

#helper function for setting up the LoRa module
def configure_lora():
    send_command("AT+IPR=9600")
    send_command("AT+CRFOP=22")
    send_command("AT+NETWORKID=6")
    send_command("AT+BAND=915000000")
    send_command("AT+PARAMETER=9,7,1,12")
    send_command("AT+ADDRESS=1")
    send_command(f"AT+CPIN={LORA_PASSWORD}")
    logging.info("LoRa module configured.")

try:
    load_addresses()
except:
    logging.error("Failed to load saved LoRa addreses:")
try:
    configure_lora()
except:
    logging.error("Failed to configure LoRa module:")

#helper function to fragment and send messages
def send_fragmented_message(message, address):
    try:
        global total_packets
        header_size = len(f"{0},{0},")
        fragment_size = PACKET_SIZE - header_size
        total_packets = (len(message) + fragment_size - 1) // fragment_size
        for i in range(total_packets):
            fragment = message[i * fragment_size: (i + 1) * fragment_size]
            packet_header = f"{i},{total_packets},"
            packet = packet_header + fragment
            command = f"AT+SEND={address},{len(packet)},{packet}"
            retries = 0
            while retries < RETRY_LIMIT:
                logging.info(f"Sending: {packet}")
                send_command(command)
                if wait_for_ack(i):
                    break
                retries += 1
            else:
                logging.info(f"Failed to receive ACK for packet {i}")
                return
    except Exception as e:
        logging.error(f"Error sending message: {e}")

#helper function to wait for and process acknowledgement messages
def wait_for_ack(expected_id):
    try:
        start_time = time.time()
        buffer = ""
        while time.time() - start_time < ACK_TIMEOUT:
            if lora.in_waiting:
                char = lora.read().decode("utf-8")
                if char == '\n':
                    logging.info(f"Received buffer: {buffer.strip()}")
                    
                    if "ACK:" in buffer:
                        ack_id = int(buffer.split("ACK:")[1].split(",")[0].strip())
                        if ack_id == expected_id:
                            logging.info(f"Received ACK for packet {expected_id}")
                            return True
                    buffer = ""
                else:
                    buffer += char
        return False
    except Exception as e:
        logging.error(f"Error wait_for_ACK: {e}")

#on message MQTT callback function
def on_message(client, userdata, msg):
    try:
        message = msg.payload.decode("utf-8")
        logging.info(f"Received MQTT message: {message}")
        logging.info(f"Received MQTT message: {msg.topic}")
        topic_parts = msg.topic.split("/")
        logging.info(f"Received MQTT message: {topic_parts}")
        address=get_address(topic_parts[1])
        logging.info(f"LoRa address of Farm ID {address}")
        if address:
                mqtt_task_list.append((message, address))
                logging.info(f"Appended task with address {address}")
    except Exception as e:
        logging.error(f"Error within on_message callback: {e}")

#helper function to process recieved data
def process_received_data(received):
    try:
        global total_packets, current_address
        logging.info(f"Processing: {received}")
        if not received.startswith("+RCV="):
            return
        parts = received.split(",")
        if len(parts) < 6:
            return
        address = parts[0].split('=')[-1]
        if (current_address==-1):
            current_address=int(address)
        if (current_address==int(address)):
            packet_id = int(parts[2])
            total_packets = int(parts[3])
            message_data = ",".join(parts[4:]).rsplit(",", 2)[0]
            for i in range(packet_id, MAX_PACKETS):
                received_packets[i] = ""
            received_packets[packet_id] = message_data
            logging.info(f"Storing Packet {packet_id + 1} of {total_packets}: {message_data}")
            ack_message = f"ACK:{packet_id}"
            logging.info("sending ack")
            send_command(f"AT+SEND={address},{len(ack_message)},{ack_message}")
            if all_packets_received():
                reconstruct_message(address)
    except Exception as e:
        logging.error(f"Failed to process recieved fragment: {e}")

#helper function to identify when all packets have been recieved
def all_packets_received():
    if total_packets == -1:
        return False
    return all(received_packets[i] for i in range(total_packets))

#helper function to reconstruct and deal with a recieved message
def reconstruct_message(address):
    try:
        global total_packets, received_packets, image_data_base64, current_address
        logging.info("Reassembling message...")
        full_message = "".join(received_packets[:total_packets])
        logging.info(f"Final Reconstructed Message: {full_message}")
        received_packets = ["" for _ in range(MAX_PACKETS)]
        total_packets = -1
        current_address = -1
        if full_message.strip().startswith("{"):
            try:
                data = json.loads(full_message)
                if "farm_id" in data:
                    #processing for handshake packets
                    logging.info("Recieved Handshake Packet")
                    logging.info(f"Farm ID: {data.get('farm_id')}")
                    logging.info(f"Farm LoRa Address: {data.get('LoRa_address')}")
                    add_address(data.get("farm_id"),data.get("LoRa_address"))
                else:
                    #processing for data packets
                    logging.info("Recieved Data Packet")
                    logging.info(f"LoRa address: {address}")
                    farm_id=get_id_by_address(address)
                    logging.info(f"Corresponding Farm ID: {farm_id}")
                    mqtt_client.publish(f"farm/{farm_id}/sensorData", full_message["sensor"])
                    mqtt_client.publish(f"farm/{farm_id}/systemLevels", full_message["power"])
                    mqtt_client.publish(f"farm/{farm_id}/status", "1")
            except json.JSONDecodeError as e:
                logging.error(f"JSON decoding failed: {e}")
        else:
            #if it's not JSON formated, then it's an image chunk
            try:
                decoded_chunk = base64.b64decode(full_message.strip())
            except Exception as e:
                logging.error(f"Failed to decode chunk, skipping: {e}")
                return
            logging.info("Recieved Image Packet")
            if b'\xFF\xD8' in decoded_chunk:
                logging.info("Image start detected.")
                image_data_base64 = full_message
            elif b'\xFF\xD9' in decoded_chunk:
                logging.info("Image end detected.")
                image_data_base64 += full_message
                try:
                        full_decoded_message=base64.b64decode(image_data_base64)
                        if ((b'\xFF\xD8' in full_decoded_message) and (b'\xFF\xD9' in full_decoded_message)):
                            json_document = {
                                "camera": image_data_base64
                            }
                            json_string = json.dumps(json_document)
                            farm_id=get_id_by_address(address)
                            mqtt_client.publish(f"farm/{farm_id}/sensorData", json_string)
                            mqtt_client.publish(f"farm/{farm_id}/status", "1")
                except Exception as e:
                    logging.error(f"Failed to decode or show image: {e}")      
                image_data_base64 = "" 
            else:
                logging.info("Image middle chunk.")
                image_data_base64 += full_message
    except Exception as e:
        logging.error(f"Failed to process recieved packet: {e}")
        
#initial MQTT connection setup
try:
    mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, "LoRa_Bridge")
    mqtt_client.on_connect = lambda client, userdata, flags, rc, properties: client.subscribe("farm/+/cage")
    mqtt_client.on_message = on_message
    mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
    mqtt_client.loop_start()
except Exception as e:
    logging.error(f"Failed to connect to MQTT broker: {e}")

#main loop
buffer = ""
while True:
    #if theres a message to send, send it
    if (mqtt_task_list):
        try:
            message, address = mqtt_task_list[0]
            send_fragmented_message(message, address)
            mqtt_task_list.remove(mqtt_task_list[0])
        except Exception as e:
                logging.error(f"Failed to proccess item from task list: {e}")
    #else, listen
    else:
        try:
            if lora.in_waiting:
                char = lora.read().decode("utf-8")
                if char == '\n':
                    process_received_data(buffer.strip())
                    buffer = ""
                else:
                    buffer += char
        except Exception as e:
                logging.error(f"Failed to decode LoRa transmission char: {e}")
