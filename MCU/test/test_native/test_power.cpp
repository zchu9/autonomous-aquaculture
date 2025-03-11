#include <unity.h>
#include "victronFunctions.h"
#include "renogy.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_renchecksum() {
    // from minModbus tests
    unsigned char data1[] = { 0x02,0x07 };
    int check1 = 0x1241;
    TEST_ASSERT_TRUE(crcCheck(check1, data1, 2));

    unsigned char data2[] = { "ABCDE" };
    int check2 = 0x500F;
    TEST_ASSERT_TRUE(crcCheck(check2, data2, 5));

    // From Renogy docs
    unsigned char data3[] = {0x01,0x03, 0x00, 0x0a, 0x00, 0x01};
    int check3 = 0x08A4;
    TEST_ASSERT_TRUE(crcCheck(check3, data3, 6));

};

void test_checksum() {
    unsigned char dataGS[] = { "allmyexesliveintexasandtexasisaplaceiddearlylovetobebutallmyexesliveintexasandthatswhyihangmyhatintennesseerosannasdownintexarkanawantedmetopushherbroomsweeteileensinabilenesheforgotihungthemoonandallisonsingalvestonsomehowlosthersanityanddimpleswhonowlivesintemplesgotthelawlookingformeallmyexesliveintexasandtexasisaplaceiddearlylovetobebutallmyexesliveintexasandthatswhyihangmyhatintennesseeirememberthatoldfrioriverwhereilearnedtoswimbutitbringstomindanothertimewhereiworemywelcomethinbytranscendentalmeditationigothereeachnightbutialwayscomebacktomyselflongbeforedaylightallmyexesliveintexasandtexasisaplaceiddearlylovetobebutallmyexesliveintexasthereforeiresideintennesseesomefolksthinkimhidinitsbeenrumoredthatidiedbutimaliveandwellintennesseek" };
    unsigned char dataV[] = { 0x0d, 0x0a, 0x50, 0x49, 0x44, 0x09, 0x30, 0x78, 0x32, 0x30, 0x33, 0x0d, 0x0a, 0x56, 0x09, 0x32, 0x36, 0x32, 0x30, 0x31, 0x0d, 0x0a, 0x49, 0x09, 0x30, 0x0d, 0x0a, 0x50, 0x09, 0x30, 0x0d, 0x0a, 0x43, 0x45, 0x09, 0x30, 0x0d, 0x0a, 0x53, 0x4f, 0x43, 0x09, 0x31, 0x30, 0x30, 0x30, 0x0d, 0x0a, 0x54, 0x54, 0x47, 0x09, 0x2d, 0x31, 0x0d, 0x0a,  0x41, 0x6c, 0x61, 0x72, 0x6d, 0x09, 0x4f, 0x46, 0x46, 0x0d, 0x0a, 0x52, 0x65, 0x6c, 0x61, 0x79,  0x09, 0x4f, 0x46, 0x46, 0x0d, 0x0a, 0x41, 0x52, 0x09, 0x30, 0x0d, 0x0a, 0x42, 0x4d, 0x56, 0x09,  0x37, 0x30, 0x30, 0x0d, 0x0a, 0x46, 0x57, 0x09, 0x30, 0x33, 0x30, 0x37, 0x0d, 0x0a, 0x43, 0x68, 0x65, 0x63, 0x6b, 0x73, 0x75, 0x6d, 0x09, 0xd8 };
    size_t dataGS_size = 752;
    size_t dataV_size = 112;
    TEST_ASSERT_TRUE(victronChecksum(dataGS, dataGS_size));
    TEST_ASSERT_TRUE(victronChecksum(dataV, dataV_size));
};

void test_parse() {
    unsigned char data[] = { "\r\ndata\tfield\r\ndata2\tfield2" };
    size_t data_size = 27;
    std::map<std::string, std::string> output;
    victronParse(output, data, data_size);
    TEST_ASSERT_TRUE(output.find("data") != output.end());
    TEST_ASSERT_EQUAL_STRING("field", output["data"].c_str());
    TEST_ASSERT_TRUE(output.find("data2") != output.end());
    TEST_ASSERT_EQUAL_STRING("field2", output["data2"].c_str());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_checksum);
    RUN_TEST(test_parse);
    RUN_TEST(test_renchecksum);
    UNITY_END();
}