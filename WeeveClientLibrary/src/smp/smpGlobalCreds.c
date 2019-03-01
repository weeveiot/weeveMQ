/**
 * @file smpGlobalCreds.c
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
