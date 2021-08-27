#ifndef _C_FAN_H
#define _C_FAN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char* fan_connector_t;

void fan_connector_delete(fan_connector_t fan);

fan_connector_t fan_get(const char* fan_id);


#ifdef __cplusplus
}
#endif

#endif
