#ifndef LORA_H
#define LORA_H
#include <Arduino.h>
#include "ArduinoJson.h"
#include <Base64.h>
#include <string>

// assume this stuff is provided by a config file
#define LORA_ADDRESS 42069       // 0 to 65535
#define LORA_PASSWORD "A3F7B9C2" // 00000001 to FFFFFFFF
#define FARM_ID "67dcad71921b525e1e669551"

#define TARGET_ADDRESS "1"
#define PACKET_SIZE 100
#define ACK_TIMEOUT 2000
#define RETRY_LIMIT 15
#define MAX_PACKETS 10
#define BUFFER_SIZE 260

/**
 * @brief Starts Serial1 and configures the LoRa transceiver with the necessary settings.\n
 *
 */
bool setupLoRa();

/**
 * @brief sends the message in fragments to the serial (LoRa module)\n
 *
 */
bool sendPackets(char *message);
/**
 * @brief Takes in a message, fragments it, and sends it all.(LoRa module)
 *
 * @param packetID is the packet number
 * @param fragment is the string fragment in the current packet
 */
void sendFragment(int packetID, const char *fragment);

/**
 * @brief Function reports when the Acknowledgment is received from the message recipient.
 *
 * @param expectedID the ID expected as an aknowledgment
 * @return true if the message is successfully sent
 * @return false if the message is not successfully sent or if the aknowledgment is not successfully received
 */
bool waitForACK(int expectedID);

/**
 * @brief This tries to recieve the message for infinity years as of right now.\n
 *        TODO: TIMER THAT DROPS THE MESSAGE
 * TODO: ERROR CHECKING??
 *
 * @param doc the json doc object to be updated.
 */
void receiveMsg(JsonDocument &doc);

/**
 * @brief Processes the received data from the LoRa module.\n
 *        It extracts the packet information, stores it, and sends an acknowledgment.\n
 *        If all packets are received, it reconstructs the complete message.
 *
 * @param received is the string received from the LoRa module in json format
 */
void processReceivedData(char *received, JsonDocument &doc);

/**
 * @brief Sends an acknowledgment for the received packet to the sender.\n
 *        The acknowledgment is sent in the format "ACK:<packetID>".
 *
 * @param packetID is the packet number being acknowledged
 */
void sendACK(int packetID);

/**
 * @brief Ensures that we received each of the message fragments from the sender.\n
 *
 * @return true if all packets have been received
 * @return false if not all packets have been received or if the total number of packets is -1
 */
bool allPacketsReceived();
/**
 * @brief reconstructs the complete message from the received packets.\n
 *
 */
void reconstructMessage(JsonDocument &doc);

/**
 * @brief performs a handshake with the server to communicate ID and maintain proof of connection
 *
 * @return true if successful
 * @return false if not successful
 */
bool sendHandshake();

#endif // LORA_H
