/**
 * @brief Use this interface to build and parse Secure Messaging Protocol
 * messages in a packing format like CBOR etc.
 *
 * @file wosMsgSmp.h
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

#ifndef WOS_MSG_SMP_H
#define WOS_MSG_SMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wclConfig.h"
#include "wclSmp.h"
#include "wosMsgCommon.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/**
 * @brief SMP Header data.
 *
 * Ref. Msg Specification Section 2.1
 */
typedef struct tWosSmpHeader {
    /* Context of message. */
    WosMsgCommonHeader_t commonHeader;
    /* Type of message. */
    WclSmpMessageType_t messageType;
    /* Client-Id. */
    WosString_t clientId;
    /* Message Id. */
    uint32_t messageId;
} WosSmpHeader_t;

/**
 * @brief Weeve MQTTS Session Establishment Message.
 *
 * MessageContext: SMP_MQTTS
 * MessageType: Session Establishment
 */
typedef struct tWosMsgMqttsSeParams {
    /* Serialized SMP Header. */
    WosBuffer_t *pEncodedSmpHeader;
    /* Cipher-scheme identifier. */
    uint8_t cipherSchemeId;
    /* ECC-DH public params. */
    WosBuffer_t *pEccDhPubParams;
    /* Standard mqtt packet. */
    WosBuffer_t *pMqttPacket;
    /* Signature. */
    WosBuffer_t *pSignature;
    /* Number of certificates in chain. */
    uint8_t numCerts;
    /* Certificate list, first certificate is verification certificate. */
    WosBuffer_t **ppCerts;
} WosMsgMqttsSeParams_t;

/**
 * @brief Weeve MQTTS Data Message.
 *
 * MessageContext: SMP_MQTTS
 * MessageType: Data
 *
 */
typedef struct tWosMsgMqttsControlParams {
    /* Serialized SMP Header. */
    WosBuffer_t *pEncodedSmpHeader;
    /* AEAD protected standard mqtt packet. */
    WosBuffer_t *pMqttPacket;
    /* Initialization vector. */
    WosBuffer_t *pIV;
    /* Authentication tag. */
    WosBuffer_t *pAuthTag;
} WosMsgMqttsControlParams_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

/**
 * @brief Pack the header data of a SMP message.
 *
 * @param pSmpHeader[in] SMP message header.
 * @param pPackedBuffer[out] The binary packed serialized buffer containing
 * header.
 *
 */
WosMsgError_t wosMsgPackSmpHeader(const WosSmpHeader_t *pSmpHeader,
                                  WosBuffer_t *pPackedBuffer);

/**
 * @brief Parse the header data of a serialized SMP header.
 *
 * @param pPackedBuffer[in] The binary packed serialized buffer containing
 * header.
 * @param pSmpHeader[out] SMP message header.
 *
 */
WosMsgError_t wosMsgUnpackSmpHeaderFromSmpMsg(const WosBuffer_t *pPackedBuffer,
                                              WosSmpHeader_t *pSmpHeader);

/**
 * @brief Pack the Session Establishment Message.
 *
 * @param pSeParams[in] Session Establishment parameters.
 * @param pPackedBuffer[out] The binary packed serialized buffer.
 *
 */
WosMsgError_t
wosMsgPackSmpMqttsSEMessage(const WosMsgMqttsSeParams_t *pSeParams,
                            WosBuffer_t *pPackedBuffer);
/**
 * @brief Unpack the binary buffer to get Session Establishment Message.
 *
 * @param pPackedBuffer[in] The binary packed serialized buffer.
 * @param pSeParams[out] Session Establishment parameters.
 *
 * Memory is allocated in this api, free pSeParams using
 * wosMsgFreeSmpMqttsSEMessage().
 *
 */
WosMsgError_t wosMsgUnpackSmpMqttsSEMessage(const WosBuffer_t *pPackedBuffer,
                                            WosMsgMqttsSeParams_t *pSeParams);

/**
 * @brief Free the WosMsgMqttsSeParams_t structure.
 *
 */
void wosMsgFreeSmpMqttsSEMessage(WosMsgMqttsSeParams_t *pSeParams);

/**
 * @brief Pack the MQtts Control Message.
 *
 * @param pControlParams[in] Mqtts data parameters.
 * @param pPackedBuffer[out] The binary packed serialized buffer.
 *
 */
WosMsgError_t wosMsgPackSmpMqttsControlMessage(
    const WosMsgMqttsControlParams_t *pControlParams,
    WosBuffer_t *pPackedBuffer);
/**
 * @brief Unpack the binary buffer to get Mqtts Control Message.
 *
 * @param pPackedBuffer[in] The binary packed serialized buffer.
 * @param pControlParams[out] Mqtts data parameters.
 *
 * Memory is allocated in this api, free pSeParams using
 * wosMsgFreeSmpMqttsControlMessage().
 *
 */
WosMsgError_t
wosMsgUnpackSmpMqttsControlMessage(const WosBuffer_t *pPackedBuffer,
                                   WosMsgMqttsControlParams_t *pControlParams);

/**
 * @brief Free the WosMsgMqttsControlParams_t structure.
 *
 */
void wosMsgFreeSmpMqttsControlMessage(
    WosMsgMqttsControlParams_t *pControlParams);

#ifdef __cplusplus
}
#endif

#endif /* WOS_MSG_SMP_H */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
