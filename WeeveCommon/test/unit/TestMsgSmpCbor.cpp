#include <stdio.h>
#include <string.h>

#include "gtest/gtest.h"

#include "wosLog.h"
#include "wosMemory.h"
#include "wosMsgCommon.h"
#include "wosMsgSmp.h"
#include "wosTypes.h"

#define LOG_TAG "TestUnitMsgSmp"

namespace
{

#define TEST_MESSAGE_CONTEXT_3 (0x03)
#define TEST_MESSAGE_CONTEXT_VERSION_4 (0x04)
#define TEST_MESSAGE_ID (0xAABBCCDD)
#define TEST_CLIENT_ID "ClientId000102030405060708091011"
#define TEST_CLIENT_ID_LENGTH (33)

#define TEST_CIPHER_SCHEME_ID_5 (0x05)
#define TEST_MQTT_PACKET "MQTT_PACKET"
#define TEST_ECC_DH_PUB_PARAM "ECC_DH_PUB_PARAM"
#define TEST_SIGNATURE "SIGNATURE"
#define TEST_VALIDATION_CERT "VALIDATION_CERT"
#define TEST_INTERMEDIATE_CA_CERT "INTERMEDIATE_CA_CERT"

#define TEST_IV "IVIVIVIVIV"
#define TEST_AUTHTAG "AUTHTAG_AUTHTAG"

/* Test packing/parsing of SMP header.
 *
 * Step 1- Pack using wosMsgPackSmpHeader() API.
 * Step 2- Validate that input parameters to wosMsgPackSmpHeader() API
 *         must not modified.
 * Step 3- Validate the encoded buffer matches to
 *         pre-calculated expected buffer.
 */
TEST(TestUnitMsgSmp, Trivial_SmpHeader)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    char clientId[TEST_CLIENT_ID_LENGTH] = {0};
    WosSmpHeader_t smpHeader1;

    uint8_t pack[128] = {0};
    WosBuffer_t packedBuffer = {pack, 128};

    // This is expected CBOR-packed buffer
    uint8_t expectedPackedBuffer[] = {
        0x9f, 0x03, 0x04, 0x01, 0x78, 0x20, 0x43, 0x6c, 0x69, 0x65, 0x6e,
        0x74, 0x49, 0x64, 0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30, 0x33,
        0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x37, 0x30, 0x38, 0x30,
        0x39, 0x31, 0x30, 0x31, 0x31, 0x1a, 0xaa, 0xbb, 0xcc, 0xdd, 0xff};
    size_t expectedPackedBufferLength = sizeof(expectedPackedBuffer);

    smpHeader1.commonHeader.messageContext = TEST_MESSAGE_CONTEXT_3;
    smpHeader1.commonHeader.messageContextVersion =
        TEST_MESSAGE_CONTEXT_VERSION_4;
    smpHeader1.messageType = WCL_SMP_MESSAGE_MQTTS_CONNECT;
    smpHeader1.clientId = TEST_CLIENT_ID;
    smpHeader1.messageId = TEST_MESSAGE_ID;

    ///// Step 1 - Pack
    msgStatus = wosMsgPackSmpHeader(&smpHeader1, &packedBuffer);
    ASSERT_EQ(0, msgStatus);

    ///// Step 2 - Check that input parameters are not modified
    EXPECT_EQ(TEST_MESSAGE_CONTEXT_3, smpHeader1.commonHeader.messageContext);
    EXPECT_EQ(TEST_MESSAGE_CONTEXT_VERSION_4,
              smpHeader1.commonHeader.messageContextVersion);
    EXPECT_EQ(WCL_SMP_MESSAGE_MQTTS_CONNECT, smpHeader1.messageType);
    EXPECT_EQ(
        0, memcmp(smpHeader1.clientId, TEST_CLIENT_ID, strlen(TEST_CLIENT_ID)));
    EXPECT_EQ(TEST_MESSAGE_ID, smpHeader1.messageId);

    ///// Step 3 - Check if expected CBor-packed buffer match
    ASSERT_NE((void *)0, packedBuffer.data);
    WLOGD_BUFFER("packed-cbor buffer", packedBuffer.data, packedBuffer.length);
    ASSERT_EQ(expectedPackedBufferLength, packedBuffer.length);
    EXPECT_EQ(0, memcmp(packedBuffer.data, expectedPackedBuffer,
                        expectedPackedBufferLength));
}

/* Test packing/parsing of Weeve MQTTS Session Establishment Message.
 *
 * Step 1- Pack using wosMsgPackSmpMqttsSEMessage() API.
 * Step 2- Validate ithat input parameters to wosMsgPackSmpMqttsSEMessage() API
 *         must not modified.
 * Step 3- Validate the encoded buffer matches to
 *         pre-calculated expected buffer.
 * Step 4- Parse the above encoded buffer using
 *         wosMsgUnpackSmpMqttsSEMessage() API.
 * Step 5- Match the parsed parameters to
 *         original parameters passed in Step 1.
 * Step 6- Free the output parameter of
 *         wosMsgUnpackSmpMqttsSEMessage() WosMsgMqttsSeParams_t using
 *         wosMsgFreeSmpMqttsSEMessage().
 * Step 7- Validate that members of
 *         WosMsgMqttsSeParams_t are set to NULL.
 */
TEST(TestUnitMsgSmp, Trivial_SeMessage)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    WosBuffer_t mqttPacket = {(uint8_t *)TEST_MQTT_PACKET,
                              strlen(TEST_MQTT_PACKET)};
    WosBuffer_t eccDhPubParam = {(uint8_t *)TEST_ECC_DH_PUB_PARAM,
                                 strlen(TEST_ECC_DH_PUB_PARAM)};
    WosBuffer_t signature = {(uint8_t *)TEST_SIGNATURE, strlen(TEST_SIGNATURE)};
    uint8_t numCerts = 2;
    WosBuffer_t validationCert = {(uint8_t *)TEST_VALIDATION_CERT,
                                  strlen(TEST_VALIDATION_CERT)};
    WosBuffer_t intermediateCACert = {(uint8_t *)TEST_INTERMEDIATE_CA_CERT,
                                      strlen(TEST_INTERMEDIATE_CA_CERT)};
    WosBuffer_t *pCertList[numCerts];
    uint8_t encodedSmpHeader[] = {
        0x9f, 0x03, 0x04, 0x01, 0x78, 0x20, 0x43, 0x6c, 0x69, 0x65, 0x6e,
        0x74, 0x49, 0x64, 0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30, 0x33,
        0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x37, 0x30, 0x38, 0x30,
        0x39, 0x31, 0x30, 0x31, 0x31, 0x1a, 0xaa, 0xbb, 0xcc, 0xdd, 0xff};
    size_t encodedSmpHeaderLength = sizeof(encodedSmpHeader);
    WosBuffer_t encodedSmpHeaderBuffer = {encodedSmpHeader,
                                          encodedSmpHeaderLength};
    uint8_t pack[128] = {0};
    WosBuffer_t packedBuffer = {pack, 128};

    char clientId[TEST_CLIENT_ID_LENGTH] = {0};
    WosSmpHeader_t smpHeader = {{0, 0}, (WclSmpMessageType_t)0, clientId, 0};

    WosMsgMqttsSeParams_t seParams1 = {
        &encodedSmpHeaderBuffer,
#if defined(SMP_MQTTS_CLIENT)
        TEST_CIPHER_SCHEME_ID_5,
#endif
        &eccDhPubParam,
        &mqttPacket,
        &signature,
        numCerts,
        pCertList
    };
    WosMsgMqttsSeParams_t seParams2 = {
        NULL,
#if defined(SMP_MQTTS_CLIENT)
        0,
#endif
        NULL,
        NULL,
        NULL,
        0,
        NULL
    };
    // This is expected CBOR-packed buffer
    uint8_t expectedPackedBuffer[] = {
        0x9f, 0x58, 0x2c, 0x9f, 0x03, 0x04, 0x01, 0x78, 0x20, 0x43, 0x6c, 0x69,
        0x65, 0x6e, 0x74, 0x49, 0x64, 0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30,
        0x33, 0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x37, 0x30, 0x38, 0x30,
        0x39, 0x31, 0x30, 0x31, 0x31, 0x1a, 0xaa, 0xbb, 0xcc, 0xdd, 0xff, 0x05,
        0x50, 0x45, 0x43, 0x43, 0x5f, 0x44, 0x48, 0x5f, 0x50, 0x55, 0x42, 0x5f,
        0x50, 0x41, 0x52, 0x41, 0x4d, 0x4b, 0x4d, 0x51, 0x54, 0x54, 0x5f, 0x50,
        0x41, 0x43, 0x4b, 0x45, 0x54, 0x49, 0x53, 0x49, 0x47, 0x4e, 0x41, 0x54,
        0x55, 0x52, 0x45, 0x02, 0x4f, 0x56, 0x41, 0x4c, 0x49, 0x44, 0x41, 0x54,
        0x49, 0x4f, 0x4e, 0x5f, 0x43, 0x45, 0x52, 0x54, 0x54, 0x49, 0x4e, 0x54,
        0x45, 0x52, 0x4d, 0x45, 0x44, 0x49, 0x41, 0x54, 0x45, 0x5f, 0x43, 0x41,
        0x5f, 0x43, 0x45, 0x52, 0x54, 0xff};
    size_t expectedPackedBufferLength = sizeof(expectedPackedBuffer);

    // A SE-Ack message on client side doesn't contain cipher-scheme-id, so for
    // our test data we make a test-data by removing cipher-scheme-id from above
    // expectedPackedBuffer
    uint8_t packedSeAckMessageData[] = {
        0x9f, 0x58, 0x2c, 0x9f, 0x03, 0x04, 0x01, 0x78, 0x20, 0x43, 0x6c, 0x69,
        0x65, 0x6e, 0x74, 0x49, 0x64, 0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30,
        0x33, 0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x37, 0x30, 0x38, 0x30,
        0x39, 0x31, 0x30, 0x31, 0x31, 0x1a, 0xaa, 0xbb, 0xcc, 0xdd, 0xff, 0x50,
        0x45, 0x43, 0x43, 0x5f, 0x44, 0x48, 0x5f, 0x50, 0x55, 0x42, 0x5f, 0x50,
        0x41, 0x52, 0x41, 0x4d, 0x4b, 0x4d, 0x51, 0x54, 0x54, 0x5f, 0x50, 0x41,
        0x43, 0x4b, 0x45, 0x54, 0x49, 0x53, 0x49, 0x47, 0x4e, 0x41, 0x54, 0x55,
        0x52, 0x45, 0x02, 0x4f, 0x56, 0x41, 0x4c, 0x49, 0x44, 0x41, 0x54, 0x49,
        0x4f, 0x4e, 0x5f, 0x43, 0x45, 0x52, 0x54, 0x54, 0x49, 0x4e, 0x54, 0x45,
        0x52, 0x4d, 0x45, 0x44, 0x49, 0x41, 0x54, 0x45, 0x5f, 0x43, 0x41, 0x5f,
        0x43, 0x45, 0x52, 0x54, 0xff};
    size_t packedSeAckMessageDataLength = sizeof(packedSeAckMessageData);
    WosBuffer_t packedSeAckBuffer = {packedSeAckMessageData,
                                     packedSeAckMessageDataLength};
    // Certificates
    pCertList[0] = &validationCert;
    pCertList[1] = &intermediateCACert;

    ///// Step 1 - Pack
    msgStatus = wosMsgPackSmpMqttsSEMessage(&seParams1, &packedBuffer);
    ASSERT_EQ(0, msgStatus);

    ///// Step 2 - Check that input parameters are not modified
    // Check if header values are correct
    ASSERT_NE((void *)0, seParams1.pEncodedSmpHeader);
    EXPECT_EQ(encodedSmpHeaderLength, (seParams1.pEncodedSmpHeader)->length);
    EXPECT_EQ(0, memcmp(encodedSmpHeader, (seParams1.pEncodedSmpHeader)->data,
                        encodedSmpHeaderLength));
#if defined(SMP_MQTTS_CLIENT)
    // Check if cipher-scheme-id matches
    EXPECT_EQ(TEST_CIPHER_SCHEME_ID_5, seParams1.cipherSchemeId);
#endif
    // Check if ecc-pub-param content matches
    ASSERT_NE((void *)0, seParams1.pEccDhPubParams);
    EXPECT_EQ(seParams1.pEccDhPubParams->length, strlen(TEST_ECC_DH_PUB_PARAM));
    EXPECT_EQ(0, memcmp(seParams1.pEccDhPubParams->data, TEST_ECC_DH_PUB_PARAM,
                        strlen(TEST_ECC_DH_PUB_PARAM)));
    // Check if mqtt packet content matches
    ASSERT_NE((void *)0, seParams1.pMqttPacket);
    EXPECT_EQ(seParams1.pMqttPacket->length, strlen(TEST_MQTT_PACKET));
    EXPECT_EQ(0, memcmp(seParams1.pMqttPacket->data, TEST_MQTT_PACKET,
                        strlen(TEST_MQTT_PACKET)));
    // Check if signature content matches
    ASSERT_NE((void *)0, seParams1.pSignature);
    EXPECT_EQ(seParams1.pSignature->length, strlen(TEST_SIGNATURE));
    EXPECT_EQ(0, memcmp(seParams1.pSignature->data, TEST_SIGNATURE,
                        strlen(TEST_SIGNATURE)));
    // Check if certificate chain matches
    // number of certificates
    ASSERT_EQ(2, seParams1.numCerts);
    // Check if validation certificate matches
    ASSERT_NE((void *)0, seParams1.ppCerts[0]);
    EXPECT_EQ((seParams1.ppCerts[0])->length, strlen(TEST_VALIDATION_CERT));
    EXPECT_EQ(0, memcmp((seParams1.ppCerts[0])->data, TEST_VALIDATION_CERT,
                        strlen(TEST_VALIDATION_CERT)));
    // Check if intermediate CA certificate matches
    ASSERT_NE((void *)0, seParams1.ppCerts[1]);
    EXPECT_EQ((seParams1.ppCerts[1])->length,
              strlen(TEST_INTERMEDIATE_CA_CERT));
    EXPECT_EQ(0, memcmp((seParams1.ppCerts[1])->data, TEST_INTERMEDIATE_CA_CERT,
                        strlen(TEST_INTERMEDIATE_CA_CERT)));

    ///// Step 3 - Check if expected CBOR-packed buffer match
    ASSERT_NE((void *)0, packedBuffer.data);
    WLOGD_BUFFER("packed-cbor buffer", packedBuffer.data, packedBuffer.length);
    ASSERT_EQ(expectedPackedBufferLength, packedBuffer.length);
    EXPECT_EQ(0, memcmp(packedBuffer.data, expectedPackedBuffer,
                        expectedPackedBufferLength));

    ///// Step 4 - First Extract Header from the Msg
    msgStatus = wosMsgUnpackSmpHeaderFromSmpMsg(&packedBuffer, &smpHeader);
    ASSERT_EQ(0, msgStatus);

    ///// Step 5 - Check if parsed values are correct
    EXPECT_EQ(TEST_MESSAGE_CONTEXT_3, smpHeader.commonHeader.messageContext);
    EXPECT_EQ(TEST_MESSAGE_CONTEXT_VERSION_4,
              smpHeader.commonHeader.messageContextVersion);
    EXPECT_EQ(WCL_SMP_MESSAGE_MQTTS_CONNECT, smpHeader.messageType);
    EXPECT_EQ(
        0, memcmp(smpHeader.clientId, TEST_CLIENT_ID, strlen(TEST_CLIENT_ID)));
    EXPECT_EQ(TEST_MESSAGE_ID, smpHeader.messageId);
    EXPECT_EQ(0, memcmp(packedBuffer.data, expectedPackedBuffer,
                        expectedPackedBufferLength));

    ///// Step 6 - Parse it all.
    msgStatus = wosMsgUnpackSmpMqttsSEMessage(&packedSeAckBuffer, &seParams2);
    ASSERT_EQ(0, msgStatus);

    ///// Step 7 - Check if parseed values are correct
    // Check if header values are correct
    ASSERT_NE((void *)0, seParams2.pEncodedSmpHeader);
    EXPECT_EQ(encodedSmpHeaderLength, (seParams2.pEncodedSmpHeader)->length);
    EXPECT_EQ(0, memcmp(encodedSmpHeader, (seParams2.pEncodedSmpHeader)->data,
                        encodedSmpHeaderLength));
    // Check if ecc-pub-param content matches
    ASSERT_NE((void *)0, seParams2.pEccDhPubParams);
    EXPECT_EQ(seParams2.pEccDhPubParams->length, strlen(TEST_ECC_DH_PUB_PARAM));
    EXPECT_EQ(0, memcmp(seParams2.pEccDhPubParams->data, TEST_ECC_DH_PUB_PARAM,
                        strlen(TEST_ECC_DH_PUB_PARAM)));
    // Check if mqtt packet content matches
    ASSERT_NE((void *)0, seParams2.pMqttPacket);
    EXPECT_EQ(seParams2.pMqttPacket->length, strlen(TEST_MQTT_PACKET));
    EXPECT_EQ(0, memcmp(seParams2.pMqttPacket->data, TEST_MQTT_PACKET,
                        strlen(TEST_MQTT_PACKET)));
    // Check if signature content matches
    ASSERT_NE((void *)0, seParams2.pSignature);
    EXPECT_EQ(seParams2.pSignature->length, strlen(TEST_SIGNATURE));
    EXPECT_EQ(0, memcmp(seParams2.pSignature->data, TEST_SIGNATURE,
                        strlen(TEST_SIGNATURE)));
    // Check if certificate chain matches
    // number of certificates
    ASSERT_EQ(2, seParams2.numCerts);
    // Check if validation certificate matches
    ASSERT_NE((void *)0, seParams2.ppCerts[0]);
    EXPECT_EQ((seParams2.ppCerts[0])->length, strlen(TEST_VALIDATION_CERT));
    EXPECT_EQ(0, memcmp((seParams2.ppCerts[0])->data, TEST_VALIDATION_CERT,
                        strlen(TEST_VALIDATION_CERT)));
    // Check if intermediate CA certificate matches
    ASSERT_NE((void *)0, seParams2.ppCerts[1]);
    EXPECT_EQ((seParams2.ppCerts[1])->length,
              strlen(TEST_INTERMEDIATE_CA_CERT));
    EXPECT_EQ(0, memcmp((seParams2.ppCerts[1])->data, TEST_INTERMEDIATE_CA_CERT,
                        strlen(TEST_INTERMEDIATE_CA_CERT)));

    ///// Step 8 - Free
    wosMsgFreeSmpMqttsSEMessage(&seParams2);

    ///// Step 9 - Check if atleast members are set to NULL
    ASSERT_EQ((void *)0, seParams2.pEncodedSmpHeader);
#if defined(SMP_MQTTS_CLIENT)
    ASSERT_EQ(0, seParams2.cipherSchemeId);
#endif
    ASSERT_EQ((void *)0, seParams2.pEccDhPubParams);
    ASSERT_EQ((void *)0, seParams2.pMqttPacket);
    ASSERT_EQ((void *)0, seParams2.pSignature);
    ASSERT_EQ(0, seParams2.numCerts);
    ASSERT_EQ((void *)0, seParams2.ppCerts);
}

/* Test packing/parsing of Weeve MQTTS Control Message.
 *
 * Step 1- Pack using wosMsgPackSmpMqttsControlMessage() API.
 * Step 2- Validate ithat input parameters to wosMsgPackSmpMqttsControlMessage()
 *         API must not modified.
 * Step 3- Validate the encoded buffer matches to
 *         pre-calculated expected buffer.
 * Step 4- Parse the above encoded buffer using
 *         wosMsgUnpackSmpMqttsControlMessage() API.
 * Step 5- Match the parsed parameters
 *         to original parameters passed in Step 1.
 * Step 6- Free the output parameter of
 *         wosMsgUnpackSmpMqttsControlMessage() WosMsgMqttsControlParams_t using
 *         wosMsgFreeSmpMqttsControlMessage().
 * Step 7- Validate that members of
 *         WosMsgMqttsControlParams_t are set to NULL.
 */
TEST(TestUnitMsgSmp, Trivial_ControlMessage)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    WosBuffer_t mqttPacket = {(uint8_t *)TEST_MQTT_PACKET,
                              strlen(TEST_MQTT_PACKET)};
    WosBuffer_t iv = {(uint8_t *)TEST_IV, strlen(TEST_IV)};
    WosBuffer_t authTag = {(uint8_t *)TEST_AUTHTAG, strlen(TEST_AUTHTAG)};
    uint8_t pack[128];
    WosBuffer_t packedBuffer = {pack, 128};

    char clientId[TEST_CLIENT_ID_LENGTH] = {0};
    WosSmpHeader_t smpHeader = {{0, 0}, (WclSmpMessageType_t)0, clientId, 0};

    uint8_t encodedSmpHeader[] = {
        0x9f, 0x03, 0x04, 0x01, 0x78, 0x20, 0x43, 0x6c, 0x69, 0x65, 0x6e,
        0x74, 0x49, 0x64, 0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30, 0x33,
        0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x37, 0x30, 0x38, 0x30,
        0x39, 0x31, 0x30, 0x31, 0x31, 0x1a, 0xaa, 0xbb, 0xcc, 0xdd, 0xff};
    size_t encodedSmpHeaderLength = sizeof(encodedSmpHeader);
    WosBuffer_t encodedSmpHeaderBuffer = {encodedSmpHeader,
                                          encodedSmpHeaderLength};
    // This is expected CBor-packed buffer
    uint8_t expectedPackedBuffer[] = {
        0x9f, 0x58, 0x2c, 0x9f, 0x03, 0x04, 0x01, 0x78, 0x20, 0x43, 0x6c,
        0x69, 0x65, 0x6e, 0x74, 0x49, 0x64, 0x30, 0x30, 0x30, 0x31, 0x30,
        0x32, 0x30, 0x33, 0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x37,
        0x30, 0x38, 0x30, 0x39, 0x31, 0x30, 0x31, 0x31, 0x1a, 0xaa, 0xbb,
        0xcc, 0xdd, 0xff, 0x4b, 0x4d, 0x51, 0x54, 0x54, 0x5f, 0x50, 0x41,
        0x43, 0x4b, 0x45, 0x54, 0x4a, 0x49, 0x56, 0x49, 0x56, 0x49, 0x56,
        0x49, 0x56, 0x49, 0x56, 0x4f, 0x41, 0x55, 0x54, 0x48, 0x54, 0x41,
        0x47, 0x5f, 0x41, 0x55, 0x54, 0x48, 0x54, 0x41, 0x47, 0xff};
    size_t expectedPackedBufferLength = sizeof(expectedPackedBuffer);

    WosMsgMqttsControlParams_t controlParams1 = {&encodedSmpHeaderBuffer,
                                                 &mqttPacket, &iv, &authTag};
    WosMsgMqttsControlParams_t controlParams2 = {NULL, NULL, NULL, NULL};

    ///// Step 1 - Pack
    msgStatus =
        wosMsgPackSmpMqttsControlMessage(&controlParams1, &packedBuffer);
    EXPECT_EQ(0, msgStatus);

    ///// Step 2 - Check that input parameters are not modified
    // Check if header values are correct
    ASSERT_NE((void *)0, controlParams1.pEncodedSmpHeader);
    EXPECT_EQ(encodedSmpHeaderLength,
              (controlParams1.pEncodedSmpHeader)->length);
    EXPECT_EQ(0,
              memcmp(encodedSmpHeader, (controlParams1.pEncodedSmpHeader)->data,
                     encodedSmpHeaderLength));
    // Check if mqtt packet content matches
    ASSERT_NE((void *)0, controlParams1.pMqttPacket);
    EXPECT_EQ(controlParams1.pMqttPacket->length, strlen(TEST_MQTT_PACKET));
    EXPECT_EQ(0, memcmp(controlParams1.pMqttPacket->data, TEST_MQTT_PACKET,
                        strlen(TEST_MQTT_PACKET)));
    // Check if iv content matches
    ASSERT_NE((void *)0, controlParams1.pIV);
    EXPECT_EQ(controlParams1.pIV->length, strlen(TEST_IV));
    EXPECT_EQ(0, memcmp(controlParams1.pIV->data, TEST_IV, strlen(TEST_IV)));
    // Check if auth-tag content matches
    ASSERT_NE((void *)0, controlParams1.pAuthTag);
    EXPECT_EQ(controlParams1.pAuthTag->length, strlen(TEST_AUTHTAG));
    EXPECT_EQ(0, memcmp(controlParams1.pAuthTag->data, TEST_AUTHTAG,
                        strlen(TEST_AUTHTAG)));

    ///// Step 3 - Check if expected CBor-packed buffer match
    ASSERT_NE((void *)0, packedBuffer.data);
    WLOGD_BUFFER("packed-cbor buffer", packedBuffer.data, packedBuffer.length);
    ASSERT_EQ(expectedPackedBufferLength, packedBuffer.length);
    EXPECT_EQ(0, memcmp(packedBuffer.data, expectedPackedBuffer,
                        expectedPackedBufferLength));

    ///// Step 4 - First Extract Header from the Msg
    msgStatus = wosMsgUnpackSmpHeaderFromSmpMsg(&packedBuffer, &smpHeader);
    ASSERT_EQ(0, msgStatus);

    ///// Step 5 - Check if parsed values are correct
    EXPECT_EQ(TEST_MESSAGE_CONTEXT_3, smpHeader.commonHeader.messageContext);
    EXPECT_EQ(TEST_MESSAGE_CONTEXT_VERSION_4,
              smpHeader.commonHeader.messageContextVersion);
    EXPECT_EQ(WCL_SMP_MESSAGE_MQTTS_CONNECT, smpHeader.messageType);
    EXPECT_EQ(
        0, memcmp(smpHeader.clientId, TEST_CLIENT_ID, strlen(TEST_CLIENT_ID)));
    EXPECT_EQ(TEST_MESSAGE_ID, smpHeader.messageId);
    EXPECT_EQ(0, memcmp(packedBuffer.data, expectedPackedBuffer,
                        expectedPackedBufferLength));

    ///// Step 6 - Parse it all.
    msgStatus =
        wosMsgUnpackSmpMqttsControlMessage(&packedBuffer, &controlParams2);
    EXPECT_EQ(0, msgStatus);

    ///// Step 7 - Check if parseed values are correct
    // Check if header values are correct
    ASSERT_NE((void *)0, controlParams2.pEncodedSmpHeader);
    EXPECT_EQ(encodedSmpHeaderLength,
              (controlParams2.pEncodedSmpHeader)->length);
    EXPECT_EQ(0,
              memcmp(encodedSmpHeader, (controlParams2.pEncodedSmpHeader)->data,
                     encodedSmpHeaderLength));
    // Check if mqtt packet content matches
    ASSERT_NE((void *)0, controlParams2.pMqttPacket);
    EXPECT_EQ(controlParams2.pMqttPacket->length, strlen(TEST_MQTT_PACKET));
    EXPECT_EQ(0, memcmp(controlParams2.pMqttPacket->data, TEST_MQTT_PACKET,
                        strlen(TEST_MQTT_PACKET)));
    // Check if iv content matches
    ASSERT_NE((void *)0, controlParams2.pIV);
    EXPECT_EQ(controlParams2.pIV->length, strlen(TEST_IV));
    EXPECT_EQ(0, memcmp(controlParams2.pIV->data, TEST_IV, strlen(TEST_IV)));
    // Check if auth-tag content matches
    ASSERT_NE((void *)0, controlParams2.pAuthTag);
    EXPECT_EQ(controlParams2.pAuthTag->length, strlen(TEST_AUTHTAG));
    EXPECT_EQ(0, memcmp(controlParams2.pAuthTag->data, TEST_AUTHTAG,
                        strlen(TEST_AUTHTAG)));

    ///// Step 8 - Free
    wosMsgFreeSmpMqttsControlMessage(&controlParams2);

    ///// Step 9 - Check if atleast members are set to NULL
    ASSERT_EQ((void *)0, controlParams2.pEncodedSmpHeader);
    ASSERT_EQ((void *)0, controlParams2.pMqttPacket);
    ASSERT_EQ((void *)0, controlParams2.pIV);
    ASSERT_EQ((void *)0, controlParams2.pAuthTag);
}

} // namespace
