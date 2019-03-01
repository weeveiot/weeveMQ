/**
 * @brief Utility functions on top of CBor APIs.
 *
 * @file msgCborUtils.c
 * @date 2018-07-27
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
#include "wosMsgCommon.h"
#include "wosString.h"

#include "msgCborUtils.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "MSG_CBOR_UTIL"

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
 * Extract the Cbor uint64.
 *
 */
WosMsgError_t msgCborParseUint64(const CborValue *pCborValue,
                                 uint64_t *pExtractedValue)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if ((NULL == pCborValue) || (NULL == pExtractedValue)) {
        WLOGE("bad parameters");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Extract the number of certificates. */
    if (false == cbor_value_is_unsigned_integer(pCborValue)) {
        WLOGE("badly formatted packet, expected unsigned integer");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }
    /* Check if next element is an integer. */
    cborStatus = cbor_value_get_uint64(pCborValue, pExtractedValue);
    if (CborNoError != cborStatus) {
        WLOGE("extracting unsigned integer %x", msgStatus);
        goto exit;
    }
    WLOGD("extracted uint64 value %x", *pExtractedValue);

    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

/*
 * Extract the CBor byte string, also fetches the next cbor-object if it's
 * available.
 */
WosMsgError_t msgCborParseByteString(const CborValue *pCborValue,
                                     WosBuffer_t **ppExtractedByteString,
                                     CborValue *pNextCborValue)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    size_t length = 0;
    WosBuffer_t *pTempBuffer = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if (NULL == pCborValue) {
        WLOGE("bad parameters");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    pTempBuffer = wosMemAlloc(sizeof(WosBuffer_t));
    if (NULL == pTempBuffer) {
        WLOGF("out of memory buffer allocation");
        msgStatus = WOS_MSG_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    pTempBuffer->data = NULL;
    pTempBuffer->length = 0;
    /* Check the type. */
    if (false == cbor_value_is_byte_string(pCborValue)) {
        WLOGE("badly formatted packet, expecting byte string");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }

    /* Copy byte string to buffer. */
    cborStatus = cbor_value_dup_byte_string(pCborValue, &(pTempBuffer->data),
                                            &length, pNextCborValue);
    pTempBuffer->length = (uint32_t)length;
    if (CborNoError != cborStatus) {
        WLOGE("dup byte string failed %x %s", cborStatus,
              cbor_error_string(cborStatus));
        goto exit;
    }
    WLOGD_BUFFER("extracted byte string", pTempBuffer->data,
                 pTempBuffer->length);

    *ppExtractedByteString = pTempBuffer;
    msgStatus = WOS_MSG_SUCCESS;

exit:
    if (CborNoError != cborStatus) {
        msgStatus = WOS_MSG_ERROR;
    }

    /* If there is failure during parsing, release memory. */
    if (WOS_MSG_SUCCESS != msgStatus) {
        WOS_FREE_BUF_AND_DATA(pTempBuffer);
    }

    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

WosMsgError_t msgCborParseTextString(const CborValue *pCborValue,
                                     WosString_t *pExtractedTextString,
                                     CborValue *pNextCborValue)
{
    WosMsgError_t msgStatus = WOS_MSG_ERROR;
    CborError cborStatus = CborNoError;
    size_t length = 0;
    WosString_t pTempString = NULL;

    FUNCTION_ENTRY();

    /* Input parameters validation. */
    if (pCborValue == NULL) {
        WLOGE("pCborValue NULL");
        msgStatus = WOS_MSG_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Check the type. */
    if (cbor_value_is_text_string(pCborValue) == false) {
        WLOGE("bad format, expecting text string");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exit;
    }

    /* Copy byte string to buffer. */
    cborStatus = cbor_value_dup_text_string(pCborValue, &pTempString, &length,
                                            pNextCborValue);
    if (CborNoError != cborStatus) {
        WLOGE("copy text string failed %x %s", cborStatus,
              cbor_error_string(cborStatus));
        goto exit;
    }

    /* We expect a null-terminated string. */
    if (pTempString[length] != WOS_STRING_NULL_TERM) {
        WLOGE("text string misses null termination");
        msgStatus = WOS_MSG_ERROR_BAD_FORMAT;
        goto exitError;
    }
    WLOGD("extracted text string = %s", pTempString);

    *pExtractedTextString = pTempString;
    msgStatus = WOS_MSG_SUCCESS;
    goto exit;

exitError:
    /* If there is failure during parsing, release memory. */
    wosMemFree(pTempString);

exit:
    FUNCTION_EXIT_RETURN(msgStatus);
    return msgStatus;
}

uint32_t msgCborCalcLengthUInt(uint64_t value)
{
    uint32_t length = 0;
    FUNCTION_ENTRY();

    /* RFC 7049 section 2.1 */
    if (value <= 23) {
        length = 1;
    } else if (value <= 255) {
        length = 2;
    } else if (value <= 65535) {
        length = 3;
    } else if (value <= 4294967295) {
        length = 5;
    } else {
        length = 9;
    }

exit:
    FUNCTION_EXIT_RETURN(length);
    return length;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
