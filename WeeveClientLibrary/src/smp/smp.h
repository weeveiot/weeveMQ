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
 * @file smp.h
 * @brief 
 * @version 0.1
 * @date 2019-02-26
 * 
 */

#ifndef SMP_H_
#define SMP_H_

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

WclError_t smpInit
(
  void
);

// Terminate the SMP component
void smpTerminate
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* SMP_H_ */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
