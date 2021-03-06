# weeveMQ
The weeveMQ is a secure, highly scalable, enterprise-ready IoT messaging broker. This version of weeveMQ is based on mosquitto release 1.5.2 and WeeveClientLibrary 0.0.1. WeeveMQ implements the MQTTS protocol, a publish and subscribe protocol inspired by the MQTT protocol and augmented with lightweight cryptography to securely publish and subscribe messages without the burden of the computationally expensive and for low-end IoT applications unsuitable SSL/TLS protocol. In a nutshell, whenever you look for secure messaging, but SSL/TLS is no option, then MQTTS is your choice! 
For details and performance comparisions to MQTT over TLS, visit [weeve.network](https://weeve.network)


## Install 

To install the MQTTS broker and client run the install script.
The script checks for all dependencies on your system and if needed installs missing packages.

1. Clone the weeveMQ repository to your maschine

2. Install the broker and client with all dependencies by executing the install script: `bash install.sh`

## Run Instructions

After the compilation of the broker and client run the following commands:

Run broker in command-prompt 1  
```shell
<WeeveMQTTSClient/build_mqttSMP>$ cd broker  
<WeeveMQTTSClient/build_mqttSMP/broker>$ ./mosquitto  
```

```shell
Run subscriber command-prompt 2  
<WeeveMQTTSClient/build_mqttSMP>$ cd subscriber  
<WeeveMQTTSClient/build_mqttSMP/subscriber>$ ./mosquitto_sub -d -t hello/world  
```

```shell
Run publisher in command-Pompt 3  
<WeeveMQTTSClient/build_mqttSMP>$ cd publisher  
<WeeveMQTTSClient/build_mqttSMP/publisher>$ ./mosquitto_pub -d -t hello/world -m "Hello world, this is weeve's MQTTS!"  
```

You should see in subscriber's command-prompt 2 message "Hello world, this is weeve's MQTTS!".  

## Manual Installation
For manual compilation of the broker and the client use the following commands: 

## Compile WeeveCommon
WeeveCommon represents an interface implementet by WeeveClientLibrary which currently uses the LibTomCrypt lib for cryptgraphic functions. 
Install tinycbor here, which is used to encode and decode messages inside of MQTTS and is used by WeeveClientLibrary

```shell
<WeeveCommon>$ cd external
<WeeveCommon/external>$ git clone https://github.com/intel/tinycbor.git
<WeeveCommon/external>$ cd tinycbor
<WeeveCommon/external>$ git checkout v0.5.2
<WeeveCommon/external/tinycbor>$ make
```

## Compile WeeveClientLibrary

The WCL provides the main functions to translate simple MQTT to MQTTS 

### Prerequisite (dev-machine):
Install git, gnu-binutils and libtool.
```shell
$ sudo apt-get install git libtool-bin binutils
```

### Build Dependencies

1. **WeeveCommon** is required, so sync it to **YOUR_WOS_ROOT_PATH** and build it following its own instructions.

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

### Build WCL

3. Build WCL Static/Shared WCL Library  
_Choose the Purpose of Library: MQTTS_CLIENT or MQTTS_BROKER._  
_Storage Provider currently have not alternative as: -DSTORAGE=STDC_FILE._  
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

### Build and Run Tests

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

### Certificate Tool

8. Build Certificate Tool  
_Choose which library type to use: static or shared._  
_Choose log level: FATAL(0), ERROR(1), WARN(2), INFO(3), DEBUG(4), TRACE(5)._  
```shell
<WeeveClientLibrary>$ cd tool
<WeeveClientLibrary/tool>$ mkdir build && cd build
<WeeveClientLibrary/tool/build>$ cmake .. -DLOG_LEVEL=5 -DWCL_LIB_TYPE=static
<WeeveClientLibrary/tool/build>$ make
```


## Compile Broker and Client

```shell
$ cd <WeeveMQTTSClient>  
<WeeveMQTTSClient>$ mkdir build && cd build  
<WeeveMQTTSClient/build>$ cmake ../mosquitto-1.5.2/ -DWITH_WEEVE_SMP=ON \
-DWITH_TLS=OFF -DWITH_TLS_PSK=OFF -DDOCUMENTATION=OFF -DCMAKE_BUILD_TYPE=Debug \
-DWCL_RELEASE_DIR=/PATH/TO/WeeveClientLibrary/export/  
<WeeveMQTTSClient/build>$ make  
```

Then copy the certificates (provided in the repository) to the binary files:

```shell
<WeeveMQTTSClient/build>$ mkdir -p broker && cp src/mosquitto broker/ && cp ../wcl/test_data/broker/* broker/
<WeeveMQTTSClient/build>$ mkdir -p publisher && cp client/mosquitto_pub publisher/ && cp ../wcl/test_data/publisher/* publisher/
<WeeveMQTTSClient/build>$ mkdir -p subscriber && cp client/mosquitto_sub subscriber/ && cp ../wcl/test_data/subscriber/* subscriber/
```



