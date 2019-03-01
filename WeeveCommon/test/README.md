WeeveOSCommon
=============

## Build and Test Instructions

1. Follow instructions to build WOS

2. Build Test
```shell
<WeeveOSCommon/test/>$ mkdir build && cd build
<WeeveOSCommon/test/build>$ cmake .. -DWCL_ROOT_DIR=<YOUR_WCL_ROOT_PATH> -DLIB_SMP_ROLE=MQTTS_CLIENT -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=5 -DWCL_LIB_TYPE=static
<WeeveOSCommon/test/build>$ make
```

3. Run Test
```shell
<WeeveOSCommon/build>$ ./testWosCommon
```
