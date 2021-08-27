#include <capi/fan.h>
#include <capi/hwmon.h>
#include <capi/sysfs.h>
#include <capi/chips.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <iomanip>


using json = nlohmann::json;


const std::string I2CFILE = "i2cPath.json";
json jdevices;


void readJson()
{
    std::ifstream i(I2CFILE);
    if (!i.is_open()) {
        std::cout << "failed to open " << I2CFILE << '\n';
    } else {      
        i >> jdevices;
        i.close();
    }
}


int main()
{
 
    // test case 0
    hwmon_connector_t hwmon;
    hwmon = hwmon_all_get();
    std::cout << hwmon << std::endl;
    hwmon_connector_delete(hwmon);

    // test case 1
    hwmon_connector_t hwmon0;
    hwmon0 = hwmon_get("hwmon0");
    std::cout << hwmon0 << std::endl;
    hwmon_connector_delete(hwmon0);
    
    return 0;
}
