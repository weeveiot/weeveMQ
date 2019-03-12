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
 * 
 */

#ifndef WCL_COMMON_H_
#define WCL_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wclTypes.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

/* Init the WCL library */
WclError_t wclInit
(
  void
);

/* Terminate the WCL library */
void wclTerminate
(
  void
);

/* Free the output buffer created by WCL */
void wclFreeBuffer
(
		WosBuffer_t                 *pBuffer
);

#ifdef __cplusplus
}
#endif

#endif /* WCL_COMMON_H_ */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
