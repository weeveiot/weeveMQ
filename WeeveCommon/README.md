WeeveCommon
=============

## Build Instructions

1. **WeeveClientLibrary** is required, so sync it to **YOUR_WCL_ROOT_PATH**.

2. Sync and build the tinycbor library
```shell
<WeeveCommon>$ cd external
<WeeveCommon/external>$ git clone https://github.com/intel/tinycbor.git
<WeeveCommon/external>$ cd tinycbor
<WeeveCommon/external>$ git checkout v0.5.2
<WeeveCommon/external/tinycbor>$ make
```

3. Build WCOM  
_Choose the Purpose of Library: MQTTS_CLIENT or MQTTS_BROKER._  
_Serializing Library currently has no alternative._  
_Choose log level: FATAL(0), ERROR(1), WARN(2), INFO(3), DEBUG(4), TRACE(5)._  
_Choose which library type to use: static or shared._  
```shell
<WeeveCommon>$ mkdir build && cd build
<WeeveCommon/build>$ cmake .. -DLIB_SMP_ROLE=MQTTS_CLIENT \
-DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=5 -DWCL_LIB_TYPE=static \
-DWCL_ROOT_DIR=<YOUR_WCL_ROOT_PATH>
<WeeveCommon/build>$ make
```
