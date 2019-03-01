/**
 * @brief Interface to Log Functions.
 * A log tag should be defined on the file that will call the log functions in
 * this file, like this:
 *     #define LOG_TAG "wosLogTest.c"
 * This log tag will be printed alongside the log messages to ease the
 * identification of them.
 * During compile time the define 'LOG_LEVEL' can be used to determine which
 * logs to print and which to ignore. For example: setting it to
 * WEEVE_LOG_LEVEL_WARN will ignore Info, Debug and Trace logs, but print
 * Warning, Error and Fatal logs.
 *
 * @file wosLog.h
 * @date 2018-07-13
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

#ifndef WOS_LOG_H
#define WOS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wosTypes.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define WEEVE_LOG_LEVEL_FATAL 0
#define WEEVE_LOG_LEVEL_ERROR 1
#define WEEVE_LOG_LEVEL_WARN 2
#define WEEVE_LOG_LEVEL_INFO 3
#define WEEVE_LOG_LEVEL_DEBUG 4
#define WEEVE_LOG_LEVEL_TRACE 5

#ifndef LOG_LEVEL
#define LOG_LEVEL WEEVE_LOG_LEVEL_FATAL
#endif

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

void wosLogInternal(uint8_t logLevel,
                    const char *tag,
                    const char *functionName,
                    const char *fmt,
                    ...);
void wosLogLicense(const char *fmt, ...);
void wosLogInternalBufferIndex(char message[],
                               uint8_t buffer[],
                               size_t len,
                               uint64_t initAddress);
void wosLogInternalBuffer(uint8_t logLevel,
                          const char *tag,
                          const char *functionName,
                          char message[],
                          uint8_t buffer[],
                          size_t len);
void wosLogInternalTrace(const char *tag,
                         const int enterOrExit,
                         const char *functionName,
                         uint32_t returnCode);

/**
 * @brief Function that logs fatal errors. Fatal errors are special errors that
 * affect the whole system, making it unusable. For example: HW problems,
 * missing libraries, etc.
 * @param[in] fmt a string that specifies how subsequent arguments are converted
 * for output.
 * @param[in] ...
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_FATAL
#define WLOGF(...)                                                             \
    wosLogInternal(WEEVE_LOG_LEVEL_FATAL, LOG_TAG, __FUNCTION__, __VA_ARGS__)
#else
#define WLOGF(...)                                                             \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs errors. Errors cannot be handled, and such
 * conditions should be further investigated. For example: Storage failures,
 * runtime errors, etc.
 * @param[in] fmt a string that specifies how subsequent arguments are converted
 * for output.
 * @param[in] ...
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_ERROR
#define WLOGE(...)                                                             \
    wosLogInternal(WEEVE_LOG_LEVEL_ERROR, LOG_TAG, __FUNCTION__, __VA_ARGS__)
#else
#define WLOGE(...)                                                             \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs warnings. Warning are conditions that can be
 * handled, but are significant. For example: A config file was not found and
 * default configs are used.
 * @param[in] fmt a string that specifies how subsequent arguments are converted
 * for output.
 * @param[in] ...
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_WARN
#define WLOGW(...)                                                             \
    wosLogInternal(WEEVE_LOG_LEVEL_WARN, LOG_TAG, __FUNCTION__, __VA_ARGS__)
#else
#define WLOGW(...)                                                             \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs information. Information on events are not errors,
 * but are important to the operation of the system. For example: First access
 * triggers database creation.
 * @param[in] fmt a string that specifies how subsequent arguments are converted
 * for output.
 * @param[in] ...
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_INFO
#define WLOGI(...)                                                             \
    wosLogInternal(WEEVE_LOG_LEVEL_INFO, LOG_TAG, __FUNCTION__, __VA_ARGS__)
#else
#define WLOGI(...)                                                             \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs debug information. Debug information is needed for
 * getting insight on values and execution path. For example: dump of buffers,
 * value of important arguments and variables, etc.
 * @param[in] fmt a string that specifies how subsequent arguments are converted
 * for output.
 * @param[in] ...
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_DEBUG
#define WLOGD(...)                                                             \
    wosLogInternal(WEEVE_LOG_LEVEL_DEBUG, LOG_TAG, __FUNCTION__, __VA_ARGS__)
#else
#define WLOGD(...)                                                             \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs the contents of a buffer in hexadecimal format.
 * @param[in] message a message to be printed before the buffer.
 * @param[in] buffer the pointer to the start of the buffer.
 * @param[in] len the length of the buffer to be printed.
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_DEBUG
#define WLOGD_BUFFER(message, buffer, len)                                     \
    wosLogInternalBuffer(WEEVE_LOG_LEVEL_DEBUG, LOG_TAG, __FUNCTION__,         \
                         message, buffer, len)
#else
#define WLOGD_BUFFER(message, buffer, len)                                     \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs the contents of a buffer in hexadecimal format,
 * with indexes printed on each line.
 * @param[in] buffer the pointer to the start of the buffer.
 * @param[in] len the length of the buffer to be printed.
 * @param[in] initAddress optional parameter that sets the address to be printed
 * as the initial buffer address.
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_DEBUG
#define WLOGD_BUFFER_INDEX(message, buffer, len, initAddress)                  \
    wosLogInternalBufferIndex(message, buffer, len, initAddress)
#else
#define WLOGD_BUFFER_INDEX(message, buffer, len, initAddress)                  \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs tracing information; i.e. information that can be
 * used to re-create the call stack. This function must be called by all
 * functions immediately after declaring all its variables and before any other
 * statement.
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_TRACE
#define FUNCTION_ENTRY() wosLogInternalTrace(LOG_TAG, 0, __FUNCTION__, 0)
#else
#define FUNCTION_ENTRY()                                                       \
    do {                                                                       \
    } while (0)
#endif

/**
 * @brief Function that logs tracing information; i.e. information that can be
 * used to re-create the call stack. This function must be called by all
 * functions immediately before returning.
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_TRACE
#define FUNCTION_EXIT() wosLogInternalTrace(LOG_TAG, 1, __FUNCTION__, 0)
#else
#define FUNCTION_EXIT()                                                        \
    do {                                                                       \
    } while (0)
#endif

/**
 * * @brief Function that logs tracing information; i.e. information that can be
 * used to re-create the call stack. This function must be called by all
 * functions that return a uint32_t value, before they return.
 * @param[in] returnCode the function return value.
 */
#if LOG_LEVEL >= WEEVE_LOG_LEVEL_TRACE
#define FUNCTION_EXIT_RETURN(returnCode)                                       \
    wosLogInternalTrace(LOG_TAG, 2, __FUNCTION__, returnCode)
#else
#define FUNCTION_EXIT_RETURN(returnCode)                                       \
    do {                                                                       \
    } while (0)
#endif

/**
 * Function that logs license and notice information. For example: print license
 * terms and conditions during initialization. This function is not affected by
 * LOG_LEVEL settings, and it always logs the provided information.
 * @param[in] fmt a string that specifies how subsequent arguments are converted
 * for output.
 * @param[in] ...
 */
#define WLOGLICENSE(...) wosLogLicense(__VA_ARGS__)

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif // WOS_LOG_H
