#ifndef _C_HWMON_H
#define _C_HWMON_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char* hwmon_connector_t;

void hwmon_connector_delete(hwmon_connector_t hwmon);

hwmon_connector_t hwmon_get(const char* hwmon_id);

hwmon_connector_t hwmon_all_get(void);

#ifdef __cplusplus
}
#endif

#endif
