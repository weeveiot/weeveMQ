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
 * @brief Implementation of Crypto Functions using LibTomCrypt
 *
 * @file wosCryptoLibtom.c
 * @date 2018-07-16
 * 
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosCommon.h"
#include "wosCrypto.h"
#include "wosLog.h"
#include "wosStorage.h"
#include "wosString.h"
#include <stdint.h>
#include <stdlib.h>
#include <tomcrypt.h>

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "CRYPTO_LIBTOM"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

typedef enum { ECC_SIGN = 0, ECC_VERIFY = 1 } WosCryptoEccSignVerify_t;

typedef enum { AE_ENCRYPT = 0, AE_DECRYPT = 1 } WosCryptoAeEncryptDecrypt_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

prng_state fortunaPrngState;

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

/**
 * @brief Auxiliary function for importing a key buffer into libtomcrypt key.
 */
static WosCryptoError_t lWosCryptoEccImportKey(WosCryptoEccOptions_t *pOptions,
                                               WosBuffer_t *pKeyBuf,
                                               ecc_key *pTomEccKey);

/**
 * @brief Auxiliary function for Hashing.
 */
static WosCryptoError_t lWosCryptoHash(WosCryptoEccHash_t hashOptions,
                                       WosBuffer_t *pData,
                                       WosBuffer_t **ppHash);

/**
 * @brief Auxiliary function for code that is common to Signing and Verifying.
 */
static WosCryptoError_t
lWosCryptoEccSignVerify(WosCryptoEccSignVerify_t signVerify,
                        WosCryptoEccOptions_t *pOptions,
                        void *pStorageContext,
                        WosString_t keyStorageId,
                        WosBuffer_t *pData,
                        WosBuffer_t **ppSignature);

/**
 * @brief Auxiliary function for small portions of Signing code.
 */
static WosCryptoError_t lWosCryptoEccSign(WosBuffer_t *pHash,
                                          ecc_key *pTomEccKey,
                                          WosBuffer_t **ppSignature);

/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

static WosCryptoError_t lWosCryptoEccImportKey(WosCryptoEccOptions_t *pOptions,
                                               WosBuffer_t *pKeyBuf,
                                               ecc_key *pTomEccKey)
{
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;

    int tomError = CRYPT_ERROR;

    FUNCTION_ENTRY();

    if (!WOS_IS_VALID_BUFFER(pKeyBuf) || pTomEccKey == NULL) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Re-create key object recognized by libtomcrypt */
    if (pKeyBuf->length == WOS_CRYPTO_ECC_NIST_P256_PUBLIC_KEY_LENGTH) {
        /* Public Key */
        /* NOTE: The expected buffer format is described on ANSI X9.63,
         * sect. 4.3.6; or SEC 1, sect. 2.3.3. It is called uncompressed octet
         * string representation of curve points.
         * It must be formatted as: "0x04 || pub point X || pub point Y". */
        tomError =
            ecc_ansi_x963_import(pKeyBuf->data, pKeyBuf->length, pTomEccKey);
        if ((tomError != CRYPT_OK)) {
            WLOGE("ecc_ansi_x963_import: %d, %s", tomError,
                  error_to_string(tomError));
            ret = WOS_CRYPTO_ERROR;
            goto exitFreeKeyTom;
        }
    } else {
        /* Private Key */
        tomError = ecc_import(pKeyBuf->data, pKeyBuf->length, pTomEccKey);
        if (tomError != CRYPT_OK) {
            WLOGE("ecc_import: %d, %s", tomError, error_to_string(tomError));
            ret = WOS_CRYPTO_ERROR;
            goto exitFreeKeyTom;
        }
    }
    ret = WOS_CRYPTO_SUCCESS;
    goto exit; /* Skip freeing key */

exitFreeKeyTom:
    ecc_free(pTomEccKey);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

static WosCryptoError_t lWosCryptoHash(WosCryptoEccHash_t hashOptions,
                                       WosBuffer_t *pData,
                                       WosBuffer_t **ppHash)
{
    /* TODO Start using WosCryptoEccHash_t: sha256, etc */
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    uint64_t length_aux = 0;
    int tomError = CRYPT_ERROR;
    int hashId = -1;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_BUFFER(pData)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    (*ppHash) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if ((*ppHash) == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    (*ppHash)->data = NULL;
    (*ppHash)->length = 0;
    (*ppHash)->data = (uint8_t *)wosMemAlloc(WOS_CRYPTO_HASH_SHA256_LENGTH);
    if ((*ppHash)->data == NULL) {
        WLOGE("could not allocate data buffer: %lu",
              WOS_CRYPTO_HASH_SHA256_LENGTH);
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeHash;
    }
    (*ppHash)->length = WOS_CRYPTO_HASH_SHA256_LENGTH;

    hashId = find_hash("sha256");
    if (hashId < 0) {
        WLOGE("find_hash error");
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeHashData;
    }
    length_aux = (*ppHash)->length;
    tomError = hash_memory(hashId, pData->data, pData->length, (*ppHash)->data,
                           &length_aux);
    (*ppHash)->length = length_aux; /* uint64_t to uint32_t */
    if (tomError != CRYPT_OK) {
        WLOGE("hash_memory: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeHashData;
    }

    ret = WOS_CRYPTO_SUCCESS;
    goto exit;

exitFreeHashData:
    wosMemFree((*ppHash)->data);
exitFreeHash:
    wosMemFree((*ppHash));
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

static WosCryptoError_t
lWosCryptoEccSignVerify(WosCryptoEccSignVerify_t signVerify,
                        WosCryptoEccOptions_t *pOptions,
                        void *pStorageContext,
                        WosString_t keyStorageId,
                        WosBuffer_t *pData,
                        WosBuffer_t **ppSignature)
{
    /* TODO Start using WosCryptoEccOptions_t: SECP256R1, fortuna_prng, etc */
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    WosBuffer_t *pHash = NULL;

    /* Libtomcrypt */
    int tomError = CRYPT_ERROR;
    int validSignature = 0;
    ecc_key tomEccKey;

    WosBuffer_t *pKeyBuf = NULL;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();
        if (
        !WOS_IS_VALID_STRING(keyStorageId) ) {
        WLOGE("bad string gaurav");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    if ((signVerify != ECC_SIGN && signVerify != ECC_VERIFY) ||
        !WOS_IS_VALID_STRING(keyStorageId) || !WOS_IS_VALID_BUFFER(pData) ||
        (signVerify == ECC_VERIFY && !WOS_IS_VALID_BUFFER(*ppSignature))) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Hash */
    ret = lWosCryptoHash(pOptions->hash, pData, &pHash);
    if (ret != WOS_CRYPTO_SUCCESS) {
        WLOGE("lWosCryptoHash error");
        goto exit;
    }

    /* Read Key */
    storageError = wosStorageRead(pStorageContext, keyStorageId, &pKeyBuf);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error reading key: %d", storageError);
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exitFreeHash;
    }

    /* Read Key and Import it into tomcrypt format */
    ret = lWosCryptoEccImportKey(pOptions, pKeyBuf, &tomEccKey);
    if (ret != WOS_CRYPTO_SUCCESS) {
        WLOGE("lWosCryptoEccImportKey error");
        goto exitFreeKey;
    }

    if (signVerify == ECC_SIGN) {
        /* Sign the hash */
        ret = lWosCryptoEccSign(pHash, &tomEccKey, ppSignature);
        if (ret != WOS_CRYPTO_SUCCESS) {
            WLOGE("error");
            goto exitFreeTomKey;
        }
    } else if (signVerify == ECC_VERIFY) {
        /* Verify the signature */
        tomError = ecc_verify_hash_rfc7518(
            (*ppSignature)->data, (*ppSignature)->length, pHash->data,
            pHash->length, &validSignature, &tomEccKey);
        if (tomError != CRYPT_OK) {
            WLOGE("ecc_verify_hash_rfc7518: %d, %s", tomError,
                  error_to_string(tomError));
            ret = WOS_CRYPTO_ERROR;
            goto exitFreeTomKey;
        }
        if (validSignature != 0) {
            ret = WOS_CRYPTO_SIGNATURE_MATCH;
        } else {
            ret = WOS_CRYPTO_SIGNATURE_ERROR;
        }
    }

exitFreeTomKey:
    ecc_free(&tomEccKey);
exitFreeKey:
    wosMemSet(pKeyBuf->data, 0, pKeyBuf->length);
    WOS_FREE_BUF_AND_DATA(pKeyBuf);
exitFreeHash:
    wosMemFree(pHash->data);
    wosMemFree(pHash);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t lWosCryptoEccSign(WosBuffer_t *pHash,
                                   ecc_key *pTomEccKey,
                                   WosBuffer_t **ppSignature)
{
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    int tomError = CRYPT_ERROR;
    int prngId = -1;
    uint64_t length_aux = 0;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_BUFFER(pHash) || pTomEccKey == NULL) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    (*ppSignature) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if ((*ppSignature) == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exit;
    }
    (*ppSignature)->data = NULL;
    (*ppSignature)->length = 0;
    (*ppSignature)->data =
        (uint8_t *)wosMemAlloc(WOS_CRYPTO_ECC_NIST_P256_SIGNATURE_LENGTH);
    if ((*ppSignature)->data == NULL) {
        WLOGE("could not allocate data buffer: %lu",
              WOS_CRYPTO_ECC_NIST_P256_SIGNATURE_LENGTH);
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeSignature;
    }
    (*ppSignature)->length = WOS_CRYPTO_ECC_NIST_P256_SIGNATURE_LENGTH;

    prngId = find_prng("fortuna");
    if (prngId < 0) {
        WLOGE("find_prng error");
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }
    length_aux = (*ppSignature)->length;
    tomError = ecc_sign_hash_rfc7518(pHash->data, pHash->length,
                                     (*ppSignature)->data, &length_aux,
                                     &fortunaPrngState, prngId, pTomEccKey);
    (*ppSignature)->length = length_aux; /* uint64_t to uint32_t */
    if (tomError != CRYPT_OK) {
        WLOGE("ecc_sign_hash_rfc7518: %d, %s", tomError,
              error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeSignatureData;
    }

    ret = WOS_CRYPTO_SUCCESS;
    goto exit;

exitFreeSignatureData:
    wosMemFree((*ppSignature)->data);
exitFreeSignature:
    wosMemFree(*ppSignature);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

WosCryptoError_t wosCryptoInitialize(WosCryptoConfig_t *pConfig)
{
    /* TODO Start using WosCryptoConfig_t: aes, sha256, fortuna, etc */
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    int tomError = CRYPT_ERROR;
    int prngId = -1;

    FUNCTION_ENTRY();
    /* HASH */
    tomError = register_hash(&sha256_desc);
    if (tomError < 0) {
        WLOGE("register_hash: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }

    /* CYPHER */
    tomError = register_cipher(&aes_desc);
    if (tomError < 0) {
        WLOGE("register_cipher: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }

    /* Setup Math Library */
    /* void */ init_LTM(); /* LibTomMath */

    /* PRNG */
    prngId = register_prng(&fortuna_desc);
    if (prngId < 0) {
        WLOGE("register_prng error");
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }

    /* Setup PRNG */
    tomError = rng_make_prng(WOS_CRYPTO_PRNG_INITIAL_ENTROPY,
                             find_prng("fortuna"), &fortunaPrngState, NULL);
    if (tomError != CRYPT_OK) {
        WLOGE("rng_make_prng: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }

    ret = WOS_CRYPTO_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoTerminate()
{
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    int tomError = CRYPT_ERROR;
    FUNCTION_ENTRY();

    tomError = fortuna_done(&fortunaPrngState);
    if (tomError != CRYPT_OK) {
        WLOGE("fortuna_done: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }

    ret = WOS_CRYPTO_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoEccGenerateKey(WosCryptoEccOptions_t *pOptions,
                                         void *pStorageContext,
                                         WosString_t privateKeyStorageId,
                                         WosString_t publicKeyStorageId)
{
    /* TODO Start using WosCryptoEccOptions_t: SECP256R1, fortuna_prng, etc */
    // TODO This thing is long, convoluted, etc. Split into parts urgently.
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    WosBuffer_t *pPrivateKey = NULL, *pPublicKey = NULL;

    /* Libtomcrypt */
    int tomError = CRYPT_ERROR;
    int prngId = -1;
    ecc_key tomEccKeyPair;
    uint64_t length_aux = 0;

    /* Storage */
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_STRING(privateKeyStorageId) ||
        !WOS_IS_VALID_STRING(publicKeyStorageId)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Generate */
    prngId = find_prng("fortuna");
    if (prngId < 0) {
        WLOGE("find_prng error");
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }
    tomError = ecc_make_key(&fortunaPrngState, prngId, 32, &tomEccKeyPair);
    if (tomError != CRYPT_OK) {
        WLOGE("ecc_make_key: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeKeyTom;
    }

    /* Private Part */
    pPrivateKey = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if (pPrivateKey == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeKeyTom;
    }
    pPrivateKey->data = NULL;
    pPrivateKey->length = 0;
    pPrivateKey->data =
        (uint8_t *)wosMemAlloc(WOS_CRYPTO_ECC_NIST_P256_KEY_LENGTH);
    if (pPrivateKey->data == NULL) {
        WLOGE("could not allocate data buffer: %lu",
              WOS_CRYPTO_ECC_NIST_P256_KEY_LENGTH);
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeKeyPrivate;
    }
    pPrivateKey->length = WOS_CRYPTO_ECC_NIST_P256_KEY_LENGTH;
    length_aux = pPrivateKey->length;
    tomError =
        ecc_export(pPrivateKey->data, &length_aux, PK_PRIVATE, &tomEccKeyPair);
    pPrivateKey->length = length_aux; /* uint64_t to uint32_t */
    if ((tomError != CRYPT_OK)) {
        WLOGE("ecc_export: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeKeyPrivateData;
    }

    /* Public Part */
    pPublicKey = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if (pPublicKey == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeKeyPrivateData;
    }
    pPublicKey->data = NULL;
    pPublicKey->length = 0;
    pPublicKey->data =
        (uint8_t *)wosMemAlloc(WOS_CRYPTO_ECC_NIST_P256_KEY_LENGTH);
    if (pPublicKey->data == NULL) {
        WLOGE("could not allocate data buffer: %lu",
              WOS_CRYPTO_ECC_NIST_P256_KEY_LENGTH);
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeKeyPublic;
    }
    pPublicKey->length = WOS_CRYPTO_ECC_NIST_P256_KEY_LENGTH;
    length_aux = pPublicKey->length;
    tomError =
        ecc_ansi_x963_export(&tomEccKeyPair, pPublicKey->data, &length_aux);
    pPublicKey->length = length_aux; /* uint64_t to uint32_t */
    if ((tomError != CRYPT_OK)) {
        WLOGE("ecc_ansi_x963_export: %d, %s", tomError,
              error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeKeyPublicData;
    }

    /* Save both private and public keys to Storage */
    storageError =
        wosStorageWrite(pStorageContext, privateKeyStorageId, pPrivateKey);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error writing private key");
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exitFreeKeyPublicData;
    }
    storageError =
        wosStorageWrite(pStorageContext, publicKeyStorageId, pPublicKey);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error writing public key");
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exitFreeKeyPublicData;
    }

    ret = WOS_CRYPTO_SUCCESS;

exitFreeKeyPublicData:
    wosMemSet(pPublicKey->data, 0, pPublicKey->length);
    wosMemFree(pPublicKey->data);
exitFreeKeyPublic:
    wosMemFree(pPublicKey);
exitFreeKeyPrivateData:
    wosMemSet(pPrivateKey->data, 0, pPrivateKey->length);
    wosMemFree(pPrivateKey->data);
exitFreeKeyPrivate:
    wosMemFree(pPrivateKey);
exitFreeKeyTom:
    ecc_free(&tomEccKeyPair);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoEccReadPubKey(WosCryptoEccOptions_t *pOptions,
                                        void *pStorageContext,
                                        WosString_t publicKeyStorageId,
                                        WosBuffer_t **ppExportedPubKey)
{
    // TODO Start using WosCryptoEccOptions_t to produce different results.
    /* Nothing really related to crypto so far... But we will have crypto stuff
    under GlobalPlatform */
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;

    /* Storage */
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_STRING(publicKeyStorageId)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    storageError =
        wosStorageRead(pStorageContext, publicKeyStorageId, ppExportedPubKey);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error reading public key: %d", storageError);
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exit;
    }

    ret = WOS_CRYPTO_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoEccSign(WosCryptoEccOptions_t *pOptions,
                                  void *pStorageContext,
                                  WosString_t keyStorageId,
                                  WosBuffer_t *pData,
                                  WosBuffer_t **ppSignature)
{
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    FUNCTION_ENTRY();
    ret = lWosCryptoEccSignVerify(ECC_SIGN, pOptions, pStorageContext,
                                  keyStorageId, pData, ppSignature);
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoEccSignKeyBuffer(WosCryptoEccOptions_t *pOptions,
                                           WosBuffer_t *pKeyBuf,
                                           WosBuffer_t *pData,
                                           WosBuffer_t **ppSignature)
{
    /* TODO Start using WosCryptoEccOptions_t: SECP256R1, fortuna_prng, etc */
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    WosBuffer_t *pHash = NULL;

    /* Libtomcrypt */
    int tomError = CRYPT_ERROR;
    int validSignature = 0;
    ecc_key tomEccKey;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_BUFFER(pKeyBuf) || !WOS_IS_VALID_BUFFER(pData)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Hash */
    ret = lWosCryptoHash(pOptions->hash, pData, &pHash);
    if (ret != WOS_CRYPTO_SUCCESS) {
        WLOGE("lWosCryptoHash error");
        goto exit;
    }

    /* Import key into tomcrypt format */
    ret = lWosCryptoEccImportKey(pOptions, pKeyBuf, &tomEccKey);
    if (ret != WOS_CRYPTO_SUCCESS) {
        WLOGE("lWosCryptoEccImportKey error");
        goto exitFreeHash;
    }

    /* Sign the hash */
    ret = lWosCryptoEccSign(pHash, &tomEccKey, ppSignature);
    if (ret != WOS_CRYPTO_SUCCESS) {
        WLOGE("error");
        goto exitFreeTomKey;
    }

exitFreeTomKey:
    ecc_free(&tomEccKey);
exitFreeHash:
    wosMemFree(pHash->data);
    wosMemFree(pHash);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoEccVerify(WosCryptoEccOptions_t *pOptions,
                                    void *pStorageContext,
                                    WosString_t keyStorageId,
                                    WosBuffer_t *pData,
                                    WosBuffer_t *pSignature)
{
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    FUNCTION_ENTRY();
    ret = lWosCryptoEccSignVerify(ECC_VERIFY, pOptions, pStorageContext,
                                  keyStorageId, pData, &pSignature);
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoEccVerifyKeyBuffer(WosCryptoEccOptions_t *pOptions,
                                             WosBuffer_t *pKeyBuf,
                                             WosBuffer_t *pData,
                                             WosBuffer_t *pSignature)
{
    /* TODO Start using WosCryptoEccOptions_t: SECP256R1, etc */
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    WosBuffer_t *pHash = NULL;

    /* Libtomcrypt */
    int tomError = CRYPT_ERROR;
    int validSignature = 0;
    ecc_key tomEccKey;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_BUFFER(pKeyBuf) || !WOS_IS_VALID_BUFFER(pData) ||
        !WOS_IS_VALID_BUFFER(pSignature)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Hash */
    ret = lWosCryptoHash(pOptions->hash, pData, &pHash);
    if (ret != WOS_CRYPTO_SUCCESS) {
        WLOGE("lWosCryptoHash error");
        goto exit;
    }

    /* Import key into tomcrypt format */
    ret = lWosCryptoEccImportKey(pOptions, pKeyBuf, &tomEccKey);
    if (ret != WOS_CRYPTO_SUCCESS) {
        WLOGE("lWosCryptoEccImportKey error");
        goto exitFreeHash;
    }

    /* Verify the signature */
    tomError = ecc_verify_hash_rfc7518(pSignature->data, pSignature->length,
                                       pHash->data, pHash->length,
                                       &validSignature, &tomEccKey);
    if (tomError != CRYPT_OK) {
        WLOGE("ecc_verify_hash_rfc7518: %d, %s", tomError,
              error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeTomKey;
    }
    if (validSignature != 0) {
        ret = WOS_CRYPTO_SIGNATURE_MATCH;
    } else {
        ret = WOS_CRYPTO_SIGNATURE_ERROR;
    }

exitFreeTomKey:
    ecc_free(&tomEccKey);
exitFreeHash:
    wosMemFree(pHash->data);
    wosMemFree(pHash);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoAeEncrypt(WosCryptoAeOptions_t *pOptions,
                                    void *pStorageContext,
                                    WosString_t symKeyStorageId,
                                    WosBuffer_t *pPlainText,
                                    WosBuffer_t *pAad,
                                    WosBuffer_t **ppIv,
                                    WosBuffer_t **ppCipherText,
                                    WosBuffer_t **ppTag)
{
    // TODO Start using WosCryptoAeOptions_t to produce different results.
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    WosBuffer_t *pSecretKey = NULL;
    uint8_t ivSupplied = 0;
    WosBuffer_t emptyBuffer = {.data = NULL, .length = 0};

    /* Libtomcrypt */
    int tomError = CRYPT_ERROR;
    int randomLength = -1;
    int cipherId = -1;
    uint64_t length_aux = 0;

    /* Storage */
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_STRING(symKeyStorageId)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }
    if (pPlainText == NULL) {
        pPlainText = &emptyBuffer;
    }
    if (pAad == NULL) {
        pAad = &emptyBuffer;
    }
    if ((pAad->length != 0 && pAad->data == NULL) ||
        (pPlainText->length != 0 && pPlainText->data == NULL)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Read Key */
    storageError =
        wosStorageRead(pStorageContext, symKeyStorageId, &pSecretKey);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error reading secret key: %d", storageError);
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exit;
    }
    if (pSecretKey->length != WOS_CRYPTO_AE_AES256_KEY_LENGTH) {
        WLOGE("secret key has wrong length");
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exitFreeKeySecret;
    }

    /* Allocate memory space && Generate IV */
    (*ppCipherText) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if ((*ppCipherText) == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeKeySecret;
    }
    (*ppCipherText)->data = NULL;
    (*ppCipherText)->length = 0;
    if (pPlainText->length != 0) {
        (*ppCipherText)->data = (uint8_t *)wosMemAlloc(pPlainText->length);
        if ((*ppCipherText)->data == NULL) {
            WLOGE("could not allocate data buffer: %lu", pPlainText->length);
            ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
            goto exitFreeCipher;
        }
    }
    (*ppCipherText)->length = pPlainText->length;

    if (*ppIv == NULL) {
        ivSupplied = 0;
        (*ppIv) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
        if ((*ppIv) == NULL) {
            WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
            ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
            goto exitFreeCipherData;
        }
        (*ppIv)->data = NULL;
        (*ppIv)->length = 0;
        (*ppIv)->data = (uint8_t *)wosMemAlloc(WOS_CRYPTO_AE_AES_GCM_IV_LENGTH);
        if ((*ppIv)->data == NULL) {
            WLOGE("could not allocate data buffer: %lu",
                  WOS_CRYPTO_AE_AES_GCM_IV_LENGTH);
            ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
            goto exitFreeIv;
        }
        (*ppIv)->length = WOS_CRYPTO_AE_AES_GCM_IV_LENGTH;

        ret = wosCryptoGetRandomBytes(*ppIv);
        if (ret != WOS_CRYPTO_SUCCESS) {
            WLOGE("wosCryptoGetRandomBytes error");
            goto exitFreeIvData;
        }
    } else {
        ivSupplied = 1;
        WLOGD("Iv already provided with length: %lu", (*ppIv)->length);
    }

    (*ppTag) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if ((*ppTag) == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeIvData;
    }
    (*ppTag)->data = NULL;
    (*ppTag)->length = 0;
    (*ppTag)->data = (uint8_t *)wosMemAlloc(WOS_CRYPTO_AE_AES_BLOCK_LENGTH);
    if ((*ppTag)->data == NULL) {
        WLOGE("could not allocate data buffer: %lu",
              WOS_CRYPTO_AE_AES_BLOCK_LENGTH);
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeTag;
    }
    (*ppTag)->length = WOS_CRYPTO_AE_AES_BLOCK_LENGTH;

    /* Encrypt */
    cipherId = find_cipher("aes");
    if (cipherId < 0) {
        WLOGE("find_cipher error");
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeTagData;
    }
    length_aux = (*ppTag)->length;
    tomError = gcm_memory(cipherId,                             /* cipher */
                          pSecretKey->data, pSecretKey->length, /* key */
                          (*ppIv)->data, (*ppIv)->length,       /* iv */
                          pAad->data, pAad->length, /* additional data */
                          pPlainText->data, pPlainText->length, /* plain text */
                          (*ppCipherText)->data,       /* cipher text */
                          (*ppTag)->data, &length_aux, /* authentication tag */
                          GCM_ENCRYPT);
    (*ppTag)->length = length_aux; /* uint64_t to uint32_t */
    if (tomError != CRYPT_OK) {
        WLOGE("gcm_memory: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeTagData;
    }

    ret = WOS_CRYPTO_SUCCESS;
    goto exitFreeKeySecret; /* skip freeing the good work we just did. */

exitFreeTagData:
    wosMemFree((*ppTag)->data);
exitFreeTag:
    wosMemFree(*ppTag);
exitFreeIvData:
    if (ivSupplied == 0) {
        wosMemFree((*ppIv)->data);
    }
exitFreeIv:
    if (ivSupplied == 0) {
        wosMemFree(*ppIv);
    }
exitFreeCipherData:
    if (pPlainText->length != 0) {
        wosMemFree((*ppCipherText)->data);
    }
exitFreeCipher:
    wosMemFree(*ppCipherText);
exitFreeKeySecret:
    wosMemSet(pSecretKey->data, 0, pSecretKey->length);
    wosMemFree(pSecretKey->data);
    wosMemFree(pSecretKey);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoAeDecrypt(WosCryptoAeOptions_t *pOptions,
                                    void *pStorageContext,
                                    WosString_t symKeyStorageId,
                                    WosBuffer_t *pCipherText,
                                    WosBuffer_t *pAad,
                                    WosBuffer_t *pIv,
                                    WosBuffer_t *pTag,
                                    WosBuffer_t **ppPlainText)
{
    /* TODO Start using WosCryptoAeOptions_t: key_length, etc */
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    WosBuffer_t *pSecretKey = NULL;
    WosBuffer_t emptyBuffer = {.data = NULL, .length = 0};

    /* Libtomcrypt */
    int tomError = CRYPT_ERROR;
    int cipherId = -1;
    uint64_t length_aux = 0;

    /* Tag comparison */
    uint8_t tagData[WOS_CRYPTO_AE_AES_BLOCK_LENGTH];
    WosBuffer_t tagAux = {.data = tagData,
                          .length = WOS_CRYPTO_AE_AES_BLOCK_LENGTH};
    uint8_t compareTags = 1;

    /* Storage */
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_STRING(symKeyStorageId) || !WOS_IS_VALID_BUFFER(pIv) ||
        !WOS_IS_VALID_BUFFER(pTag)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }
    if (pCipherText == NULL) {
        pCipherText = &emptyBuffer;
    }
    if (pAad == NULL) {
        pAad = &emptyBuffer;
    }
    if ((pAad->length != 0 && pAad->data == NULL) ||
        (pCipherText->length != 0 && pCipherText->data == NULL)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Read Key */
    storageError =
        wosStorageRead(pStorageContext, symKeyStorageId, &pSecretKey);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error reading secret key: %d", storageError);
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exit;
    }
    if (pSecretKey->length != WOS_CRYPTO_AE_AES256_KEY_LENGTH) {
        WLOGE("secret key has wrong length");
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exitFreeKeySecret;
    }

    /* Allocate Plain Text */
    (*ppPlainText) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if ((*ppPlainText) == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeKeySecret;
    }
    (*ppPlainText)->data = NULL;
    (*ppPlainText)->length = 0;
    if (pCipherText->length != 0) {
        (*ppPlainText)->data = (uint8_t *)wosMemAlloc(pCipherText->length);
        if ((*ppPlainText)->data == NULL) {
            WLOGE("could not allocate data buffer: %lu", pCipherText->length);
            ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
            goto exitFreePlain;
        }
    }
    (*ppPlainText)->length = pCipherText->length;

    /* Decrypt */
    cipherId = find_cipher("aes");
    if (cipherId < 0) {
        WLOGE("find_cipher error");
        ret = WOS_CRYPTO_ERROR;
        goto exitFreePlainData;
    }
    length_aux = tagAux.length;
    tomError = gcm_memory(cipherId,                             /* cipher */
                          pSecretKey->data, pSecretKey->length, /* key */
                          pIv->data, pIv->length,               /* iv */
                          pAad->data, pAad->length, /* additional data */
                          (*ppPlainText)->data,
                          (*ppPlainText)->length,   /* plain text */
                          pCipherText->data,        /* cipher text */
                          tagAux.data, &length_aux, /* authentication tag */
                          GCM_DECRYPT);
    if (tomError != CRYPT_OK) {
        WLOGE("gcm_memory: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreePlainData;
    }
    tagAux.length = length_aux; /* uint64_t to uint32_t */

    if (tagAux.length == pTag->length) {
        /* Libtomcrypt v1.8.2 release version lacked this memory comparison. */
        /* Remove it when a new version is launched. */
        compareTags =
            wosMemComparisonConstTime(pTag->data, tagAux.data, tagAux.length);
        if (compareTags == 0) {
            /* skip freeing the good work we just did. */
            ret = WOS_CRYPTO_SUCCESS;
            goto exitFreeKeySecret;
        }
    }
    /* Error */
    ret = WOS_CRYPTO_ERROR;
    goto exitFreePlainData;

exitFreePlainData:
    if (pCipherText->length != 0) {
        wosMemFree((*ppPlainText)->data);
    }
exitFreePlain:
    wosMemFree(*ppPlainText);
exitFreeKeySecret:
    wosMemSet(pSecretKey->data, 0, pSecretKey->length);
    wosMemFree(pSecretKey->data);
    wosMemFree(pSecretKey);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoDeriveSymKey(WosCryptoEccOptions_t *pOptions,
                                       void *pStorageContext,
                                       WosBuffer_t *pPublicKey,
                                       WosString_t privateKeyStorageId,
                                       WosString_t symmetricKeyStorageId)
{
    // TODO This thing is long, convoluted, etc. Split into parts urgently.
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    WosBuffer_t *pPrivateKey = NULL, *pSharedSecret = NULL;

    /* Libtomcrypt */
    int tomError = CRYPT_ERROR;
    ecc_key tomOtherPublicKey, tomDevicePrivateKey;
    uint64_t length_aux = 0;

    /* Storage */
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_BUFFER(pPublicKey) ||
        !WOS_IS_VALID_STRING(privateKeyStorageId) ||
        !WOS_IS_VALID_STRING(symmetricKeyStorageId)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Read device's private key */
    storageError =
        wosStorageRead(pStorageContext, privateKeyStorageId, &pPrivateKey);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error reading private key: %d", storageError);
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exit;
    }

    /* Convert the device's private key to libtomcrypt object */
    tomError = ecc_import(pPrivateKey->data, pPrivateKey->length,
                          &tomDevicePrivateKey);
    if ((tomError != CRYPT_OK)) {
        WLOGE("ecc_import: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeKeyPrivate;
    }

    /* NOTE: Convert the other party's public key to libtomcrypt object. The
     * expected buffer format is described on ANSI X9.63, sect. 4.3.6; or SEC 1,
     * sect. 2.3.3. It is called uncompressed octet string representation of
     * curve points. It must be formatted as: "0x04 || X || Y". */
    tomError = ecc_ansi_x963_import(pPublicKey->data, pPublicKey->length,
                                    &tomOtherPublicKey);
    if ((tomError != CRYPT_OK)) {
        WLOGE("ecc_ansi_x963_import: %d, %s", tomError,
              error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeKeyTomPrivate;
    }

    /* Derive shared secret */
    pSharedSecret = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if (pSharedSecret == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeKeyTomPublic;
    }
    pSharedSecret->data = NULL;
    pSharedSecret->length = 0;
    pSharedSecret->data =
        (uint8_t *)wosMemAlloc(WOS_CRYPTO_ECC_NIST_P256_SHARED_SECRET_LENGTH);
    if (pSharedSecret->data == NULL) {
        WLOGE("could not allocate data buffer: %lu",
              WOS_CRYPTO_ECC_NIST_P256_SHARED_SECRET_LENGTH);
        ret = WOS_CRYPTO_ERROR_OUT_OF_MEMORY;
        goto exitFreeShared;
    }
    pSharedSecret->length = WOS_CRYPTO_ECC_NIST_P256_SHARED_SECRET_LENGTH;

    length_aux = pSharedSecret->length;
    tomError = ecc_shared_secret(&tomDevicePrivateKey, &tomOtherPublicKey,
                                 pSharedSecret->data, &length_aux);
    pSharedSecret->length = length_aux;
    if ((tomError != CRYPT_OK)) {
        WLOGE("ecc_shared_secret: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exitFreeSharedData;
    }

    /* Save to storage */
    storageError =
        wosStorageWrite(pStorageContext, symmetricKeyStorageId, pSharedSecret);
    if (storageError != WOS_STORAGE_SUCCESS) {
        WLOGE("storage error writing shared secret");
        ret = WOS_CRYPTO_ERROR_STORAGE;
        goto exitFreeSharedData;
    }

    ret = WOS_CRYPTO_SUCCESS;

exitFreeSharedData:
    wosMemSet(pSharedSecret->data, 0, pSharedSecret->length);
    wosMemFree(pSharedSecret->data);
exitFreeShared:
    wosMemFree(pSharedSecret);
exitFreeKeyTomPublic:
    ecc_free(&tomOtherPublicKey);
exitFreeKeyTomPrivate:
    ecc_free(&tomDevicePrivateKey);
exitFreeKeyPrivate:
    wosMemSet(pPrivateKey->data, 0, pPrivateKey->length);
    WOS_FREE_BUF_AND_DATA(pPrivateKey);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosCryptoError_t wosCryptoGetRandomBytes(WosBuffer_t *pBuffer)
{
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    int tomError = CRYPT_ERROR;
    int randomLength = -1;

    FUNCTION_ENTRY();

    if (!WOS_IS_VALID_BUFFER(pBuffer)) {
        WLOGE("bad params");
        ret = WOS_CRYPTO_ERROR_BAD_PARAMS;
        goto exit;
    }

    tomError = fortuna_ready(&fortunaPrngState);
    if (tomError != CRYPT_OK) {
        WLOGE("fortuna_ready: %d, %s", tomError, error_to_string(tomError));
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }

    randomLength =
        fortuna_read(pBuffer->data, pBuffer->length, &fortunaPrngState);
    if (randomLength != pBuffer->length) {
        WLOGE("fortuna_read byte length: %s", randomLength);
        ret = WOS_CRYPTO_ERROR;
        goto exit;
    }
    ret = WOS_CRYPTO_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
