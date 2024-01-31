# BMC Restful API
Read data from hardware sensors using libsensors. Get Json fromat data by Restful API.


## Run Restful API
    1. cd ${HOME}/go/bin
    2. ./go-server
    
## Test Restful API
    <http://localhost:8093/v1/>
    > BMC Restful API v1!

    <http://localhost:8093/openconfig-platform:components/component=hwmon>
    > Get all howmon devices

    <http://localhost:8093/openconfig-platform:components/component={hwmon_id}>
    > Get one howmon device

## APIs  

- hwmon_connector_t hwmon_all_get(void)
- hwmon_connector_t hwmon_get(const char* hwmon_id)

## Libraries 

#### 1 Libsensors
- Use lm-sensors lib to parsing /sys/class/hwmon files. libsensors privoide the following APIs to get hwmon strings. 

    - int sysfs_read_chips(void)
    - int sysfs_read_one_chip(const char *hwmon_path)

#### 2 Libsensors Header Files
    libcgocommon/inlcude/capi/access.h
    libcgocommon/inlcude/capi/error.h
    libcgocommon/inlcude/capi/general.h
    libcgocommon/inlcude/capi/sysfs.h

#### 3 Libsensors Source Codes
    libcgocommon/src/access.cpp 
    libcgocommon/src/error.cpp
    libcgocommon/src/general.cpp
    libcgocommon/src/sysfs.cpp

## Documentation

- Naming and data format standards for sysfs files  
    https://docs.kernel.org/hwmon/sysfs-interface.html
- lm-sensors   
    https://github.com/lm-sensors/lm-sensors
    https://wiki.archlinux.org/title/lm_sensors
