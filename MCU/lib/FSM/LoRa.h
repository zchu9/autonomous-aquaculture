#include <Arduino.h>

#define TARGET_ADDRESS "0"
#define PACKET_SIZE 100
// random line I was using to test prior to using json
#define MESSAGE "behes rhwerhat haeth et hs rth a rh at h ateh aet h awrgawfWRH  rhathrsths thaerheh dhj dohnsoiajtiphj airjhioejtiph aiperjhiopejrih aiperjhipanh aiperjhipaerh aiprjhipjaiprnawpitn arphjpierjhn aprjhiphj"
// possibly increase
#define ACK_TIMEOUT 10000
// possibly increase
#define RETRY_LIMIT 3
// arduino memory constraint
#define MAX_PACKETS 10
// LoRa constraint
#define BUFFER_SIZE 240

char message[] = MESSAGE;
int currentPacket = 0;
int totalPackets;
char receivedPackets[MAX_PACKETS][BUFFER_SIZE];
int receivedTotalPackets = -1;
char loraBuffer[BUFFER_SIZE];
int bufferIndex = 0;

void setupLoRa()
{
    Serial1.begin(9600);
    delay(100);
    Serial1.println("AT+IPR=9600");
    delay(100);
    Serial1.println("AT+CRFOP=3");
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
    delay(5000);

    totalPackets = (strlen(message) + PACKET_SIZE - 1) / PACKET_SIZE;
    // Until I integrate with Daniel I've just been commenting or uncommenting this line to choose between transmit and reciept.
    // sendPackets();
}

void sendPackets()
{
    char fragment[PACKET_SIZE + 1];
    for (int i = 0; i < totalPackets; i++)
    {
        int startIdx = i * PACKET_SIZE;
        int length = min(PACKET_SIZE, (int)strlen(message) - startIdx);
        strncpy(fragment, message + startIdx, length);
        fragment[length] = '\0';

        int retries = 0;
        while (retries < RETRY_LIMIT)
        {
            sendFragment(i, fragment);
            if (waitForACK(i))
            {
                break;
            }
            retries++;
        }

        if (retries == RETRY_LIMIT)
        {
            Serial.print("Failed to receive ACK for packet ");
            Serial.println(i);
        }
    }
    Serial.println("Message sent");
}

void sendFragment(int packetID, const char *fragment)
{
    char packet[PACKET_SIZE + 20];
    snprintf(packet, sizeof(packet), "%d,%d,%s", packetID, totalPackets, fragment);

    char command[PACKET_SIZE + 40];
    snprintf(command, sizeof(command), "AT+SEND=%s,%d,%s", TARGET_ADDRESS, (int)strlen(packet), packet);

    Serial.print("Sending: ");
    Serial.println(packet);
    Serial1.println(command);
}

bool waitForACK(int expectedID)
{
    unsigned long startTime = millis();
    char buffer[50];
    int index = 0;
    while (millis() - startTime < ACK_TIMEOUT)
    {
        if (Serial1.available())
        {
            char c = Serial1.read();
            if (c == '\n' || index >= sizeof(buffer) - 1)
            {
                buffer[index] = '\0';
                Serial.print("Received buffer: ");
                Serial.println(buffer);

                if (strstr(buffer, "ACK:") != NULL)
                {
                    int ackID = atoi(strstr(buffer, "ACK:") + 4);
                    if (ackID == expectedID)
                    {
                        Serial.print("Received ACK for packet ");
                        Serial.println(expectedID);
                        return true;
                    }
                }
                index = 0;
            }
            else
            {
                buffer[index++] = c;
            }
        }
    }
    return false;
}

void loop()
{
    while (Serial1.available())
    {
        char c = Serial1.read();
        if (c == '\n' || bufferIndex >= BUFFER_SIZE - 1)
        {
            loraBuffer[bufferIndex] = '\0';
            Serial.println(loraBuffer);
            processReceivedData(loraBuffer);
            bufferIndex = 0;
        }
        else
        {
            loraBuffer[bufferIndex++] = c;
        }
    }
}

void processReceivedData(char *received)
{
    // Debug info
    Serial.print("Processing: ");
    Serial.println(received);

    if (strncmp(received, "+RCV=", 5) != 0)
    {
        Serial.println("Invalid format");
        return;
    }

    // Extract packet information
    char *token = strtok(received + 5, ","); // Skip +RCV=
    if (!token)
        return;
    token = strtok(NULL, ","); // Skip sender address
    if (!token)
        return;
    token = strtok(NULL, ","); // Skip message length
    if (!token)
        return;

    int packetID = atoi(token); // Get the packet ID
    token = strtok(NULL, ",");
    if (!token)
        return;
    totalPackets = atoi(token); // Get the total number of packets

    token = strtok(NULL, ""); // Get the actual data
    if (!token)
        return;

    // Remove signal strength info (after the last two commas)
    char *lastComma = strrchr(token, ',');
    if (lastComma)
    {
        *lastComma = '\0';
        char *secondLastComma = strrchr(token, ',');
        if (secondLastComma)
        {
            *secondLastComma = '\0';
        }
    }

    // Store the received packet fragment
    strncpy(receivedPackets[packetID], token, BUFFER_SIZE);
    receivedPackets[packetID][BUFFER_SIZE - 1] = '\0';

    // Debug info
    Serial.print("Storing Packet ");
    Serial.print(packetID + 1);
    Serial.print(" out of ");
    Serial.print(totalPackets);
    Serial.print(": ");
    Serial.println(receivedPackets[packetID]);

    // Send ACK for the received packet
    sendACK(packetID);

    // If all packets have been received, reconstruct the message
    if (allPacketsReceived())
    {
        Serial.println("Complete message received:");
        reconstructMessage();
    }
}

void sendACK(int packetID)
{
    // Create the ACK message for the received packet
    char ackMessage[20];
    snprintf(ackMessage, sizeof(ackMessage), "ACK:%d", packetID);
    char ackCommand[BUFFER_SIZE];
    snprintf(ackCommand, sizeof(ackCommand), "AT+SEND=%s,%d,%s", TARGET_ADDRESS, (int)strlen(ackMessage), ackMessage);

    // Send the ACK command to the sender
    Serial1.println(ackCommand);
    Serial.print("Sending ACK for packet ");
    Serial.println(packetID);
}

bool allPacketsReceived()
{
    if (totalPackets == -1)
        return false;
    for (int i = 0; i < totalPackets; i++)
    {
        if (receivedPackets[i][0] == '\0')
            return false;
    }
    return true;
}

void reconstructMessage()
{
    // Debug stuff
    Serial.println("Reassembling message...");
    Serial.print("Final Reconstructed Message: ");
    for (int i = 0; i < totalPackets; i++)
    {
        if (receivedPackets[i][0] == '\0')
        {
            Serial.print("Missing packet: ");
            Serial.println(i);
            return;
        }
        Serial.print(receivedPackets[i]);
    }
    Serial.println();
}