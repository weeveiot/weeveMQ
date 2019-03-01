WeeveClientLibrary
==================

## Prerequisite (dev-machine):
Install git, gnu-binutils and libtool.
```shell
$ sudo apt-get install git libtool-bin binutils
```

## Build Dependencies

1. **WeeveOSCommon** is required, so sync it to **YOUR_WOS_ROOT_PATH** and build it following its own instructions.

2. Sync and build the external dependencies  - Storage and Crypto libraries

a. libtommath  
```shell
<WeeveClientLibrary>$ cd external
<WeeveClientLibrary/external>$ git clone https://github.com/libtom/libtommath.git
<WeeveClientLibrary/external>$ cd libtommath
<WeeveClientLibrary/external/libtommath>$ git checkout v1.0.1
<WeeveClientLibrary/external/libtommath>$ make -f makefile.shared CFLAGS="-fPIC"
<WeeveClientLibrary/external/libtommath>$ make
```

b. libtomcrypt  
```shell
<WeeveClientLibrary/external>$ git clone https://github.com/libtom/libtomcrypt.git
<WeeveClientLibrary/external>$ cd libtomcrypt
<WeeveClientLibrary/external/libtomcrypt>$ git checkout v1.18.2
<WeeveClientLibrary/external/libtomcrypt>$ make -f makefile.shared \
CFLAGS="-DUSE_LTM -DLTM_DESC -fPIC -I../libtommath" EXTRALIBS="../libtommath/libtommath.la" all
<WeeveClientLibrary/external/libtomcrypt>$ make CFLAGS="-DUSE_LTM -DLTM_DESC -I../libtommath" \
EXTRALIBS="../libtommath/libtommath.a" all
```

c. hiredis  
_Only for redis-based database storage, and **no longer maintained** as of now._
```shell
<WeeveClientLibrary/external>$ git clone https://github.com/redis/hiredis.git
<WeeveClientLibrary/external>$ cd hiredis
<WeeveClientLibrary/external/hiredis>$ make
<WeeveClientLibrary/external/hiredis>$ cd ..
```

_If Redis is to be used, run Redis in a separate terminal._
```shell
<Redis>$ wget http://download.redis.io/releases/redis-4.0.11.tar.gz
<Redis>$ tar xzf redis-4.0.11.tar.gz
<Redis>$ cd redis-4.0.11
<Redis/redis-4.0.11>$ make
<Redis/redis-4.0.11>$ src/redis-server
```

## Build WCL

3. Build WCL Static/Shared WCL Library  
_Choose the Purpose of Library: MQTTS_CLIENT or MQTTS_BROKER._  
_Choose the Storage Provider between: -DSTORAGE=REDIS or -DSTORAGE=STDC_FILE._  
_Crypto and Serializing Libraries currently have no alternative._  
_Choose log level: FATAL(0), ERROR(1), WARN(2), INFO(3), DEBUG(4), TRACE(5)._  
_Choose which library type to use: static or shared._
```shell
<WeeveClientLibrary>$ mkdir build && cd build
<WeeveClientLibrary/build>$ cmake .. -DLIB_SMP_ROLE=MQTTS_CLIENT \
-DCRYPTO=TOMCRYPT -DSTORAGE=STDC_FILE -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=5 \
-DWCL_LIB_TYPE=shared -DWOS_ROOT_DIR=<YOUR_WOS_ROOT_PATH>
<WeeveClientLibrary/build>$ make && make install
<WeeveClientLibrary/build>$ cmake .. -DLIB_SMP_ROLE=MQTTS_BROKER \
-DCRYPTO=TOMCRYPT -DSTORAGE=STDC_FILE -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=5 \
-DWCL_LIB_TYPE=shared -DWOS_ROOT_DIR=<YOUR_WOS_ROOT_PATH>
<WeeveClientLibrary/build>$ make && make install
```

4. Install WCL Static/Shared Library into export folder.
```shell
<WeeveClientLibrary/build>$ make install
```

## Build and Run Tests

5. Build Test  
_Choose which library type to use: static or shared._
```shell
<WeeveClientLibrary>$ cd test
<WeeveClientLibrary/test>$ mkdir build && cd build
<WeeveClientLibrary/test/build>$ cmake .. -DLOG_LEVEL=5 -DWCL_LIB_TYPE=static
<WeeveClientLibrary/test/build>$ make
```

6. Copy the test assets to build Library
```shell
<WeeveClientLibrary/test/build>$ cp ../data/* .
```

7. Run different module or unit tests  
For example:
```shell
<WeeveClientLibrary/test/build>$ ./TestWclSmp
```

## Certificate Tool

8. Build Certificate Tool  
_Choose which library type to use: static or shared._  
_Choose log level: FATAL(0), ERROR(1), WARN(2), INFO(3), DEBUG(4), TRACE(5)._  
```shell
<WeeveClientLibrary>$ cd tool
<WeeveClientLibrary/tool>$ mkdir build && cd build
<WeeveClientLibrary/tool/build>$ cmake .. -DLOG_LEVEL=5 -DWCL_LIB_TYPE=static
<WeeveClientLibrary/tool/build>$ make
```
