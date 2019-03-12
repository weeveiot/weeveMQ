/* Licensed to weeveMQ under one or more contributor license agreements.
* See the LICENCE file distributed with this work for additional information
* regarding copyright ownership. You may obtain a copy of the License at
*
*     https://github.com/weeveiot/weeveMQ/blob/master/LICENCE
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/**
 * @brief
 */


#ifndef WCL_SMP_H_
#define WCL_SMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wclTypes.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ======== SMP Specification Section 3.3 Message Type Definition ============*/
typedef enum tWclSmpMessageType {
  WCL_SMP_MESSAGE_RESERVED = 0,
  /* Keeping value same as MQTT Control Packet type defined in MQTT spec. */
  WCL_SMP_MESSAGE_MQTTS_CONNECT = 1,
  WCL_SMP_MESSAGE_MQTTS_CONNACK = 2,
  WCL_SMP_MESSAGE_MQTTS_PUBLISH = 3,
  WCL_SMP_MESSAGE_MQTTS_PUBACK = 4,
  WCL_SMP_MESSAGE_MQTTS_PUBREC = 5,
  WCL_SMP_MESSAGE_MQTTS_PUBREL = 6,
  WCL_SMP_MESSAGE_MQTTS_PUBCOMP = 7,
  WCL_SMP_MESSAGE_MQTTS_SUBSCRIBE = 8,
  WCL_SMP_MESSAGE_MQTTS_SUBACK = 9,
  WCL_SMP_MESSAGE_MQTTS_UNSUBSCRIBE = 10,
  WCL_SMP_MESSAGE_MQTTS_UNSUBACK = 11,
  WCL_SMP_MESSAGE_MQTTS_PINGREQ = 12,
  WCL_SMP_MESSAGE_MQTTS_PINGRESP = 13,
  WCL_SMP_MESSAGE_MQTTS_DISCONNECT = 14
} WclSmpMessageType_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

/**
 * @brief Open a SMP session.
 * @param[out] pSmpSession hold the session context to be used in other SMP
 * interfaces.
 */
WclError_t wclSmpOpen(WclSession_t *pSmpSession);

/**
 * @brief Initiator uses this interface to get the authenticated and private
 *        message to send it to Responder. SMP will add security parameters
          depending to standard protocol packet like MQTT control packet and
          provide back a protected SMP message.
 *
 * @param[in] smpSession session value obtained in wclSmpOpen() API.
 * @param[in] messageType the type of message.
 * @param[in] pStdProtocolPacket the standard MQTT or other protocol packet
 *            to be protected.
 * @param[out] pSmpMessage the SMP message which securely contains the
 *             pStdProtocolPacket. Caller should free this using
 *             wclFreeBuffer().
 *
 * Valid MQTTS messageType for this API for a mqtts Client:
     WCL_SMP_MESSAGE_MQTTS_CONNECT
     WCL_SMP_MESSAGE_MQTTS_SUBSCRIBE
     WCL_SMP_MESSAGE_MQTTS_UNSUBSCRIBE
     WCL_SMP_MESSAGE_MQTTS_DISCONNECT

 * Valid MQTTS messageType for this API for a mqtts Broker:
     WCL_SMP_MESSAGE_MQTTS_CONNACK
     WCL_SMP_MESSAGE_MQTTS_SUBACK
     WCL_SMP_MESSAGE_MQTTS_UNSUBACK

 * Valid MQTTS messageType for this API for both mqtts Client and Broker:
     WCL_SMP_MESSAGE_MQTTS_PUBLISH
     WCL_SMP_MESSAGE_MQTTS_PUBACK
     WCL_SMP_MESSAGE_MQTTS_PUBREC
     WCL_SMP_MESSAGE_MQTTS_PUBREL
     WCL_SMP_MESSAGE_MQTTS_PUBCOMP
 */
WclError_t wclSmpGetMessage(WclSession_t smpSession,
                            WclSmpMessageType_t messageType,
                            const WosBuffer_t *pStdProtocolPacket,
                            WosBuffer_t *pSmpMessage);

/**
 * @brief Responder uses this interface to processe the authenticated and
          private message coming from Initiator. SMP will check the integrity
          and other other security parameter of message and output a clear
          standard protocol packet like MQTT control packets contained in SMP
          message.
 *
 * @param[in] smpSession session value obtained in wclSmpOpen() API.
 * @param[in] messageType the type of message.
 * @param[out] pStdProtocolPacket the standard MQTT or other protocol packet
 *             in clear. Caller should free this using wclFreeBuffer().
 *
 * Valid MQTTS messageType for this API for a mqtts Client:
     WCL_SMP_MESSAGE_MQTTS_CONNACK
     WCL_SMP_MESSAGE_MQTTS_SUBACK
     WCL_SMP_MESSAGE_MQTTS_UNSUBACK

 * Valid MQTTS messageType for this API for a mqtts Broker:
     WCL_SMP_MESSAGE_MQTTS_CONNECT
     WCL_SMP_MESSAGE_MQTTS_SUBSCRIBE
     WCL_SMP_MESSAGE_MQTTS_UNSUBSCRIBE
     WCL_SMP_MESSAGE_MQTTS_DISCONNECT

 * Valid MQTTS messageType for this API for both mqtts Client and Broker:
     WCL_SMP_MESSAGE_MQTTS_PUBLISH
     WCL_SMP_MESSAGE_MQTTS_PUBACK
     WCL_SMP_MESSAGE_MQTTS_PUBREC
     WCL_SMP_MESSAGE_MQTTS_PUBREL
     WCL_SMP_MESSAGE_MQTTS_PUBCOMP
 */
WclError_t wclSmpProcessMessage(WclSession_t smpSession,
                                const WosBuffer_t *pSmpMessage,
                                WosBuffer_t *pStdProtocolPacket);

/**
 * @brief Close a SMP session.
 * @param[in] smpSession valid session context opened using wclSmpOpen() API.
 */
WclError_t wclSmpClose(WclSession_t smpSession);

#ifdef __cplusplus
}
#endif

#endif /* WCL_SMP_H_ */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
