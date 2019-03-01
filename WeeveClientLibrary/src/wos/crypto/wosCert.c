
/**
 * @brief Implementation of Certificate Functions
 *
 * @file wosCert.c
 * @date 2018-09-19
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

#include "wosCert.h"
#include "wosCborCert.h"
#include "wosCommon.h"
#include "wosCrypto.h"
#include "wosLog.h"
#include "wosMemory.h"
#include "wosString.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "CERT"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

typedef struct WosCertChain {
    WosCertType_t *pCert;
    struct WosCertChain *pNext;
} WosCertChain_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

static WosCertError_t lWosCertAddBufferToChain(WosCertChain_t **ppChain,
                                               WosBuffer_t *pCertBuf);

static WosCertError_t
lWosCertAddStructToChain(WosCertChain_t **ppChain,
                         WosCertContainerType_t *pContainer,
                         WosCertTbsType_t *pTbs);

static WosCertError_t lWosCertCheckIssuance(WosCertOptions_t *pOptions,
                                            WosCertType_t *pPrevious,
                                            WosCertType_t *pThis);

static WosCertError_t lWosCertCheckCertParams(WosCertOptions_t *pOptions,
                                              WosCertType_t *pContainer);

static WosCertError_t lWosCertValidateChain(WosCertOptions_t *pOptions,
                                            WosCertChain_t *pChain);

static WosCertError_t
lWosCertGetEndPublicKey(WosCertOptions_t *pOptions,
                        WosCertChain_t *pChain,
                        WosBuffer_t **ppPubKey,
                        WosCryptoEccOptions_t *pEccOptions);

static void lWosCertFreeChain(WosCertChain_t *pChain);

/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

static WosCertError_t lWosCertAddBufferToChain(WosCertChain_t **ppChain,
                                               WosBuffer_t *pCertBuf)
{
    WosCertError_t result = WOS_CERT_ERROR;
    WosMsgError_t packResult = WOS_MSG_ERROR;
    WosCertContainerType_t *pContainerAux;
    WosCertTbsType_t *pTbsAux;

    FUNCTION_ENTRY();

    /* Validate params */
    /* ppChain might be NULL, when called for the first time. */
    if (!WOS_IS_VALID_BUFFER(pCertBuf)) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Unpack Certificate */
    pContainerAux = wosMemAlloc(sizeof(WosCertContainerType_t));
    if (pContainerAux == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosCertContainerType_t));
        result = WOS_CERT_SERIALIZATION_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    packResult = wosCborUnpackCertificateContainer(pCertBuf, pContainerAux);
    if (packResult != WOS_MSG_SUCCESS) {
        WLOGE("wosCborUnpackCertificateContainer failed %d", packResult);
        result = WOS_CERT_SERIALIZATION_ERROR_WOS_MSG_ERROR_BAD_FORMAT;
        goto exitFreeContainer;
    }

    /* Unpack Signed part */
    pTbsAux = wosMemAlloc(sizeof(WosCertTbsType_t));
    if (pTbsAux == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosCertTbsType_t));
        result = WOS_CERT_SERIALIZATION_ERROR_OUT_OF_MEMORY;
        goto exitFreeContainer;
    }
    packResult =
        wosCborUnpackCertificateTbs(pContainerAux->pEncodedTbs, pTbsAux);
    if (packResult != WOS_MSG_SUCCESS) {
        WLOGE("wosCborUnpackCertificateTbs failed %d", packResult);
        result = WOS_CERT_SERIALIZATION_ERROR_WOS_MSG_ERROR_BAD_FORMAT;
        goto exitFreeTbs;
    }

    /* Add both parts to Chain */
    result = lWosCertAddStructToChain(ppChain, pContainerAux, pTbsAux);
    if (result != WOS_CERT_SUCCESS) {
        WLOGE("lWosCertAddStructToChain failed %d", result);
        goto exitFreeTbs;
    }

    /* Skip freeing */
    result = WOS_CERT_SUCCESS;
    goto exit;

exitFreeTbs:
    wosMemFree(pTbsAux);
exitFreeContainer:
    wosMemFree(pContainerAux);
exit:
    FUNCTION_EXIT_RETURN(result);
    return result;
}

static WosCertError_t
lWosCertAddStructToChain(WosCertChain_t **ppChain,
                         WosCertContainerType_t *pContainer,
                         WosCertTbsType_t *pTbs)
{
    WosCertError_t result = WOS_CERT_ERROR;
    WosCertChain_t *pAux;

    FUNCTION_ENTRY();

    /* Validate params */
    /* pChain might be NULL, when called for the first time. */
    if (pContainer == NULL || pTbs == NULL) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    pAux = wosMemAlloc(sizeof(WosCertChain_t));
    if (pAux == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosCertChain_t));
        result = WOS_CERT_SERIALIZATION_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    pAux->pCert = wosMemAlloc(sizeof(WosCertType_t));
    if (pAux->pCert == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosCertType_t));
        result = WOS_CERT_SERIALIZATION_ERROR_OUT_OF_MEMORY;
        goto exitFreeChain;
    }
    pAux->pCert->pContainer = pContainer;
    pAux->pCert->pTbs = pTbs;
    pAux->pNext = *ppChain;
    *ppChain = pAux;

    /* Skip freeing */
    result = WOS_CERT_SUCCESS;
    goto exit;

    /* No need to call: wosMemFree(pAux->pCert); */
exitFreeChain:
    wosMemFree(pAux);
exit:
    FUNCTION_EXIT_RETURN(result);
    return result;
}

static WosCertError_t lWosCertCheckIssuance(WosCertOptions_t *pOptions,
                                            WosCertType_t *pPrevious,
                                            WosCertType_t *pThis)
{
    WosCertError_t result = WOS_CERT_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    int32_t compare;

    FUNCTION_ENTRY();

    /* pPrevious and pThis could be the same! */
    if (pPrevious == NULL || pThis == NULL) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    if (wosStringComparison(pPrevious->pTbs->subjectId,
                            pThis->pTbs->issuerId) != 0) {
        WLOGE("issuer mismatch");
        result = WOS_CERT_CHAIN_ERROR_ISSUER_MISMATCH;
        goto exit;
    }

    if (pPrevious->pTbs->subjectPubKeyInfo ==
        WOS_CERT_OID_PUBKEY_INFO_SECP256R1) {
        eccOptions.curve = WOS_CRYPTO_ECC_CURVE_NIST_P256;
    }
    if (pPrevious->pTbs->signatureAlgorithm ==
        WOS_CERT_OID_SIGNATURE_ALGORITHM_ECDSA_SHA256) {
        eccOptions.hash = WOS_CRYPTO_ECC_HASH_SHA256;
    }

    cryptoResult = wosCryptoEccVerifyKeyBuffer(
        &eccOptions, pPrevious->pTbs->pSubjectPubKey,
        pThis->pContainer->pEncodedTbs, pThis->pContainer->pSignature);
    if (cryptoResult != WOS_CRYPTO_SIGNATURE_MATCH) {
        WLOGE("wosCryptoEccVerifyKeyBuffer failed %d", cryptoResult);
        result = WOS_CERT_CHAIN_ERROR_SIGNATURE_MISMATCH;
        goto exit;
    }

    result = WOS_CERT_CHAIN_VALID;

exit:
    FUNCTION_EXIT_RETURN(result);
    return result;
}

static WosCertError_t lWosCertCheckCertParams(WosCertOptions_t *pOptions,
                                              WosCertType_t *pCert)
{
    WosCertError_t result = WOS_CERT_ERROR;
    FUNCTION_ENTRY();

    if (pCert->pContainer == NULL || pCert->pTbs == NULL) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Fields that are signed and unsigned must match. */
    if (pCert->pTbs->version != pCert->pContainer->version) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    result = WOS_CERT_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(result);
    return result;
}

static WosCertError_t lWosCertValidateChain(WosCertOptions_t *pOptions,
                                            WosCertChain_t *pChain)
{
    /* TODO Start using WosCertOptions_t: trusted CA, revocation, approved
     * algorithms, Length of keys and signatures, ID naming standards, etc */
    WosCertError_t result = WOS_CERT_ERROR;
    WosCertChain_t *pThis, *pPrevious = NULL;
    bool selfSigned = false;
    int i = 0;

    FUNCTION_ENTRY();

    /* Validate params */
    if (pChain == NULL) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Go through chain */
    pThis = pChain;
    while (pThis != NULL) {
        if (pThis->pCert == NULL) {
            WLOGE("bad params for cert %d", i);
            result = WOS_CERT_ERROR_BAD_PARAMS;
            goto exit;
        }

        result = lWosCertCheckCertParams(pOptions, pThis->pCert);
        if (result != WOS_CERT_SUCCESS) {
            WLOGE("lWosCertCheckCertParams failed for cert %d with result %d",
                  i, result);
            goto exit;
        }

        /* Check if this is the first on the chain, i.e., the root CA. */
        if (pPrevious == NULL) {
            /* Set own key for validating signature. */
            pPrevious = pThis;
        }

        result =
            lWosCertCheckIssuance(pOptions, pPrevious->pCert, pThis->pCert);
        if (result != WOS_CERT_CHAIN_VALID) {
            WLOGE("lWosCertCheckIssuance failed for cert %d with result: %d ",
                  i, result);
            goto exit;
        }

        /* Move to next */
        pPrevious = pThis;
        pThis = pThis->pNext;
        ++i;
    }

    result = WOS_CERT_CHAIN_VALID;

exit:
    FUNCTION_EXIT_RETURN(result);
    return result;
}

static WosCertError_t
lWosCertGetEndPublicKey(WosCertOptions_t *pOptions,
                        WosCertChain_t *pChain,
                        WosBuffer_t **ppPubKey,
                        WosCryptoEccOptions_t *pEccOptions)
{
    WosCertError_t result = WOS_CERT_ERROR;

    FUNCTION_ENTRY();

    /* Validate params */
    if (pChain == NULL) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    while (pChain->pNext != NULL) {
        pChain = pChain->pNext;
    }

    *ppPubKey = pChain->pCert->pTbs->pSubjectPubKey;
    result = WOS_CERT_SUCCESS;

    if (pChain->pCert->pTbs->subjectPubKeyInfo ==
        WOS_CERT_OID_PUBKEY_INFO_SECP256R1) {
        pEccOptions->curve = WOS_CRYPTO_ECC_CURVE_NIST_P256;
    }
    if (pChain->pCert->pTbs->signatureAlgorithm ==
        WOS_CERT_OID_SIGNATURE_ALGORITHM_ECDSA_SHA256) {
        pEccOptions->hash = WOS_CRYPTO_ECC_HASH_SHA256;
    }

exit:
    FUNCTION_EXIT_RETURN(result);
    return result;
}

static void lWosCertFreeChain(WosCertChain_t *pChain)
{
    WosCertChain_t *pAux;
    FUNCTION_ENTRY();

    while (pChain != NULL) {
        WLOGI("Freeing one more certificate from the chain.");
        if (pChain->pCert != NULL) {
            wosCborFreeCertificateTbs(pChain->pCert->pTbs);
            wosMemFree(pChain->pCert->pTbs);
            wosCborFreeCertificateContainer(pChain->pCert->pContainer);
            wosMemFree(pChain->pCert->pContainer);
            wosMemFree(pChain->pCert);
        }

        /* Next! */
        pAux = pChain->pNext;
        wosMemFree(pChain);
        pChain = pAux;
    }

    FUNCTION_EXIT();
    return;
}

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

WosCertError_t wosCertValidateData(WosCertOptions_t *pOptions,
                                   WosBuffer_t *pRootBuf,
                                   uint8_t numCerts,
                                   WosBuffer_t **ppCerts,
                                   WosBuffer_t *pSignedData,
                                   WosBuffer_t *pSignature)
{
    WosCertError_t result = WOS_CERT_ERROR;
    WosMsgError_t packResult = WOS_MSG_ERROR;
    WosCryptoError_t cryptoResult = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions;
    uint8_t i = 0;

    WosCertContainerType_t *pContainerAux = NULL;
    WosCertTbsType_t *pTbsAux = NULL;
    WosCertChain_t *pChain = NULL;
    WosBuffer_t *pPubKey = NULL;

    FUNCTION_ENTRY();
    /* Validate params */
    if (pOptions == NULL || !WOS_IS_VALID_BUFFER(pRootBuf) ||
        numCerts > WOS_CERT_CHAIN_LIMIT || ppCerts == NULL ||
        !WOS_IS_VALID_BUFFER(pSignedData) || !WOS_IS_VALID_BUFFER(pSignature)) {
        WLOGE("bad params");
        result = WOS_CERT_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Unpack and Create Chain */
    for (i = 0; i < numCerts; ++i) {
        if (!WOS_IS_VALID_BUFFER(ppCerts[i])) {
            WLOGE("bad params");
            result = WOS_CERT_ERROR_BAD_PARAMS;
            goto exitFreeChain;
        }
        result = lWosCertAddBufferToChain(&pChain, ppCerts[i]);
        if (result != WOS_CERT_SUCCESS) {
            WLOGE("lWosCertAddBufferToChain failed %d", result);
            goto exitFreeChain;
        }
    }

    /* Unpack and Add Root CA to Chain */
    result = lWosCertAddBufferToChain(&pChain, pRootBuf);
    if (result != WOS_CERT_SUCCESS) {
        WLOGE("lWosCertAddBufferToChain root failed %d", result);
        goto exitFreeChain;
    }

    /* Validate Chain */
    result = lWosCertValidateChain(pOptions, pChain);
    if (result != WOS_CERT_CHAIN_VALID) {
        WLOGE("lWosCertValidateChain failed %d", result);
        goto exitFreeChain;
    }

    /* Get Public Key of End User Certificate */
    result = lWosCertGetEndPublicKey(pOptions, pChain, &pPubKey, &eccOptions);
    if (result != WOS_CERT_SUCCESS) {
        WLOGE("lWosCertGetEndPublicKey failed %d", result);
        goto exitFreeChain;
    }

    /* Verify Signed Data with the Public Key */
    cryptoResult = wosCryptoEccVerifyKeyBuffer(&eccOptions, pPubKey,
                                               pSignedData, pSignature);
    if (cryptoResult == WOS_CRYPTO_SIGNATURE_MATCH) {
        result = WOS_CERT_SIGNATURE_MATCH;
    } else {
        WLOGE("wosCryptoEccVerifyKeyBuffer failed");
        result = WOS_CERT_SIGNATURE_INVALID;
    }

exitFreeChain:
    lWosCertFreeChain(pChain);

exit:
    FUNCTION_EXIT_RETURN(result);
    return result;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
