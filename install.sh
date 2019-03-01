#!/bin/bash

# Change this path to your preferred folder.
BUILD_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# Change the log level to your preferred one.
SCRIPT_LOG_LEVEL=3

# Don't change below. These are all relative paths
WCL_ROOT=$BUILD_ROOT/WeeveClientLibrary
WCL_EXTERNAL=$WCL_ROOT/external
WCL_TEST=$WCL_ROOT/test/build
WCL_TOOL=$WCL_ROOT/tool/build

WOS_ROOT=$BUILD_ROOT/WeeveCommon
WOS_EXTERNAL=$WOS_ROOT/external
WOS_TEST=$WOS_ROOT/test/build

MOSQ_ROOT=$BUILD_ROOT/WeeveMQTTSClient

TOOL_ROOT=$BUILD_ROOT/CertTool

LOGS_ROOT=$BUILD_ROOT/logs


checkInstallPackage() {
    if ! which $1 > /dev/null; then
        echo -e "$1 not found! Install $2? (y/n) \c"
        read USER_INPUT
        if [ "$USER_INPUT" == "y" ]; then
            sudo apt-get install $2
        fi
    fi
}

checkInstallLibrary() {
	if ! dpkg -s "$1" > /dev/null 2>&1; then
		echo -e "$1 not found! Install $1? (y/n) \c"
        read USER_INPUT
        if [ "$USER_INPUT" == "y" ]; then
            sudo apt-get install $1
        fi
    fi
}


init () {
    checkInstallPackage "git" "git"
    checkInstallPackage "g++" "g++"
    checkInstallPackage "cmake" "cmake"
    checkInstallPackage "libtool" "libtool-bin"
    checkInstallPackage "ranlib" "gnu-binutils"
    checkInstallPackage "openssl" "openssl"
    checkInstallLibrary "libssl-dev"

}

syncRepos() {
    
    echo -e "Would you like to install weeveMQ to $BUILD_ROOT? (y/n) \c"
    read USER_INPUT
    if [ "$USER_INPUT" == "n" ]; then
        exit
    fi
    cd $BUILD_ROOT

    # WOS
    mkdir -p $WOS_EXTERNAL && cd $WOS_EXTERNAL
    echo "Pulling tinycbor..."
    git clone https://github.com/intel/tinycbor.git
    cd $WOS_EXTERNAL/tinycbor
    git checkout v0.5.2

    # WCL
    mkdir -p $WCL_EXTERNAL && cd $WCL_EXTERNAL
    echo "Pulling tommath..."
    git clone https://github.com/libtom/libtommath.git
    cd $WCL_EXTERNAL/libtommath
    git checkout v1.0.1
    echo "Pulling tomcrypt..."
    cd $WCL_EXTERNAL
    git clone https://github.com/libtom/libtomcrypt.git
    cd $WCL_EXTERNAL/libtomcrypt
    git checkout v1.18.2

    echo "Syncing all repos done"
}

buildWos() {
    echo "Build WOS dependency tinycbor..."
    cd $WOS_EXTERNAL/tinycbor
    # Static + Shared
    make
}

testWos() {
    echo "Test WOS..."
    mkdir -p $WOS_TEST && cd $WOS_TEST
    cmake .. -DLIB_SMP_ROLE=MQTTS_CLIENT -DWCL_LIB_TYPE=static -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=$SCRIPT_LOG_LEVEL -DWCL_ROOT_DIR=$WCL_ROOT -DCMAKE_BUILD_TYPE=Debug
    make
    echo "Running test cases..."
    mkdir -p $LOGS_ROOT
    ./testWosCommon 2>&1 | tee $LOGS_ROOT/result_wos.log
}

buildWclLibs() {
    echo "Build WCL dependency tommath..."
    cd $WCL_EXTERNAL/libtommath
    make clean
    # Shared
    make -f makefile.shared
    # Static
    make

    echo "Build WCL dependency tomcrypt..."
    cd $WCL_EXTERNAL/libtomcrypt
    make clean
    # Shared
    make -f makefile.shared CFLAGS="-g -DUSE_LTM -DLTM_DESC -I../libtommath" EXTRALIBS="../libtommath/libtommath.la" all
    # Static
    make CFLAGS="-DUSE_LTM -DLTM_DESC -I../libtommath" EXTRALIBS="../libtommath/libtommath.a" all
}

buildWcl() {
    echo "Build & Install WCL..."
    cd $WCL_ROOT
    rm -rf build
    # Shared & Client
    mkdir -p build && cd build
    cmake .. -DLIB_SMP_ROLE=MQTTS_CLIENT -DWCL_LIB_TYPE=shared -DCRYPTO=TOMCRYPT -DSTORAGE=STDC_FILE -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=$SCRIPT_LOG_LEVEL -DWOS_ROOT_DIR=$WOS_ROOT -DCMAKE_BUILD_TYPE=Debug
    make install
    # Shared & Broker
    rm * -rf
    cmake .. -DLIB_SMP_ROLE=MQTTS_BROKER -DWCL_LIB_TYPE=shared -DCRYPTO=TOMCRYPT -DSTORAGE=STDC_FILE -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=$SCRIPT_LOG_LEVEL -DWOS_ROOT_DIR=$WOS_ROOT -DCMAKE_BUILD_TYPE=Debug
    make install
    # Static & Client
    rm * -rf
    cmake .. -DLIB_SMP_ROLE=MQTTS_CLIENT -DWCL_LIB_TYPE=static -DCRYPTO=TOMCRYPT -DSTORAGE=STDC_FILE -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=$SCRIPT_LOG_LEVEL -DWOS_ROOT_DIR=$WOS_ROOT -DCMAKE_BUILD_TYPE=Debug
    make install
    # Static & Broker
    rm * -rf
    cmake .. -DLIB_SMP_ROLE=MQTTS_BROKER -DWCL_LIB_TYPE=static -DCRYPTO=TOMCRYPT -DSTORAGE=STDC_FILE -DMESSAGE_PACK=TINYCBOR -DLOG_LEVEL=$SCRIPT_LOG_LEVEL -DWOS_ROOT_DIR=$WOS_ROOT -DCMAKE_BUILD_TYPE=Debug
    make install
}

testWcl() {
    echo "Test WCL..."
    mkdir -p $WCL_TEST && cd $WCL_TEST
    cmake .. -DWCL_LIB_TYPE=static -DLOG_LEVEL=$SCRIPT_LOG_LEVEL 
    make

    echo "Copy test data..."
    #Copy the test assets to build Library
    cp $WCL_ROOT/test/data/* $WCL_TEST

    echo "Running test cases..."
    mkdir -p $LOGS_ROOT
    ./TestWosCert 2>&1 | tee $LOGS_ROOT/result_cert.log
    ./TestWosCrypto 2>&1 | tee $LOGS_ROOT/result_crypto.log
    ./TestWosMemory 2>&1 | tee $LOGS_ROOT/result_memory.log
    ./TestWosStorage 2>&1 | tee $LOGS_ROOT/result_storage.log
    ./TestWosString 2>&1 | tee $LOGS_ROOT/result_string.log
    ./TestWclSmp 2>&1 | tee $LOGS_ROOT/result_smp.log
}

buildMosquittoSmp() {
    cd $MOSQ_ROOT
    rm -rf build_mqttSMP
    mkdir -p build_mqttSMP && cd build_mqttSMP
    cmake ../mosquitto-1.5.2/ -DWITH_WEEVE_SMP=ON -DWITH_TLS=OFF -DWITH_TLS_PSK=OFF -DDOCUMENTATION=OFF -DCMAKE_BUILD_TYPE=Debug -DWCL_RELEASE_DIR=$WCL_ROOT/export/ -DWITH_STATIC_LIBRARIES=ON -DCMAKE_BUILD_TYPE=Debug
    make
    #Copy the binary and certificates
    mkdir -p broker && cp src/mosquitto broker/ && cp ../wcl/test_data/broker/* broker/
    mkdir -p publisher && cp client/mosquitto_pub publisher/ && cp ../wcl/test_data/publisher/* publisher/
    mkdir -p subscriber && cp client/mosquitto_sub subscriber/ && cp ../wcl/test_data/subscriber/* subscriber/
}



buildTool() {
    echo "Build Tool..."
    mkdir -p $WCL_TOOL && cd $WCL_TOOL
    cmake .. -DWCL_LIB_TYPE=static -DLOG_LEVEL=$SCRIPT_LOG_LEVEL -DCMAKE_BUILD_TYPE=Debug
    make
    mkdir -p $TOOL_ROOT
    cp $WCL_TOOL/WeeveCertTool $TOOL_ROOT/
}

runMosquitto() {
    echo ""
    echo ""
    echo ""
    echo "Build has been completed. Please check log for errors."
    echo "Instructions for running mosquitto broker and clients."
    echo "It is needed to use 3 terminal windows:"
    echo ""
    echo "1."
    echo "cd "$MOSQ_ROOT"/build_mqttSMP/broker/"
    echo "./mosquitto"
    echo ""
    echo "2."
    echo "cd "$MOSQ_ROOT"/build_mqttSMP/subscriber/"
    echo "./mosquitto_sub -d -t hello/world"
    echo ""
    echo "3."
    echo "cd "$MOSQ_ROOT"/build_mqttSMP/publisher/"
    echo "./mosquitto_pub -d -t hello/world -m \"Hello world, this is weeve\'s MQTTS!\""
    echo ""
}


init
syncRepos
buildWos
testWos
buildWclLibs
buildWcl
testWcl
buildTool
buildMosquittoSmp
runMosquitto
