#include <Arduino.h>
#include <string>
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

char message[] = MESSAGE;                       //
int currentPacket = 0;                          // the current packet being sent
int totalPackets;                               // the calculated number of packets from the message length
char receivedPackets[MAX_PACKETS][BUFFER_SIZE]; // array to store the received packets
int receivedTotalPackets = -1;                  // the total number of packets received
char loraBuffer[BUFFER_SIZE];                   // buffer to store the received data
int bufferIndex = 0;                            // index for the loraBuffer

/**
 * @brief Starts Serial1 and configures the LoRa transceiver with the necessary settings.\n
 *
 */
void setupLoRa();

/**
 * @brief sends the message in fragments to the serial (LoRa module)\n
 *
 */
void sendPackets();
/**
 * @brief sends a fragment of a message to the serial (LoRa module)
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

void receiveMsg();

/**
 * @brief Processes the received data from the LoRa module.\n
 *        It extracts the packet information, stores it, and sends an acknowledgment.\n
 *        If all packets are received, it reconstructs the complete message.
 *
 * @param received is the string received from the LoRa module in json format
 */
void processReceivedData(char *received);
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
std::string reconstructMessage();