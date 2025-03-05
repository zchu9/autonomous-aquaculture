#######################################################################
#This is also kinda dogwater :(
#Hopefully it'll work for tonight; likely to fine tune throughout the day
#May add ACK functionality depending on MCU side development progress
#######################################################################
import paho.mqtt.client as mqtt
import serial
import time

MQTT_BROKER = "73.152.188.231" 
MQTT_PORT = 1883
MQTT_SUB_TOPIC = "server_to_farm"  #!!!not a real topic!!!
MQTT_PUB_TOPIC = "farm_to_server" #!!!not a real topic!!!
LORA_PORT = "/dev/serial0"  #!!!need to confirm this!!!
BAUD_RATE = 9600 #!!!need to fine tune this!!!
PACKET_SIZE = 100 #!!!need to fine tune this!!!
MAX_PACKETS = 10  #!!!need to fine tune this!!!
BUFFER_SIZE = 240 #constant

total_packets = -1
received_packets = ["" for _ in range(MAX_PACKETS)]

# Initialize LoRa module
lora = serial.Serial(LORA_PORT, BAUD_RATE, timeout=1)
time.sleep(1) #!!!need to fine tune this!!!

def send_command(command):
    lora.write((command + "\r\n").encode('utf-8'))
    time.sleep(0.1) #!!!need to fine tune this!!!
    lora.flushInput()

def configure_lora():
    send_command("AT+IPR=9600")
    send_command("AT+CRFOP=3")
    send_command("AT+NETWORKID=6")
    send_command("AT+BAND=915000000")
    send_command("AT+PARAMETER=9,7,1,12")
    send_command("AT+ADDRESS=0") #!!!same as default transciever address; may cause problems during farm expansion; needs fine tuning!!!
    print("LoRa module configured.") #debug info

configure_lora()

def send_fragmented_message(message):
    global total_packets
    total_packets = (len(message) + PACKET_SIZE - 1) // PACKET_SIZE
    for i in range(total_packets):
        fragment = message[i * PACKET_SIZE: (i + 1) * PACKET_SIZE]
        packet = f"{i},{total_packets},{fragment}"
        command = f"AT+SEND=1,{len(packet)},{packet}"
        print(f"Sending: {packet}") #debug info
        send_command(command)
        time.sleep(10)  #Don't overload the receiver, won't be necessary once ACKs are working.
    print("Message sent successfully!") #debug info

def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT Broker with result code {rc}") #debug info
    client.subscribe(MQTT_SUB_TOPIC)

def on_message(client, userdata, msg):
    message = msg.payload.decode("utf-8")
    print(f"Received MQTT message: {message}") #debug info
    send_fragmented_message(message)

def process_received_data(received):
    global total_packets
    print(f"Processing: {received}") #debug info
    if not received.startswith("+RCV="):
        return
    parts = received.split(",")
    if len(parts) < 6:
        return
    try:
        packet_id = int(parts[2])
        total_packets = int(parts[3])
        message_data = ",".join(parts[4:]).rsplit(",", 2)[0]
        received_packets[packet_id] = message_data
        print(f"Storing Packet {packet_id + 1} of {total_packets}: {message_data}")
        if all_packets_received():
            reconstruct_message()
    except ValueError:
        print("Error parsing received data") #debug info

def all_packets_received():
    if total_packets == -1:
        return False
    return all(received_packets[i] for i in range(total_packets))

def reconstruct_message():
    print("Reassembling message...") #debug info
    full_message = "".join(received_packets[:total_packets])
    print(f"Final Reconstructed Message: {full_message}") #debug info
    #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    # This is where a function call would go to decipher and format the message before publishing it to the server
    #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    mqtt_client.publish(MQTT_PUB_TOPIC, full_message)

mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60) #!!!potentially adust keep alive interval!!!
mqtt_client.loop_start()

buffer = ""
while True:
    if lora.in_waiting:
        char = lora.read().decode("utf-8")
        if char == '\n':
            process_received_data(buffer.strip())
            buffer = ""
        else:
            buffer += char
