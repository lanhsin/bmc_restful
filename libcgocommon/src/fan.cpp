#include <capi/fan.h>
#include <iostream>
#include <cstring>
#include <string>


extern "C" void fan_connector_delete(fan_connector_t fan)
{
    delete fan;
}


extern "C" fan_connector_t fan_get(const char* fan_id)
{
    fan_connector_t ret = new char[std::strlen(fan_id) + 1];
    std::strcpy(ret, fan_id);
    return ret;
}
/*
extern "C" table_t fan_post(db_connector_t db, const char *tableName)
{
    auto pt = new swss::Table(static_cast<swss::DBConnector*>(db), std::string(tableName));
    return static_cast<table_t>(pt);
}
*/

/*
extern "C" bool fan_del(table_t pt)
{
    delete static_cast<swss::Table*>(pt);
}
*/


