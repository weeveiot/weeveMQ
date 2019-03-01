/**
 * @brief Use this interfaces to build and parse Weeve Certificates in a CBOR
 * format.
 *
 * @file wosCborCert.c
 * @date 2018-09-25
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

#include "wosCborCert.h"
#include "wosCommon.h"
#include "wosLog.h"
#include "wosMemory.h"
#include "wosString.h"

#include "msgCborUtils.h"

#include "wosMsgCommon.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "MSG_CERT"

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

WosMsgError_t wosCborCalcCertificateTbs(const WosCertTbsType_t *pCertTbs,
                                        uint32_t *pPackedLength)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    uint32_t aux = 0;

    FUNCTION_ENTRY();

    if (pCertTbs == NULL || !WOS_IS_VALID_STRING(pCertTbs->issuerId) ||
        !WOS_IS_VALID_STRING(pCertTbs->subjectId) ||
        !WOS_IS_VALID_BUFFER(pCertTbs->pSubjectPubKey) ||
        pPackedLength == NULL) {
        WLOGE("bad parameters");
        msgResult = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    *pPackedLength = 0;
    *pPackedLength += 1; /* Array */
    *pPackedLength += msgCborCalcLengthUInt(pCertTbs->version);
    *pPackedLength += msgCborCalcLengthUInt(pCertTbs->signatureAlgorithm);
    aux = wosStringLength(pCertTbs->issuerId);
    *pPackedLength += msgCborCalcLengthUInt(aux) + aux;
    aux = wosStringLength(pCertTbs->subjectId);
    *pPackedLength += msgCborCalcLengthUInt(aux) + aux;
    *pPackedLength += msgCborCalcLengthUInt(pCertTbs->subjectPubKeyInfo);
    *pPackedLength += msgCborCalcLengthUInt(pCertTbs->pSubjectPubKey->length) +
                      pCertTbs->pSubjectPubKey->length;
    *pPackedLength += 1; /* Array Closure */

    msgResult = WOS_MSG_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(msgResult);
    return msgResult;
}

WosMsgError_t wosCborPackCertificateTbs(const WosCertTbsType_t *pCertTbs,
                                        WosBuffer_t *pPackedBuffer)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborEncoder encoder;
    CborEncoder dataArray;

    FUNCTION_ENTRY();

    /* Input parameter validation. */
    /* TODO Also validate pCertTbs->version */
    if ((pCertTbs == NULL) || (!WOS_IS_VALID_STRING(pCertTbs->issuerId)) ||
        (!WOS_IS_VALID_STRING(pCertTbs->subjectId)) ||
        (!WOS_IS_VALID_BUFFER(pCertTbs->pSubjectPubKey)) ||
        (!WOS_IS_VALID_BUFFER(pPackedBuffer))) {
        WLOGE("bad parameters");
        msgResult = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the CBOR encoder. */
    /* void */ cbor_encoder_init(&encoder, pPackedBuffer->data,
                                 pPackedBuffer->length, 0);

    /* Create the top level array container. */
    cborStatus =
        cbor_encoder_create_array(&encoder, &dataArray, CborIndefiniteLength);
    if (CborNoError != cborStatus) {
        WLOGE("create root array failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Version */
    cborStatus = cbor_encode_uint(&dataArray, pCertTbs->version);
    if (CborNoError != cborStatus) {
        WLOGE("encode version failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Signature Algorithm */
    cborStatus = cbor_encode_uint(&dataArray, pCertTbs->signatureAlgorithm);
    if (CborNoError != cborStatus) {
        WLOGE("encode signatureAlgorithm failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Issuer ID */
    cborStatus = cbor_encode_text_stringz(&dataArray, pCertTbs->issuerId);
    if (CborNoError != cborStatus) {
        WLOGE("encode issuerId failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Subject ID */
    cborStatus = cbor_encode_text_stringz(&dataArray, pCertTbs->subjectId);
    if (CborNoError != cborStatus) {
        WLOGE("encode subjectId failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Subject Public Key Algorithm */
    cborStatus = cbor_encode_uint(&dataArray, pCertTbs->subjectPubKeyInfo);
    if (CborNoError != cborStatus) {
        WLOGE("encode subjectPubKeyInfo failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Subject Public Key Data */
    cborStatus =
        cbor_encode_byte_string(&dataArray, pCertTbs->pSubjectPubKey->data,
                                pCertTbs->pSubjectPubKey->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode subjectPubKey failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Close the top level array container. */
    cborStatus = cbor_encoder_close_container_checked(&encoder, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("close root array failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Get the actual encoded buffer size. */
    pPackedBuffer->length =
        cbor_encoder_get_buffer_size(&encoder, pPackedBuffer->data);

    WLOGD("total encoded length %d", pPackedBuffer->length);

    msgResult = WOS_MSG_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(msgResult);
    return msgResult;
}

WosMsgError_t wosCborUnpackCertificateTbs(const WosBuffer_t *pPackedBuffer,
                                          WosCertTbsType_t *pCertTbs)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    CborError cborStatus = CborUnknownError;
    CborParser parser;
    CborValue iterator, iterator2;
    uint64_t version = 0, signatureAlgorithm = 0, subjectPubKeyInfo = 0;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((!WOS_IS_VALID_BUFFER(pPackedBuffer)) || (pCertTbs == NULL)) {
        WLOGE("bad parameter");
        msgResult = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the parser. */
    cborStatus = cbor_parser_init(pPackedBuffer->data, pPackedBuffer->length, 0,
                                  &parser, &iterator2);
    if (cborStatus != CborNoError) {
        WLOGE("parser initialization failed %d", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Get into root array. */
    if (cbor_value_is_array(&iterator2) == false) {
        WLOGE("badly formatted packet");
        msgResult = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }
    cborStatus = cbor_value_enter_container(&iterator2, &iterator);
    if (cborStatus != CborNoError) {
        WLOGE("entering root array failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Version */
    msgResult = msgCborParseUint64(&iterator, &version);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting version failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }
    pCertTbs->version = (uint8_t)version;
    /* msgCborParseUint64 does not auto advance to next */
    cborStatus = cbor_value_advance_fixed(&iterator);
    if (cborStatus != CborNoError) {
        WLOGE("advancing after version failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Signature Algorithm */
    msgResult = msgCborParseUint64(&iterator, &signatureAlgorithm);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting signatureAlgorithm failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }
    pCertTbs->signatureAlgorithm = (uint8_t)signatureAlgorithm;
    /* msgCborParseUint64 does not auto advance to next */
    cborStatus = cbor_value_advance_fixed(&iterator);
    if (cborStatus != CborNoError) {
        WLOGE("advancing after signatureAlgorithm failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Issuer ID */
    msgResult =
        msgCborParseTextString(&iterator, &(pCertTbs->issuerId), &iterator2);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting issuerId failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Subject ID */
    msgResult =
        msgCborParseTextString(&iterator2, &(pCertTbs->subjectId), &iterator);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting subjectId failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Subject Public Key Algorithm */
    msgResult = msgCborParseUint64(&iterator, &subjectPubKeyInfo);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting subjectPubKeyInfo failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }
    pCertTbs->subjectPubKeyInfo = (uint8_t)subjectPubKeyInfo;
    /* msgCborParseUint64 does not auto advance to next */
    cborStatus = cbor_value_advance_fixed(&iterator);
    if (cborStatus != CborNoError) {
        WLOGE("advancing after subjectPubKeyInfo failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Subject Public Key Data */
    msgResult = msgCborParseByteString(&iterator, &(pCertTbs->pSubjectPubKey),
                                       &iterator2);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting subjectPubKey failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    msgResult = WOS_MSG_SUCCESS;
    goto exit;

exit:
    FUNCTION_EXIT_RETURN(msgResult);
    return msgResult;
}

void wosCborFreeCertificateTbs(WosCertTbsType_t *pCertTbs)
{
    FUNCTION_ENTRY();

    if (pCertTbs != NULL) {
        pCertTbs->version = 0;
        pCertTbs->signatureAlgorithm = 0;
        wosMemFree(pCertTbs->issuerId);
        pCertTbs->issuerId = NULL;
        wosMemFree(pCertTbs->subjectId);
        pCertTbs->subjectId = NULL;
        pCertTbs->subjectPubKeyInfo = 0;
        WOS_FREE_BUF_AND_DATA(pCertTbs->pSubjectPubKey);
    }

    FUNCTION_EXIT();
}

WosMsgError_t
wosCborCalcCertificateContainer(const WosCertContainerType_t *pContainer,
                                uint32_t *pPackedLength)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    uint32_t aux = 0;

    FUNCTION_ENTRY();

    if (pContainer == NULL || !WOS_IS_VALID_BUFFER(pContainer->pEncodedTbs) ||
        !WOS_IS_VALID_BUFFER(pContainer->pSignature) || pPackedLength == NULL) {
        WLOGE("bad parameters");
        msgResult = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    *pPackedLength = 0;
    *pPackedLength += 1; /* Array */
    *pPackedLength += msgCborCalcLengthUInt(pContainer->version);
    *pPackedLength += msgCborCalcLengthUInt(pContainer->pEncodedTbs->length) +
                      pContainer->pEncodedTbs->length;
    *pPackedLength += msgCborCalcLengthUInt(pContainer->pSignature->length) +
                      pContainer->pSignature->length;
    *pPackedLength += 1; /* Array Closure */

    msgResult = WOS_MSG_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(msgResult);
    return msgResult;
}

WosMsgError_t
wosCborPackCertificateContainer(const WosCertContainerType_t *pCert,
                                WosBuffer_t *pPackedBuffer)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    CborEncoder encoder;
    CborEncoder dataArray;

    FUNCTION_ENTRY();

    /* Input parameter validation. */
    /* TODO Also validate pCert->version */
    if (pCert == NULL || !WOS_IS_VALID_BUFFER(pCert->pEncodedTbs) ||
        !WOS_IS_VALID_BUFFER(pCert->pSignature) ||
        !WOS_IS_VALID_BUFFER(pPackedBuffer)) {
        WLOGE("bad parameters");
        msgResult = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the CBOR encoder. */
    /* void */ cbor_encoder_init(&encoder, pPackedBuffer->data,
                                 pPackedBuffer->length, 0);

    /* Create the top level array container. */
    cborStatus =
        cbor_encoder_create_array(&encoder, &dataArray, CborIndefiniteLength);
    if (CborNoError != cborStatus) {
        WLOGE("create root array failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Version */
    cborStatus = cbor_encode_uint(&dataArray, pCert->version);
    if (CborNoError != cborStatus) {
        WLOGE("encode version failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Encoded TBS */
    cborStatus = cbor_encode_byte_string(&dataArray, pCert->pEncodedTbs->data,
                                         pCert->pEncodedTbs->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode TBS failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Signature */
    cborStatus = cbor_encode_byte_string(&dataArray, pCert->pSignature->data,
                                         pCert->pSignature->length);
    if (CborNoError != cborStatus) {
        WLOGE("encode TBS failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Close the top level array container. */
    cborStatus = cbor_encoder_close_container_checked(&encoder, &dataArray);
    if (CborNoError != cborStatus) {
        WLOGE("close root array failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Get the actual encoded buffer size. */
    pPackedBuffer->length =
        cbor_encoder_get_buffer_size(&encoder, pPackedBuffer->data);
    WLOGD("total encoded length %d", pPackedBuffer->length);

    msgResult = WOS_MSG_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(msgResult);
    return msgResult;
}

WosMsgError_t
wosCborUnpackCertificateContainer(const WosBuffer_t *pPackedBuffer,
                                  WosCertContainerType_t *pCert)
{
    WosMsgError_t msgResult = WOS_MSG_ERROR;
    CborError cborStatus = CborUnknownError;
    CborParser parser;
    CborValue iterator, iterator2;
    uint64_t version = 0;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((!WOS_IS_VALID_BUFFER(pPackedBuffer)) || (pCert == NULL)) {
        WLOGE("bad parameter");
        msgResult = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Initialize the parser. */
    cborStatus = cbor_parser_init(pPackedBuffer->data, pPackedBuffer->length, 0,
                                  &parser, &iterator2);
    if (cborStatus != CborNoError) {
        WLOGE("parser initialization failed %d", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Get into root array. */
    if (cbor_value_is_array(&iterator2) == false) {
        WLOGE("badly formatted packet");
        msgResult = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }
    cborStatus = cbor_value_enter_container(&iterator2, &iterator);
    if (cborStatus != CborNoError) {
        WLOGE("entering root array failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Version */
    msgResult = msgCborParseUint64(&iterator, &version);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting version failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }
    pCert->version = (uint8_t)version;
    /* msgCborParseUint64 does not auto advance to next */
    cborStatus = cbor_value_advance_fixed(&iterator);
    if (cborStatus != CborNoError) {
        WLOGE("advancing after version failed %x", cborStatus);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Encoded TBS */
    msgResult =
        msgCborParseByteString(&iterator, &(pCert->pEncodedTbs), &iterator2);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting encoded TBS failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exit;
    }

    /* Signature */
    msgResult =
        msgCborParseByteString(&iterator2, &(pCert->pSignature), &iterator);
    if (msgResult != WOS_MSG_SUCCESS) {
        WLOGE("extracting signature failed %x", msgResult);
        msgResult = WOS_MSG_ERROR;
        goto exitFreeTbs;
    }

    msgResult = WOS_MSG_SUCCESS;
    goto exit;

exitFreeTbs:
    WOS_FREE_DATA(pCert->pEncodedTbs);
exit:
    FUNCTION_EXIT_RETURN(msgResult);
    return msgResult;
}

void wosCborFreeCertificateContainer(WosCertContainerType_t *pCert)
{
    FUNCTION_ENTRY();

    if (pCert != NULL) {
        pCert->version = 0;
        WOS_FREE_BUF_AND_DATA(pCert->pEncodedTbs);
        WOS_FREE_BUF_AND_DATA(pCert->pSignature);
    }

    FUNCTION_EXIT();
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
