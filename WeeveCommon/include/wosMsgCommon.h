
/**
 * @brief Messaging MSG component implements the interfaces to build and parse
 * (serialize and deserialize) messages in a binary or other packing format.
 * The message structure is defined in following documents:
 *  1. "Secure Messaging Protocol" specification
 *
 * This interface abstracts the packing/unpacking logic of a container format
 * like CBOR. MessagePack etc.
 *
 * The APIs and data structure is splited in several header files based on
 * MessageContext catogries described in various specifications.
 *
 * This header contains the definitions which are commonn for all messages.
 *
 * @file wosMsgCommon.h
 * @date 2018-07-27
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

#ifndef WOS_MSG_COMMON_H
#define WOS_MSG_COMMON_H

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

/* =======SMP Specification 2.1.1 Message Context Definitions ================*/
/* Communication Message Context, define values in range from 0x00 to 0x0F */
/* SMP MQTTS Message Context */
#define WOS_MSG_CONTEXT_SMP_MQTTS (0x00)

/* Device Characteristics Message Context, define values in range from 0x10 to
 * 0x1F */
#define WOS_MSG_CONTEXT_DEVICE_CHARS (0x10)

/* Economy of Things Message Context, define values in range from 0x20
 * to 0x2F */
#define WOS_MSG_CONTEXT_EOT (0x20)
/* Define other message contexts here. */

/* Message Context Undefined */
#define WOS_MSG_CONTEXT_UNDEFINED (0xFF)

/* Message Type Undefined */
#define WOS_MSG_MESSAGE_TYPE_UNDEFINED (0xFF)

/** Message Type Version is not defined, so version field of header is not
 * relevant and must be ignored during encoding and parsing.
 */
#define WOS_MSG_MESSAGE_TYPE_VERSION_UNDEFINED (0xFF)
/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/**
 * @brief Msg Status.
 */
typedef enum tWosMsgError {
  /* Operation successful. */
  WOS_MSG_SUCCESS = 0,
  /* Generic error during processing. */
  WOS_MSG_ERROR = 1,
  /* Bad input parameters. */
  WOS_MSG_ERROR_BAD_PARAMS = 2,
  /* Out of memory. */
  WOS_MSG_ERROR_OUT_OF_MEMORY = 3,
  /* Bad message context, type or version. */
  WOS_MSG_ERROR_BAD_MESSAGE = 4,
  /* Bad serialized format. */
  WOS_MSG_ERROR_BAD_FORMAT = 5
} WosMsgError_t;

/**
 * @brief Msg Header data.
 *
 * Ref. Msg Specification Section 2.1
 */
typedef struct tWosMsgCommonHeader {
  /* Context of message. */
  uint8_t messageContext;
  /* Type of message. */
  uint8_t messageContextVersion;
} WosMsgCommonHeader_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* WOS_MSG_COMMON_H */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
