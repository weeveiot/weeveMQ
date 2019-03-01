/**
 * @brief Interface to Cryptographic Functions
 *
 * @file wosCrypto.h
 * @date 2018-07-16
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

#ifndef WOS_CRYPTO_H
#define WOS_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosTypes.h"
#include <stddef.h>

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define WOS_CRYPTO_PRNG_INITIAL_ENTROPY 128 /* 128 bits */
#define WOS_CRYPTO_ECC_NIST_P256_KEY_LENGTH 256
#define WOS_CRYPTO_ECC_NIST_P256_SIGNATURE_LENGTH 64
#define WOS_CRYPTO_ECC_NIST_P256_PUBLIC_KEY_LENGTH 65 /* 1 + (32 * 2) */
#define WOS_CRYPTO_ECC_NIST_P256_SHARED_SECRET_LENGTH 32
#define WOS_CRYPTO_HASH_SHA256_LENGTH 32 /* 256 bits */
#define WOS_CRYPTO_AE_AES256_KEY_LENGTH 32
#define WOS_CRYPTO_AE_AES_BLOCK_LENGTH 16  /* AES Block is always 128b */
#define WOS_CRYPTO_AE_AES_GCM_IV_LENGTH 12 /* AE IV is always 96b */

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

typedef enum {
    WOS_CRYPTO_SUCCESS = 0,
    WOS_CRYPTO_ERROR = 1,         /* Generic errors */
    WOS_CRYPTO_ERROR_BAD_PARAMS = 2,    /* Invalid parameters */
    WOS_CRYPTO_ERROR_STORAGE = 3, /* Error using Storage */
    WOS_CRYPTO_ERROR_OUT_OF_MEMORY = 4, /* Error using Storage */
    /* Signing */
    WOS_CRYPTO_SIGNATURE_MATCH = 100,   /* Signature matches. */
    WOS_CRYPTO_SIGNATURE_ERROR = 101, /* Signature doesn't match. */
} WosCryptoError_t;

typedef struct WosCryptoConfig {

} WosCryptoConfig_t;

/********** Elliptic Curve Cryptography **********/
typedef enum {
    WOS_CRYPTO_ECC_CURVE_NIST_P256 = 0 /*  */
} WosCryptoEccCurve_t;

typedef enum {
    WOS_CRYPTO_ECC_HASH_SHA256 = 0 /*  */
} WosCryptoEccHash_t;

typedef struct WosCryptoEccOptions {
    WosCryptoEccCurve_t curve;
    WosCryptoEccHash_t hash;
} WosCryptoEccOptions_t;

/********** Authenticated Encryption **********/
typedef enum {
    WOS_CRYPTO_AE_ALGORITHM_AES = 0 /*  */
} WosCryptoAeOptionsAlgorithm_t;

typedef enum {
    WOS_CRYPTO_AE_MODE_GCM = 0 /*  */
} WosCryptoAeOptionsMode_t;

typedef struct WosCryptoAeOptions {
    WosCryptoAeOptionsAlgorithm_t algorithm;
    WosCryptoAeOptionsMode_t mode;
} WosCryptoAeOptions_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

/**
 * @brief Initializes the Cryptographic Provider.
 *
 * @param[in] pConfig The configuration preferences for the Cryptographic
 * Provider.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoInitialize(WosCryptoConfig_t *pConfig);

/**
 * @brief Terminates the context of using the Cryptographic Provider.
 *
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoTerminate();

/**
 * @brief Generates a key pair for using elliptic curve cryptographic. Then the
 * public and private parts are split and stored with the provided storage
 * identifiers.
 *
 * @param[in] pOptions The options for generating the ECC key.
 * @param[in] pStorageContext The storage context used to store the generated
 *                            ECC key parts.
 * @param[in] privateKeyStorageId The storage identifier used to store the
 * ECC private key.
 * @param[in] publicKeyStorageId The storage identifier used to store the
 * ECC public key.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoEccGenerateKey(WosCryptoEccOptions_t *pOptions,
                                         void *pStorageContext,
                                         WosString_t privateKeyStorageId,
                                         WosString_t publicKeyStorageId);

/**
 * @brief Generates an ECC signature data using a key that is already stored,
 * returning its signature.
 *
 * @param[in] pOptions The options for the ECC used.
 * @param[in] pStorageContext The storage context used to read the ECC key.
 * @param[in] keyStorageId The storage identifier of the key used to sign.
 * @param[in] pData The data to be signed.
 * @param[out] ppSignature The generated signature for the supplied data. It
 *                         must be freed by the caller after usage.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoEccSign(WosCryptoEccOptions_t *pOptions,
                                  void *pStorageContext,
                                  WosString_t keyStorageId,
                                  WosBuffer_t *pData,
                                  WosBuffer_t **ppSignature);


WosCryptoError_t wosCryptoEccSignKeyBuffer(WosCryptoEccOptions_t *pOptions,
                                           WosBuffer_t *pKeyBuf,
                                           WosBuffer_t *pData,
                                           WosBuffer_t **ppSignature);

/**
 * @brief Verifies an ECC signature.
 *
 * @param[in] pOptions The options for the ECC used.
 * @param[in] pStorageContext The storage context used to read the ECC key.
 * @param[in] keyStorageId The storage identifier of the key used to verify the
 *                         signature.
 * @param[in] pData The data that has been signed.
 * @param[in] pSignature The signature that has to be verified.
 * @return WosCryptoError_t #WOS_CRYPTO_SIGNATURE_MATCH in case of success.
 */
WosCryptoError_t wosCryptoEccVerify(WosCryptoEccOptions_t *pOptions,
                                    void *pStorageContext,
                                    WosString_t keyStorageId,
                                    WosBuffer_t *pData,
                                    WosBuffer_t *pSignature);

/**
 * @brief Verifies an ECC signature.
 *
 * @param[in] pOptions The options for the ECC used.
 * @param[in] pKeyBuf The key used for generating the signature.
 * @param[in] pData The data that has been signed.
 * @param[in] pSignature The signature that has to be verified.
 * @return WosCryptoError_t #WOS_CRYPTO_SIGNATURE_MATCH in case of success.
 */
WosCryptoError_t wosCryptoEccVerifyKeyBuffer(WosCryptoEccOptions_t *pOptions,
                                             WosBuffer_t *pKeyBuf,
                                             WosBuffer_t *pData,
                                             WosBuffer_t *pSignature);

/**
 * @brief Encrypts data using Symmetric Authenticated Encryption. The key used
 * has to be already stored.
 *
 * @param[in] pOptions The options used during the process.
 * @param[in] pStorageContext The storage context used to read the symmetric
 *                            key.
 * @param[in] symKeyStorageId The storage identifier of the key used to encrypt
 *                            data.
 * @param[in] pPlainText The plain data to be encrypted.
 * @param[in] pAad The (optional) additional authenticated data that is not
 *                 encrypted, but authenticated with the plain data.
 * @param[inout] ppIv The initialization vector used during encryption. If one
 *                    is not provided by the caller, it is randomly generated.
 *                    An unique IV should be used for every new pair key-data.
 * @param[out] ppCipherText The generated encrypted data. It must be freed by
 *                          the caller after usage.
 * @param[out] ppTag The generated authentication tag. It must be freed by the
 *                   caller after usage.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoAeEncrypt(WosCryptoAeOptions_t *pOptions,
                                    void *pStorageContext,
                                    WosString_t symKeyStorageId,
                                    WosBuffer_t *pPlainText,
                                    WosBuffer_t *pAad,
                                    WosBuffer_t **ppIv,
                                    WosBuffer_t **ppCipherText,
                                    WosBuffer_t **ppTag);

/**
 * @brief Decrypts data using Symmetric Authenticated Encryption. The key used
 * has to be already stored.
 *
 * @param[in] pOptions The options used during the process.
 * @param[in] pStorageContext The storage context used to read the symmetric
 *                            key.
 * @param[in] symKeyStorageId The storage identifier of the key used to encrypt
 *                            data.
 * @param[in] pCipherText The encrypted data.
 * @param[in] pAad The (optional) additional authenticated data used during
 *                 encryption.
 * @param[in] pIv The initialization vector used during encryption.
 * @param[in] pTag The authentication tag is verified during decryption.
 * @param[out] ppPlainText The generated plain data.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoAeDecrypt(WosCryptoAeOptions_t *pOptions,
                                    void *pStorageContext,
                                    WosString_t symKeyStorageId,
                                    WosBuffer_t *pCipherText,
                                    WosBuffer_t *pAad,
                                    WosBuffer_t *pIv,
                                    WosBuffer_t *pTag,
                                    WosBuffer_t **ppPlainText);

/**
 * @brief Derives a shared secret from a key exchange using Elliptic Curve
 * Diffie Hellman. This shared secret is used to generate a symmetric key that
 * can be used Authenticated Encryption.
 *
 * @param[in] pOptions The options for the ECC used.
 * @param[in] pStorageContext The storage context used to read and write the
 *                            keys.
 * @param[in] pPublicKey The storage identifier for the other party's public
 *                       key.
 * @param[in] privateKeyStorageId The storage identifier for this party's keys.
 * @param[in] symmetricKeyStorageId The storage identifier used to write the
 *                                  generated symmetric key.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoDeriveSymKey(WosCryptoEccOptions_t *pOptions,
                                       void *pStorageContext,
                                       WosBuffer_t *pPublicKey,
                                       WosString_t privateKeyStorageId,
                                       WosString_t symmetricKeyStorageId);

/**
 * @brief Reads the public part of a ECC key from storage.
 *
 * @param[in] pOptions The options for the ECC used.
 * @param[in] pStorageContext The storage context used to read the key.
 * @param[in] publicKeyStorageId The storage identifier for the key.
 * @param[out] ppExportedPubKey The public part of the ECC key. It must be freed
 *                              by the caller after usage.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoEccReadPubKey(WosCryptoEccOptions_t *pOptions,
                                        void *pStorageContext,
                                        WosString_t publicKeyStorageId,
                                        WosBuffer_t **ppExportedPubKey);

/**
 * @brief Generates random bytes.
 *
 * @param pBuffer The buffer to be filled with random bytes.
 * @return WosCryptoError_t The result of the call.
 */
WosCryptoError_t wosCryptoGetRandomBytes(WosBuffer_t *pBuffer);

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* WOS_CRYPTO_H */
