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
 * @file smpInternalUtils.c
 * @brief
 * @version 0.1
 * @date 2019-02-26
 *
 * @copyright Copyright (c) 2019
 *
 */
/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */
#include "wosLog.h"

/* For SmpSessionContext_t definitions. */
#include "smpInternal.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "SMP_UTILS"

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

/* Generate a random string(length=32) used as id. */
WclError_t smpUtilsGen32CharRandomId(WosString_t randomId)
{
    WclError_t smpResult = WCL_ERROR;
    WosCryptoError_t ret = WOS_CRYPTO_ERROR;
    uint32_t inputStringLen = 0;
    uint8_t randData[16] = {0};
    WosBuffer_t randomBuffer = {.data = randData, .length = 16};
    const static char hexString[] = "0123456789ABCDEF";
    uint8_t i = 0;

    FUNCTION_ENTRY();

    /* Check input params. */
    if (NULL == randomId) {
        WLOGE("bad params");
        smpResult = WCL_ERROR_BAD_PARAMS;
        goto exit;
    }

    /* Generate 16-bytes random Bytes. */
    ret = wosCryptoGetRandomBytes(&randomBuffer);
    if ((WOS_CRYPTO_SUCCESS != ret) || (NULL == randomBuffer.data)) {
        WLOGE("generating random bytes failed");
        smpResult = WCL_ERROR_CRYPTO_OPERATION;
        goto exit;
    }

    for (i = 0; i < 16; ++i) {
        randomId[i * 2 + 0] = hexString[(randomBuffer.data[i] >> 4) & 0x0F];
        randomId[i * 2 + 1] = hexString[(randomBuffer.data[i]) & 0x0F];
    }
    randomId[33] = 0;
    smpResult = WCL_SUCCESS;

exit:
    FUNCTION_EXIT_RETURN(smpResult);
    return smpResult;
}
/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
