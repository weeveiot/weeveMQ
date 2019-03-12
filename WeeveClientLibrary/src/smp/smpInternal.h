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
 * @file smpInternal.h
 * @brief
 * @version 0.1
 * @date 2019-02-26.
 *
 */

#ifndef SMP_INTERNAL_H_
#define SMP_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wclTypes.h"
#include "wosCrypto.h"

#include "wclSmp.h"

#include "wclConfig.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

/* Length of random IDs. */
#define SMP_INTERNAL_ID_LENGTH (0x20)

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

typedef WosBuffer_t SmpStorageId_t;

/* Context to hold a SMP session */
typedef struct tSmpSessionContext {
  /* Client-Id. */
  char clientId[WCL_SMP_CLIENT_ID_LENGTH + 1];
  /* Indicates whether pre-session secrets has been generated. */
  bool isPreSessionSecretsGenerated;
  /* Indiacates whether session key has been generated. */
  bool isSessionKeyEstablished;
  /* Storage context for stored keys. */
  void *pStorageContext;
  /* Session Key Exchange: ECDH Private Key storage id. */
  char sessionPrivateKeyStorageId[SMP_INTERNAL_ID_LENGTH + 1];
  /* Session Key Exchange: ECDH Public Key storage id. */
  char sessionPublicKeyStorageId[SMP_INTERNAL_ID_LENGTH + 1];
  /* Session Key Exchange: ECDH Shared Secret storage id. */
  char sessionSharedKeyStorageId[SMP_INTERNAL_ID_LENGTH + 1];
  /* ECC Cipher suite options. */
  WosCryptoEccOptions_t *pEccOptions;
  /* AEAD Cipher options. */
  WosCryptoAeOptions_t *pAeadOptions;
  /* Message id of the message to be sent. */
  uint32_t toBeSentMessageId;
  /* Message id of the last message received. */
  uint32_t lastReceivedMessageId;
  /* Storage ID for self's Private Key. */
  WosString_t selfPrivKeyStorageId;
  /* Storage ID for self's certificate + chain. */
  WosString_t selfCertChainStorageId;
  /* Storage ID for number of certificates on the chain. */
  uint8_t selfCertChainSize;
  /* Storage ID for Root CA certificate. */
  WosString_t rootCaStorageId;
} SmpSessionContext_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

/* Generate the crypto assets. */
WclError_t smpInitialiseSessionParams(SmpSessionContext_t **ppSmpCtx);

/* Export the session establishment message containing public keys for
 * key-exchange. */
WclError_t
smpExportSessionEstablishmentParams(SmpSessionContext_t *pSmpCtx,
                                    const WosBuffer_t *pStdProtocolSEParams,
                                    WosBuffer_t *pSmpSEMessage);

/* Secure(encrypt/sign) an initiator's payload to be sent. */
WclError_t smpSecureMessage(SmpSessionContext_t *pSmpCtx,
                            WclSmpMessageType_t messageType,
                            const WosBuffer_t *pClearMessage,
                            WosBuffer_t *pSecuredMessage);

/* Process Session Establishment Ack Message or Validate(decrypt/verify) a
 * payload from responder. */
WclError_t smpProcessMessage(SmpSessionContext_t *pSmpCtx,
                             const WosBuffer_t *pSecuredMessage,
                             WosBuffer_t *pClearMessage);

/* Delete the crypto assets. */
WclError_t smpDeleteSessionCredentials(SmpSessionContext_t *pSmpCtx);

#ifdef __cplusplus
}
#endif

#endif /* SMP_INTERNAL_H_ */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
