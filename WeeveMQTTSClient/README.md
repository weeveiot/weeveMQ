Weeve MQTTS Client
==================

Based on mosquitto release 1.5.2 and WCL release 0.0.1

## Build Instructions

```shell
$ cd <WeeveMQTTSClient>  
<WeeveMQTTSClient>$ mkdir build && cd build  
<WeeveMQTTSClient/build>$ cmake ../mosquitto-1.5.2/ -DWITH_WEEVE_SMP=ON \
-DWITH_TLS=OFF -DWITH_TLS_PSK=OFF -DDOCUMENTATION=OFF -DCMAKE_BUILD_TYPE=Debug \
-DWCL_RELEASE_DIR=/path/to/WeeveClientLibrary/export/  
<WeeveMQTTSClient/build>$ make  
```

## Certificate Provisioning
Copy the test data(certificates and private-keys) in respective build folder of client and broker.
(for just testing purpose -  we are using just one set of keys as of now)
```shell
<WeeveMQTTSClient/build>$ mkdir -p broker && cp src/mosquitto broker/ && cp ../wcl/test_data/broker/* broker/
<WeeveMQTTSClient/build>$ mkdir -p publisher && cp client/mosquitto_pub publisher/ && cp ../wcl/test_data/publisher/* publisher/
<WeeveMQTTSClient/build>$ mkdir -p subscriber && cp client/mosquitto_sub subscriber/ && cp ../wcl/test_data/subscriber/* subscriber/
``` 

## Running Instructions

Run broker in command-prompt 1  
```shell
<WeeveMQTTSClient/build>$ cd broker  
<WeeveMQTTSClient/build/broker>$ ./mosquitto  
```

```shell
Run subscriber command-prompt 2  
<WeeveMQTTSClient/build>$ cd subscriber  
<WeeveMQTTSClient/build/subscriber>$ ./mosquitto_sub -d -t hello/world  
```

```shell
Run publisher in command-Pompt 3  
<WeeveMQTTSClient/build>$ cd publisher  
<WeeveMQTTSClient/build/publisher>$ ./mosquitto_pub -d -t hello/world -m "Hello world, this is weeve's MQTTS!"  
```

You should see in subscriber's command-prompt 2, message "Hello world, this is weeve's MQTTS!" is displayed.  
