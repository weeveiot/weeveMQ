/**
 * @file smp.c
 * @brief
 * @version 0.1
 * @date 2019-02-26
 *
 * @LICENCES
 * 
 * 1 – Preface
 * This License governs use of the accompanying Software, and your use of the Software constitutes acceptance of this license.
 * 
 * 2 – Definitions
 * The SOFTWARE is defined as all successive versions of weeveMQ and their documentation that have been developed by Eciotify GmbH, Lohmühlenstraße 65, 12435 Berlin, Germany. 
 * The DERIVED SOFTWARE is defined as all or part of the SOFTWARE that you have modified and/or translated and/or adapted and/or merged.
 * SOFTWARE and DERIVED SOFTWARE are provided under the terms of Eclipse Public License, Version 1.0 (EPL-1.0)
 * The CRYPTO SOFTWARE is defined as all or a part of the SOFTWARE or a separate module connectable to the SOFTWARE that provides cryptographic functionalities being interfaced with SOFTWARE or with a software, an application package, a hardware or a toolbox of which you are owner or entitled beneficiary.
 * 
 * 3 – Non-commercial Use 
 * You may use this SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE for any non-commercial purpose, subject to the restrictions in this license. Some purposes which can be non-commercial are teaching, academic research, and personal experimentation. You may also distribute this SOFTWARE with books or other teaching materials, or publish the SOFTWARE on websites, that are intended to teach the use of the SOFTWARE.
 * By definition non-commercial use means use of SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE with a number of services, number of connections established or established or received by SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE, said number being lower than 10.000.
 * Redistribution and use for any non-commercial purpose in source and binary forms, with or without modification, are permitted, provided that the following conditions are met:
 * 
 * You may modify this SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE and distribute the modified SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE for non-commercial purposes, however, you may not grant rights to the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE or derivative works that are broader than those provided by this License. For example, you may not distribute modifications of the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE under terms that would permit commercial use, or under terms that purport to require the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE or derivative works to be sublicensed to others.
 * You may use any information in intangible form that you remember after accessing the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE. However, this right does not grant you a license to any of Eciotify’s copyrights or patents for anything you might create using such information.
 * You may not to remove any copyright or other notices from the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE.
 * If you distribute SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE in source or object form, you shall include a verbatim copy of this license.
 * If you distribute derivative works of the SOFTWARE, and/or CRYPTO SOFTWARE in source code form you do so only under a license that includes all of the provisions of this License, and if you distribute derivative works of the Software solely in object form you do so only under a license that complies with this License.
 * If you have modified the SOFTWARE and/or the CRYPTO SOFTWARE or created derivative works, and distribute such modifications or derivative works, you will cause the modified files to carry prominent notices so that recipients know that they are not receiving the original SOFTWARE and/or the CRYPTO SOFTWARE. Such notices must state: (i) that you have changed the SOFTWARE and/or the CRYPTO SOFTWARE; and (ii) the date of any changes.
 * Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this SOFTWARE or CRYPTO SOFTWARE without specific prior written permission.
 * 
 * Redistributions have to retain the following disclaimer:
 * 
 * THE SOFTWARE AND/OR CRYPTO SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS, IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR ANY WARRANTY OF TITLE OR NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE and/or CRYPTO SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE and/or CRYPTO SOFTWARE. THIS DISCLAIMER MUST BE PASSED ON WHENEVER YOU DISTRIBUTE THE SOFTWARE, DERIVED SOFTWARE AND/OR CRYPTO SOFTWARE OR DERIVATIVE WORKS. 
 * ECIOTIFY WILL NOT BE LIABLE FOR ANY DAMAGES RELATED TO THE SOFTWARE, and/or CRYPTO SOFTWARE OR THIS LICENSE, INCLUDING DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON. ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE THE SOFTWARE, DERIVED SOFTWARE AND/OR CRYPTO SOFTWARE OR DERIVATIVE WORKS.
 * 
 * 4 – Commercial Use
 * 
 * You may not use or distribute this SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE or any derivative works in any form for commercial purposes except for the terms set forth below. In particular, you may not use or distribute modified and/or translated and/or adapted and/or merged CRYPTO SOFTWARE in any form for commercial purposes.
 * Examples of commercial purposes are, but are not limited to, running business operations, licensing, leasing, or selling the Software, or distributing the Software for use with commercial products.
 * By definition commercial use means use of SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE with a number of services, number of connections established and/or received by, with or via the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE, said number being higher than or equal to 10.000. For example, a commercial use is assumed when a server, on which SOFTWARE and the CRYPTO SOFTWARE is running, provides services with this SOFTWARE via the CRYPTO SOFTWARE to 12.000 devices connected to said server or having accounts for the SOFTWARE on that server to access said provided services by said server.
 * Any commercial use or circulation of the DERIVED SOFTWARE and/or CRYPTO SOFTWARE or any derivative works in any form must have been previously authorized and licensed by Eciotify GmbH, Lohmühlenstraße 65, 12435 Berlin, Germany.
 * 
 * 5 – General
 * If you sue anyone over patents that you think may apply to the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE or anyone's use of the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE, your license to the SOFTWARE, DERIVED SOFTWARE and/or CRYPTO SOFTWARE ends automatically.
 * Your rights under the License end automatically if you breach it in any way.
 * 
 * Eciotify reserves all rights not expressly granted to you in this license.
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
    return wclResult;
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
