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
 * @file smp.c
 * @brief
 * @version 0.1
 * @date 2019-02-26
 *
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosCommon.h"
#include "wosLog.h"
#include "wosMemory.h"

#include "smp.h"
#include "wclSmp.h"

#include "smpInternal.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "SMP"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

/* Setup the initial state of SMP component */
WclError_t smpInit(void)
{
    WclError_t wclResult = WCL_ERROR;

    FUNCTION_ENTRY();

    /* Initialize global configurations and static config validation. */
    wclResult = smpInitGlobalCreds();
    if (WCL_SUCCESS != wclResult) {
        WLOGE("SMP initialization failed %x", wclResult);
    }

    FUNCTION_EXIT_RETURN(wclResult);
    return wclResult;
}

/* Cleanup and terminate SMP component */
void smpTerminate(void)
{
    WclError_t wclResult = WCL_ERROR;

    FUNCTION_ENTRY();

    /* Destroy global configurations. */
    wclResult = smpDeInitGlobalCreds();
    if (WCL_SUCCESS != wclResult) {
        WLOGE("SMP initialization failed %x", wclResult);
    }

    FUNCTION_EXIT_RETURN(wclResult);
    return;
}

/* Open a Smp session. */
WclError_t wclSmpOpen(WclSession_t *pSmpSession)
{
    WclError_t wclResult = WCL_ERROR;
    SmpSessionContext_t *pSmpCtx = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if (NULL == pSmpSession) {
        WLOGE("bad params");
        wclResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize session params, cert validation, dh-params generation etc. */
    wclResult = smpInitialiseSessionParams(&pSmpCtx);
    if (WCL_SUCCESS != wclResult) {
        WLOGE("session initialization failed %x", wclResult);
        goto exit;
    }
    *pSmpSession = (WclSession_t)(pSmpCtx);

exit:
    FUNCTION_EXIT_RETURN(wclResult);
    return wclResult;
}

/* Close Smp session. */
WclError_t wclSmpClose(WclSession_t smpSession)
{
    WclError_t wclResult = WCL_ERROR;
    SmpSessionContext_t *pSmpCtx = NULL;

    FUNCTION_ENTRY();
    WLOGI("session-id %x", smpSession);

    if (WCL_SESSION_INVALID == smpSession) {
        WLOGE("invalid session");
        wclResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    pSmpCtx = (SmpSessionContext_t *)smpSession;

    wclResult = smpDeleteSessionCredentials(pSmpCtx);

exit:
    FUNCTION_EXIT_RETURN(wclResult);
    return wclResult;
}

/* Build a SMP message. */
WclError_t wclSmpGetMessage(WclSession_t smpSession,
                            WclSmpMessageType_t messageType,
                            const WosBuffer_t *pStdProtocolPacket,
                            WosBuffer_t *pSmpMessage)
{
    WclError_t smpResult = WCL_ERROR;
    SmpSessionContext_t *pSmpCtx = NULL;

    FUNCTION_ENTRY();
    WLOGI("session-id %x, message-type %x", smpSession, messageType);

    /* Input parameters validation. */
    if (WCL_SESSION_INVALID == smpSession) {
        WLOGE("invalid session");
        smpResult = WCL_ERROR_BAD_SESSION;
        goto exit;
    }
    if (((messageType < WCL_SMP_MESSAGE_MQTTS_CONNECT) ||
         (messageType > WCL_SMP_MESSAGE_MQTTS_DISCONNECT)) ||
        (!WOS_IS_VALID_BUFFER(pStdProtocolPacket)) || (NULL == pSmpMessage)) {
        WLOGE("bad parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }
    WLOGD_BUFFER("input data", pStdProtocolPacket->data,
                 pStdProtocolPacket->length);

    pSmpCtx = (SmpSessionContext_t *)smpSession;

    /* Build the message. */
    switch (messageType) {
#if defined(SMP_MQTTS_CLIENT)
    case WCL_SMP_MESSAGE_MQTTS_CONNECT:
#elif defined(SMP_MQTTS_BROKER)
    case WCL_SMP_MESSAGE_MQTTS_CONNACK:
#endif
        smpResult = smpExportSessionEstablishmentParams(
            pSmpCtx, pStdProtocolPacket, pSmpMessage);
        break;

    case WCL_SMP_MESSAGE_MQTTS_PUBLISH:
    case WCL_SMP_MESSAGE_MQTTS_PUBACK:
    case WCL_SMP_MESSAGE_MQTTS_PUBREC:
    case WCL_SMP_MESSAGE_MQTTS_PUBREL:
    case WCL_SMP_MESSAGE_MQTTS_PUBCOMP:
    case WCL_SMP_MESSAGE_MQTTS_PINGREQ:
    case WCL_SMP_MESSAGE_MQTTS_PINGRESP:
#if defined(SMP_MQTTS_CLIENT)
    case WCL_SMP_MESSAGE_MQTTS_SUBSCRIBE:
    case WCL_SMP_MESSAGE_MQTTS_UNSUBSCRIBE:
    case WCL_SMP_MESSAGE_MQTTS_DISCONNECT:
#elif defined(SMP_MQTTS_BROKER)
    case WCL_SMP_MESSAGE_MQTTS_SUBACK:
    case WCL_SMP_MESSAGE_MQTTS_UNSUBACK:
#endif

        smpResult = smpSecureMessage(pSmpCtx, messageType, pStdProtocolPacket,
                                     pSmpMessage);
        break;

    default:
        WLOGW("message-type %x is not a valid use-case for mqtts-client",
              messageType);
        break;
    }
    if (!WOS_IS_VALID_BUFFER(pStdProtocolPacket)) {
        WLOGE("unexpected error");
        smpResult = WCL_ERROR_UNKNOWN;
        goto exit;
    }
    WLOGD_BUFFER("output data", pSmpMessage->data, pSmpMessage->length);

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* Process a SMP message. */
WclError_t wclSmpProcessMessage(WclSession_t smpSession,
                                const WosBuffer_t *pSmpMessage,
                                WosBuffer_t *pStdProtocolPacket)
{
    WclError_t smpResult = WCL_ERROR;
    SmpSessionContext_t *pSmpCtx = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if (WCL_SESSION_INVALID == smpSession) {
        WLOGE("invalid session");
        smpResult = WCL_ERROR_BAD_SESSION;
        goto exit;
    }
    if ((!WOS_IS_VALID_BUFFER(pSmpMessage)) || (NULL == pStdProtocolPacket)) {
        WLOGE("bad parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }
    WLOGI("session-id %x", smpSession);
    WLOGD_BUFFER("input data", pSmpMessage->data, pSmpMessage->length);

    pSmpCtx = (SmpSessionContext_t *)smpSession;

    /* Process the message. */
    smpResult = smpProcessMessage(pSmpCtx, pSmpMessage, pStdProtocolPacket);

    if (WCL_SUCCESS != smpResult) {
        WLOGE("operation failed %x", smpResult);
        goto exit;
    }
    if (!WOS_IS_VALID_BUFFER(pStdProtocolPacket)) {
        WLOGE("unexpected error");
        smpResult = WCL_ERROR_UNKNOWN;
        goto exit;
    }
    WLOGD_BUFFER("output data", pStdProtocolPacket->data,
                 pStdProtocolPacket->length);

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
