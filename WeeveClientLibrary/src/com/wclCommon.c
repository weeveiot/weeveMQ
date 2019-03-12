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

/*
 * 
 * \file wclCommon.c
 *  Created on: 23-Jun-2018
 * 
 */
/* ========================================================================== */
/*                                Includes                                    */
/* ========================================================================== */

#include "wclCommon.h"
#include "smp.h"
#include "wclTypes.h"
#include "wosCommon.h"
#include "wosCrypto.h"
#include "wosLog.h"
#include "wclVersion.h"

/* ========================================================================== */
/*                                Constants                                   */
/* ========================================================================== */

#define LOG_TAG "WCL_COMMON"

/* ========================================================================== */
/*                                Types                                       */
/* ========================================================================== */

/* ========================================================================== */
/*                                Global Variables                            */
/* ========================================================================== */

/* ========================================================================== */
/*                                Local Function Declarations                 */
/* ========================================================================== */

/* TODO Log license, version, type, etc */
void wclLog();

/* ========================================================================== */
/*                                Local Function Definitions                  */
/* ========================================================================== */

void wclLog()
{
    /* License */
    /* TODO Add a license to files, and log it here as well. */
    WLOGLICENSE("Weeve Client Library\nLicense Information...\n\n");

    /* Terms and Conditions */
    /* TODO Add a T&C for using the library. */
    WLOGLICENSE("Weeve Client Library\nTerms and Conditions...\nEnd-User "
                "License Agreement...\n\n");

    /* Type */
#if defined(SMP_MQTTS_CLIENT)
    WLOGLICENSE("Library built with LIB_SMP_ROLE: SMP_MQTTS_CLIENT\n\n");
#endif
#if defined(SMP_MQTTS_BROKER)
    WLOGLICENSE("Library built with LIB_SMP_ROLE: SMP_MQTTS_BROKER\n\n");
#endif

    /* Version */
    /* TODO Add the version to the build scripts, and log it here. */
    WLOGLICENSE("Weeve Client Library\nversion %d.%d revision %d\n\n", WCL_VERSION_MAJOR, WCL_VERSION_MINOR, WCL_VERSION_REVISION );
}

/* ========================================================================== */
/*                                Implementation                              */
/* ========================================================================== */

/* Initialize the WCL library,
   Any global configuration for all components need to be initialized here. */
WclError_t wclInit(void)
{
    WclError_t wclStatus = WCL_ERROR;
    WosCryptoError_t cryptoStatus = WOS_CRYPTO_ERROR;
    WosCryptoConfig_t *pCryptoConfig = NULL;

    FUNCTION_ENTRY();

    wclLog();

    /* Crypto component initialization. */
    cryptoStatus = wosCryptoInitialize(pCryptoConfig);
    if (WOS_CRYPTO_SUCCESS != cryptoStatus) {
        WLOGE("crypto init failed %x", cryptoStatus);
        goto exitCrypto;
    }

    /* SMP component initialization. */
    wclStatus = smpInit();
    if (WCL_SUCCESS != wclStatus) {
        WLOGE("SMP init failed %x", wclStatus);
        goto exitSmp;
    }

    wclStatus = WCL_SUCCESS;
    goto exitCrypto;

exitSmp:
    wosCryptoTerminate();

exitCrypto:
    FUNCTION_EXIT_RETURN(wclStatus);
    return wclStatus;
}

/* Terminate the WCL library. */
void wclTerminate(void)
{

    FUNCTION_ENTRY();

    /* Crypto component termination. */
    wosCryptoTerminate();

    /* SMP component termination. */
    smpTerminate();

    FUNCTION_EXIT();
    return;
}

/* Free the output buffer created by WCL */
void wclFreeBuffer(WosBuffer_t *pBuffer)
{
    FUNCTION_ENTRY();
    WOS_FREE_DATA(pBuffer);
    FUNCTION_EXIT();
    return;
}
/* ========================================================================== */
/*                                End of File                                 */
/* ========================================================================== */
