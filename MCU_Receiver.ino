//////////////////////////////////////////////////////////////
//This is all dog water :(
//It doesn't even support acknowledgements yet :(
//I don't even have have transmission and receipt integrated into the same program yet :(
//At least fragmentation and reasembly work :/
//////////////////////////////////////////////////////////////

#include <Arduino.h>

#define MAX_PACKETS 10 // !!!I pulled this number out of my ass; adjust latter!!!
#define BUFFER_SIZE 240 // 240 is max packet size so I guess it's also max buffer size

char loraBuffer[BUFFER_SIZE];
int bufferIndex = 0;
char receivedPackets[MAX_PACKETS][BUFFER_SIZE];
int totalPackets = -1;

void setup() {
    Serial.begin(19200); //debug serial
    Serial1.begin(9600); //lora serial
    delay(1000); //!!!I pulled this number out of my ass; adjust latter!!!
    sendCommand("AT+IPR=9600");
    sendCommand("AT+CRFOP=3");
    sendCommand("AT+NETWORKID=6");
    sendCommand("AT+BAND=915000000");
    sendCommand("AT+PARAMETER=9,7,1,12");
    sendCommand("AT+ADDRESS=1"); //!!!Remember to adjust addresses accordingly!!!
    Serial1.setTimeout(1000); //!!!I pulled this number out of my ass; adjust latter!!!

    Serial.println("Receiver setup complete!!! :D");
}

//helper function for LoRa setup
void sendCommand(const char *command) {
    Serial1.println(command);
    delay(100); //!!!I pulled this number out of my ass; adjust latter!!!
    //clear away the response messages
    while (Serial1.available()) {
        Serial1.read();
    }
    delay(100); //!!!I pulled this number out of my ass; adjust latter!!!
}

void loop() {
    while (Serial1.available()) {
        char c = Serial1.read();
        Serial.print(c);
        if (c == '\n' || bufferIndex >= BUFFER_SIZE - 1) {
            loraBuffer[bufferIndex] = '\0';
            Serial.print("\nRaw LoRa Data: ");
            Serial.println(loraBuffer);
            processReceivedData(loraBuffer);
            bufferIndex = 0;
        } else {
            loraBuffer[bufferIndex++] = c;
        }
    }
}

void processReceivedData(char *received) {
    //debug info
    Serial.print("Processing: ");
    Serial.println(received);

    if (strncmp(received, "+RCV=", 5) != 0) {
        Serial.println("Invalid format");
        return;
    }

    char *token = strtok(received + 5, ",");  // Skip +RCV=
    if (!token) return;
    token = strtok(NULL, ","); // Skip sender address
    if (!token) return;
    token = strtok(NULL, ","); // Skip message length
    if (!token) return;

    int packetID = atoi(token);
    token = strtok(NULL, ",");
    if (!token) return;
    totalPackets = atoi(token);

    token = strtok(NULL, ""); // Get the actual data
    if (!token) return;

    // Remove signal strength info
    char *lastComma = strrchr(token, ',');
    if (lastComma) {
        *lastComma = '\0';
        char *secondLastComma = strrchr(token, ',');
        if (secondLastComma) {
            *secondLastComma = '\0';
        }
    }

    strncpy(receivedPackets[packetID], token, BUFFER_SIZE);
    receivedPackets[packetID][BUFFER_SIZE - 1] = '\0';

    //debug stuff
    Serial.print("Storing Packet ");
    Serial.print(packetID + 1);
    Serial.print(" out of ");
    Serial.print(totalPackets);
    Serial.print(": ");
    Serial.println(receivedPackets[packetID]);
    //if all packets recieved, then reconstruct the full message
    if (allPacketsReceived()) {
        Serial.println("Complete message received:");
        reconstructMessage();
    }
}

bool allPacketsReceived() {
    if (totalPackets == -1) return false;
    for (int i = 0; i < totalPackets; i++) {
        if (receivedPackets[i][0] == '\0') return false;
    }
    return true;
}

void reconstructMessage() {
    //debug stuff
    Serial.println("Reassembling message...");
    Serial.print("Final Reconstructed Message: ");
    //
    for (int i = 0; i < totalPackets; i++) {
        if (receivedPackets[i][0] == '\0') {
            Serial.print("Missing packet: ");
            Serial.println(i);
            return;
        }
        Serial.print(receivedPackets[i]);
    }
    Serial.println();
}
