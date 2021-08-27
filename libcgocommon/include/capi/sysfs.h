#ifndef _C_SYSFS_H
#define _C_SYSFS_H

#include <string>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* A chip name is encoded in this structure */
typedef struct sensors_chip_name {
	char* name;
	char* path;
} sensors_chip_name;

/* These defines are used in the flags field of sensors_subfeature */
#define SENSORS_MODE_R			1
#define SENSORS_MODE_W			2
#define SENSORS_COMPUTE_MAPPING		4

typedef enum sensors_feature_type {
	SENSORS_FEATURE_IN		= 0x00,
	SENSORS_FEATURE_FAN		= 0x01,
	SENSORS_FEATURE_TEMP		= 0x02,
	SENSORS_FEATURE_POWER		= 0x03,
	SENSORS_FEATURE_ENERGY		= 0x04,
	SENSORS_FEATURE_CURR		= 0x05,
	SENSORS_FEATURE_HUMIDITY	= 0x06,
	SENSORS_FEATURE_MAX_MAIN,
	SENSORS_FEATURE_VID		= 0x10,
	SENSORS_FEATURE_INTRUSION	= 0x11,
	SENSORS_FEATURE_MAX_OTHER,
	SENSORS_FEATURE_BEEP_ENABLE	= 0x18,
	SENSORS_FEATURE_MAX,
	SENSORS_FEATURE_UNKNOWN		= INT_MAX,
} sensors_feature_type;

/* All the sensor types (in, fan, temp, vid) are a multiple of 0x100 apart,
   and sensor subfeatures which have no compute mapping have bit 7 set. */
typedef enum sensors_subfeature_type {
	SENSORS_SUBFEATURE_IN_INPUT = SENSORS_FEATURE_IN << 8,
	SENSORS_SUBFEATURE_IN_MIN,
	SENSORS_SUBFEATURE_IN_MAX,
	SENSORS_SUBFEATURE_IN_LCRIT,
	SENSORS_SUBFEATURE_IN_CRIT,
	SENSORS_SUBFEATURE_IN_AVERAGE,
	SENSORS_SUBFEATURE_IN_LOWEST,
	SENSORS_SUBFEATURE_IN_HIGHEST,
	SENSORS_SUBFEATURE_IN_ALARM = (SENSORS_FEATURE_IN << 8) | 0x80,
	SENSORS_SUBFEATURE_IN_MIN_ALARM,
	SENSORS_SUBFEATURE_IN_MAX_ALARM,
	SENSORS_SUBFEATURE_IN_BEEP,
	SENSORS_SUBFEATURE_IN_LCRIT_ALARM,
	SENSORS_SUBFEATURE_IN_CRIT_ALARM,

	SENSORS_SUBFEATURE_FAN_INPUT = SENSORS_FEATURE_FAN << 8,
	SENSORS_SUBFEATURE_FAN_MIN,
	SENSORS_SUBFEATURE_FAN_MAX,
	SENSORS_SUBFEATURE_FAN_ALARM = (SENSORS_FEATURE_FAN << 8) | 0x80,
	SENSORS_SUBFEATURE_FAN_FAULT,
	SENSORS_SUBFEATURE_FAN_DIV,
	SENSORS_SUBFEATURE_FAN_BEEP,
	SENSORS_SUBFEATURE_FAN_PULSES,
	SENSORS_SUBFEATURE_FAN_MIN_ALARM,
	SENSORS_SUBFEATURE_FAN_MAX_ALARM,

	SENSORS_SUBFEATURE_TEMP_INPUT = SENSORS_FEATURE_TEMP << 8,
	SENSORS_SUBFEATURE_TEMP_MAX,
	SENSORS_SUBFEATURE_TEMP_MAX_HYST,
	SENSORS_SUBFEATURE_TEMP_MIN,
	SENSORS_SUBFEATURE_TEMP_CRIT,
	SENSORS_SUBFEATURE_TEMP_CRIT_HYST,
	SENSORS_SUBFEATURE_TEMP_LCRIT,
	SENSORS_SUBFEATURE_TEMP_EMERGENCY,
	SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST,
	SENSORS_SUBFEATURE_TEMP_LOWEST,
	SENSORS_SUBFEATURE_TEMP_HIGHEST,
	SENSORS_SUBFEATURE_TEMP_MIN_HYST,
	SENSORS_SUBFEATURE_TEMP_LCRIT_HYST,
	SENSORS_SUBFEATURE_TEMP_ALARM = (SENSORS_FEATURE_TEMP << 8) | 0x80,
	SENSORS_SUBFEATURE_TEMP_MAX_ALARM,
	SENSORS_SUBFEATURE_TEMP_MIN_ALARM,
	SENSORS_SUBFEATURE_TEMP_CRIT_ALARM,
	SENSORS_SUBFEATURE_TEMP_FAULT,
	SENSORS_SUBFEATURE_TEMP_TYPE,
	SENSORS_SUBFEATURE_TEMP_OFFSET,
	SENSORS_SUBFEATURE_TEMP_BEEP,
	SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM,
	SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM,

	SENSORS_SUBFEATURE_POWER_AVERAGE = SENSORS_FEATURE_POWER << 8,
	SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST,
	SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST,
	SENSORS_SUBFEATURE_POWER_INPUT,
	SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST,
	SENSORS_SUBFEATURE_POWER_INPUT_LOWEST,
	SENSORS_SUBFEATURE_POWER_CAP,
	SENSORS_SUBFEATURE_POWER_CAP_HYST,
	SENSORS_SUBFEATURE_POWER_MAX,
	SENSORS_SUBFEATURE_POWER_CRIT,
	SENSORS_SUBFEATURE_POWER_AVERAGE_INTERVAL = (SENSORS_FEATURE_POWER << 8) | 0x80,
	SENSORS_SUBFEATURE_POWER_ALARM,
	SENSORS_SUBFEATURE_POWER_CAP_ALARM,
	SENSORS_SUBFEATURE_POWER_MAX_ALARM,
	SENSORS_SUBFEATURE_POWER_CRIT_ALARM,

	SENSORS_SUBFEATURE_ENERGY_INPUT = SENSORS_FEATURE_ENERGY << 8,

	SENSORS_SUBFEATURE_CURR_INPUT = SENSORS_FEATURE_CURR << 8,
	SENSORS_SUBFEATURE_CURR_MIN,
	SENSORS_SUBFEATURE_CURR_MAX,
	SENSORS_SUBFEATURE_CURR_LCRIT,
	SENSORS_SUBFEATURE_CURR_CRIT,
	SENSORS_SUBFEATURE_CURR_AVERAGE,
	SENSORS_SUBFEATURE_CURR_LOWEST,
	SENSORS_SUBFEATURE_CURR_HIGHEST,
	SENSORS_SUBFEATURE_CURR_ALARM = (SENSORS_FEATURE_CURR << 8) | 0x80,
	SENSORS_SUBFEATURE_CURR_MIN_ALARM,
	SENSORS_SUBFEATURE_CURR_MAX_ALARM,
	SENSORS_SUBFEATURE_CURR_BEEP,
	SENSORS_SUBFEATURE_CURR_LCRIT_ALARM,
	SENSORS_SUBFEATURE_CURR_CRIT_ALARM,

	SENSORS_SUBFEATURE_HUMIDITY_INPUT = SENSORS_FEATURE_HUMIDITY << 8,

	SENSORS_SUBFEATURE_VID = SENSORS_FEATURE_VID << 8,

	SENSORS_SUBFEATURE_INTRUSION_ALARM = SENSORS_FEATURE_INTRUSION << 8,
	SENSORS_SUBFEATURE_INTRUSION_BEEP,

	SENSORS_SUBFEATURE_BEEP_ENABLE = SENSORS_FEATURE_BEEP_ENABLE << 8,

	SENSORS_SUBFEATURE_UNKNOWN = INT_MAX,
} sensors_subfeature_type;


/* Data about a single chip feature (or category leader) */
struct sensors_feature {
	char* name;
	int number;
	sensors_feature_type type;
	/* Members below are for libsensors internal use only */
	int first_subfeature;
};

/* Data about a single chip subfeature:
   name is the string name used to refer to this subfeature (in config files)
   number is the internal subfeature number, used in many functions to refer
     to this subfeature
   type is the subfeature type
   mapping is the number of a main feature this subfeature belongs to
     (for example subfeatures fan1_input, fan1_min, fan1_div and fan1_alarm
      are mapped to main feature fan1)
   flags is a bitfield, its value is a combination of SENSORS_MODE_R (readable),
     SENSORS_MODE_W (writable) and SENSORS_COMPUTE_MAPPING (affected by the
     computation rules of the main feature) */
typedef struct sensors_subfeature {
	char* name;
	int number;
	sensors_subfeature_type type;
	int mapping;
	unsigned int flags;
} sensors_subfeature;

/* Internal data about all features and subfeatures of a chip */
typedef struct sensors_chip_features {
	struct sensors_chip_name chip;
	struct sensors_feature *feature;
	struct sensors_subfeature *subfeature;
	int feature_count;
	int subfeature_count;
} sensors_chip_features;


extern sensors_chip_features *sensors_proc_chips;
extern int sensors_proc_chips_count;
extern int sensors_proc_chips_max;

int sysfs_read_one_chip(const char *hwmon_path);
int sysfs_read_chips(void);
int sysfs_read_attr(const char *device, const char*attr, double *value);
void sysfs_write_attr(const std::string& device, const std::string& attr, const std::string& s_val);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* def _C_SYSFS_H */