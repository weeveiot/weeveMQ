#include "gtest/gtest.h"

#include "wclCommon.h"
#include "wclSmp.h"
#include "wosMemory.h"

#include "TestAssetsSmp.h"

#define MQTT_MESSAGE "MQTT_MESSAGE_HELLO"

namespace
{

class TestSmp : public testing::Test
{
  protected:
    virtual void SetUp()
    {
        WclError_t smpResult = WCL_ERROR;
        smpResult = wclInit();
        ASSERT_EQ(WCL_SUCCESS, smpResult);
    }

    virtual void TearDown() { wclTerminate(); }
};

/* Test opening/closing of a SMP session.
 *
 * Step 1- Open a smp session.
 * Step 2- Close the session.
 * */
TEST_F(TestSmp, Trivial_OpenClose)
{
    WclError_t smpResult = WCL_ERROR;
    WclSession_t smpSession = WCL_SESSION_INVALID;

    smpResult = wclSmpOpen(&smpSession);
    ASSERT_EQ(WCL_SUCCESS, smpResult);
    EXPECT_NE(WCL_SESSION_INVALID, smpSession);

    smpResult = wclSmpClose(smpSession);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
}

/* Test creation of SMP MQTTS CONNECT message.
 *
 * CLIENT end only.
 *
 * Step 1- Open a smp session.
 * Step 2- Get a CONNECT message.
 * Step 3- Close the session.
 * */
TEST_F(TestSmp, Trivial_GetMessageConnect)
{
    WclError_t smpResult = WCL_ERROR;
    WclSession_t smpSession = WCL_SESSION_INVALID;
    uint32_t mqttPacketLength = strlen(MQTT_MESSAGE);
    WosBuffer_t mqttPacket = {.data = (uint8_t *)MQTT_MESSAGE,
                              .length = mqttPacketLength};
    WosBuffer_t smpMessage = {.data = NULL, .length = 0};
    WclSmpMessageType_t messageType = WCL_SMP_MESSAGE_MQTTS_CONNECT;

    smpResult = wclSmpOpen(&smpSession);
    ASSERT_EQ(WCL_SUCCESS, smpResult);
    ASSERT_NE(WCL_SESSION_INVALID, smpSession);

    smpResult =
        wclSmpGetMessage(smpSession, messageType, &mqttPacket, &smpMessage);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
    if (smpMessage.data != NULL) {
        wosMemFree(smpMessage.data);
        smpMessage.data = NULL;
        smpMessage.length = 0;
    }

    smpResult = wclSmpClose(smpSession);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
}

/* Test creation of SMP MQTTS CONNECT_ACK message.
 *
 * BROKER end only.
 *
 * Step 1- Open a smp session.
 * Step 2- Get a CONNECT_ACK message.
 * Step 3- Close the session.
 * */
TEST_F(TestSmp, Trivial_GetMessageConnectAck)
{
    WclError_t smpResult = WCL_ERROR;
    WclSession_t smpSession = WCL_SESSION_INVALID;
    uint32_t mqttPacketLength = strlen(MQTT_MESSAGE);
    WosBuffer_t mqttPacket = {.data = (uint8_t *)MQTT_MESSAGE,
                              .length = mqttPacketLength};
    WosBuffer_t smpMessage = {.data = NULL, .length = 0};
    WclSmpMessageType_t messageType = WCL_SMP_MESSAGE_MQTTS_CONNACK;

    smpResult = wclSmpOpen(&smpSession);
    ASSERT_EQ(WCL_SUCCESS, smpResult);
    ASSERT_NE(WCL_SESSION_INVALID, smpSession);

    smpResult =
        wclSmpGetMessage(smpSession, messageType, &mqttPacket, &smpMessage);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
    if (smpMessage.data != NULL) {
        wosMemFree(smpMessage.data);
        smpMessage.data = NULL;
        smpMessage.length = 0;
    }

    smpResult = wclSmpClose(smpSession);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
}

/* Test processing of SMP MQTTS CONNECT message.
 *
 * BROKER end only.
 *
 * Step 1- Open a smp session.
 * Step 2- Process a CONNECT message.
 * Step 3- Close the session.
 * */
TEST_F(TestSmp, Trivial_ProcessMessageConnect)
{
    WclError_t smpResult = WCL_ERROR;
    WclSession_t smpSession = WCL_SESSION_INVALID;
    WosBuffer_t mqttPacket = {.data = NULL, .length = 0};
    WosBuffer_t smpMessage; // TODO Pre-calculated test asset - CONNECT message

    smpResult = wclSmpOpen(&smpSession);
    ASSERT_EQ(WCL_SUCCESS, smpResult);
    ASSERT_NE(WCL_SESSION_INVALID, smpSession);

    smpResult = wclSmpProcessMessage(smpSession, &smpMessage, &mqttPacket);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
    if (mqttPacket.data != NULL) {
        wosMemFree(mqttPacket.data);
        mqttPacket.data = NULL;
        mqttPacket.length = 0;
    }

    smpResult = wclSmpClose(smpSession);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
}

/* Test processing of SMP MQTTS CONNECT_ACK message.
 *
 * CLIENT end only.
 *
 * Step 1- Open a smp session.
 * Step 2- Process a CONNECT_ACK message.
 * Step 3- Close the session.
 * */
TEST_F(TestSmp, Trivial_ProcessMessageConnectAck)
{
    WclError_t smpResult = WCL_ERROR;
    WclSession_t smpSession = WCL_SESSION_INVALID;
    WosBuffer_t mqttPacket = {.data = NULL, .length = 0};
    WosBuffer_t smpMessage; // TODO Pre-calculated test asset - CONNACK message

    smpResult = wclSmpOpen(&smpSession);
    ASSERT_EQ(WCL_SUCCESS, smpResult);
    ASSERT_NE(WCL_SESSION_INVALID, smpSession);

    smpResult = wclSmpProcessMessage(smpSession, &smpMessage, &mqttPacket);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
    if (mqttPacket.data != NULL) {
        wosMemFree(mqttPacket.data);
        mqttPacket.data = NULL;
        mqttPacket.length = 0;
    }

    smpResult = wclSmpClose(smpSession);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
}

/* Test creation of different SMP MQTTS messages.
 *
 * Client end only.
 *
 * Step 1- Open a smp session.
 * Step 2- Get a CONNECT message.
 * Step 3- Process a CONNACK message.
 * Step 4- Get a SUBSCRIBE message.
 * Step 5- Close the session.
 * */
TEST_F(TestSmp, Trivial_GetMessagePublish)
{
    WclError_t smpResult = WCL_ERROR;
    WclSession_t smpSession = WCL_SESSION_INVALID;
    uint32_t mqttPacketLength = strlen(MQTT_MESSAGE);
    WosBuffer_t connectMqttPacket = {.data = (uint8_t *)MQTT_MESSAGE,
                                     .length = mqttPacketLength};
    WosBuffer_t publishMqttPacket = {.data = (uint8_t *)MQTT_MESSAGE,
                                     .length = mqttPacketLength};
    WosBuffer_t smpMessage = {.data = NULL, .length = 0};
    WosBuffer_t connAckMqttPacket = {.data = NULL, .length = 0};
    WosBuffer_t connAckSmpMessage = {.data = (uint8_t *)SMP_CONNACK_MESSAGE_1,
                                     .length = length_SMP_CONNACK_MESSAGE_1};
    WclSmpMessageType_t messageType = WCL_SMP_MESSAGE_MQTTS_CONNECT;

    smpResult = wclSmpOpen(&smpSession);
    ASSERT_EQ(WCL_SUCCESS, smpResult);
    ASSERT_NE(WCL_SESSION_INVALID, smpSession);

    smpResult = wclSmpGetMessage(smpSession, messageType, &connectMqttPacket,
                                 &smpMessage);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
    if (smpMessage.data != NULL) {
        wosMemFree(smpMessage.data);
        smpMessage.data = NULL;
        smpMessage.length = 0;
    }

    smpResult = wclSmpProcessMessage(smpSession, &connAckSmpMessage,
                                     &connAckMqttPacket);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
    if (connAckMqttPacket.data != NULL) {
        wosMemFree(connAckMqttPacket.data);
        connAckMqttPacket.data = NULL;
        connAckMqttPacket.length = 0;
    }

    messageType = WCL_SMP_MESSAGE_MQTTS_PUBLISH;
    smpResult = wclSmpGetMessage(smpSession, messageType, &publishMqttPacket,
                                 &smpMessage);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
    if (smpMessage.data != NULL) {
        wosMemFree(smpMessage.data);
        smpMessage.data = NULL;
        smpMessage.length = 0;
    }

    smpResult = wclSmpClose(smpSession);
    EXPECT_EQ(WCL_SUCCESS, smpResult);
}

} // namespace
