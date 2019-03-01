/**
 * @file smpInternal.c
 * @brief
 * @version 0.1
 * @date 2019-02-26
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
 *
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wclTypes.h"

#include "wosCert.h"
#include "wosCommon.h"
#include "wosLog.h"
#include "wosMemory.h"
#include "wosMsgSmp.h"
#include "wosStorage.h"
#include "wosString.h"

#include "smpGlobalCreds.h"
#include "smpInternalUtils.h"

#include "smp.h"
#include "smpInternal.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "SMP"

/* We need to estimate the size for serialized buffer. CBOR adds few bytes for
 * every data fields(like int, byte-string etc.)  it encodes to carry the
 * type-information and size. We estimate roughly following values depending on
 * number of elements in WosSmpHeader_t, WosMsgMqttsSeParams_t and
 * WosMsgMqttsControlParams_t.
 */
#define SMP_HEADER_MSG_SERIALIZER_SIZE_OVERHEAD (3 + 8 + 8)
#define SMP_MQTTS_SE_MSG_SERIALIZER_SIZE_OVERHEAD (10 * 4)
#define SMP_MQTTS_CONTROL_MSG_SERIALIZER_SIZE_OVERHEAD (4 * 4)

#define SMP_ENCODED_HEADER_LENGTH                                              \
    (SMP_HEADER_MSG_SERIALIZER_SIZE_OVERHEAD + WCL_SMP_CLIENT_ID_LENGTH)

/* The below constant are based on WCL_SMP_CIPHER_SCHEME_ID0, not hardcoding in
 * deeper in code so that in future we can easily parameterized it to configure
 * SMP for other crypto schemes. */
/* ECC curve used for DH key exchange and in DSA operation. */
const WosCryptoEccOptions_t gEccDHOptions = {
    .curve = WOS_CRYPTO_ECC_CURVE_NIST_P256,
    .hash = WOS_CRYPTO_ECC_HASH_SHA256};

/* For session key crypto operations. */
const WosCryptoAeOptions_t gAeadOptions = {
    .algorithm = WOS_CRYPTO_AE_ALGORITHM_AES, .mode = WOS_CRYPTO_AE_MODE_GCM};

/* Definitions of message lebels, keep synchronized with WclSmpMessageType_t. */
const WosString_t gClientToBrokerMsgLabels[] = {
    NULL,         /* MQTTS_CONNECT = 1, */
    NULL,         /* MQTTS_CONNACK = 2, */
    "C2B_PUBLIS", /* MQTTS_PUBLISH = 3, */
    "C2B_PUBACK", /* MQTTS_PUBACK = 4, */
    "C2B_PUBREC", /* MQTTS_PUBREC = 5, */
    "C2B_PUBREL", /* MQTTS_PUBREL = 6, */
    "C2B_PUBCOM", /* MQTTS_PUBCOMP = 7, */
    "C2B_SUBSCR", /* MQTTS_SUBSCRIBE = 8, */
    "C2B_SUBACK", /* MQTTS_SUBACK = 9, */
    "C2B_USUBSC", /* MQTTS_UNSUBSCRIBE = 10, */
    "C2B_USUBAC", /* MQTTS_UNSUBACK = 11, */
    "C2B_PINGRQ", /* MQTTS_PINGREQ = 12, */
    "C2B_PINGRS", /* MQTTS_PINGRESP = 13, */
    "C2B_DISCON"  /* MQTTS_DISCONNECT = 14 */
};
const WosString_t gBrokerToClientMsgLabels[] = {
    NULL,         /* MQTTS_CONNECT = 1, */
    NULL,         /* MQTTS_CONNACK = 2, */
    "B2C_PUBLIS", /* MQTTS_PUBLISH = 3, */
    "B2C_PUBACK", /* MQTTS_PUBACK = 4, */
    "B2C_PUBREC", /* MQTTS_PUBREC = 5, */
    "B2C_PUBREL", /* MQTTS_PUBREL = 6, */
    "B2C_PUBCOM", /* MQTTS_PUBCOMP = 7, */
    "B2C_SUBSCR", /* MQTTS_SUBSCRIBE = 8, */
    "B2C_SUBACK", /* MQTTS_SUBACK = 9, */
    "B2C_USUBSC", /* MQTTS_UNSUBSCRIBE = 10, */
    "B2C_USUBAC", /* MQTTS_UNSUBACK = 11, */
    "B2C_PINGRQ", /* MQTTS_PINGREQ = 12, */
    "B2C_PINGRS", /* MQTTS_PINGRESP = 13, */
    "B2C_DISCON"  /* MQTTS_DISCONNECT = 14 */
};

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

/* Helper to get the serialized SMP header. */
static WclError_t lSmpPackHeader(SmpSessionContext_t *pSmpCtx,
                                 WclSmpMessageType_t messageType,
                                 WosBuffer_t *pEncodedHeader);

/* Sign the SMP header and SE Params. Also populates the pMqttSeParams with
 * required fields to be directly used in wosMsgPackSmpMqttsSEMessage(). We also
 * need to calculate total length(in bytes) taken by all the members of
 * WosMsgMqttsSeParams_t. This is needed to estimate the size of memory needed
 * for serialized SMP MQTTS SE Message.  */
static WclError_t lSmpSignSeParams(SmpSessionContext_t *pSmpCtx,
                                   WosMsgMqttsSeParams_t *pMqttSeParams,
                                   uint32_t *pSeParamsTotalBytesLength);

static WclError_t
lSmpGetDeviceCertificates(SmpSessionContext_t *pSmpCtx,
                          WosMsgMqttsSeParams_t *pMqttSeParams);

/* Verify the SMP header and SE Params from responder. */
static WclError_t lSmpVerifySeParams(SmpSessionContext_t *pSmpCtx,
                                     WosMsgMqttsSeParams_t *pMqttSeParams);

/* Import the session establishment message containing public keys for session
 * key computation. */
static WclError_t
lSmpImportSessionEstablishmentParams(SmpSessionContext_t *pSmpCtx,
                                     const WosBuffer_t *pSmpSEMessage);

/* Validate the SMP common header data like client-id, context,
 * message-counter etc. */
static WclError_t lSmpValidateSmpHeader(SmpSessionContext_t *pSmpCtx,
                                        const WosSmpHeader_t *pSmpHeader);

/* Process the Session Establishment message, generate and persist the
 * session key. */
static WclError_t lSmpProcessSeMessage(SmpSessionContext_t *pSmpCtx,
                                       const WosBuffer_t *pSmpSEMessage,
                                       WosBuffer_t *pClearMessage);

/* Process the MQTTS control message. */

static WclError_t lSmpProcessControlMessage(SmpSessionContext_t *pSmpCtx,
                                            WclSmpMessageType_t messageType,
                                            const WosBuffer_t *pSecuredMessage,
                                            WosBuffer_t *pClearMessage);

/* ========================================================================== */
/*                                Local Function Definitions */
/* ========================================================================== */

static WclError_t lSmpPackHeader(SmpSessionContext_t *pSmpCtx,
                                 WclSmpMessageType_t messageType,
                                 WosBuffer_t *pEncodedHeader)
{
    WclError_t smpResult = WCL_ERROR;
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosSmpHeader_t smpHeader;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (NULL == pEncodedHeader)) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Pack SMP header. */
    /* The input parameters. */
    smpHeader.commonHeader.messageContext = WOS_MSG_CONTEXT_SMP_MQTTS;
    smpHeader.commonHeader.messageContextVersion = 0; /* current version */
    smpHeader.messageType = messageType;
    smpHeader.clientId = pSmpCtx->clientId;
    smpHeader.messageId = pSmpCtx->toBeSentMessageId;
    /* Allocate the output. */
    pEncodedHeader->data = wosMemAlloc(SMP_ENCODED_HEADER_LENGTH);
    if (NULL == pEncodedHeader->data) {
        WLOGE("error allocating memory.");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    pEncodedHeader->length = SMP_ENCODED_HEADER_LENGTH;
    msgResult = wosMsgPackSmpHeader(&smpHeader, pEncodedHeader);
    if (WOS_MSG_SUCCESS != msgResult) {
        WLOGE("serialization of smp-header failed %x", msgResult);
        smpResult = WCL_ERROR_SERIALIZATION;
        goto exit;
    }

    smpResult = WCL_SUCCESS;

exit:
    if (WCL_SUCCESS != smpResult) {
        WOS_FREE_DATA(pEncodedHeader);
    }
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

static WclError_t lSmpSignSeParams(SmpSessionContext_t *pSmpCtx,
                                   WosMsgMqttsSeParams_t *pMqttSeParams,
                                   uint32_t *pSeParamsTotalBytesLength)
{
    WclError_t smpResult = WCL_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    WosBuffer_t toBeSignedData = {.data = NULL, .length = 0};
    uint32_t offset = 0;
    uint8_t i = 0;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (NULL == pMqttSeParams) ||
        (NULL == pMqttSeParams->pEncodedSmpHeader) ||
        (NULL == pMqttSeParams->pEccDhPubParams) ||
        (NULL == pMqttSeParams->pMqttPacket) ||
        (NULL == pSeParamsTotalBytesLength)) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Concat the encoded SMP header, cipher scheme, ECC-DH public params and
     * protocol packet for sigining. */
#if defined(SMP_MQTTS_CLIENT)
    pMqttSeParams->cipherSchemeId = WCL_SMP_CIPHER_SCHEME_ID0;
#endif
    toBeSignedData.length = (pMqttSeParams->pEncodedSmpHeader)->length +
                            (pMqttSeParams->pEccDhPubParams)->length +
                            (pMqttSeParams->pMqttPacket)->length;
#if defined(SMP_MQTTS_CLIENT)
    toBeSignedData.length += 1; // cipher-scheme-id
#endif
    toBeSignedData.data = wosMemAlloc(toBeSignedData.length);
    if (NULL == toBeSignedData.data) {
        WLOGE("error allocating memory.");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    wosMemCopy(toBeSignedData.data, (pMqttSeParams->pEncodedSmpHeader)->data,
               (pMqttSeParams->pEncodedSmpHeader)->length);
    offset = (pMqttSeParams->pEncodedSmpHeader)->length;
#if defined(SMP_MQTTS_CLIENT)
    toBeSignedData.data[offset] = pMqttSeParams->cipherSchemeId;
    offset += 1;
#endif
    wosMemCopy(toBeSignedData.data + offset,
               (pMqttSeParams->pEccDhPubParams)->data,
               (pMqttSeParams->pEccDhPubParams)->length);
    offset += (pMqttSeParams->pEccDhPubParams)->length;
    wosMemCopy(toBeSignedData.data + offset, (pMqttSeParams->pMqttPacket)->data,
               (pMqttSeParams->pMqttPacket)->length);

    /* Sign */
    cryptoResult =
        wosCryptoEccSign(pSmpCtx->pEccOptions, pSmpCtx->pStorageContext,
                         pSmpCtx->selfPrivKeyStorageId, &toBeSignedData,
                         &(pMqttSeParams->pSignature));
    if (cryptoResult != WOS_CRYPTO_SUCCESS) {
        WLOGE("signing failed %x", cryptoResult);
        smpResult = WCL_ERROR_CRYPTO_OPERATION;
        goto exit;
    }

    /* Add numCerts and certchain in pMqttSeParams */
    smpResult = lSmpGetDeviceCertificates(pSmpCtx, pMqttSeParams);
    if (smpResult != WCL_SUCCESS) {
        WLOGE("error reading device certificates.");
        goto exit;
    }

    // #TODO calculate the pSeParamsTotalBytesLength
    *pSeParamsTotalBytesLength = SMP_MQTTS_SE_MSG_SERIALIZER_SIZE_OVERHEAD +
                                 toBeSignedData.length +
                                 pMqttSeParams->pSignature->length + 1;
    for (i = 0; i < pMqttSeParams->numCerts; ++i) {
        *pSeParamsTotalBytesLength += pMqttSeParams->ppCerts[i]->length;
    }

    smpResult = WCL_SUCCESS;

exit:
    if (NULL != toBeSignedData.data) {
        WOS_FREE_DATA(&toBeSignedData);
    }
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

static WclError_t
lSmpGetDeviceCertificates(SmpSessionContext_t *pSmpCtx,
                          WosMsgMqttsSeParams_t *pMqttSeParams)
{
    WclError_t smpResult = WCL_ERROR;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();

    // TODO FIX and remove const
    // As of now we are supporting only one level certificate-chain
    pSmpCtx->selfCertChainSize = 1;
    // TODO Parse larger chain
    pMqttSeParams->ppCerts = wosMemAlloc(sizeof(WosBuffer_t *));
    if (NULL == pMqttSeParams->ppCerts) {
        WLOGE("error allocating memory.");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }

    storageResult = wosStorageRead(pSmpCtx->pStorageContext,
                                   pSmpCtx->selfCertChainStorageId,
                                   &(pMqttSeParams->ppCerts[0]));
    if (storageResult != WOS_STORAGE_SUCCESS) {
        WLOGE("error getting buffer with root certificate.");
        smpResult = WCL_ERROR_STORAGE_OPERATION;
        goto exit;
    }
    pMqttSeParams->numCerts = pSmpCtx->selfCertChainSize;
    smpResult = WCL_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

static WclError_t lSmpVerifySeParams(SmpSessionContext_t *pSmpCtx,
                                     WosMsgMqttsSeParams_t *pMqttSeParams)
{
    WclError_t smpResult = WCL_ERROR;
    WosBuffer_t signedData = {.data = NULL, .length = 0};
    uint32_t offset = 0;
    uint8_t i = 0;
    WosCertError_t certResult;
    WosCertOptions_t certOptions;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;
    WosBuffer_t *pRootCa = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (NULL == pMqttSeParams)) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Check if parsed message(in lSmpProcessSeAckMessage() call) has all the
     * values to further process. */
    if ((!WOS_IS_VALID_BUFFER(pMqttSeParams->pEncodedSmpHeader)) ||
        (!WOS_IS_VALID_BUFFER(pMqttSeParams->pEccDhPubParams)) ||
        (!WOS_IS_VALID_BUFFER(pMqttSeParams->pMqttPacket)) ||
#if defined(SMP_MQTTS_BROKER)
        /* We get cipher-scheme-id on broker side, as of now we expect only one
           value "0". */
        (WCL_SMP_CIPHER_SCHEME_ID0 != pMqttSeParams->cipherSchemeId) ||
#endif
        (pMqttSeParams->numCerts < 1) || (NULL == pMqttSeParams->ppCerts)) {
        WLOGE("bad message");
        smpResult = WCL_ERROR_INVALID_MESSAGE;
        goto exit;
    }
    for (i = 0; i < pMqttSeParams->numCerts; ++i) {
        if (NULL == pMqttSeParams->ppCerts[i]) {
            WLOGE("bad message");
            smpResult = WCL_ERROR_INVALID_MESSAGE;
            goto exit;
        }
    }
    /* Concat the encoded SMP header, ECC-DH public params and
     * protocol packet for verifying. */
    signedData.length = (pMqttSeParams->pEncodedSmpHeader)->length +
#if defined(SMP_MQTTS_BROKER)
                        sizeof(pMqttSeParams->cipherSchemeId) +
#endif
                        (pMqttSeParams->pEccDhPubParams)->length +
                        (pMqttSeParams->pMqttPacket)->length;
    signedData.data = wosMemAlloc(signedData.length);
    if (NULL == signedData.data) {
        WLOGE("error allocating memory.");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    wosMemCopy(signedData.data, (pMqttSeParams->pEncodedSmpHeader)->data,
               (pMqttSeParams->pEncodedSmpHeader)->length);
    offset = (pMqttSeParams->pEncodedSmpHeader)->length;
#if defined(SMP_MQTTS_BROKER)
    /* WARNING if datatype of cipherSchemeId is changed from uint8_t, this
     * should be reimplemented. */
    *(signedData.data + offset) = pMqttSeParams->cipherSchemeId;
    offset += 1;
#endif
    wosMemCopy(signedData.data + offset, (pMqttSeParams->pEccDhPubParams)->data,
               (pMqttSeParams->pEccDhPubParams)->length);
    offset += (pMqttSeParams->pEccDhPubParams)->length;
    wosMemCopy(signedData.data + offset, (pMqttSeParams->pMqttPacket)->data,
               (pMqttSeParams->pMqttPacket)->length);

    /* Verify the message. */
    storageResult = wosStorageRead(pSmpCtx->pStorageContext,
                                   pSmpCtx->rootCaStorageId, &pRootCa);
    if (storageResult != WOS_STORAGE_SUCCESS) {
        WLOGE("error getting buffer with root certificate.");
        smpResult = WCL_ERROR_STORAGE_OPERATION;
        goto exit;
    }
    certResult = wosCertValidateData(
        &certOptions, pRootCa, pMqttSeParams->numCerts, pMqttSeParams->ppCerts,
        &signedData, pMqttSeParams->pSignature);
    if (certResult != WOS_CERT_SIGNATURE_MATCH) {
        WLOGE("error validating signed data.");
        smpResult = WCL_ERROR_INVALID_MESSAGE;
        goto exit;
    }
    smpResult = WCL_SUCCESS;

exit:
    WOS_FREE_BUF_AND_DATA(pRootCa);
    if (NULL != signedData.data) {
        WOS_FREE_DATA(&signedData);
    }
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

static WclError_t lSmpValidateSmpHeader(SmpSessionContext_t *pSmpCtx,
                                        const WosSmpHeader_t *pSmpHeader)
{
    WclError_t smpResult = WCL_ERROR;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (NULL == pSmpHeader)) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Check if context is correct. */
    if (WOS_MSG_CONTEXT_SMP_MQTTS !=
        ((pSmpHeader->commonHeader).messageContext)) {
        WLOGE("bad message context");
        smpResult = WCL_ERROR_INVALID_MESSAGE;
        goto exit;
    }
    /* As of now, we dont care the context version. */
    /* Check if message-type is valid for the client or broker side processing.
     */
    if (!((WCL_SMP_MESSAGE_MQTTS_PUBLISH == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_PUBACK == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_PUBREC == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_PUBREL == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_PUBCOMP == pSmpHeader->messageType) ||
#if defined(SMP_MQTTS_CLIENT)
          (WCL_SMP_MESSAGE_MQTTS_CONNACK == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_SUBACK == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_UNSUBACK == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_PINGRESP == pSmpHeader->messageType))) {
#else
          (WCL_SMP_MESSAGE_MQTTS_CONNECT == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_SUBSCRIBE == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_UNSUBSCRIBE == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_PINGREQ == pSmpHeader->messageType) ||
          (WCL_SMP_MESSAGE_MQTTS_DISCONNECT == pSmpHeader->messageType))) {
#endif
        WLOGE("bad message type %x", pSmpHeader->messageType);
        smpResult = WCL_ERROR_INVALID_MESSAGE;
        goto exit;
    }

    /* Message-id of current received message must be greater than message-id of
     * last received message. */
    if (pSmpHeader->messageId < pSmpCtx->lastReceivedMessageId) {
        WLOGE("bad message");
        smpResult = WCL_ERROR_INVALID_MESSAGE;
        goto exit;
    }

    // #TODO other fields like client-id
    smpResult = WCL_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

static WclError_t lSmpProcessSeMessage(SmpSessionContext_t *pSmpCtx,
                                       const WosBuffer_t *pSmpSEAckMessage,
                                       WosBuffer_t *pClearMessage)
{
    WclError_t smpResult = WCL_ERROR;
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;
    WosMsgMqttsSeParams_t mqttsSeParams = {NULL, 0, NULL, NULL, NULL, 0, NULL};

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (!WOS_IS_VALID_BUFFER(pSmpSEAckMessage))) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Deserialize the ack-message. */
    msgResult = wosMsgUnpackSmpMqttsSEMessage(pSmpSEAckMessage, &mqttsSeParams);
    if (WOS_MSG_SUCCESS != msgResult) {
        WLOGE("deserializing the se-ack-message failed", msgResult);
        smpResult = WCL_ERROR_SERIALIZATION;
        goto exit;
    }

    /* Verify the signature and completeness of message. */
    smpResult = lSmpVerifySeParams(pSmpCtx, &mqttsSeParams);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("verification of of se-ack message failed");
        goto exit;
    }

    /* Generate and store the session key. */
    cryptoResult = wosCryptoDeriveSymKey(
        pSmpCtx->pEccOptions, pSmpCtx->pStorageContext,
        mqttsSeParams.pEccDhPubParams, pSmpCtx->sessionPrivateKeyStorageId,
        pSmpCtx->sessionSharedKeyStorageId);
    if (WOS_CRYPTO_SUCCESS != cryptoResult) {
        WLOGE("generating session-key failed %x", cryptoResult);
        smpResult = WCL_ERROR_CRYPTO_OPERATION;
        goto exit;
    }
    pSmpCtx->isSessionKeyEstablished = true;
    /* We generated the session key, now we can remove EC DH Keys from storage.
     */
    storageResult = wosStorageDelete(pSmpCtx->pStorageContext,
                                     pSmpCtx->sessionPrivateKeyStorageId);
    if (WOS_STORAGE_SUCCESS != storageResult) {
        /* We log the error and continue. */
        WLOGW("removing session private key from storage failed %x",
              storageResult);
    }
#if defined(SMP_MQTTS_CLIENT)
    storageResult = wosStorageDelete(pSmpCtx->pStorageContext,
                                     pSmpCtx->sessionPublicKeyStorageId);
    if (WOS_STORAGE_SUCCESS != storageResult) {
        /* We log the error and continue. */
        WLOGW("removing ecc dh pub key from storage failed %x", storageResult);
    }
#endif
    /* Copy the standard MQTT packet to output. */
    pClearMessage->data = wosMemAlloc((mqttsSeParams.pMqttPacket)->length);
    if (NULL == pClearMessage->data) {
        WLOGE("error allocating memory");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    pClearMessage->length = (mqttsSeParams.pMqttPacket)->length;
    wosMemCopy(pClearMessage->data, (mqttsSeParams.pMqttPacket)->data,
               pClearMessage->length);

    smpResult = WCL_SUCCESS;

exit:
    wosMsgFreeSmpMqttsSEMessage(&mqttsSeParams);
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

static WclError_t lSmpProcessControlMessage(SmpSessionContext_t *pSmpCtx,
                                            WclSmpMessageType_t messageType,
                                            const WosBuffer_t *pSecuredMessage,
                                            WosBuffer_t *pClearMessage)
{
    WclError_t smpResult = WCL_ERROR;
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    WosMsgMqttsControlParams_t mqttsControlParams = {NULL, NULL, NULL, NULL};
    WosBuffer_t aad = {.data = NULL, .length = 0};
    size_t labelLength = 0;
    bool hasClearMqttPacket = false;
    uint32_t offset = 0;
    WosBuffer_t *pCipherText = NULL;
    WosBuffer_t *pPlainText = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (!WOS_IS_VALID_BUFFER(pSecuredMessage))) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Check if session keys has been generated. */
    if (!pSmpCtx->isSessionKeyEstablished) {
        WLOGE("session has not been established");
        smpResult = WCL_ERROR_BAD_SESSION;
        goto exit;
    }

    /* Deserialize the mqtts-control-message. */
    msgResult = wosMsgUnpackSmpMqttsControlMessage(pSecuredMessage,
                                                   &mqttsControlParams);
    if (WOS_MSG_SUCCESS != msgResult) {
        WLOGE("deserializing the mqtts-control-message failed", msgResult);
        smpResult = WCL_ERROR_SERIALIZATION;
        goto exit;
    }
    /* Check if parsed message has all the values to further process. */
    if ((!WOS_IS_VALID_BUFFER(mqttsControlParams.pEncodedSmpHeader)) ||
        (!WOS_IS_VALID_BUFFER(mqttsControlParams.pMqttPacket)) ||
        (!WOS_IS_VALID_BUFFER(mqttsControlParams.pIV)) ||
        (!WOS_IS_VALID_BUFFER(mqttsControlParams.pAuthTag))) {
        WLOGE("bad message");
        smpResult = WCL_ERROR_INVALID_MESSAGE;
        goto exit;
    }

    /* A PUBLISH message received at client end has authentically encrypted MQTT
     * payload. Other messages has MQTT payload in clear since they are
     * only authenticated. Similarly only PUBLISH, SUBSCRIBE and UNSUBSCRIBE
     * message message received at broker end has authentically encrypted MQTT
     * payload.*/
#if defined(SMP_MQTTS_CLIENT)
    if ((WCL_SMP_MESSAGE_MQTTS_PUBLISH == messageType) ||
        (WCL_SMP_MESSAGE_MQTTS_SUBACK == messageType)) {
#else
    if ((WCL_SMP_MESSAGE_MQTTS_PUBLISH == messageType) ||
        (WCL_SMP_MESSAGE_MQTTS_SUBSCRIBE == messageType) ||
        (WCL_SMP_MESSAGE_MQTTS_UNSUBSCRIBE == messageType)) {
#endif
        hasClearMqttPacket = false;
    } else {
        hasClearMqttPacket = true;
    }

    /* Prepare the authentication data, SMP-header || label (|| MQTT packet). */
#if defined(SMP_MQTTS_CLIENT)
    labelLength = wosStringLength(gBrokerToClientMsgLabels[messageType]);
#else
    labelLength = wosStringLength(gClientToBrokerMsgLabels[messageType]);
#endif
    aad.length = (mqttsControlParams.pEncodedSmpHeader)->length + labelLength;
    if (hasClearMqttPacket) {
        aad.length += (mqttsControlParams.pMqttPacket)->length;
    }
    aad.data = wosMemAlloc(aad.length);
    if (NULL == aad.data) {
        WLOGE("error allocating memory");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    wosMemCopy(aad.data, (mqttsControlParams.pEncodedSmpHeader)->data,
               (mqttsControlParams.pEncodedSmpHeader)->length);
    offset = (mqttsControlParams.pEncodedSmpHeader)->length;
#if defined(SMP_MQTTS_CLIENT)
    wosMemCopy(aad.data + offset, gBrokerToClientMsgLabels[messageType],
               labelLength);
#else
    wosMemCopy(aad.data + offset, gClientToBrokerMsgLabels[messageType],
               labelLength);
#endif
    if (hasClearMqttPacket) {
        offset += labelLength;
        wosMemCopy(aad.data + offset, (mqttsControlParams.pMqttPacket)->data,
                   (mqttsControlParams.pMqttPacket)->length);
    }

    /* Only authenticate or authenticate and decrypt. */
    if (hasClearMqttPacket) {
        pCipherText = NULL;
    } else {
        pCipherText = mqttsControlParams.pMqttPacket;
    }
    cryptoResult = wosCryptoAeDecrypt(
        pSmpCtx->pAeadOptions, pSmpCtx->pStorageContext,
        pSmpCtx->sessionSharedKeyStorageId, pCipherText, &aad,
        mqttsControlParams.pIV, mqttsControlParams.pAuthTag, &pPlainText);
    if (WOS_CRYPTO_SUCCESS != cryptoResult) {
        WLOGE("message authentication failed");
        smpResult = WCL_ERROR_CRYPTO_OPERATION;
        goto exit;
    }
    /* Assign the clear MQTT packet to output data. */
    if (hasClearMqttPacket) {
        pClearMessage->data =
            wosMemAlloc((mqttsControlParams.pMqttPacket)->length);
        if (NULL == pClearMessage->data) {
            WLOGE("error allocating memory for output data");
            smpResult = WCL_ERROR_OUT_OF_MEMORY;
            goto exit;
        }
        wosMemCopy(pClearMessage->data, (mqttsControlParams.pMqttPacket)->data,
                   (mqttsControlParams.pMqttPacket)->length);
        pClearMessage->length = (mqttsControlParams.pMqttPacket)->length;
    } else {
        if (NULL == pPlainText) {
            WLOGE("message decryption failed");
            smpResult = WCL_ERROR_CRYPTO_OPERATION;
            goto exit;
        }
        pClearMessage->data = pPlainText->data;
        pClearMessage->length = pPlainText->length;
    }
    if (pPlainText != NULL) {
        /* Just free the buffer pointer not data. */
        wosMemFree(pPlainText);
    }

    smpResult = WCL_SUCCESS;

exit:
    wosMsgFreeSmpMqttsControlMessage(&mqttsControlParams);
    if (NULL != aad.data) {
        wosMemFree(aad.data);
    }
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

/* Generate the crypto assets. */
WclError_t smpInitialiseSessionParams(SmpSessionContext_t **ppSmpCtx)
{
    WclError_t smpResult = WCL_ERROR;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    SmpSessionContext_t *pSmpCtx = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if (NULL == ppSmpCtx) {
        WLOGE("bad params");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exitBadParam;
    }

    /* Allocate context. */
    pSmpCtx = wosMemAlloc(sizeof(SmpSessionContext_t));
    if (NULL == pSmpCtx) {
        WLOGE("error allocating memory");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    WLOGI("context %x", pSmpCtx);
    wosMemSet(pSmpCtx, 0, sizeof(SmpSessionContext_t));

    /* Initialize storage. */
#if defined(WOS_STORAGE_IMPL_REDIS)
    /* Initialize with REDIS default hostname/port. Check config. */
    storageResult = wosStorageInitialize(&(pSmpCtx->pStorageContext), NULL);
#elif defined(WOS_STORAGE_IMPL_STDC)
    storageResult = wosStorageInitialize(NULL, NULL);
#else
#error "Specify how storage need to be initialized."
#endif
    if (WOS_STORAGE_SUCCESS != storageResult) {
        WLOGE("storage-initialization failed %x", storageResult);
        smpResult = WCL_ERROR_STORAGE_OPERATION;
        goto exit;
    }

    /* Initialize global configurations and static config validation. */
    smpResult = smpConfigGlobalCreds(pSmpCtx);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("SMP initialization failed %x", smpResult);
        goto exit;
    }

    /* Generate ECC dh params. As of now we support only one curve as defined by
     * gEccDHOptions. */
    pSmpCtx->pEccOptions = &gEccDHOptions;
    smpResult = smpUtilsGen32CharRandomId(pSmpCtx->sessionPrivateKeyStorageId);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("generating random storage id for session private key failed");
        smpResult = WCL_ERROR;
        goto exit;
    }
    smpResult = smpUtilsGen32CharRandomId(pSmpCtx->sessionPublicKeyStorageId);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("generating random storage id for session public key failed");
        smpResult = WCL_ERROR;
        goto exit;
    }
    cryptoResult =
        wosCryptoEccGenerateKey(pSmpCtx->pEccOptions, pSmpCtx->pStorageContext,
                                pSmpCtx->sessionPrivateKeyStorageId,
                                pSmpCtx->sessionPublicKeyStorageId);
    if (WOS_CRYPTO_SUCCESS != cryptoResult) {
        WLOGE("session keys generation failed %x", cryptoResult);
        smpResult = WCL_ERROR_CRYPTO_OPERATION;
        goto exit;
    }

    pSmpCtx->isPreSessionSecretsGenerated = true;

    pSmpCtx->pAeadOptions = &gAeadOptions;
    smpResult = smpUtilsGen32CharRandomId(pSmpCtx->sessionSharedKeyStorageId);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("generating random session-key storage Id failed");
        smpResult = WCL_ERROR;
        goto exit;
    }

    *ppSmpCtx = pSmpCtx;
    smpResult = WCL_SUCCESS;

exit:
    if (WCL_SUCCESS != smpResult) {
        if (NULL != pSmpCtx->pStorageContext) {
            storageResult = wosStorageTerminate(pSmpCtx->pStorageContext);
        }
        if (NULL != pSmpCtx) {
            wosMemFree(pSmpCtx);
        }
    }
exitBadParam:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* Export the session establishment message containing public keys for
 * key-exchange. */
WclError_t
smpExportSessionEstablishmentParams(SmpSessionContext_t *pSmpCtx,
                                    const WosBuffer_t *pStdProtocolSEParams,
                                    WosBuffer_t *pSmpSEMessage)
{
    WclError_t smpResult = WCL_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosBuffer_t encodedHeader = {.data = NULL, .length = 0};
    WosMsgMqttsSeParams_t mqttsSeParams = {NULL, 0, NULL, NULL, NULL, 0, NULL};
    WosSmpHeader_t smpHeader = {{0, 0}, 0, NULL, 0};
    uint32_t seParamsTotalBytesLength = 0;
    uint8_t indexCert = 0;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (!WOS_IS_VALID_BUFFER(pStdProtocolSEParams)) ||
        (NULL == pSmpSEMessage)) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }
    WLOGI("context %x", pSmpCtx);

    /* Pack SMP header. */
    smpResult = lSmpPackHeader(pSmpCtx,
#if defined(SMP_MQTTS_CLIENT)
                               WCL_SMP_MESSAGE_MQTTS_CONNECT,
#else
                               WCL_SMP_MESSAGE_MQTTS_CONNACK,
#endif
                               &encodedHeader);

    if (WCL_SUCCESS != smpResult) {
        WLOGE("error packing header");
        goto exit;
    }

    /* Read the ECC-DH params. */
    cryptoResult = wosCryptoEccReadPubKey(
        pSmpCtx->pEccOptions, pSmpCtx->pStorageContext,
        pSmpCtx->sessionPublicKeyStorageId, &(mqttsSeParams.pEccDhPubParams));
    if (WOS_CRYPTO_SUCCESS != cryptoResult ||
        (!WOS_IS_VALID_BUFFER(mqttsSeParams.pEccDhPubParams))) {
        WLOGE("reading ecc-dh-public-params header failed %x", cryptoResult);
        smpResult = WCL_ERROR_CRYPTO_OPERATION;
        goto exit;
    }

    /* Concatenate the encoded SMP header, cipher scheme, ECC-DH public params
     * and protocol packet for sigining. */
    mqttsSeParams.pEncodedSmpHeader = &encodedHeader;
    mqttsSeParams.pMqttPacket = pStdProtocolSEParams;
    smpResult =
        lSmpSignSeParams(pSmpCtx, &mqttsSeParams, &seParamsTotalBytesLength);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("siginging the se-params failed");
        goto exit;
    }
    WLOGD("SE-params total byte length %x", seParamsTotalBytesLength);

    /* Finally serialize the SMP Session Establishment Packet. */
    /* Allocate the memory for serialized buffer. */
    pSmpSEMessage->data = wosMemAlloc(seParamsTotalBytesLength);
    if (NULL == pSmpSEMessage->data) {
        WLOGE("error allocating memory");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    pSmpSEMessage->length = seParamsTotalBytesLength;
    /* Serialize the SE-Params. */
    msgResult = wosMsgPackSmpMqttsSEMessage(&mqttsSeParams, pSmpSEMessage);
    if (WOS_MSG_SUCCESS != msgResult) {
        WLOGE("serializing the se-message failed %x", msgResult);
        smpResult = WCL_ERROR_SERIALIZATION;
        goto exit;
    }

    /* Increment the message counter. */
    pSmpCtx->toBeSentMessageId++;
    smpResult = WCL_SUCCESS;

exit:
    WOS_FREE_DATA(&encodedHeader);
    WOS_FREE_BUF_AND_DATA(mqttsSeParams.pEccDhPubParams);
    for (indexCert = 0; indexCert < mqttsSeParams.numCerts; indexCert++) {
        WOS_FREE_BUF_AND_DATA(mqttsSeParams.ppCerts[indexCert]);
    }
    if (mqttsSeParams.ppCerts != NULL) {
        wosMemFree(mqttsSeParams.ppCerts);
    }
    WOS_FREE_BUF_AND_DATA(mqttsSeParams.pSignature);
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* Secure(encrypt/sign) an initiator's payload to be sent. */
WclError_t smpSecureMessage(SmpSessionContext_t *pSmpCtx,
                            WclSmpMessageType_t messageType,
                            const WosBuffer_t *pClearMessage,
                            WosBuffer_t *pSecuredMessage)
{
    WclError_t smpResult = WCL_ERROR;
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    WosBuffer_t encodedHeader = {.data = NULL, .length = 0};
    WosMsgMqttsControlParams_t mqttsControlParams = {NULL, NULL, NULL, NULL};
    bool encryptMqttPacket = false;
    WosBuffer_t aad = {NULL, 0};
    size_t labelLength = 0;
    size_t offset = 0;
    WosBuffer_t *pCipherText = NULL;
    WosBuffer_t *pPlainText = NULL;
    WosBuffer_t *pIv = NULL;
    WosBuffer_t *pAuthTag = NULL;
    size_t seializedBufSize = 0;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (!WOS_IS_VALID_BUFFER(pClearMessage)) ||
        (NULL == pSecuredMessage)) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }
    if ((messageType < WCL_SMP_MESSAGE_MQTTS_CONNECT) ||
        (messageType > WCL_SMP_MESSAGE_MQTTS_DISCONNECT)) {
        WLOGE("bad message type %x", messageType);
        smpResult = WCL_ERROR_INVALID_MESSAGE;
        goto exit;
    }
    WLOGI("context %x", pSmpCtx);

    /* Check if session keys has been generated. */
    if (!pSmpCtx->isSessionKeyEstablished) {
        WLOGE("session has not been established");
        smpResult = WCL_ERROR_BAD_SESSION;
        goto exit;
    }

    /* Pack SMP header. */
    smpResult = lSmpPackHeader(pSmpCtx, messageType, &encodedHeader);
    if (WOS_MSG_SUCCESS != smpResult) {
        WLOGE("Error packing header.");
        goto exit;
    }

    /* A PUBLISH or SUBSCRIBE or SUB_ACK MQTT payload to be sent
    from client/broker end has to be authentically encrypted. MQTT payload of
    other messages will be only authenticated. */
    if ((WCL_SMP_MESSAGE_MQTTS_PUBLISH == messageType) ||
        (WCL_SMP_MESSAGE_MQTTS_SUBSCRIBE == messageType) ||
        (WCL_SMP_MESSAGE_MQTTS_SUBACK == messageType) ||
        (WCL_SMP_MESSAGE_MQTTS_UNSUBSCRIBE == messageType)) {
        encryptMqttPacket = true;
    }

    /* Prepare the authentication data, SMP-header || label (|| MQTT packet). */
#if defined(SMP_MQTTS_CLIENT)
    labelLength = wosStringLength(gClientToBrokerMsgLabels[messageType]);
#else
    labelLength = wosStringLength(gBrokerToClientMsgLabels[messageType]);
#endif
    aad.length = encodedHeader.length + labelLength;
    if (!encryptMqttPacket) {
        aad.length += pClearMessage->length;
    }
    aad.data = wosMemAlloc(aad.length);
    if (NULL == aad.data) {
        WLOGE("error allocating memory for aad");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    wosMemCopy(aad.data, encodedHeader.data, encodedHeader.length);
    offset = encodedHeader.length;
#if defined(SMP_MQTTS_CLIENT)
    wosMemCopy(aad.data + offset, gClientToBrokerMsgLabels[messageType],
               labelLength);
#else
    wosMemCopy(aad.data + offset, gBrokerToClientMsgLabels[messageType],
               labelLength);
#endif
    if (!encryptMqttPacket) {
        offset += labelLength;
        wosMemCopy(aad.data + offset, pClearMessage->data,
                   pClearMessage->length);
    }

    /* Only authenticate or authenticate and encrypt. */
    if (encryptMqttPacket) {
        pPlainText = pClearMessage;
    } else {
        pPlainText = NULL;
    }
    cryptoResult =
        wosCryptoAeEncrypt(pSmpCtx->pAeadOptions, pSmpCtx->pStorageContext,
                           pSmpCtx->sessionSharedKeyStorageId, pPlainText, &aad,
                           &pIv, &pCipherText, &pAuthTag);
    if ((WOS_CRYPTO_SUCCESS != cryptoResult) || (!WOS_IS_VALID_BUFFER(pIv)) ||
        (!WOS_IS_VALID_BUFFER(pAuthTag)) ||
        (encryptMqttPacket && (!WOS_IS_VALID_BUFFER(pCipherText)))) {
        WLOGE("encryption failed %x", cryptoResult);
        smpResult = WCL_ERROR_CRYPTO_OPERATION;
        goto exit;
    }

    /* Serialize the SMP MQTTS Control Message. */
    /* Prepare the input params. */
    mqttsControlParams.pEncodedSmpHeader = &encodedHeader;
    if (encryptMqttPacket) {
        mqttsControlParams.pMqttPacket = pCipherText;
    } else {
        mqttsControlParams.pMqttPacket = pClearMessage;
    }
    mqttsControlParams.pIV = pIv;
    mqttsControlParams.pAuthTag = pAuthTag;
    /* Allocate memory for the output buffer. */
    seializedBufSize = SMP_MQTTS_CONTROL_MSG_SERIALIZER_SIZE_OVERHEAD +
                       encodedHeader.length + pIv->length + pAuthTag->length;
    /* In case of padding if CT size is greater than PT size. */
    if (encryptMqttPacket) {
        seializedBufSize += pCipherText->length;
    } else {
        seializedBufSize += pClearMessage->length;
    }
    pSecuredMessage->data = wosMemAlloc(seializedBufSize);
    if (NULL == pSecuredMessage->data) {
        WLOGE("error allocating memory for output buffer");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    pSecuredMessage->length = seializedBufSize;
    msgResult =
        wosMsgPackSmpMqttsControlMessage(&mqttsControlParams, pSecuredMessage);
    if (WOS_MSG_SUCCESS != msgResult) {
        WLOGE("serialization of control-message failed %x", msgResult);
        smpResult = WCL_ERROR_SERIALIZATION;
        goto exit;
    }

    /* Increment the message counter. */
    pSmpCtx->toBeSentMessageId++;
    smpResult = WCL_SUCCESS;

exit:
    WOS_FREE_DATA(&encodedHeader);
    WOS_FREE_DATA(&aad);
    WOS_FREE_BUF_AND_DATA(pIv);
    WOS_FREE_BUF_AND_DATA(pAuthTag);
    WOS_FREE_BUF_AND_DATA(pCipherText);
    if (WCL_SUCCESS != smpResult) {
        WOS_FREE_DATA(pSecuredMessage);
    }

    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* Process Session Establishment Ack Message or Validate(decrypt/verify) a
 * payload from responder. */
WclError_t smpProcessMessage(SmpSessionContext_t *pSmpCtx,
                             const WosBuffer_t *pSecuredMessage,
                             WosBuffer_t *pClearMessage)
{
    WclError_t smpResult = WCL_ERROR;
    WosMsgError_t msgError = WOS_MSG_ERROR;
    WosSmpHeader_t smpHeader = {{0, 0}, 0, NULL, 0};

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pSmpCtx) || (!WOS_IS_VALID_BUFFER(pSecuredMessage)) ||
        (NULL == pClearMessage)) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }
    WLOGI("context %x", pSmpCtx);

    /* First we need to find out what type of message it is and if it contains
     * the right context and client-id etc. */
    /* Unpack SMP header. */
    smpHeader.clientId = wosMemAlloc(WCL_SMP_CLIENT_ID_LENGTH * sizeof(char));
    if (NULL == smpHeader.clientId) {
        WLOGE("error allocating memory");
        smpResult = WCL_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    msgError = wosMsgUnpackSmpHeaderFromSmpMsg(pSecuredMessage, &smpHeader);
    if (WOS_MSG_SUCCESS != msgError) {
        WLOGE("deserializing smp-header failed %x", msgError);
        goto exit;
    }
    /* Validate the SMP common header data like client-id, context,
     * message-counter etc. */
    smpResult = lSmpValidateSmpHeader(pSmpCtx, &smpHeader);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("invalid message");
        goto exit;
    }

    WLOGD("message type %x", smpHeader.messageType);
    /* Process the message. */
#if defined(SMP_MQTTS_CLIENT)
    if (WCL_SMP_MESSAGE_MQTTS_CONNACK == smpHeader.messageType) {
#else
    if (WCL_SMP_MESSAGE_MQTTS_CONNECT == smpHeader.messageType) {
#endif
        smpResult =
            lSmpProcessSeMessage(pSmpCtx, pSecuredMessage, pClearMessage);
    } else {
        smpResult = lSmpProcessControlMessage(pSmpCtx, smpHeader.messageType,
                                              pSecuredMessage, pClearMessage);
    }
    if (WCL_SUCCESS != smpResult) {
        WLOGE("message processing failed");
        goto exit;
    }

    /* Update the last received message-id. */
    pSmpCtx->lastReceivedMessageId = smpHeader.messageId;

    smpResult = WCL_SUCCESS;

exit:
    if (NULL != smpHeader.clientId) {
        wosMemFree(smpHeader.clientId);
    }
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* Delete the crypto assets. */
WclError_t smpDeleteSessionCredentials(SmpSessionContext_t *pSmpCtx)
{
    WclError_t smpResult = WCL_ERROR;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if (NULL == pSmpCtx) {
        WLOGE("invalid parameter");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }
    WLOGI("context %x", pSmpCtx);

    /* We removed ECC-DH params from transient storage after generating
     * session-key, as a safety measure, removing again for the case when user
     * generated ECC-DH params but didn't generated session-key and decided to
     * close the session. */
    if (pSmpCtx->isPreSessionSecretsGenerated) {
        storageResult = wosStorageDelete(pSmpCtx->pStorageContext,
                                         pSmpCtx->sessionPrivateKeyStorageId);
        if (WOS_STORAGE_SUCCESS != storageResult) {
            /* We log the error and continue. */
            WLOGW("removing session private key from storage failed %x",
                  storageResult);
        }
        storageResult = wosStorageDelete(pSmpCtx->pStorageContext,
                                         pSmpCtx->sessionPublicKeyStorageId);
        if (WOS_STORAGE_SUCCESS != storageResult) {
            /* We log the error and continue. */
            WLOGW("removing session public key from storage failed %x",
                  storageResult);
        }
    }

    /* Remove session-key storage. */
    if (pSmpCtx->isSessionKeyEstablished) {
        storageResult = wosStorageDelete(pSmpCtx->pStorageContext,
                                         pSmpCtx->sessionSharedKeyStorageId);
        if (WOS_STORAGE_SUCCESS != storageResult) {
            /* We log the error and continue. */
            WLOGW("removing session shared key from storage failed %x",
                  storageResult);
        }
    }

    storageResult = wosStorageTerminate(pSmpCtx->pStorageContext);
    if (WOS_STORAGE_SUCCESS != storageResult) {
        /* We log the error and continue. */
        WLOGW("storage termination failed %x", storageResult);
    }

    /* Free context. */
    wosMemFree(pSmpCtx);
    pSmpCtx = NULL;

    smpResult = WCL_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
