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

bool waitForACK(int expectedID);

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

void processReceivedData(char *received);

void sendACK(int packetID);

bool allPacketsReceived();

void reconstructMessage();