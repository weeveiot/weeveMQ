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
 * @brief Implementation of String Functions using Lib C
 *
 * @file wosStringLibC.c
 * @date 2018-07-13
 * 
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosLog.h"
#include "wosMemory.h"
#include "wosString.h"
#include <string.h>

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "STRING_LIBC"

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

WosString_t wosStringConcatenate(WosString_t firstString,
                                 WosString_t secondString)
{
    WosString_t ret = NULL;
    size_t len;
    FUNCTION_ENTRY();
    if (!WOS_IS_VALID_STRING(firstString) ||
        !WOS_IS_VALID_STRING(secondString)) {
        goto exit;
    }

    len = strlen(firstString) + strlen(secondString) + 1;
    ret = wosMemAlloc(len);
    if (ret == NULL) {
        WLOGE("could not allocate: %lu", len);
        goto exit;
    }
    // No point on using the str 'n' functions.
    // We have just allocated it using the length of the strings.
    ret = strcpy(ret, firstString);
    ret = strcat(ret, secondString);

exit:
    FUNCTION_EXIT();
    return ret;
}

size_t wosStringLength(WosString_t string)
{
    size_t ret = 0;
    FUNCTION_ENTRY();
    if (string != NULL) {
        ret = strlen(string);
    }
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

int32_t wosStringComparison(WosString_t firstString, WosString_t secondString)
{
    int32_t ret = 0;
    FUNCTION_ENTRY();
    if (firstString == NULL) {
        ret = -1;
        goto exit;
    }
    if (secondString == NULL) {
        ret = 1;
        goto exit;
    }
    ret = strcmp(firstString, secondString);
exit:
    FUNCTION_EXIT_RETURN(ret);
    return ret;
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
