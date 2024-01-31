# bmc_restful
    Read data from hardware sensors using libsensors. Get Json fromat data by Restful API.

## APIs  

    * hwmon_connector_t hwmon_all_get(void)
    * hwmon_connector_t hwmon_get(const char* hwmon_id)

## Libraries 

#### Libsensors
    Use lm-sensors lib to parsing /sys/class/hwmon files. 

    libsensors privoide the following APIs to get hwmon strings. 

    * int sysfs_read_chips(void)
    * int sysfs_read_one_chip(const char *hwmon_path)

#### Libsensors Header Files
    libcgocommon/inlcude/capi/access.h
    libcgocommon/inlcude/capi/error.h
    libcgocommon/inlcude/capi/general.h
    libcgocommon/inlcude/capi/sysfs.h

#### Libsensors Source Codes
    libcgocommon/src/access.cpp 
    libcgocommon/src/error.cpp
    libcgocommon/src/general.cpp
    libcgocommon/src/sysfs.cpp

## Documentation

    1.  Naming and data format standards for sysfs files
        https://docs.kernel.org/hwmon/sysfs-interface.html
    2.  lm-sensors 
        https://github.com/lm-sensors/lm-sensors
        https://wiki.archlinux.org/title/lm_sensors
