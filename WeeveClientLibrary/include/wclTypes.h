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
 * @brief 
 */


#ifndef WCL_TYPES_H_
#define WCL_TYPES_H_

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

/* Invalid session. */
#define WCL_SESSION_INVALID (void*)(-1)

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* WCL session type. */
typedef void* WclSession_t;

typedef enum {
  /* Operation successful. */
  WCL_SUCCESS = 0,
  /* Unknown error. */
  WCL_ERROR = 1,
  /* Bad parameters. */
  WCL_ERROR_BAD_PARAMS = 2,
  /* Bad session. */
  WCL_ERROR_BAD_SESSION = 3,
  /* Error during dynamic memory allocation. */
  WCL_ERROR_OUT_OF_MEMORY = 4,
  /* Error while doing a persistant storage operation. */
  WCL_ERROR_STORAGE_OPERATION = 5,
  /* Error while doing a crypto operation. */
  WCL_ERROR_CRYPTO_OPERATION = 6,
  /* Error while doing a serialization/deserialization(e.g. using CBOR
     packing/unpacking) operation. */
  WCL_ERROR_SERIALIZATION = 7,
  /* The message field or values are not according to spec. */
  WCL_ERROR_INVALID_MESSAGE = 8,
  /* Error unknown. */
  WCL_ERROR_UNKNOWN = 9
} WclError_t;

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* WCL_COMMON_H_ */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
