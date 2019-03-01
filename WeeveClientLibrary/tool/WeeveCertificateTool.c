/* INSERT LICENSE HERE */

/**
 * @brief Implementation of Tool to Handle Certificate
 *
 * @file WeeveCertificateTool.c
 * @version 0.3
 * @date 2018-10-25
 *
 * @copyright Copyright (c) 2019
 */

/**
 * Version History
 * v0.1 - Initial Version
 * v0.2 - Added Signature Generation and Verification
 * v0.3 - Added argument parsing
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "wosCborCert.h"
#include "wosCert.h"
#include "wosCommon.h"
#include "wosCrypto.h"
#include "wosLog.h"
#include "wosMemory.h"
#include "wosStorage.h"
#include "wosString.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define COLOR_RESET "\033[0m"
#define COLOR_BLACK "\033[30m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"
#define COLOR_BOLD_BLACK "\033[1m\033[30m"
#define COLOR_BOLD_RED "\033[1m\033[31m"
#define COLOR_BOLD_GREEN "\033[1m\033[32m"
#define COLOR_BOLD_YELLOW "\033[1m\033[33m"
#define COLOR_BOLD_BLUE "\033[1m\033[34m"
#define COLOR_BOLD_MAGENTA "\033[1m\033[35m"
#define COLOR_BOLD_CYAN "\033[1m\033[36m"
#define COLOR_BOLD_WHITE "\033[1m\033[37m"

#define LOG_TAG "WeeveCertificateTool.c"

#define MAX_STRING_LENGTH 1025

#define CHECK_VALUE(gotValue, expectedValue)                                   \
    do {                                                                       \
        if (gotValue != expectedValue) {                                       \
            WLOGE("Error, expected %d, got %d", expectedValue, gotValue);      \
            result = -1;                                                       \
            goto exit;                                                         \
        }                                                                      \
    } while (0)

#define CHECK_NOT_VALUE(gotValue, unexpectedValue)                             \
    do {                                                                       \
        if (gotValue == unexpectedValue) {                                     \
            WLOGE("Error, expected %d, got %d", unexpectedValue, gotValue);    \
            result = -1;                                                       \
            goto exit;                                                         \
        }                                                                      \
    } while (0)

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

static const char *shortOpts = "";

static const struct option longOpts[] = {
    /* COMMANDS */
    {"genKey", no_argument, NULL, 'k'},
    {"genCsr", no_argument, NULL, 'r'},
    {"genCert", no_argument, NULL, 'c'},
    {"print", no_argument, NULL, 'p'},
    {"sign", no_argument, NULL, 's'},
    {"verify", no_argument, NULL, 'x'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    /* PARAMETERS */
    {"privKey", required_argument, NULL, '0'},
    {"pubKey", required_argument, NULL, '1'},
    {"issuer", required_argument, NULL, '2'},
    {"subject", required_argument, NULL, '3'},
    {"csr", required_argument, NULL, '4'},
    {"cert", required_argument, NULL, '5'},
    {"data", required_argument, NULL, '6'},
    {"signature", required_argument, NULL, '7'},
    /* man page: The last element of the array has to be filled with zeros. */
    {NULL, no_argument, NULL, 0}};

struct globalArgs_t {
    /* COMMANDS */
    char genKey;
    char genCsr;
    char genCert;
    char print;
    char sign;
    char verify;
    /* PARAMETERS */
    char *privKey;
    char *pubKey;
    char *issuer;
    char *subject;
    char *csr;
    char *cert;
    char *data;
    char *signature;
};

struct globalArgs_t globalArgs;

/* ========================================================================== */
/*                                Aux Functions                               */
/* ========================================================================== */

void initArgs(void)
{
    /* COMMANDS */
    globalArgs.genKey = 0;
    globalArgs.genCsr = 0;
    globalArgs.genCert = 0;
    globalArgs.print = 0;
    globalArgs.sign = 0;
    globalArgs.verify = 0;
    /* PARAMETERS */
    globalArgs.privKey = NULL;
    globalArgs.pubKey = NULL;
    globalArgs.issuer = NULL;
    globalArgs.subject = NULL;
    globalArgs.csr = NULL;
    globalArgs.cert = NULL;
    globalArgs.data = NULL;
    globalArgs.signature = NULL;
    return;
}

void printBuffer(char message[], uint8_t buffer[], size_t len)
{
    char string[69]; /* 20 + 16*3 + 1 (snprintf automatically adds /0 to the end
                        of its strings) */
    int strLen = 0, strPos = 0, bufPos = 0;

    if (message == NULL || buffer == NULL || len == 0) {
        return;
    }
    printf("%s\nBase Address:%p  Length:%lud, %#lx\n", message, buffer, len,
           len);
    strLen = sizeof(string);
    for (bufPos = 0; bufPos < len; bufPos++) {
        /* Start a new line (each should contain 16 bytes) */
        if (bufPos % 16 == 0) {
            /* 64 bits means 16 hex chars. */
            strPos +=
                snprintf(string + strPos, strLen - strPos, "%#08x: ", bufPos);
        }

        /* Each byte printed as 2 hex chars */
        strPos +=
            snprintf(string + strPos, strLen - strPos, "%02X ", buffer[bufPos]);

        /* Finish a line */
        if (bufPos % 16 == 15 || bufPos == len - 1) {
            printf("%s\n", string);
            strPos = 0;
        }
    }
    return;
}

void flushStream(FILE *stream)
{
    int c;
    while ((c = fgetc(stream)) != '\n' && c != EOF)
        ;
}

void readString(char *string, int maxLength)
{
    int i;
    fgets(string, maxLength, stdin);
    i = strlen(string) - 1;
    if (string[i] == '\n') {
        string[i] = '\0';
    }
    return;
}

int readStorageWithUserInput(void *pStorageContext,
                             WosString_t message,
                             WosBuffer_t **ppBuffer)
{
    int result = -1;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosString_t userInput = NULL;

    userInput = wosMemAlloc(MAX_STRING_LENGTH);
    CHECK_NOT_VALUE(userInput, NULL);
    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "%s: " COLOR_RESET,
           message);
    readString(userInput, MAX_STRING_LENGTH);
    storageError = wosStorageRead(pStorageContext, userInput, ppBuffer);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    result = 0;

exit:
    if (userInput != NULL) {
        wosMemFree(userInput);
    }
    return result;
}

int writeStorageWithUserInput(void *pStorageContext,
                              WosString_t message,
                              WosBuffer_t *pBuffer)
{
    int result = -1;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosString_t userInput = NULL;

    userInput = wosMemAlloc(MAX_STRING_LENGTH);
    CHECK_NOT_VALUE(userInput, NULL);
    printf("Enter the Storage ID for writing the " COLOR_BOLD_WHITE
           "%s: " COLOR_RESET,
           message);
    readString(userInput, MAX_STRING_LENGTH);
    storageError = wosStorageWrite(pStorageContext, userInput, pBuffer);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    result = 0;

exit:
    if (userInput != NULL) {
        wosMemFree(userInput);
    }
    return result;
}

/* ========================================================================== */
/*                                Print About                                 */
/* ========================================================================== */

void printAbout(void)
{
    printf("\n\nThis tool was created to help people generating certificates "
           "to be used on the Weeve Network and weeveOS...\n\n");
    return;
}

void printVersion(void)
{
    printf("Weeve's Certificate Tool v0.3\n");
    return;
}

void printHelp(void)
{
    printf("\n");
    printVersion();
    printAbout();
    printf("\n");
    printf("This help:\n");
    printf("    --help\n");
    printf("Version:\n");
    printf("    --version\n");
    printf("\n");
    printf("Generate a Key Pair:\n");
    printf("    --genKey --privKey=filename --pubKey=filename\n");
    printf("\n");
    printf("Generate a Certificate Signing Request:\n");
    printf("    --genCsr --issuer=issuerID --subject=subjectID "
           "--pubKey=filename --csr=filename\n");
    printf("\n");
    printf("Print a Certificate Signing Request (CSR):\n");
    printf("    --print --csr=filename\n");
    printf("\n");
    printf("Generate a Certificate (signing a CSR):\n");
    printf("    --genCert --csr=filename --privKey=filename --cert=filename\n");
    printf("\n");
    printf("Print a Certificate:\n");
    printf("    --print --cert=filename\n");
    printf("\n");
    printf("Sign data:\n");
    printf("    --sign [--privKey=filename/--pubKey=filename] --data=filename "
           "--signature=filename\n");
    printf("\n");
    printf("Verify data signature:\n");
    printf("    --verify [--privKey=filename/--pubKey=filename] --data=filename"
           " --signature=filename\n");
    printf("\n");
    return;
}

/* ========================================================================== */
/*                                Key Pair                                    */
/* ========================================================================== */

int generateKeyArg(void *pStorageContext,
                   char *privKeyPairStorageId,
                   char *pubKeyPairStorageId)
{
    int result = -1;

    WosCryptoError_t cryptoError;
    WosCryptoEccOptions_t eccOptions = {WOS_CRYPTO_ECC_CURVE_NIST_P256,
                                        WOS_CRYPTO_ECC_HASH_SHA256};

    cryptoError =
        wosCryptoEccGenerateKey(&eccOptions, pStorageContext,
                                privKeyPairStorageId, pubKeyPairStorageId);
    CHECK_VALUE(cryptoError, WOS_CRYPTO_SUCCESS);

    result = 0;

exit:
    return result;
}

int generateKey(void *pStorageContext)
{
    int result = -1;
    WosString_t privKeyPairStorageId = NULL, pubKeyPairStorageId = NULL;

    privKeyPairStorageId = wosMemAlloc(MAX_STRING_LENGTH);
    CHECK_NOT_VALUE(privKeyPairStorageId, NULL);
    printf("Enter the storage id for the private part of the keypair: ");
    readString(privKeyPairStorageId, MAX_STRING_LENGTH);

    pubKeyPairStorageId = wosMemAlloc(MAX_STRING_LENGTH);
    CHECK_NOT_VALUE(pubKeyPairStorageId, NULL);
    printf("Enter the storage id for the public part of the keypair: ");
    readString(pubKeyPairStorageId, MAX_STRING_LENGTH);

    printf("Generated Keys: \'%s\' and \'%s\'\n", privKeyPairStorageId,
           pubKeyPairStorageId);

    result = generateKeyArg(pStorageContext, privKeyPairStorageId,
                            pubKeyPairStorageId);
    CHECK_VALUE(result, 0);

exit:
    if (privKeyPairStorageId != NULL) {
        wosMemFree(privKeyPairStorageId);
    }
    if (pubKeyPairStorageId != NULL) {
        wosMemFree(pubKeyPairStorageId);
    }
    return result;
}

/* ========================================================================== */
/*                           CSR & Certificate                                */
/* ========================================================================== */

int generateCsrArg(
    void *pStorageContext, char *issuer, char *subject, char *pubKey, char *csr)
{
    int result = -1;

    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosBuffer_t *pSubjectPubKey = NULL;

    WosMsgError_t msgError = WOS_MSG_ERROR;
    WosCertTbsType_t certTbs;
    WosBuffer_t *pPackedTbs = NULL;
    uint32_t packedLength = 0;

    certTbs.issuerId = issuer;
    certTbs.subjectId = subject;
    certTbs.pSubjectPubKey = NULL;
    certTbs.version = 0;
    certTbs.signatureAlgorithm = WOS_CERT_OID_SIGNATURE_ALGORITHM_ECDSA_SHA256;
    certTbs.subjectPubKeyInfo = WOS_CERT_OID_PUBKEY_INFO_SECP256R1;

    storageError = wosStorageRead(pStorageContext, pubKey, &pSubjectPubKey);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);
    certTbs.pSubjectPubKey = pSubjectPubKey;

    msgError = wosCborCalcCertificateTbs(&certTbs, &packedLength);
    CHECK_VALUE(msgError, WOS_MSG_SUCCESS);
    pPackedTbs = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    CHECK_NOT_VALUE(pPackedTbs, NULL);
    pPackedTbs->data = (uint8_t *)wosMemAlloc(packedLength);
    CHECK_NOT_VALUE(pPackedTbs->data, NULL);
    pPackedTbs->length = packedLength;

    msgError = wosCborPackCertificateTbs(&certTbs, pPackedTbs);
    CHECK_VALUE(msgError, WOS_MSG_SUCCESS);
    if (pPackedTbs->length != packedLength) {
        printf("Error calculating tbs length %d\n", msgError);
    }
    printBuffer("To Be Signed:", pPackedTbs->data, pPackedTbs->length);

    storageError = wosStorageWrite(pStorageContext, csr, pPackedTbs);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    result = 0;

exit:
    WOS_FREE_BUF_AND_DATA(pPackedTbs);
    WOS_FREE_BUF_AND_DATA(pSubjectPubKey);
    return result;
}

int generateCsr(void *pStorageContext)
{
    int result = -1;
    char issuerId[MAX_STRING_LENGTH], subjectId[MAX_STRING_LENGTH],
        pubKey[MAX_STRING_LENGTH], csr[MAX_STRING_LENGTH];

    printf("Enter the " COLOR_BOLD_WHITE "Issuer" COLOR_RESET " ID: ");
    readString(issuerId, MAX_STRING_LENGTH);

    printf("Enter the " COLOR_BOLD_WHITE "Subject" COLOR_RESET " ID: ");
    readString(subjectId, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Subject Public Key" COLOR_RESET ":");
    readString(pubKey, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for writing the " COLOR_BOLD_WHITE
           "CSR" COLOR_RESET ":");
    readString(csr, MAX_STRING_LENGTH);

    result = generateCsrArg(pStorageContext, issuerId, subjectId, pubKey, csr);
    CHECK_VALUE(result, 0);

exit:
    return result;
}

int signCsrArg(void *pStorageContext, char *csr, char *privKey, char *cert)
{
    int result = -1;
    WosBuffer_t *pPackedTbs = NULL;

    /* Cbor */
    WosMsgError_t msgError = WOS_MSG_ERROR;
    WosCertContainerType_t certContainer;
    WosBuffer_t *pContainer = NULL;
    uint32_t packedLength = 0;

    /* Crypto */
    WosCryptoError_t cryptoError;
    WosCryptoEccOptions_t eccOptions = {WOS_CRYPTO_ECC_CURVE_NIST_P256,
                                        WOS_CRYPTO_ECC_HASH_SHA256};
    WosBuffer_t *pSignature = NULL;

    /* Storage */
    WosStorageError_t storageError = WOS_STORAGE_ERROR;

    /* Sign TBS */
    storageError = wosStorageRead(pStorageContext, csr, &pPackedTbs);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);
    cryptoError = wosCryptoEccSign(&eccOptions, pStorageContext, privKey,
                                   pPackedTbs, &pSignature);
    CHECK_VALUE(cryptoError, WOS_CRYPTO_SUCCESS);

    certContainer.version = 0;
    certContainer.pEncodedTbs = pPackedTbs;
    certContainer.pSignature = pSignature;

    msgError = wosCborCalcCertificateContainer(&certContainer, &packedLength);
    CHECK_VALUE(msgError, WOS_MSG_SUCCESS);
    pContainer = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    CHECK_NOT_VALUE(pContainer, NULL);
    pContainer->data = (uint8_t *)wosMemAlloc(packedLength);
    CHECK_NOT_VALUE(pContainer->data, NULL);
    pContainer->length = packedLength;

    msgError = wosCborPackCertificateContainer(&certContainer, pContainer);
    CHECK_VALUE(msgError, WOS_MSG_SUCCESS);
    if (pContainer->length != packedLength) {
        printf("Error calculating container length %d\n", msgError);
    }
    printBuffer("Certificate:", pContainer->data, pContainer->length);

    storageError = wosStorageWrite(pStorageContext, cert, pContainer);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    result = 0;

exit:
    WOS_FREE_BUF_AND_DATA(pPackedTbs);
    WOS_FREE_BUF_AND_DATA(pSignature);
    WOS_FREE_BUF_AND_DATA(pContainer);
    return result;
}

int signCsr(void *pStorageContext)
{
    int result = -1;
    char privKey[MAX_STRING_LENGTH], csr[MAX_STRING_LENGTH],
        cert[MAX_STRING_LENGTH];

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "CSR" COLOR_RESET ":");
    readString(csr, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Issuer Private Key" COLOR_RESET ":");
    readString(privKey, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for writing the " COLOR_BOLD_WHITE
           "Certificate" COLOR_RESET ":");
    readString(cert, MAX_STRING_LENGTH);

    result = signCsrArg(pStorageContext, csr, privKey, cert);
    CHECK_VALUE(result, 0);

exit:
    return result;
}

/* ========================================================================== */
/*                                File / Storage                              */
/* ========================================================================== */

int hex2Buffer(WosString_t hexString, WosBuffer_t **ppBuffer)
{
    int result = -1;
    int i = 0, j = 0;
    uint32_t hexValue = 0;
    uint8_t byte = 0;
    char flagInsideByte = false;
    uint32_t length = -1;

    length = wosStringLength(hexString);
    (*ppBuffer)->data = wosMemAlloc(length / 2 + 1);
    printf("hexString: %s\n", hexString);

    for (i = 0, j = 0; i < length; ++i) {
        byte = hexString[i];
        if (byte >= '0' && byte <= '9') {
            byte = byte - '0';
        } else if (byte >= 'a' && byte <= 'f') {
            byte = byte - 'a' + 10;
        } else if (byte >= 'A' && byte <= 'F') {
            byte = byte - 'A' + 10;
        } else {
            /* Skip any other character */
            continue;
        }
        /* Every 2 chars = 1 byte */
        if (flagInsideByte) {
            hexValue = (hexValue << 4) | (byte & 0xF);
            (*ppBuffer)->data[j] = hexValue;
            flagInsideByte = false;
            j++;
        } else {
            hexValue = byte & 0xF;
            flagInsideByte = true;
        }
    }

    if (flagInsideByte) {
        hexValue = (hexValue << 4) | (byte & 0xF);
        (*ppBuffer)->data[j] = hexValue;
        hexValue = 0;
        j++;
    }

    (*ppBuffer)->length = j;
    result = 0;

exit:
    return result;
}

int hex2Storage(void *pStorageContext)
{
    int result = -1;
    WosString_t hexString = NULL;
    WosBuffer_t *pBuffer = NULL;

    hexString = wosMemAlloc(MAX_STRING_LENGTH);
    CHECK_NOT_VALUE(hexString, NULL);
    printf("Enter the hex string: ");
    readString(hexString, MAX_STRING_LENGTH);

    pBuffer = wosMemAlloc(sizeof(WosBuffer_t));
    CHECK_NOT_VALUE(pBuffer, NULL);
    pBuffer->data = NULL;
    pBuffer->length = 0;

    result = hex2Buffer(hexString, &pBuffer);
    CHECK_VALUE(result, 0);

    result = writeStorageWithUserInput(pStorageContext, "hex data", pBuffer);
    CHECK_VALUE(result, 0);

exit:
    if (hexString != NULL) {
        wosMemFree(hexString);
    }
    WOS_FREE_BUF_AND_DATA(pBuffer);
    return result;
}

int storage2Hex(void *pStorageContext)
{
    int result = -1;
    WosBuffer_t *pBuffer = NULL;

    /* Get storage id and read the data */
    result = readStorageWithUserInput(pStorageContext, "data", &pBuffer);
    CHECK_VALUE(result, 0);

    /* Print the data */
    printBuffer("Data: ", pBuffer->data, pBuffer->length);
    result = 0;

exit:
    WOS_FREE_BUF_AND_DATA(pBuffer);
    return result;
}

/* ========================================================================== */
/*                                Signing                                     */
/* ========================================================================== */

int signDataArg(void *pStorageContext, char *key, char *data, char *signature)
{
    int result = -1;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosBuffer_t *pSignature = NULL;
    WosBuffer_t *pData = NULL;

    /* Data */
    storageError = wosStorageRead(pStorageContext, data, &pData);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    /* Crypto */
    WosCryptoError_t cryptoError;
    WosCryptoEccOptions_t eccOptions = {WOS_CRYPTO_ECC_CURVE_NIST_P256,
                                        WOS_CRYPTO_ECC_HASH_SHA256};

    cryptoError =
        wosCryptoEccSign(&eccOptions, pStorageContext, key, pData, &pSignature);
    CHECK_VALUE(cryptoError, WOS_CRYPTO_SUCCESS);

    /* Store */
    storageError = wosStorageWrite(pStorageContext, signature, pSignature);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    result = 0;

exit:
    WOS_FREE_BUF_AND_DATA(pData);
    WOS_FREE_BUF_AND_DATA(pSignature);
    return result;
}

int signData(void *pStorageContext)
{
    int result = -1;
    char key[MAX_STRING_LENGTH], data[MAX_STRING_LENGTH],
        signature[MAX_STRING_LENGTH];

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Signing Key: " COLOR_RESET);
    readString(key, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Data to be signed" COLOR_RESET ":");
    readString(data, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for writing the " COLOR_BOLD_WHITE
           "Signature" COLOR_RESET ":");
    readString(signature, MAX_STRING_LENGTH);

    result = signDataArg(pStorageContext, key, data, signature);
    CHECK_VALUE(result, 0);

exit:
    return result;
}

int verifyDataArg(void *pStorageContext, char *key, char *data, char *signature)
{
    int result = -1;
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosBuffer_t *pSignature = NULL;
    WosBuffer_t *pData = NULL;

    /* Crypto */
    WosCryptoError_t cryptoError = WOS_CRYPTO_ERROR;
    WosCryptoEccOptions_t eccOptions = {WOS_CRYPTO_ECC_CURVE_NIST_P256,
                                        WOS_CRYPTO_ECC_HASH_SHA256};

    /* Data */
    storageError = wosStorageRead(pStorageContext, data, &pData);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    /* Signature */
    storageError = wosStorageRead(pStorageContext, signature, &pSignature);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    cryptoError = wosCryptoEccVerify(&eccOptions, pStorageContext, key, pData,
                                     pSignature);
    if (cryptoError == WOS_CRYPTO_SIGNATURE_MATCH) {
        printf("Signature Match.\n");
    } else {
        printf("Signature Error: %d\n", cryptoError);
    }
    CHECK_VALUE(cryptoError, WOS_CRYPTO_SIGNATURE_MATCH);

    result = 0;

exit:
    WOS_FREE_BUF_AND_DATA(pData);
    WOS_FREE_BUF_AND_DATA(pSignature);
    return result;
}

int verifyData(void *pStorageContext)
{
    int result = -1;
    char key[MAX_STRING_LENGTH], data[MAX_STRING_LENGTH],
        signature[MAX_STRING_LENGTH];

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Signing Key: " COLOR_RESET);
    readString(key, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Data to be signed" COLOR_RESET ":");
    readString(data, MAX_STRING_LENGTH);

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Signature" COLOR_RESET ":");
    readString(signature, MAX_STRING_LENGTH);

    result = verifyDataArg(pStorageContext, key, data, signature);
    CHECK_VALUE(result, 0);

exit:
    return result;
}

/* ========================================================================== */
/*                                Printing                                    */
/* ========================================================================== */

int printCertContainer(WosCertContainerType_t *pContainer)
{
    int result = -1;
    printf("HEADER\n");
    printf("version:               %d\n", pContainer->version);
    printBuffer("pEncodedTbs:", pContainer->pEncodedTbs->data,
                pContainer->pEncodedTbs->length);
    printBuffer("pSignature:", pContainer->pSignature->data,
                pContainer->pSignature->length);
    result = 0;

exit:
    return result;
}

int printCertTbs(WosCertTbsType_t *pTbs)
{
    int result = -1;
    printf("TO BE SIGNED\n");
    printf("version:               %d\n", pTbs->version);
    printf("signatureAlgorithm:    %d\n", pTbs->signatureAlgorithm);
    printf("issuerId:              %s\n", pTbs->issuerId);
    printf("subjectId:             %s\n", pTbs->subjectId);
    printf("subjectPubKeyInfo:     %d\n", pTbs->subjectPubKeyInfo);
    printBuffer("pSubjectPubKey:", pTbs->pSubjectPubKey->data,
                pTbs->pSubjectPubKey->length);
    result = 0;

exit:
    return result;
}

int printCsrArg(void *pStorageContext, char *csr)
{
    int result = -1;

    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosBuffer_t *pBuf = NULL;

    WosMsgError_t msgError = WOS_MSG_ERROR;
    WosCertTbsType_t tbs;

    storageError = wosStorageRead(pStorageContext, csr, &pBuf);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    printBuffer("CSR Buffer:", pBuf->data, pBuf->length);

    /* TBS */
    msgError = wosCborUnpackCertificateTbs(pBuf, &tbs);
    CHECK_VALUE(msgError, WOS_MSG_SUCCESS);
    result = printCertTbs(&tbs);
    CHECK_VALUE(result, 0);

exit:
    wosCborFreeCertificateTbs(&tbs);
    WOS_FREE_BUF_AND_DATA(pBuf);
    return result;
}

int printCertificateArg(void *pStorageContext, char *cert)
{
    int result = -1;

    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    WosBuffer_t *pBuf = NULL;

    WosMsgError_t msgError = WOS_MSG_ERROR;
    WosCertContainerType_t container;
    WosCertTbsType_t tbs;

    storageError = wosStorageRead(pStorageContext, cert, &pBuf);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);

    printBuffer("Certificate Buffer:", pBuf->data, pBuf->length);

    /* Container */
    msgError = wosCborUnpackCertificateContainer(pBuf, &container);
    CHECK_VALUE(msgError, WOS_MSG_SUCCESS);
    result = printCertContainer(&container);
    CHECK_VALUE(result, 0);

    /* TBS */
    msgError = wosCborUnpackCertificateTbs(container.pEncodedTbs, &tbs);
    CHECK_VALUE(msgError, WOS_MSG_SUCCESS);
    result = printCertTbs(&tbs);
    CHECK_VALUE(result, 0);

exit:
    wosCborFreeCertificateContainer(&container);
    wosCborFreeCertificateTbs(&tbs);
    WOS_FREE_BUF_AND_DATA(pBuf);
    return result;
}

int printCsr(void *pStorageContext)
{
    int result = -1;
    char csr[MAX_STRING_LENGTH];

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "CSR" COLOR_RESET ":");
    readString(csr, MAX_STRING_LENGTH);

    result = printCsrArg(pStorageContext, csr);
    CHECK_VALUE(result, 0);

exit:
    return result;
}

int printCertificate(void *pStorageContext)
{
    int result = -1;
    char cert[MAX_STRING_LENGTH];

    printf("Enter the Storage ID for reading the " COLOR_BOLD_WHITE
           "Certificate" COLOR_RESET ":");
    readString(cert, MAX_STRING_LENGTH);

    result = printCertificateArg(pStorageContext, cert);
    CHECK_VALUE(result, 0);

exit:
    return result;
}

/* ========================================================================== */
/*                                Main                                        */
/* ========================================================================== */

int main(int argc, char const *argv[])
{
    int result = -1;
    char userOption;
    int opt = 0;
    int index = -1;

    /* Crypto */
    WosCryptoError_t cryptoError;
    WosCryptoConfig_t cryptoConfig;

    /* Storage */
    WosStorageError_t storageError = WOS_STORAGE_ERROR;
    void *pStorageContext = NULL;
    WosStorageConfig_t *pStorageConfig = NULL;

    /* Initialize */
    storageError = wosStorageInitialize(&pStorageContext, pStorageConfig);
    CHECK_VALUE(storageError, WOS_STORAGE_SUCCESS);
    cryptoError = wosCryptoInitialize(&cryptoConfig);
    CHECK_VALUE(cryptoError, WOS_CRYPTO_SUCCESS);

    if (argc > 1) {
        /* void */ initArgs();

        while ((opt = getopt_long(argc, argv, shortOpts, longOpts, &index)) !=
               -1) {

            switch (opt) {
            /* COMMANDS */
            /* genKey */
            case 'k':
                globalArgs.genKey = 1;
                break;
            /* genCsr */
            case 'r':
                globalArgs.genCsr = 1;
                break;
            /* genCert */
            case 'c':
                globalArgs.genCert = 1;
                break;
            /* print */
            case 'p':
                globalArgs.print = 1;
                break;
            /* sign */
            case 's':
                globalArgs.sign = 1;
                break;
            /* verify */
            case 'x':
                globalArgs.verify = 1;
                break;
                /* help */
            case 'h':
            case '?':
                printHelp();
                result = 0;
                goto exit;
                break;
            /* version */
            case 'v':
                printVersion();
                result = 0;
                goto exit;
                break;

            /* PARAMETERS */
            /* privKey */
            case '0':
                globalArgs.privKey = optarg;
                break;
            /* pubKey */
            case '1':
                globalArgs.pubKey = optarg;
                break;
            /* issuer */
            case '2':
                globalArgs.issuer = optarg;
                break;
            /* subject */
            case '3':
                globalArgs.subject = optarg;
                break;
            /* csr */
            case '4':
                globalArgs.csr = optarg;
                break;
            /* cert */
            case '5':
                globalArgs.cert = optarg;
                break;
            /* data */
            case '6':
                globalArgs.data = optarg;
                break;
            /* signature */
            case '7':
                globalArgs.signature = optarg;
                break;

            /* It shouldn't happen... */
            default:
                goto exit;
                break;
            }

            index = -1;
        }

        /* Unknown arguments */
        if (optind < argc) {
            printf("non-option ARGV-elements: ");
            while (optind < argc)
                printf("%s ", argv[optind++]);
            printf("\n");
        }

        /* Finally check for missing arguments and call functions */
        if (globalArgs.genKey != 0) {
            if (globalArgs.privKey == NULL || globalArgs.pubKey == NULL) {
                printf("Missing arguments.\n");
                goto exit;
            }
            result = generateKeyArg(pStorageContext, globalArgs.privKey,
                                    globalArgs.pubKey);
            CHECK_VALUE(result, 0);
        } else if (globalArgs.genCsr != 0) {
            if (globalArgs.issuer == NULL || globalArgs.subject == NULL ||
                globalArgs.pubKey == NULL || globalArgs.csr == NULL) {
                printf("Missing arguments.\n");
                goto exit;
            }
            result = generateCsrArg(pStorageContext, globalArgs.issuer,
                                    globalArgs.subject, globalArgs.pubKey,
                                    globalArgs.csr);
            CHECK_VALUE(result, 0);
        } else if (globalArgs.genCert != 0) {
            if (globalArgs.csr == NULL || globalArgs.privKey == NULL ||
                globalArgs.cert == NULL) {
                printf("Missing arguments.\n");
                goto exit;
            }
            result = signCsrArg(pStorageContext, globalArgs.csr,
                                globalArgs.privKey, globalArgs.cert);
            CHECK_VALUE(result, 0);
        } else if (globalArgs.print != 0) {
            if (globalArgs.csr == NULL && globalArgs.cert == NULL) {
                printf("Missing arguments.\n");
                goto exit;
            }
            if (globalArgs.csr != NULL) {
                result = printCsrArg(pStorageContext, globalArgs.csr);
                CHECK_VALUE(result, 0);
            } else if (globalArgs.cert != NULL) {
                result = printCertificateArg(pStorageContext, globalArgs.cert);
                CHECK_VALUE(result, 0);
            } else {
                printf("Wrong arguments.\n");
                goto exit;
            }
        } else if (globalArgs.sign != 0) {
            if (globalArgs.data == NULL || globalArgs.signature == NULL) {
                printf("Missing arguments.\n");
                goto exit;
            }
            if (globalArgs.privKey != NULL) {
                result = signDataArg(pStorageContext, globalArgs.privKey,
                                     globalArgs.data, globalArgs.signature);
                CHECK_VALUE(result, 0);
            } else if (globalArgs.pubKey != NULL) {
                result = signDataArg(pStorageContext, globalArgs.pubKey,
                                     globalArgs.data, globalArgs.signature);
                CHECK_VALUE(result, 0);
            } else {
                printf("Wrong arguments.\n");
                goto exit;
            }
        } else if (globalArgs.verify != 0) {
            if (globalArgs.data == NULL || globalArgs.signature == NULL) {
                printf("Missing arguments.\n");
                goto exit;
            }
            if (globalArgs.privKey != NULL) {
                result = verifyDataArg(pStorageContext, globalArgs.privKey,
                                       globalArgs.data, globalArgs.signature);
                CHECK_VALUE(result, 0);
            } else if (globalArgs.pubKey != NULL) {
                result = verifyDataArg(pStorageContext, globalArgs.pubKey,
                                       globalArgs.data, globalArgs.signature);
                CHECK_VALUE(result, 0);
            } else {
                printf("Wrong arguments.\n");
                goto exit;
            }
        } else {
            printf("No command argument provided.\n");
            goto exit;
        }
    } else {
        /* If no arguments provided, prompt user to enter options */
        while (true) {
            printf("Weeve's Certificate Tool\n");
            printf("Available Options:\n");
            printf("    0. About this tool;\n");
            printf("    1. Generate Key Pair;\n");
            printf("    2. Generate CSR (Certificate Signing Request);\n");
            printf("    3. Print CSR;\n");
            printf("    4. Generate Certificate (by signing CSR);\n");
            printf("    5. Print Certificate;\n");
            printf("    6. Put hex in storage;\n");
            printf("    7. Get hex from storage;\n");
            printf("    8. Sign Data;\n");
            printf("    9. Verify Data;\n");
            printf("    [Q]uit;\n");
            printf("> ");
            userOption = fgetc(stdin);
            flushStream(stdin);

            if (userOption == '0') {
                printAbout();
            } else if (userOption == '1') {
                result = generateKey(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '2') {
                result = generateCsr(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '3') {
                result = printCsr(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '4') {
                result = signCsr(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '5') {
                result = printCertificate(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '6') {
                result = hex2Storage(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '7') {
                result = storage2Hex(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '8') {
                result = signData(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == '9') {
                result = verifyData(pStorageContext);
                CHECK_VALUE(result, 0);
            } else if (userOption == 'Q' || userOption == 'q') {
                result = 0;
                break;
            } else {
                printf("Undefined action: \'%c\'\n", userOption);
                result = -1;
                break;
            }
            printf("\n");
        }
    }

exit:
    cryptoError = wosCryptoTerminate();
    storageError = wosStorageTerminate(pStorageContext);
    printf("Result: %d\n", result);
    return result;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
