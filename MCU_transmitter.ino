//////////////////////////////////////////////////////////////
//This is all dog water :(
//It doesn't even support acknowledgements yet :(
//I don't even have transmission and receipt integrated into the same program yet :(
//At least fragmentation and reassembly work :/
//////////////////////////////////////////////////////////////

#include <Arduino.h>

#define TARGET_ADDRESS "0"
#define PACKET_SIZE 100 //!!!fine tune this!!!
#define MESSAGE "According to all known laws of aviation there is no way a bee should be able to fly. Its wings are too small to get its fat little body off the ground. The bee of course flies anyway because bees don't care what humans think is impossible."

char message[] = MESSAGE;
int currentPacket = 0;
int totalPackets;

void setup() {
    Serial.begin(19200); //debug serial
    Serial1.begin(9600); //lora serial
    delay(100);
    Serial1.println("AT+IPR=9600");
    delay(100);
    Serial1.println("AT+CRFOP=3"); //!!!set max(22) in future!!!
    delay(100);
    Serial1.println("AT+NETWORKID=6");
    delay(100);
    Serial1.println("AT+BAND=915000000");
    delay(100);
    Serial1.println("AT+PARAMETER=9,7,1,12");
    delay(100);
    Serial1.println("AT+ADDRESS=1");
    delay(2000);
    Serial.println("Transceiver setup complete!!! :D");

    totalPackets = (strlen(message) + PACKET_SIZE - 1) / PACKET_SIZE;
    sendPackets();
}

void sendPackets() {
    char fragment[PACKET_SIZE + 1];
    for (int i = 0; i < totalPackets; i++) {
        int startIdx = i * PACKET_SIZE;
        int length = min(PACKET_SIZE, (int)strlen(message) - startIdx);
        strncpy(fragment, message + startIdx, length);
        fragment[length] = '\0';
        sendFragment(i, fragment);
        delay(10000);
    }

    Serial.println("Message sent successfully!");
}

void sendFragment(int packetID, const char* fragment) {
    char packet[PACKET_SIZE + 20];
    snprintf(packet, sizeof(packet), "%d,%d,%s", packetID, totalPackets, fragment);
    
    char command[PACKET_SIZE + 40];
    snprintf(command, sizeof(command), "AT+SEND=%s,%d,%s", TARGET_ADDRESS, (int)strlen(packet), packet);
    //debug info
    Serial.print("Sending: ");
    Serial.println(packet);
    Serial1.println(command);
}

void loop() {
}
