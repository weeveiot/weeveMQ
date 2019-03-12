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
 * @brief Implementation of Storage Functions using Redis
 *
 * @file wosStorageRedis.c
 * @date 2018-07-16
 *
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosCommon.h"
#include "wosLog.h"
#include "wosStorage.h"
#include "wosString.h"
#include <hiredis.h>
#include <stdint.h>
#include <stdlib.h>

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "STORAGE_REDIS"

const WosString_t redisDefaultHost = "127.0.0.1";
const int redisDefaultPort = 6379;

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

/**
 * @brief Internal function used to change the redis logical database to the
 * desired one.
 *
 * @param context the non null redis context.
 * @param id the desired logical database id.
 * @return WosStorageError_t The result of the call.
 */
static WosStorageError_t lWosStorageChangeDb(redisContext *context, int id);

/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

static WosStorageError_t lWosStorageChangeDb(redisContext *context, int id)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    redisReply *reply = NULL;
    FUNCTION_ENTRY();

    reply = redisCommand(context, "SELECT %i", id);
    if (reply == NULL) {
        WLOGE("Change DB Error.");
        ret = WOS_STORAGE_ERROR;
    } else if (reply->type == REDIS_REPLY_ERROR) {
        WLOGE("Change DB Error Type: %s", reply->str);
        ret = WOS_STORAGE_ERROR;
    } else {
        ret = WOS_STORAGE_SUCCESS;
    }
    freeReplyObject(reply);
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

WosStorageError_t wosStorageInitialize(void **ppContext,
                                       WosStorageConfig_t *pConfig)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    redisContext *internalCtx = NULL;
    WosString_t hostname = redisDefaultHost;
    int port = redisDefaultPort;
    FUNCTION_ENTRY();

    if (pConfig != NULL) {
        WLOGI("Configuration provided.");
        hostname = pConfig->hostname;
        port = pConfig->port;
    }

    internalCtx = redisConnect(hostname, port);
    if (internalCtx == NULL) {
        WLOGE("Connection error: can't allocate redis context");
        ret = WOS_STORAGE_ERROR;
        goto exit;
    } else if (internalCtx->err) {
        WLOGE("Connection error: %s", internalCtx->errstr);
        /*void*/ redisFree(internalCtx);
        ret = WOS_STORAGE_ERROR;
        goto exit;
    }

    /* If a specific database ID is required, change to it. */
    if (pConfig != NULL && pConfig->dbId != 0) {
        if (lWosStorageChangeDb(internalCtx, pConfig->dbId) !=
            WOS_STORAGE_SUCCESS) {
            /* From redis doc: "Once an error is returned the context cannot be
             * reused..." */
            WLOGD("Freeing storage context because of error.");
            /*void*/ redisFree(internalCtx);
            ret = WOS_STORAGE_ERROR;
            goto exit;
        }
    }
    *ppContext = internalCtx;
    ret = WOS_STORAGE_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosStorageError_t wosStorageTerminate(void *pContext)
{
    WosStorageError_t ret = WOS_STORAGE_SUCCESS;
    FUNCTION_ENTRY();
    /*void*/ redisFree(pContext);
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosStorageError_t
wosStorageRead(void *pContext, WosString_t storageKey, WosBuffer_t **ppBuffer)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    redisContext *internalCtx = NULL;
    redisReply *reply = NULL;
    size_t length;

    FUNCTION_ENTRY();

    if (pContext == NULL) {
        WLOGI("Context is null.");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }
    if (!WOS_IS_VALID_STRING(storageKey)) {
        WLOGE("storageKey is invalid");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }

    internalCtx = pContext;
    reply = redisCommand(internalCtx, "GET %b", storageKey,
                         wosStringLength(storageKey));
    /* Error */
    if (reply == NULL) {
        if (internalCtx->err != 0) {
            WLOGE("error: %s", internalCtx->errstr);
        } else {
            WLOGE("error");
        }
        ret = WOS_STORAGE_ERROR;
        goto exit;
    } else if (reply->type == REDIS_REPLY_ERROR) {
        WLOGE("error reply: %s", reply->str);
        ret = WOS_STORAGE_ERROR;
        goto exit;
    }
    if (reply->len == 0 || reply->type != REDIS_REPLY_STRING) {
        WLOGI("error: len = %d, type = %d", reply->len, reply->type);
        ret = WOS_STORAGE_ERROR_NOT_FOUND;
        goto exit;
    } else {
        /* Success */
        length = (sizeof(uint8_t)) * (reply->len);
        (*ppBuffer) = (WosBuffer_t *)wosMemAlloc(sizeof(WosBuffer_t));
        if ((*ppBuffer) == NULL) {
            WLOGE("could not allocate: %lu", sizeof(WosBuffer_t));
            ret = WOS_STORAGE_ERROR_OUT_OF_MEMORY;
            goto exit;
        }
        (*ppBuffer)->data = NULL;
        (*ppBuffer)->length = 0;
        (*ppBuffer)->data = (uint8_t *)wosMemAlloc(length);
        if ((*ppBuffer)->data == NULL) {
            WLOGE("could not allocate data buffer: %lu", length);
            ret = WOS_STORAGE_ERROR_OUT_OF_MEMORY;
            goto exitError;
        }
        (*ppBuffer)->length = length;
        wosMemCopy((*ppBuffer)->data, reply->str, (*ppBuffer)->length);
        ret = WOS_STORAGE_SUCCESS;
        goto exit;
    }

exitError:
    WOS_FREE_BUF_AND_DATA(*ppBuffer);
exit:
    freeReplyObject(reply);
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosStorageError_t
wosStorageWrite(void *pContext, WosString_t storageKey, WosBuffer_t *pBuffer)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    redisContext *internalCtx = NULL;
    redisReply *reply = NULL;
    FUNCTION_ENTRY();
    if (pContext == NULL) {
        WLOGI("Context is null.");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }
    if (!WOS_IS_VALID_STRING(storageKey)) {
        WLOGE("storageKey is invalid");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }
    if (!WOS_IS_VALID_BUFFER(pBuffer)) {
        WLOGE("pBuffer is invalid");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }

    internalCtx = pContext;
    reply = redisCommand(internalCtx, "SET %b %b", storageKey,
                         wosStringLength(storageKey), pBuffer->data,
                         pBuffer->length);
    /* Error */
    if (reply == NULL) {
        if (internalCtx->err != 0) {
            WLOGE("error: %s", internalCtx->errstr);
        } else {
            WLOGE("error");
        }
        ret = WOS_STORAGE_ERROR;
        goto exit;
    } else if (reply->type == REDIS_REPLY_ERROR) {
        WLOGE("error reply: %s", reply->str);
        ret = WOS_STORAGE_ERROR;
        goto exit;
    } else if (reply->type == REDIS_REPLY_STATUS) {
        /* Simple string reply: OK if SET was executed correctly. */
        if (wosStringComparison(reply->str, "OK") == 0) {
            ret = WOS_STORAGE_SUCCESS;
            goto exit;
        } else {
            WLOGI("error reply: %s", reply->str);
            ret = WOS_STORAGE_ERROR;
            goto exit;
        }
    }
    ret = WOS_STORAGE_ERROR;

exit:
    freeReplyObject(reply);
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

WosStorageError_t wosStorageDelete(void *pContext, WosString_t storageKey)
{
    WosStorageError_t ret = WOS_STORAGE_ERROR;
    redisContext *internalCtx = NULL;
    redisReply *reply = NULL;
    FUNCTION_ENTRY();
    if (pContext == NULL) {
        WLOGI("Context is null.");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }
    if (!WOS_IS_VALID_STRING(storageKey)) {
        WLOGE("storageKey is invalid");
        ret = WOS_STORAGE_ERROR_BAD_PARAMS;
        goto exit;
    }

    internalCtx = pContext;
    reply = redisCommand(internalCtx, "DEL %b", storageKey,
                         wosStringLength(storageKey));
    /* Error */
    if (reply == NULL) {
        if (internalCtx->err != 0) {
            WLOGE("error: %s", internalCtx->errstr);
        } else {
            WLOGE("error");
        }
        ret = WOS_STORAGE_ERROR;
        goto exit;
    } else if (reply->type == REDIS_REPLY_ERROR) {
        WLOGE("error reply: %s", reply->str);
        ret = WOS_STORAGE_ERROR;
        goto exit;
    } else if (reply->type == REDIS_REPLY_INTEGER) {
        /* The number of keys that were removed. */
        if (reply->integer == 1) {
            ret = WOS_STORAGE_SUCCESS;
            goto exit;
        } else {
            WLOGI("error reply: %d", reply->integer);
            ret = WOS_STORAGE_ERROR_NOT_FOUND;
            goto exit;
        }
    }
    ret = WOS_STORAGE_ERROR;
exit:
    freeReplyObject(reply);
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
