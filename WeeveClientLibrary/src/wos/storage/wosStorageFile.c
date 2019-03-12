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
 * @brief Implementation of Storage Functions using the C File API
 *
 * @file wosStorageFile.c
 * @date 2018-10-09
 * 
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
