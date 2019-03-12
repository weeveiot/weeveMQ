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
 * @brief Implementation of Logging Functions using Lib C
 *
 * @file wosLogLibC.c
 * @date 2018-07-13
 *
 */

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosLog.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_MAX_LINE_SIZE 1024

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

const char *WEEVE_LOG_LEVEL_NAME[] = {"FATAL", "ERROR", "WARN",
                                      "INFO",  "DEBUG", "TRACE"};

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

void wosLogInternal(uint8_t logLevel,
                    const char *tag,
                    const char *functionName,
                    const char *fmt,
                    ...)
{
    char buffer[LOG_MAX_LINE_SIZE];
    size_t len = 0;
    int position = 0;

    time_t currentTime;
    struct tm *currentTimeStruct = NULL;
    char currentTimeString[50];

    va_list ap;
    va_start(ap, fmt);

    // Date & Time - using libc
    time(&currentTime);
    currentTimeStruct = localtime(&currentTime);
    strftime(currentTimeString, 50, "%F %T (%z)", currentTimeStruct);

    // Add a header
    len = sizeof(buffer);
    position = snprintf(buffer, len, "%s: %-5s [%s] %s ", currentTimeString,
                        WEEVE_LOG_LEVEL_NAME[logLevel], tag, functionName);

    // Concatenates the user content
    vsnprintf(buffer + position, len - position, fmt, ap);

    // Prints to system log
    printf("%s\n", buffer);

    va_end(ap);
    return;
}

void wosLogLicense(const char *fmt, ...)
{
    char buffer[LOG_MAX_LINE_SIZE];

    time_t currentTime;
    struct tm *currentTimeStruct = NULL;
    char currentTimeString[50];

    va_list ap;
    va_start(ap, fmt);

    // Date & Time - using libc
    time(&currentTime);
    currentTimeStruct = localtime(&currentTime);
    strftime(currentTimeString, 50, "%F %T (%z)", currentTimeStruct);

    // Add a header
    printf("%s\n", currentTimeString);

    // Then the user content
    vprintf(fmt, ap);

    va_end(ap);
    return;
}

void wosLogInternalBufferIndex(char message[],
                               uint8_t buffer[],
                               size_t len,
                               uint64_t initAddress)
{
    char string[69]; /* 20 + 16*3 + 1 (snprintf automatically adds /0 to the end
                        of its strings) */
    int strLen = 0, strPos = 0, bufPos = 0;

    if (message == NULL || buffer == NULL || len == 0) {
        return;
    }
    printf("%s\nBase Address:%p  Length:%lu, %#lx\n", message, buffer, len,
           len);
    strLen = sizeof(string);
    for (bufPos = 0; bufPos < len; bufPos++) {
        /* Start a new line (each should contain 16 bytes) */
        if (bufPos % 16 == 0) {
            /* 64 bits means 16 hex chars. */
            strPos += snprintf(string + strPos, strLen - strPos,
                               "%#016lx: ", initAddress + bufPos);
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

void wosLogInternalBuffer(uint8_t logLevel,
                          const char *tag,
                          const char *functionName,
                          char message[],
                          uint8_t buffer[],
                          size_t len)
{
    int bufPos = 0;
    time_t currentTime;
    struct tm *currentTimeStruct = NULL;
    char currentTimeString[50];
    if ((NULL == message) || (NULL == buffer) || (0 == len)) {
        return;
    }
    // Date & Time - using libc
    time(&currentTime);
    currentTimeStruct = localtime(&currentTime);
    strftime(currentTimeString, 50, "%F %T (%z)", currentTimeStruct);

    printf("%s: %-5s [%s] %s ", currentTimeString,
           WEEVE_LOG_LEVEL_NAME[logLevel], tag, functionName);
    printf("%s Address:%p Length:%#lx Data:[ ", message, buffer, len);
    for (bufPos = 0; bufPos < len; bufPos++) {
        printf("%02x ", buffer[bufPos]);
    }
    printf("]\n");
    return;
}

void wosLogInternalTrace(const char *tag,
                         const int enterOrExit,
                         const char *functionName,
                         uint32_t returnCode)
{
    if (enterOrExit == 0) {
        wosLogInternal(WEEVE_LOG_LEVEL_TRACE, tag, functionName, "Enter");
    } else if (enterOrExit == 1) {
        wosLogInternal(WEEVE_LOG_LEVEL_TRACE, tag, functionName, "Exit");
    } else if (enterOrExit == 2) {
        wosLogInternal(WEEVE_LOG_LEVEL_TRACE, tag, functionName,
                       "Exit returns %d", returnCode);
    } else {
        wosLogInternal(WEEVE_LOG_LEVEL_TRACE, tag, functionName, "Log");
    }
}

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
