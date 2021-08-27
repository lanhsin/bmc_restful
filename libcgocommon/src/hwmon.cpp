#include <capi/hwmon.h>
#include <capi/chips.h>
#include <iostream>
#include <cstring>
#include <string>

extern "C" void hwmon_connector_delete(hwmon_connector_t hwmon)
{
    delete hwmon;
}


extern "C" hwmon_connector_t hwmon_get(const char* hwmon_id)
{
    std::string path("/sys/class/hwmon");
    std::string fullPath = path + '/' + hwmon_id;

    int ret = sysfs_read_one_chip(fullPath.c_str());
    // Empty features or errors occur
    if(ret <= 0)
        return nullptr;

    // Confirm to get only one chip
    if(chips_get_count() != 1){
        chips_cleanup();
        return nullptr;
    }

    sensors_chip_name *name = chip_get_name(0);
    if(name == nullptr){
        chips_cleanup();
        return nullptr;
    }

    std::stringstream ss;
    chip_dump_json(ss, name);
    const std::string & s = ss.str();
    hwmon_connector_t c = static_cast<hwmon_connector_t>(calloc(s.size()+1, sizeof(char)));
    std::memcpy(c, s.c_str(), s.size());

    chips_cleanup();

    return c;
}

extern "C" hwmon_connector_t hwmon_all_get()
{
    int ret = sysfs_read_chips();
    if(ret <= 0)
    	return nullptr;

    std::stringstream ss;
    chips_dump_json(ss);
    const std::string & s = ss.str();
    hwmon_connector_t c = static_cast<hwmon_connector_t>(calloc(s.size()+1, sizeof(char)));
    std::memcpy(c, s.c_str(), s.size());

    chips_cleanup();

    return c;
}

/*
extern "C" table_t hwmon_post(db_connector_t db, const char *tableName)
{
    auto pt = new swss::Table(static_cast<swss::DBConnector*>(db), std::string(tableName));
    return static_cast<table_t>(pt);
}
*/

/*
extern "C" bool hwmon_del(table_t pt)
{
    delete static_cast<swss::Table*>(pt);
}
*/
