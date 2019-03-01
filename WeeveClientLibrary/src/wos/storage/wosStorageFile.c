/**
 * @brief Implementation of Storage Functions using the C File API
 *
 * @file wosStorageFile.c
 * @date 2018-10-09
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

#include "wosCommon.h"
#include "wosLog.h"
#include "wosStorage.h"
#include "wosString.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "STORAGE_STDC"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

static bool lWosStorageIsKeyValid(WosString_t key);

/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

static bool lWosStorageIsKeyValid(WosString_t key)
{
    size_t len = 0;
    bool isValid = false;
    FUNCTION_ENTRY();

    if (!WOS_IS_VALID_STRING(key)) {
        WLOGE("key is invalid");
        isValid = false;
        goto exit;
    }

    len = wosStringLength(key);

    for (int i = 0; i < len; ++i) {
        if (key[i] < 31 || key[i] > 127 || key[i] == 47 || key[i] == 92) {
            WLOGD("found invalid char value: %d", key[i]);
            isValid = false;
            goto exit;
        }
    }
    isValid = true;

exit:
    FUNCTION_EXIT_RETURN(isValid);
    return isValid;
}

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

WosStorageError_t wosStorageInitialize(void **ppContext,
                                       WosStorageConfig_t *pConfig)
{
    WosStorageError_t ret = WOS_STORAGE_SUCCESS;
    FUNCTION_ENTRY();
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosStorageError_t wosStorageTerminate(void *pContext)
{
    WosStorageError_t ret = WOS_STORAGE_SUCCESS;
    FUNCTION_ENTRY();
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosStorageError_t
wosStorageWrite(void *pContext, WosString_t storageKey, WosBuffer_t *pBuffer)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    FILE *fp = NULL;
    int fileError = 0;
    FUNCTION_ENTRY();

    /* if (pContext == NULL) {
        WLOGI("wosStorageWrite: Context is null.");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    } */
    if (!WOS_IS_VALID_STRING(storageKey)) {
        WLOGE("bad params, storageKey is invalid");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }
    if (!WOS_IS_VALID_BUFFER(pBuffer)) {
        WLOGE("bad params, null buffer");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }

    if (lWosStorageIsKeyValid(storageKey) == false) {
        WLOGE("invalid filename %s", storageKey);
        ret = WOS_STORAGE_ERROR;
        goto exit;
    }

    WLOGD("writing to storage id: %s", storageKey);
    fp = fopen(storageKey, "w");
    if (fp == NULL) {
        fileError = errno;
        WLOGE("fopen errno: %d, description: %s", fileError,
              strerror(fileError));
        ret = WOS_STORAGE_ERROR_NOT_FOUND;
        goto exit;
    }

    if (fwrite(pBuffer->data, pBuffer->length, 1, fp) != 1) {
        WLOGE("fwrite failed");
        ret = WOS_STORAGE_ERROR;
        goto exitClose;
    }
    ret = WOS_STORAGE_SUCCESS;

exitClose:
    fclose(fp);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosStorageError_t
wosStorageRead(void *pContext, WosString_t storageKey, WosBuffer_t **ppBuffer)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    size_t storageSize = 0;
    FILE *fp = NULL;
    long curOffset = 0;
    int fileError = 0;
    FUNCTION_ENTRY();

    if (!WOS_IS_VALID_STRING(storageKey)) {
        WLOGE("bad params, storageKey is invalid");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }

    if (lWosStorageIsKeyValid(storageKey) == false) {
        WLOGE("invalid filename %s", storageKey);
        ret = WOS_STORAGE_ERROR;
        goto exit;
    }

    WLOGD("reading storage id: %s", storageKey);
    fp = fopen(storageKey, "r");
    if (fp == NULL) {
        fileError = errno;
        WLOGW("fopen errno: %d, description: %s", fileError,
              strerror(fileError));
        ret = WOS_STORAGE_ERROR_NOT_FOUND;
        goto exit;
    }

    curOffset = fseek(fp, 0L, SEEK_END);
    if (curOffset < 0) {
        fileError = errno;
        WLOGE("fseek errno: %d, description: %s", fileError,
              strerror(fileError));
        ret = WOS_STORAGE_ERROR;
        goto exitClose;
    }

    curOffset = ftell(fp);
    if (curOffset < 0) {
        fileError = errno;
        WLOGE("ftell errno: %d, description: %s", fileError,
              strerror(fileError));
        ret = WOS_STORAGE_ERROR;
        goto exitClose;
    }
    storageSize = sizeof(uint8_t) * curOffset;
    /* void */ rewind(fp);

    (*ppBuffer) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
    if ((*ppBuffer) == NULL) {
        WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
        ret = WOS_STORAGE_ERROR_OUT_OF_MEMORY;
        goto exitClose;
    }
    (*ppBuffer)->data = NULL;
    (*ppBuffer)->length = 0;
    (*ppBuffer)->data = (uint8_t *)wosMemAlloc(storageSize);
    if ((*ppBuffer)->data == NULL) {
        WLOGE("could not allocate data buffer: %lu", storageSize);
        ret = WOS_STORAGE_ERROR_OUT_OF_MEMORY;
        goto exitFreeBuf;
    }
    (*ppBuffer)->length = storageSize;

    if (fread((*ppBuffer)->data, (*ppBuffer)->length, 1, fp) != 1) {
        ret = WOS_STORAGE_ERROR;
        WLOGE("fread failed");
        goto exitFreeBufData;
    }

    ret = WOS_STORAGE_SUCCESS;
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

WosStorageError_t wosStorageDelete(void *pContext, WosString_t storageKey)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    int fileError = 0;
    FUNCTION_ENTRY();

    /* if (pContext == NULL) {
        WLOGI("wosStorageDelete: Context is null.");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    } */
    if (!WOS_IS_VALID_STRING(storageKey)) {
        WLOGE("bad params, storageKey is invalid");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }

    if (lWosStorageIsKeyValid(storageKey) == false) {
        WLOGE("bad file name %s", storageKey);
        ret = WOS_STORAGE_ERROR;
        goto exit;
    }

    WLOGD("removing storage id: %s", storageKey);
    if (remove(storageKey) != 0) {
        ret = WOS_STORAGE_ERROR_NOT_FOUND;
        fileError = errno;
        WLOGW("remove errno: %d, description: %s", fileError,
              strerror(fileError));
        goto exit;
    }
    ret = WOS_STORAGE_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
