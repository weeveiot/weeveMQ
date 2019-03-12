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
 * @file smpGlobalCreds.c
 * @brief
 * @version 0.1
 * @date 2019-02-26
 *
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wclTypes.h"
#include "wosCommon.h"
#include "wosLog.h"
#include "wosStorage.h"
#include "wosString.h"

#include "smpGlobalCreds.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "SMP_GLOBAL_CREDS"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* Storage context for device key and certificates. */
static void *gpStorageContext = NULL;
/* Certificate chain size. */
static uint8_t gSelfCertChainSize = 0;

/* Storage ID for Root CA. */
const WosString_t gRootCaStorageId = "root_ca";
/* Storage ID for Self Certificate. */
const WosString_t gSelfCertStorageId = "self_cert";
/* Storage ID for Self Private Key. */
const WosString_t gSelfPrivKeyStorageId = "self_key";

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

static bool lSmpUtilsCheckPath(WosString_t filePath);

static WclError_t lSmpUtilInitFileToStorage(void *pStorageContext,
                                            WosString_t fileName,
                                            WosString_t storageId);

static WclError_t lSmpUtilsReadFile(WosString_t filePath,
                                    WosBuffer_t **ppBuffer);

/* Check/validate the root-CA, self cert and parse the keys. */

WclError_t lSmpUtilInitCerts(void *pStorageContext,
                             WosString_t rootCaStorageId,
                             WosString_t selfCertStorageId,
                             WosString_t selfPrivKeyStorageId);
/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

static bool lSmpUtilsCheckPath(WosString_t filePath)
{
    size_t len = 0;
    bool isValid = false;
    FUNCTION_ENTRY();

    if (!WOS_IS_VALID_STRING(filePath)) {
        WLOGE("filePath %s is invalid", filePath);
        isValid = false;
        goto exit;
    }

    // FIXME Validate the path.
    isValid = true;

exit:
    FUNCTION_EXIT_RETURN(isValid);
    return isValid;
}

static WclError_t lSmpUtilInitFileToStorage(void *pStorageContext,
                                            WosString_t fileName,
                                            WosString_t storageId)
{
    WclError_t smpResult = WCL_ERROR;
    WosBuffer_t *pBuf = NULL;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();

    if (fileName == NULL || storageId == NULL) {
        WLOGE("invalid parameters");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Check if certificate is already in storage. */
    storageResult = wosStorageRead(pStorageContext, storageId, &pBuf);
    if (storageResult == WOS_STORAGE_SUCCESS) {
        WLOGD("Certificate is already present.");
        smpResult = WCL_SUCCESS;
        goto exitFreeBuf;
    } else if (storageResult != WOS_STORAGE_ERROR_NOT_FOUND) {
        /* WOS_STORAGE_ERROR_NOT_FOUND happens when calling the 1st time. */
        WLOGD("Could not access Storage.");
        smpResult = WCL_ERROR_STORAGE_OPERATION;
        goto exit;
    }

    /* If certificate has not been found, read it from file. */
    smpResult = lSmpUtilsReadFile(fileName, &pBuf);
    if (smpResult != WCL_SUCCESS) {
        WLOGE("Could not read file");
        goto exit;
    }

    /* Then write it to storage. */
    storageResult = wosStorageWrite(pStorageContext, storageId, pBuf);
    if (storageResult != WOS_STORAGE_SUCCESS) {
        WLOGE("Storage Write failed %x", storageResult);
        smpResult = WCL_ERROR_STORAGE_OPERATION;
        goto exitFreeBuf;
    }
    smpResult = WCL_SUCCESS;

exitFreeBuf:
    WOS_FREE_BUF_AND_DATA(pBuf);
exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

static WclError_t lSmpUtilsReadFile(WosString_t filePath,
                                    WosBuffer_t **ppBuffer)
{
    WclError_t ret = WCL_ERROR;
    size_t storageSize = 0;
    FILE *fp = NULL;
    long curOffset = 0;
    int fileError = 0;
    char curDir[256];
    char *pCurDir;
    FUNCTION_ENTRY();

    if (!WOS_IS_VALID_STRING(filePath)) {
        WLOGE("file name %s is invalid", filePath);
        ret = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    if (lSmpUtilsCheckPath(filePath) == false) {
        WLOGE("filePath %s is invalid", filePath);
        ret = WCL_ERROR;
        goto exit;
    }

    WLOGD("file path: %s", filePath);

    /* TODO If a full path is used, then we don't need the current directory */
    pCurDir = getcwd(curDir, sizeof(curDir));
    if (pCurDir != NULL) {
        WLOGD("current directory: %s", curDir);
    }

    fp = fopen(filePath, "r");
    if (fp == NULL) {
        fileError = errno;
        WLOGE("fopen errno: %d, description: %s", fileError,
              strerror(fileError));
        ret = WCL_ERROR_STORAGE_OPERATION;
        goto exit;
    }

    curOffset = fseek(fp, 0L, SEEK_END);
    if (curOffset < 0) {
        fileError = errno;
        WLOGE("fseek errno: %d, description: %s", fileError,
              strerror(fileError));
        ret = WCL_ERROR_STORAGE_OPERATION;
        goto exitClose;
    }

    curOffset = ftell(fp);
    if (curOffset < 0) {
        fileError = errno;
        WLOGE("ftell errno: %d, description: %s", fileError,
              strerror(fileError));
        ret = WCL_ERROR_STORAGE_OPERATION;
        goto exitClose;
    }
    storageSize = sizeof(uint8_t) * curOffset;
    /* void */ rewind(fp);

    (*ppBuffer) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if ((*ppBuffer) == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WCL_ERROR_OUT_OF_MEMORY;
        goto exitClose;
    }
    (*ppBuffer)->data = NULL;
    (*ppBuffer)->length = 0;
    (*ppBuffer)->data = (uint8_t *)wosMemAlloc(storageSize);
    if ((*ppBuffer)->data == NULL) {
        WLOGE("could not allocate data buffer: %lu", storageSize);
        ret = WCL_ERROR_OUT_OF_MEMORY;
        goto exitFreeBuf;
    }
    (*ppBuffer)->length = storageSize;

    if (fread((*ppBuffer)->data, (*ppBuffer)->length, 1, fp) != 1) {
        ret = WCL_ERROR_STORAGE_OPERATION;
        WLOGE("fread failed");
        goto exitFreeBufData;
    }

    ret = WCL_SUCCESS;
    goto exitClose;

exitFreeBufData:
    wosMemFree((*ppBuffer)->data);
exitFreeBuf:
    wosMemFree(*ppBuffer);
exitClose:
    fclose(fp);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* Check/validate the root-CA, self cert and parse the keys. */

WclError_t lSmpUtilInitCerts(void *pStorageContext,
                             WosString_t rootCaStorageId,
                             WosString_t selfCertStorageId,
                             WosString_t selfPrivKeyStorageId)
{
    WclError_t smpResult = WCL_ERROR;

    FUNCTION_ENTRY();

    WLOGD("Process the root-ca certificate");
    smpResult = lSmpUtilInitFileToStorage(
        pStorageContext, WCL_SMP_ROOT_CA_CERT_PATH, rootCaStorageId);
    if (smpResult != WCL_SUCCESS) {
        WLOGE("Processing root CA failed.");
        goto exit;
    }

    WLOGD("Process the self certificate");
    smpResult = lSmpUtilInitFileToStorage(
        pStorageContext, WCL_SMP_SELF_CERT_PATH, selfCertStorageId);
    if (smpResult != WCL_SUCCESS) {
        WLOGE("Processing self certificate failed.");
        goto exit;
    }

    // If file path is configurable, use: pConfigParams->selfCertChainSize;
    gSelfCertChainSize = 1; // TODO Parse larger chain

    WLOGD("Process the self signing key");
    smpResult = lSmpUtilInitFileToStorage(
        pStorageContext, WCL_SMP_SELF_SIGNING_KEY_PATH, selfPrivKeyStorageId);
    if (smpResult != WCL_SUCCESS) {
        WLOGE("Processing self signing key failed.");
        goto exit;
    }
    smpResult = WCL_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}
/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

/* Init the global crypto assets. */
WclError_t smpInitGlobalCreds(void)
{
    WclError_t smpResult = WCL_ERROR;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();

    /* Initialize storage. */
#if defined(WOS_STORAGE_IMPL_REDIS)
    /* Initialize with REDIS default hostname/port. Check config. */
    storageResult = wosStorageInitialize(&(gpStorageContext), NULL);
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

    /* Check/validate the root-CA, self cert and parse the keys. */
    smpResult = lSmpUtilInitCerts(gpStorageContext, gRootCaStorageId,
                                  gSelfCertStorageId, gSelfPrivKeyStorageId);
    if (WCL_SUCCESS != smpResult) {
        WLOGE("certificate loading failed");
        smpResult = WCL_ERROR;
        goto exit;
    }
    smpResult = WCL_SUCCESS;

exit:
    if (WCL_SUCCESS != smpResult) {
        if (NULL != gpStorageContext) {
            storageResult = wosStorageTerminate(gpStorageContext);
        }
    }

    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* Destroy the global crypto assets. */
WclError_t smpDeInitGlobalCreds(void)
{
    WclError_t smpResult = WCL_ERROR;
    WosStorageError_t storageResult = WOS_STORAGE_ERROR;

    FUNCTION_ENTRY();

    /* Check if initialization has been called. */
#if !defined(WOS_STORAGE_IMPL_STDC)
    if (NULL == gpStorageContext) {
        WLOGW("global creds has not been initialized");
        goto exit;
    }
#endif

    /* Remove root-ca storage. */
    storageResult = wosStorageDelete(gpStorageContext, gRootCaStorageId);
    if (WOS_STORAGE_SUCCESS != storageResult) {
        if (WOS_STORAGE_ERROR_NOT_FOUND != storageResult) {
            /* We log the error and continue. */
            WLOGW("removing root-ca from storage failed %x", storageResult);
        }
        /* ignore */
    }

    /* Remove self-certificate storage. */
    storageResult = wosStorageDelete(gpStorageContext, gSelfCertStorageId);
    if (WOS_STORAGE_SUCCESS != storageResult) {
        if (WOS_STORAGE_ERROR_NOT_FOUND != storageResult) {
            /* We log the error and continue. */
            WLOGW("removing self-certificate from storage failed %x",
                  storageResult);
        }
        /* ignore */
    }

    /* Remove self-signing-key storage. */
    storageResult = wosStorageDelete(gpStorageContext, gSelfPrivKeyStorageId);
    if (WOS_STORAGE_SUCCESS != storageResult) {
        if (WOS_STORAGE_ERROR_NOT_FOUND != storageResult) {
            /* We log the error and continue. */
            WLOGW("removing self-signing-key from storage failed %x",
                  storageResult);
        }
        /* ignore */
    }

    storageResult = wosStorageTerminate(gpStorageContext);
    if (WOS_STORAGE_SUCCESS != storageResult) {
        /* We log the error and continue. */
        WLOGW("storage termination failed %x", storageResult);
    }

    gpStorageContext = NULL;

    smpResult = WCL_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* Config the path of different certificate and key files in Smp Context. */
WclError_t smpConfigGlobalCreds(SmpSessionContext_t *pSmpCtx)
{
    WclError_t smpResult = WCL_ERROR;

    FUNCTION_ENTRY();

#if !defined(WOS_STORAGE_IMPL_STDC)
    if (NULL == gpStorageContext) {
        WLOGW("global creds has not been initialized");
        goto exit;
    }
#endif

    /* Input parameters validation. */
    if (NULL == pSmpCtx) {
        WLOGE("bad params");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    // Shallow copy
    pSmpCtx->rootCaStorageId = gRootCaStorageId;
    pSmpCtx->selfCertChainStorageId = gSelfCertStorageId;
    pSmpCtx->selfPrivKeyStorageId = gSelfPrivKeyStorageId;
    pSmpCtx->selfCertChainSize = gSelfCertChainSize;

    smpResult = WCL_SUCCESS;

exit:

    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
