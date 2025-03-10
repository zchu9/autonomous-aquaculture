#include <map>
#include <string>

bool victronChecksum(unsigned char* serialData, size_t dataSize) {
    int checksum = 0;
    for (size_t i = 0; i < dataSize; i++) {
        if (serialData[i] != '\0') {
            checksum = (checksum + serialData[i]) & 255;
        }
        else {
            break;  // buffer isn't full, exit loop.
        }
    }
    return (!checksum);
};

void victronParse(std::map<std::string, std::string>& stats, unsigned char* buffer, size_t buffer_size) {
    std::string label = "";
    std::string field = "";
    for (size_t i = 0; i < buffer_size; i++) {
        label = "";
        field = "";
        if (buffer[i] == '\n') {
            i++;
            while (i < buffer_size) {
                if (buffer[i] == '\t') {
                    i++;
                    break;
                }
                label += buffer[i];
                i++;
            }

            while (i < buffer_size) {
                if (buffer[i] == '\r') {
                    break;
                }
                field += buffer[i];
                i++;
            }
            stats[label] = field;
        }
    }
}