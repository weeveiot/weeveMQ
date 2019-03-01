/**
 * @brief Use this interfaces to build and parse Weeve
 * Network Messages of "Communication Context" in a CBOR format.
 *
 * @file wosMsgCommMessage.c
 * @date 2018-07-14
 * 
 * LICENCES
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

#include <cbor.h>

#include "wosCommon.h"
#include "wosLog.h"
#include "wosMemory.h"
#include "wosString.h"

#include "msgCborUtils.h"

#include "wosMsgCommon.h"
#include "wosMsgSmp.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "MSG_SMP"

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

/*
 * Pack the header data of a SMP message.
 */
WosMsgError_t wosMsgPackSmpHeader(const WosSmpHeader_t *pMessageHeader,
                                  WosBuffer_t *pPackedBuffer)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborEncoder encoder;
    CborEncoder dataArray;
    size_t clientIdLength = 0;

    FUNCTION_ENTRY();

    /* Input parameter validation. */
    if ((NULL == pMessageHeader) || (NULL == pMessageHeader->clientId) ||
        (NULL == pPackedBuffer)) {
        WLOGE("bad parameters");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }
    clientIdLength = wosStringLength(pMessageHeader->clientId);
    if (clientIdLength > WCL_SMP_CLIENT_ID_LENGTH) {
        WLOGE("bad client-id");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the CBOR encoder. */
    cbor_encoder_init(&encoder, pPackedBuffer->data, pPackedBuffer->length, 0);

    /* Create the top level array container. */
    cborStatus =
        cbor_encoder_create_array(&encoder, &dataArray, CborIndefiniteLength);
    if (CborNoError != cborStatus) {
        WLOGE("create root array failed %x", cborStatus);
        goto exit;
    }

    /* Add message context. */
    cborStatus = cbor_encode_uint(&dataArray,
                                  pMessageHeader->commonHeader.messageContext);
    if (CborNoError != cborStatus) {
        WLOGE("encode message context failed %x", cborStatus);
        goto exit;
    }

    /* Add version. */
    cborStatus = cbor_encode_uint(
        &dataArray, pMessageHeader->commonHeader.messageContextVersion);
    if (CborNoError != cborStatus) {
        WLOGE("encode message context version failed %x", cborStatus);
        goto exit;
    }

    /* Add message type. */
    cborStatus = cbor_encode_uint(&dataArray, pMessageHeader->messageType);
    if (CborNoError != cborStatus) {
        WLOGE("encode message type failed %x", cborStatus);
        goto exit;
    }

    /* Add Client-Id. */
    cborStatus = cbor_encode_text_stringz(&dataArray, pMessageHeader->clientId);
    if (CborNoError != cborStatus) {
        WLOGE("encode client-id failed %x", cborStatus);
        goto exit;
    }

    /* Add message-id. */
    cborStatus = cbor_encode_uint(&dataArray, pMessageHeader->messageId);
    if (CborNoError != cborStatus) {
        WLOGE("encode message-id failed %x", cborStatus);
        goto exit;
    }

    /* Close the top level array container. */
    cborStatus = cbor_encoder_close_container_checked(&encoder, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("close root array failed %x", cborStatus);
        goto exit;
    }

    /* Get the actual encoded buffer size. */
    pPackedBuffer->length =
        cbor_encoder_get_buffer_size(&encoder, pPackedBuffer->data);

    WLOGD("total encoded length %d", pPackedBuffer->length);

    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

/*
 * Parse the header data of a serialized SMP header.
 */
WosMsgError_t wosMsgUnpackSmpHeaderFromSmpMsg(const WosBuffer_t *pPackedBuffer,
                                              WosSmpHeader_t *pMessageHeader)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborParser parser;
    CborValue value;
    CborValue dataArray;
    uint64_t messageContext = WOS_MSG_CONTEXT_UNDEFINED;
    uint64_t messageType = WOS_MSG_MESSAGE_TYPE_UNDEFINED;
    uint64_t messageContextVersion = WOS_MSG_MESSAGE_TYPE_VERSION_UNDEFINED;
    uint64_t messageId = 0;
    size_t clientIdLength = WCL_SMP_CLIENT_ID_LENGTH + 1;
    WosBuffer_t *pExtractedSmpCborHeader = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((!WOS_IS_VALID_BUFFER(pPackedBuffer)) || (NULL == pMessageHeader)) {
        WLOGE("bad parameter");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the output. */
    pMessageHeader->commonHeader.messageContext = (uint8_t)messageContext;
    pMessageHeader->commonHeader.messageContextVersion =
        (uint8_t)messageContextVersion;
    pMessageHeader->messageType = (uint8_t)messageType;
    pMessageHeader->messageId = (uint32_t)messageId;

    /* First get the SMP header cbor byte-buffer from SMP message. */
    /* Initialize the parser. */
    cborStatus = cbor_parser_init(pPackedBuffer->data, pPackedBuffer->length, 0,
                                  &parser, &value);
    if (CborNoError != cborStatus) {
        WLOGE("parser initialization failed %x", cborStatus);
        goto exit;
    }
    /* Check if top level container is an array. */
    if (false == cbor_value_is_array(&value)) {
        WLOGE("Badly formatted packet");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }
    /* Enter into root array. */
    cborStatus = cbor_value_enter_container(&value, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("entering root array failed %x", cborStatus);
        goto exit;
    }
    /* Get the CBOR-encoded-smp-header. */
    msgStatus =
        msgCborParseByteString(&dataArray, &pExtractedSmpCborHeader, NULL);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting smp-header failed %x", msgStatus);
        goto exit;
    }
    WLOGD_BUFFER("smp-header-cbor-buffer", pExtractedSmpCborHeader->data,
                 pExtractedSmpCborHeader->length);

    /* Now extract the SMP header. */
    /* Initialize the parser. */
    cborStatus =
        cbor_parser_init(pExtractedSmpCborHeader->data,
                         pExtractedSmpCborHeader->length, 0, &parser, &value);
    if (CborNoError != cborStatus) {
        WLOGE("parser initialization failed %x", cborStatus);
        goto exit;
    }

    /* Check if top level container is an array. */
    if (false == cbor_value_is_array(&value)) {
        WLOGE("Badly formatted packet");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }

    /* Enter into root array. */
    cborStatus = cbor_value_enter_container(&value, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("entering root array failed %x", cborStatus);
        goto exit;
    }

    /* Extract the message context. */
    msgStatus = msgCborParseUint64(&dataArray, &messageContext);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting message context failed %x", msgStatus);
        goto exit;
    }

    /* Extract message context version. */
    cborStatus = cbor_value_advance_fixed(&dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("advancing to message context version failed %x", cborStatus);
        goto exit;
    }
    msgStatus = msgCborParseUint64(&dataArray, &messageContextVersion);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting message context version failed %x", msgStatus);
        goto exit;
    }

    /* Extract message type. */
    cborStatus = cbor_value_advance_fixed(&dataArray);
    if (CborNoError != cborStatus) {
        WLOGW("advancing to message type failed %x", cborStatus);
        goto exit;
    }
    msgStatus = msgCborParseUint64(&dataArray, &messageType);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting message type version failed %x", msgStatus);
        goto exit;
    }

    /* Get the client-id. */
    cborStatus = cbor_value_advance(&dataArray);
    if (CborNoError != cborStatus) {
        WLOGW("advancing to next item failed %x", cborStatus);
        goto exit;
    }
    /* Check the type. */
    if (false == cbor_value_is_text_string(&dataArray)) {
        WLOGE("badly formatted header, expecting text string");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }
    /* Copy byte string to buffer. */
    cborStatus = cbor_value_copy_text_string(
        &dataArray, pMessageHeader->clientId, &clientIdLength, NULL);
    if (CborNoError != cborStatus) {
        WLOGE("copy client-id string failed %x %s", cborStatus,
              cbor_error_string(cborStatus));
        goto exit;
    }
    /* We expect a null-terminated client-id with permissible length. */
    if ((clientIdLength > WCL_SMP_CLIENT_ID_LENGTH) ||
        (WOS_STRING_NULL_TERM != pMessageHeader->clientId[clientIdLength])) {
        WLOGE("bad client-id");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }
    WLOGD("extracted client-id = %s", pMessageHeader->clientId);

    /* Extract message-id. */
    cborStatus = cbor_value_advance(&dataArray);
    if (CborNoError != cborStatus) {
        WLOGW("advancing to message-id failed %x", cborStatus);
        goto exit;
    }
    msgStatus = msgCborParseUint64(&dataArray, &messageId);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting message-id failed %x", msgStatus);
        goto exit;
    }

    /* Update the output. */
    pMessageHeader->commonHeader.messageContext = (uint8_t)messageContext;
    pMessageHeader->commonHeader.messageContextVersion =
        (uint8_t)messageContextVersion;
    pMessageHeader->messageType = (uint8_t)messageType;
    pMessageHeader->messageId = (uint32_t)messageId;

    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    WOS_FREE_BUF_AND_DATA(pExtractedSmpCborHeader);

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

/**
 * Pack the Session Establishment Message.
 */
WosMsgError_t
wosMsgPackSmpMqttsSEMessage(const WosMsgMqttsSeParams_t *pSeParams,
                            WosBuffer_t *pPackedBuffer)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborEncoder encoder;
    CborEncoder dataArray;
    uint8_t i = 0;
    WosBuffer_t *pCertBuffer = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSeParams) ||
        (!WOS_IS_VALID_BUFFER(pSeParams->pEncodedSmpHeader)) ||
        (!WOS_IS_VALID_BUFFER(pSeParams->pEccDhPubParams)) ||
        (!WOS_IS_VALID_BUFFER(pSeParams->pMqttPacket)) ||
        (!WOS_IS_VALID_BUFFER(pSeParams->pSignature)) ||
        (pSeParams->numCerts < 1) || (NULL == pSeParams->ppCerts) ||
        (!WOS_IS_VALID_BUFFER(pPackedBuffer))) {
        WLOGE("bad parameter");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the CBOR encoder. */
    cbor_encoder_init(&encoder, pPackedBuffer->data, pPackedBuffer->length, 0);

    /* Create the top level array container. */
    cborStatus =
        cbor_encoder_create_array(&encoder, &dataArray, CborIndefiniteLength);
    if (CborNoError != cborStatus) {
        WLOGE("create root array failed %x", cborStatus);
        goto exit;
    }

    /* Add CBOR-encoded SMP header. */
    cborStatus =
        cbor_encode_byte_string(&dataArray, pSeParams->pEncodedSmpHeader->data,
                                pSeParams->pEncodedSmpHeader->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode smp-header failed %x", cborStatus);
        goto exit;
    }

#if defined(SMP_MQTTS_CLIENT)
    /* encode cipher scheme id. */
    cborStatus = cbor_encode_uint(&dataArray, pSeParams->cipherSchemeId);
    if (CborNoError != cborStatus) {
        WLOGE("encode cipher-scheme-id failed %x", cborStatus);
        goto exit;
    }
#endif
    /* Add ECC-DH public param. */
    cborStatus =
        cbor_encode_byte_string(&dataArray, pSeParams->pEccDhPubParams->data,
                                pSeParams->pEccDhPubParams->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode dh-x failed %x", cborStatus);
        goto exit;
    }

    /* Add standard MQTT packet. */
    cborStatus =
        cbor_encode_byte_string(&dataArray, pSeParams->pMqttPacket->data,
                                pSeParams->pMqttPacket->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode mqtt-packet failed %x", cborStatus);
        goto exit;
    }

    /* Add signature. */
    cborStatus = cbor_encode_byte_string(
        &dataArray, pSeParams->pSignature->data, pSeParams->pSignature->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode signature failed %x", cborStatus);
        goto exit;
    }

    /* encode number of certificates. */
    cborStatus = cbor_encode_uint(&dataArray, pSeParams->numCerts);
    if (CborNoError != cborStatus) {
        WLOGE("encode number of certificates failed %x", cborStatus);
        goto exit;
    }

    /* Add certificate chain. */
    for (i = 0; i < pSeParams->numCerts; ++i) {
        pCertBuffer = pSeParams->ppCerts[i];
        if (!WOS_IS_VALID_BUFFER(pCertBuffer)) {
            WLOGE("bad certificate chain");
            msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
            goto exit;
        }
        cborStatus = cbor_encode_byte_string(&dataArray, pCertBuffer->data,
                                             pCertBuffer->length);
        if (CborNoError != cborStatus) {
            WLOGE("encode certificate-%d failed %x", i + 1, cborStatus);
            goto exit;
        }
    }

    /* Close the top level array container. */
    cborStatus = cbor_encoder_close_container_checked(&encoder, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("close root array failed %x", cborStatus);
        goto exit;
    }

    /* Get the actual encoded buffer size. */
    pPackedBuffer->length =
        cbor_encoder_get_buffer_size(&encoder, pPackedBuffer->data);

    WLOGD("total encoded length %d", pPackedBuffer->length);

    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

/**
 * Unpack the binary buffer to get Session Establishment Message.
 */
WosMsgError_t wosMsgUnpackSmpMqttsSEMessage(const WosBuffer_t *pPackedBuffer,
                                            WosMsgMqttsSeParams_t *pSeParams)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborParser parser;
    CborValue value;
    CborValue value1;
    CborValue value2;
    CborValue dataArray;
    size_t length = 0;
    uint64_t version = 0;
    uint64_t numCerts = 0;
#if defined(SMP_MQTTS_BROKER)
    uint64_t cipherSchemeId = 0;
#endif
    uint8_t i = 0;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((!WOS_IS_VALID_BUFFER(pPackedBuffer)) || (NULL == pSeParams)) {
        WLOGE("bad parameter");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Output initialization. */
    /* This is to prevent to an undefined number of loops in free routine in
     * case an error occurs. */
    pSeParams->numCerts = 0;

    /* Initialize the parser. */
    cborStatus = cbor_parser_init(pPackedBuffer->data, pPackedBuffer->length, 0,
                                  &parser, &value);
    if (CborNoError != cborStatus) {
        WLOGE("parser initialization failed %x", cborStatus);
        goto exit;
    }

    /* Check if top level container is an array. */
    if (false == cbor_value_is_array(&value)) {
        WLOGE("Badly formatted packet");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }

    /* Enter into root array. */
    cborStatus = cbor_value_enter_container(&value, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("entering root array failed %x", cborStatus);
        goto exit;
    }

    /* Get the CBOR-encoded-smp-header. */
    msgStatus = msgCborParseByteString(
        &dataArray, &(pSeParams->pEncodedSmpHeader), &value1);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting smp-header failed %x", msgStatus);
        goto exit;
    }

#if defined(SMP_MQTTS_BROKER)
    /* Extract the cipher-scheme-id. */
    msgStatus = msgCborParseUint64(&value1, &cipherSchemeId);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting number of certificates failed %x", msgStatus);
        goto exit;
    }
    pSeParams->cipherSchemeId = (uint8_t)cipherSchemeId;
    /* Advance to next item. */
    cborStatus = cbor_value_advance(&value1);
    if (CborNoError != cborStatus) {
        WLOGW("advancing to next item mecdh-publik-key failed %x", cborStatus);
        goto exit;
    }
#endif

    /* Get the ECC-DH-public-params, value2 is iterated to the next element. */
    msgStatus =
        msgCborParseByteString(&value1, &(pSeParams->pEccDhPubParams), &value2);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting ECC-DH-X failed %x", msgStatus);
        goto exit;
    }

    /* Get the standard mqtt packet, value1 is iterated to the next element. */
    msgStatus =
        msgCborParseByteString(&value2, &(pSeParams->pMqttPacket), &value1);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting mqtt packet failed %x", msgStatus);
        goto exit;
    }

    /* Get the signature, value2 is iterated to the next element. */
    msgStatus =
        msgCborParseByteString(&value1, &(pSeParams->pSignature), &value2);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting signature failed %x", msgStatus);
        goto exit;
    }

    /* Extract the number of certificates. */
    msgStatus = msgCborParseUint64(&value2, &numCerts);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting number of certificates failed %x", msgStatus);
        goto exit;
    }
    pSeParams->numCerts = (uint8_t)numCerts;

    WLOGD("number of certificates %x", numCerts);
    /* Allocate the certificate list. */
    pSeParams->ppCerts =
        wosMemAlloc((pSeParams->numCerts) * (sizeof(WosBuffer_t *)));
    if (NULL == pSeParams->ppCerts) {
        WLOGF("out of memory cert-list allocation");
        msgStatus = WOS_MSG_ERROR_OUT_OF_MEMORY;
        goto exit;
    }

    /* Extract the certificate chain. */
    /* Advance to next item. */
    cborStatus = cbor_value_advance(&value2);
    if (CborNoError != cborStatus) {
        WLOGW("advancing to next item failed %x", cborStatus);
        goto exit;
    }
    /* Extract the certificates. */
    for (i = 0; i < numCerts; ++i) {
        msgStatus =
            msgCborParseByteString(&value2, &(pSeParams->ppCerts[i]), NULL);
        if (WOS_MSG_SUCCESS != msgStatus) {
            WLOGE("extracting certificate %d failed %x", i + 1, msgStatus);
            goto exit;
        }
        cborStatus = cbor_value_advance(&value2);
        if (CborNoError != cborStatus) {
            WLOGE("advancing to certificate %d failed %x", i + 2, cborStatus);
            goto exit;
        }
    }

    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    /* If there is failure during parsing, release memory. */
    if (WOS_MSG_SUCCESS != msgStatus) {
        wosMsgFreeSmpMqttsSEMessage(pSeParams);
    }

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

/**
 * Free the WosMsgMqttsSeParams_t structure.
 */
void wosMsgFreeSmpMqttsSEMessage(WosMsgMqttsSeParams_t *pSeParams)
{
    uint8_t i = 0;
    FUNCTION_ENTRY();

    /* Release memory. */
    if (NULL != pSeParams) {
        WOS_FREE_BUF_AND_DATA(pSeParams->pEncodedSmpHeader);
        WOS_FREE_BUF_AND_DATA(pSeParams->pMqttPacket);
        WOS_FREE_BUF_AND_DATA(pSeParams->pEccDhPubParams);
        WOS_FREE_BUF_AND_DATA(pSeParams->pSignature);
        if (NULL != pSeParams->ppCerts) {
            for (i = 0; i < pSeParams->numCerts; ++i) {
                WOS_FREE_BUF_AND_DATA(pSeParams->ppCerts[i]);
            }
            wosMemFree(pSeParams->ppCerts);
            pSeParams->ppCerts = NULL;
            pSeParams->numCerts = 0;
        }
    }

    FUNCTION_EXIT();
}

/**
 * Pack the MQtts Control Message.
 */
WosMsgError_t wosMsgPackSmpMqttsControlMessage(
    const WosMsgMqttsControlParams_t *pControlParams,
    WosBuffer_t *pPackedBuffer)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborEncoder encoder;
    CborEncoder dataArray;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pControlParams) ||
        (!WOS_IS_VALID_BUFFER(pControlParams->pEncodedSmpHeader)) ||
        (!WOS_IS_VALID_BUFFER(pControlParams->pMqttPacket)) ||
        (!WOS_IS_VALID_BUFFER(pControlParams->pIV)) ||
        (!WOS_IS_VALID_BUFFER(pControlParams->pAuthTag)) ||
        (!(WOS_IS_VALID_BUFFER(pPackedBuffer)))) {
        WLOGE("bad parameter");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the CBOR encoder. */
    cbor_encoder_init(&encoder, pPackedBuffer->data, pPackedBuffer->length, 0);

    /* Create the top level array container. */
    cborStatus =
        cbor_encoder_create_array(&encoder, &dataArray, CborIndefiniteLength);
    if (CborNoError != cborStatus) {
        WLOGE("create root array failed %x", cborStatus);
        goto exit;
    }

    /* Add CBOR-encoded SMP header. */
    cborStatus = cbor_encode_byte_string(
        &dataArray, pControlParams->pEncodedSmpHeader->data,
        pControlParams->pEncodedSmpHeader->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode smp-header failed %x", cborStatus);
        goto exit;
    }

    /* Add standard MQTT packet. */
    cborStatus =
        cbor_encode_byte_string(&dataArray, pControlParams->pMqttPacket->data,
                                pControlParams->pMqttPacket->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode mqtt-packet failed %x", cborStatus);
        goto exit;
    }

    /* Add IV. */
    cborStatus = cbor_encode_byte_string(&dataArray, pControlParams->pIV->data,
                                         pControlParams->pIV->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode IV failed %x", cborStatus);
        goto exit;
    }

    /* Add auth tag. */
    cborStatus =
        cbor_encode_byte_string(&dataArray, pControlParams->pAuthTag->data,
                                pControlParams->pAuthTag->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode auth-tag failed %x", cborStatus);
        goto exit;
    }

    /* Close the top level array container. */
    cborStatus = cbor_encoder_close_container_checked(&encoder, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("close root array failed %x", cborStatus);
        goto exit;
    }

    /* Get the actual encoded buffer size. */
    pPackedBuffer->length =
        cbor_encoder_get_buffer_size(&encoder, pPackedBuffer->data);

    WLOGD("total encoded length %d", pPackedBuffer->length);

    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

/**
 * Unpack the binary buffer to get Mqtts Control Message.
 */
WosMsgError_t
wosMsgUnpackSmpMqttsControlMessage(const WosBuffer_t *pPackedBuffer,
                                   WosMsgMqttsControlParams_t *pControlParams)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborParser parser;
    CborValue value;
    CborValue nextValue1, nextValue2;
    CborValue dataArray;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((!WOS_IS_VALID_BUFFER(pPackedBuffer)) || (NULL == pControlParams)) {
        WLOGE("bad parameter");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the parser. */
    cborStatus = cbor_parser_init(pPackedBuffer->data, pPackedBuffer->length, 0,
                                  &parser, &value);
    if (CborNoError != cborStatus) {
        WLOGE("parser initialization failed %x", cborStatus);
        goto exit;
    }

    /* Check if top level container is an array. */
    if (false == cbor_value_is_array(&value)) {
        WLOGE("badly formatted packet");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }

    /* Enter into root array. */
    cborStatus = cbor_value_enter_container(&value, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("entering root array failed %x", cborStatus);
        goto exit;
    }

    /* Get the CBOR-encoded-smp-header. */
    msgStatus = msgCborParseByteString(
        &dataArray, &(pControlParams->pEncodedSmpHeader), &nextValue1);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting smp-header failed %x", msgStatus);
        goto exit;
    }

    /* Get the AEAD protected mqtt packet. */
    msgStatus = msgCborParseByteString(
        &nextValue1, &(pControlParams->pMqttPacket), &nextValue2);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting mqtt packet failed %x", msgStatus);
        goto exit;
    }

    /* Get the IV. */
    msgStatus = msgCborParseByteString(&nextValue2, &(pControlParams->pIV),
                                       &nextValue1);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting IV failed %x", msgStatus);
        goto exit;
    }

    /* Get the auth-tag. */
    msgStatus =
        msgCborParseByteString(&nextValue1, &(pControlParams->pAuthTag), NULL);
    if (WOS_MSG_SUCCESS != msgStatus) {
        WLOGE("extracting authTag failed %x", msgStatus);
        goto exit;
    }

    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    /* If there is failure during parsing, release memory. */
    if (WOS_MSG_SUCCESS != msgStatus) {
        wosMsgFreeSmpMqttsControlMessage(pControlParams);
    }

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

/**
 * Free the WosMsgMqttsControlParams_t structure.
 */
void wosMsgFreeSmpMqttsControlMessage(
    WosMsgMqttsControlParams_t *pControlParams)
{
    FUNCTION_ENTRY();

    /* Release memory. */
    if (NULL != pControlParams) {
        WOS_FREE_BUF_AND_DATA(pControlParams->pEncodedSmpHeader);
        WOS_FREE_BUF_AND_DATA(pControlParams->pMqttPacket);
        WOS_FREE_BUF_AND_DATA(pControlParams->pIV);
        WOS_FREE_BUF_AND_DATA(pControlParams->pAuthTag);
    }

    FUNCTION_EXIT();
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
