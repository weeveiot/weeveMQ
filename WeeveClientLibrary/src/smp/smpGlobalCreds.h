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
 * @file smpGlobalCreds.h
 * @brief
 * @version 0.1
 * @date 2019-02-26
 *
 */

#ifndef SMP_GLOBAL_CREDS_H_
#define SMP_GLOBAL_CREDS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "smpInternal.h"
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

/* Init the global crypto assets. */
WclError_t smpInitGlobalCreds(void);

/* Destroy the global crypto assets. */
WclError_t smpDeInitGlobalCreds(void);

/* Config the path of different certificate and key files in Smp Context. */
WclError_t smpConfigGlobalCreds(SmpSessionContext_t *pSmpCtx);

#ifdef __cplusplus
}
#endif

#endif /* SMP_GLOBAL_CREDS_H_ */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
