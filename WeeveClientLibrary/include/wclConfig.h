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


#ifndef WCL_CONFIG_H_
#define WCL_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

/* Message Header Client ID Length. */
#define WCL_SMP_CLIENT_ID_LENGTH (0x20)

/* Cipher scheme IDs. */
#define WCL_SMP_CIPHER_SCHEME_ID0 (0x00)

/* The location of Root-CA certificate. */
#define WCL_SMP_ROOT_CA_CERT_PATH "root.wcr"

/* The location of self certificate.
 * The self-certificate is signed by root-CA, there is no intermediate CA 
 * support as of now. */
#define WCL_SMP_SELF_CERT_PATH "self.wcr"

/* The location of self signing key. */
#define WCL_SMP_SELF_SIGNING_KEY_PATH "self.key"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Function Declarations                       */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* WCL_CONFIG_H_ */

/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
