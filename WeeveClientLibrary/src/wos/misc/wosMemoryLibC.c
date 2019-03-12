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
 * @brief Implementation of Memory Functions using Lib C
 *
 * @file wosMemoryLibC.c
 * @date 2018-07-13
 * 
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosCommon.h"
#include "wosLog.h"
#include "wosMemory.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "MEM_LIBC"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

void *wosMemSet(void *s, uint8_t c, size_t n)
{
    void *ret = NULL;
    FUNCTION_ENTRY();
    if (NULL != s) {
        ret = memset(s, c, n);
    }
    FUNCTION_EXIT();
    return ret;
}

void *wosMemCopy(void *dest, const void *src, size_t n)
{
    void *ret = NULL;
    FUNCTION_ENTRY();
    if ((NULL != dest) && (NULL != src)) {
        ret = memcpy(dest, src, n);
    }
    FUNCTION_EXIT();
    return ret;
}

void *wosMemAlloc(size_t size)
{
    int mallocError = 0;
    void *ret = NULL;
    FUNCTION_ENTRY();
    if (size == 0) {
        goto exit;
    }
    ret = malloc(size);
    if (ret == NULL) {
        mallocError = errno;
        WLOGE("wosMemAlloc error. size: %lu, errno: %d, error description: %s",
              size, mallocError, strerror(mallocError));
    }
exit:
    FUNCTION_EXIT();
    return ret;
}

void wosMemFree(void *ptr)
{
    FUNCTION_ENTRY();
    if (NULL != ptr) {
        free(ptr);
    }
    FUNCTION_EXIT();
}

int32_t
wosMemComparison(uint8_t *pFirstBuffer, uint8_t *pSecondBuffer, size_t len)
{
    int32_t ret = 0;
    FUNCTION_ENTRY();
    if (pFirstBuffer == NULL) {
        ret = -1;
        goto exit;
    }
    if (pSecondBuffer == NULL) {
        ret = 1;
        goto exit;
    }
    ret = memcmp(pFirstBuffer, pSecondBuffer, len);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

uint8_t wosMemComparisonConstTime(uint8_t *pFirstBuffer,
                                  uint8_t *pSecondBuffer,
                                  size_t len)
{
    uint8_t ret = 0;
    size_t i = 0;

    FUNCTION_ENTRY();
    if (pFirstBuffer == NULL || pSecondBuffer == NULL) {
        ret = 1;
        goto exit;
    }

    /* Compares without branching (except for checking length). */
    for (i = 0; i < len; ++i) {
        ret |= pFirstBuffer[i] ^ pSecondBuffer[i];
    }

    /* Reduces difference to only one bit. */
    ret |= ret >> 4;
    ret |= ret >> 2;
    ret |= ret >> 1;
    ret &= 1;

exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
