Weeve MQTTS Demo
================

Based on mosquitto release 1.5.2
Pull and build "WeeveClientLibrary" in the hikey device.

## Build Instructions for Mosquitto Library(2 versions, with TLS and with SMP)

```shell
$ cd <WeeveMQTTSClient>  
<WeeveMQTTSClient>$ mkdir build && cd build  
<WeeveMQTTSClient/build>$ mkdir tls smp  
<WeeveMQTTSClient/build>$ cd smp  
<WeeveMQTTSClient/build/smp>$ cmake ../../mosquitto-1.5.2/ -DWITH_WEEVE_SMP=ON \
-DWITH_TLS=OFF -DWITH_TLS_PSK=OFF -DDOCUMENTATION=OFF -DCMAKE_BUILD_TYPE=Debug \
-DWCL_RELEASE_DIR=/path/to/WeeveClientLibrary/export/  
<WeeveMQTTSClient/build/smp>$ make  
<WeeveMQTTSClient/build/smp>$ cd ../tls  
<WeeveMQTTSClient/build/tls>$ cmake ../../mosquitto-1.5.2/ -DWITH_TLS=ON -DWITH_TLS_PSK=OFF -DDOCUMENTATION=OFF -DCMAKE_BUILD_TYPE=Debug  
<WeeveMQTTSClient/build/tls>$ make  
```

## Build Instructions for Demo Publisher-Application(2 versions, with TLS and with SMP)
```shell
$cd <WeeveMQTTSClient/demos/current_sensor>  
$<WeeveMQTTSClient/demos/current_sensor>$ mkdir build && cd build  
$<WeeveMQTTSClient/demos/current_sensor/build>$ cmake ..  
$<WeeveMQTTSClient/demos/current_sensor/build>$ make  
$<WeeveMQTTSClient/demos/current_sensor/build>$ cmake .. -DWITH_WEEVE_SMP=ON -DWCL_RELEASE_DIR=/path/to/WeeveClientLibrary/export/  
$<WeeveMQTTSClient/demos/current_sensor/build>$ make  
```
2 Applications(mqttspoc-smp  mqttspoc-tls) has been generated

## Running Instructions for SMP(repeat same for TLS with relevant certificates)
Attach the Arduino to hikey(via USB cable).  

Run broker in command-prompt 1  
```shell
<WeeveMQTTSClient/build/smp/src>$ cp ../../../wcl/test_data/broker/* .  
<WeeveMQTTSClient/build/smp/src>$ ./mosquitto  
```

```shell
Run subscriber command-prompt 2  
<WeeveMQTTSClient/build/smp/client>$ cp ../../../wcl/test_data/subscriber/* .  
<WeeveMQTTSClient/build/smp/client>$ ./mosquitto_sub -d -t /weevepoc/ram/dev01  
```

```shell
Run demo-applications(publisher) in command-Pompt 3  
$<WeeveMQTTSClient/demos/current_sensor/build>$ cp ../../../wcl/test_data/publisher/* . 
$<WeeveMQTTSClient/demos/current_sensor/build>$./mqttspoc-smp  
```

You should see something like below in subscriber's command-prompt 2,  
Success: -0.03  
Success: -0.01  
Success: 0.04  
