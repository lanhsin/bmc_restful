#include <capi/sysfs.h>
#include <capi/general.h>
#include <capi/error.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring> 				// strlen strchr
#include <cstdio> 				// sscanf
#include <dirent.h> 				// closedir
#include <sys/stat.h> 				// file state


/* Static mappings for use by sensors_subfeature_get_type() */
struct subfeature_type_match
{
	const char *name;
	sensors_subfeature_type type;
};

struct feature_type_match
{
	const char *name;
	const struct subfeature_type_match *submatches;
};

static const struct subfeature_type_match temp_matches[] = {
	{ "input", SENSORS_SUBFEATURE_TEMP_INPUT },
	{ "max", SENSORS_SUBFEATURE_TEMP_MAX },
	{ "max_hyst", SENSORS_SUBFEATURE_TEMP_MAX_HYST },
	{ "min", SENSORS_SUBFEATURE_TEMP_MIN },
	{ "min_hyst", SENSORS_SUBFEATURE_TEMP_MIN_HYST },
	{ "crit", SENSORS_SUBFEATURE_TEMP_CRIT },
	{ "crit_hyst", SENSORS_SUBFEATURE_TEMP_CRIT_HYST },
	{ "lcrit", SENSORS_SUBFEATURE_TEMP_LCRIT },
	{ "lcrit_hyst", SENSORS_SUBFEATURE_TEMP_LCRIT_HYST },
	{ "emergency", SENSORS_SUBFEATURE_TEMP_EMERGENCY },
	{ "emergency_hyst", SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST },
	{ "lowest", SENSORS_SUBFEATURE_TEMP_LOWEST },
	{ "highest", SENSORS_SUBFEATURE_TEMP_HIGHEST },
	{ "alarm", SENSORS_SUBFEATURE_TEMP_ALARM },
	{ "min_alarm", SENSORS_SUBFEATURE_TEMP_MIN_ALARM },
	{ "max_alarm", SENSORS_SUBFEATURE_TEMP_MAX_ALARM },
	{ "crit_alarm", SENSORS_SUBFEATURE_TEMP_CRIT_ALARM },
	{ "emergency_alarm", SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM },
	{ "lcrit_alarm", SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM },
	{ "fault", SENSORS_SUBFEATURE_TEMP_FAULT },
	{ "type", SENSORS_SUBFEATURE_TEMP_TYPE },
	{ "offset", SENSORS_SUBFEATURE_TEMP_OFFSET },
	{ "beep", SENSORS_SUBFEATURE_TEMP_BEEP },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match in_matches[] = {
	{ "input", SENSORS_SUBFEATURE_IN_INPUT },
	{ "min", SENSORS_SUBFEATURE_IN_MIN },
	{ "max", SENSORS_SUBFEATURE_IN_MAX },
	{ "lcrit", SENSORS_SUBFEATURE_IN_LCRIT },
	{ "crit", SENSORS_SUBFEATURE_IN_CRIT },
	{ "average", SENSORS_SUBFEATURE_IN_AVERAGE },
	{ "lowest", SENSORS_SUBFEATURE_IN_LOWEST },
	{ "highest", SENSORS_SUBFEATURE_IN_HIGHEST },
	{ "alarm", SENSORS_SUBFEATURE_IN_ALARM },
	{ "min_alarm", SENSORS_SUBFEATURE_IN_MIN_ALARM },
	{ "max_alarm", SENSORS_SUBFEATURE_IN_MAX_ALARM },
	{ "lcrit_alarm", SENSORS_SUBFEATURE_IN_LCRIT_ALARM },
	{ "crit_alarm", SENSORS_SUBFEATURE_IN_CRIT_ALARM },
	{ "beep", SENSORS_SUBFEATURE_IN_BEEP },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match fan_matches[] = {
	{ "input", SENSORS_SUBFEATURE_FAN_INPUT },
	{ "min", SENSORS_SUBFEATURE_FAN_MIN },
	{ "max", SENSORS_SUBFEATURE_FAN_MAX },
	{ "div", SENSORS_SUBFEATURE_FAN_DIV },
	{ "pulses", SENSORS_SUBFEATURE_FAN_PULSES },
	{ "alarm", SENSORS_SUBFEATURE_FAN_ALARM },
	{ "min_alarm", SENSORS_SUBFEATURE_FAN_MIN_ALARM },
	{ "max_alarm", SENSORS_SUBFEATURE_FAN_MAX_ALARM },
	{ "fault", SENSORS_SUBFEATURE_FAN_FAULT },
	{ "beep", SENSORS_SUBFEATURE_FAN_BEEP },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match power_matches[] = {
	{ "average", SENSORS_SUBFEATURE_POWER_AVERAGE },
	{ "average_highest", SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST },
	{ "average_lowest", SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST },
	{ "input", SENSORS_SUBFEATURE_POWER_INPUT },
	{ "input_highest", SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST },
	{ "input_lowest", SENSORS_SUBFEATURE_POWER_INPUT_LOWEST },
	{ "cap", SENSORS_SUBFEATURE_POWER_CAP },
	{ "cap_hyst", SENSORS_SUBFEATURE_POWER_CAP_HYST },
	{ "cap_alarm", SENSORS_SUBFEATURE_POWER_CAP_ALARM },
	{ "alarm", SENSORS_SUBFEATURE_POWER_ALARM },
	{ "max", SENSORS_SUBFEATURE_POWER_MAX },
	{ "max_alarm", SENSORS_SUBFEATURE_POWER_MAX_ALARM },
	{ "crit", SENSORS_SUBFEATURE_POWER_CRIT },
	{ "crit_alarm", SENSORS_SUBFEATURE_POWER_CRIT_ALARM },
	{ "average_interval", SENSORS_SUBFEATURE_POWER_AVERAGE_INTERVAL },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match energy_matches[] = {
	{ "input", SENSORS_SUBFEATURE_ENERGY_INPUT },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match curr_matches[] = {
	{ "input", SENSORS_SUBFEATURE_CURR_INPUT },
	{ "min", SENSORS_SUBFEATURE_CURR_MIN },
	{ "max", SENSORS_SUBFEATURE_CURR_MAX },
	{ "lcrit", SENSORS_SUBFEATURE_CURR_LCRIT },
	{ "crit", SENSORS_SUBFEATURE_CURR_CRIT },
	{ "average", SENSORS_SUBFEATURE_CURR_AVERAGE },
	{ "lowest", SENSORS_SUBFEATURE_CURR_LOWEST },
	{ "highest", SENSORS_SUBFEATURE_CURR_HIGHEST },
	{ "alarm", SENSORS_SUBFEATURE_CURR_ALARM },
	{ "min_alarm", SENSORS_SUBFEATURE_CURR_MIN_ALARM },
	{ "max_alarm", SENSORS_SUBFEATURE_CURR_MAX_ALARM },
	{ "lcrit_alarm", SENSORS_SUBFEATURE_CURR_LCRIT_ALARM },
	{ "crit_alarm", SENSORS_SUBFEATURE_CURR_CRIT_ALARM },
	{ "beep", SENSORS_SUBFEATURE_CURR_BEEP },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match humidity_matches[] = {
	{ "input", SENSORS_SUBFEATURE_HUMIDITY_INPUT },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match cpu_matches[] = {
	{ "vid", SENSORS_SUBFEATURE_VID },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static const struct subfeature_type_match intrusion_matches[] = {
	{ "alarm", SENSORS_SUBFEATURE_INTRUSION_ALARM },
	{ "beep", SENSORS_SUBFEATURE_INTRUSION_BEEP },
	{ NULL, SENSORS_SUBFEATURE_UNKNOWN }
};

static struct feature_type_match matches[] = {
	{ "temp%d%c", temp_matches },
	{ "in%d%c", in_matches },
	{ "fan%d%c", fan_matches },
	{ "cpu%d%c", cpu_matches },
	{ "power%d%c", power_matches },
	{ "curr%d%c", curr_matches },
	{ "energy%d%c", energy_matches },
	{ "intrusion%d%c", intrusion_matches },
	{ "humidity%d%c", humidity_matches },
};


/* Return the subfeature type and channel number based on the subfeature
   name */
static
sensors_subfeature_type sensors_subfeature_get_type(const char *name, int *nr)
{
	char c;
	int i, count;
	const struct subfeature_type_match *submatches;

	/* Special case */
	if (!std::strcmp(name, "beep_enable")) {
		*nr = 0;
		return SENSORS_SUBFEATURE_BEEP_ENABLE;
	}

	for (i = 0; i < ARRAY_SIZE(matches); i++)
		if ((count = std::sscanf(name, matches[i].name, nr, &c)))
			break;

	if (i == ARRAY_SIZE(matches) || count != 2 || c != '_')
		return SENSORS_SUBFEATURE_UNKNOWN;  /* no match */

	submatches = matches[i].submatches;
	name = std::strchr(name + 3, '_') + 1;
	for (i = 0; submatches[i].name != NULL; i++)
		if (!std::strcmp(name, submatches[i].name))
			return submatches[i].type;

	return SENSORS_SUBFEATURE_UNKNOWN;
}

/*
 * Read an attribute from sysfs
 * Returns a pointer to a freshly allocated string; free it yourself.
 * If the file doesn't exist or can't be read, NULL is returned.
 */
static char *sysfs_read_attr(const char *device, const char *attr)
{
	char path[NAME_MAX];
	char buf[ATTR_MAX], *p;
	FILE *f;

	snprintf(path, NAME_MAX, "%s/%s", device, attr);

	if (!(f = fopen(path, "r")))
		return NULL;
	p = fgets(buf, ATTR_MAX, f);
	fclose(f);
	if (!p)
		return NULL;

	/* Last byte is a '\n'; chop that off */
	p = strndup(buf, strlen(buf) - 1);
	if (!p)
		sensors_fatal_error(__func__, "Out of memory");
	return p;
}


int sysfs_read_attr(const char *device, const char*attr, double *value)
{
	char n[NAME_MAX];
	FILE *f;

	snprintf(n, NAME_MAX, "%s/%s", device, attr);
	if ((f = fopen(n, "r"))) {
		int res, err = 0;

		errno = 0;
		res = fscanf(f, "%lf", value);
		if (res == EOF && errno == EIO)
			err = -SENSORS_ERR_IO;
		else if (res != 1)
			err = -SENSORS_ERR_ACCESS_R;
		res = fclose(f);
		if (err)
			return err;

		if (res == EOF) {
			if (errno == EIO)
				return -SENSORS_ERR_IO;
			else
				return -SENSORS_ERR_ACCESS_R;
		}
		//*value /= get_type_scaling(subfeature->type);
	} else
		return -SENSORS_ERR_KERNEL;

	return 0;
}

void sysfs_write_attr(const std::string& device, const std::string& attr, const std::string& s_val)
{
    std::string command = "echo " + s_val + " > " + device + '/' + attr;
    std::system(command.c_str());
}


static
char *get_feature_name(sensors_feature_type ftype, char *sfname)
{
	char *name, *underscore;

	switch (ftype) {
	case SENSORS_FEATURE_IN:
	case SENSORS_FEATURE_FAN:
	case SENSORS_FEATURE_TEMP:
	case SENSORS_FEATURE_POWER:
	case SENSORS_FEATURE_ENERGY:
	case SENSORS_FEATURE_CURR:
	case SENSORS_FEATURE_HUMIDITY:
	case SENSORS_FEATURE_INTRUSION:
		underscore = strchr(sfname, '_');
		name = strndup(sfname, underscore - sfname);
		if (!name)
			sensors_fatal_error(__func__, "Out of memory");

		break;
	default:
		name = strdup(sfname);
		if (!name)
			sensors_fatal_error(__func__, "Out of memory");
	}

	return name;
}

sensors_chip_features *sensors_proc_chips = NULL;
int sensors_proc_chips_count = 0;
int sensors_proc_chips_max = 0;

static int sensors_compute_max_sf(void)
{
	int i, j, max, offset;
	const struct subfeature_type_match *submatches;
	sensors_feature_type ftype;

	max = 0;
	for (i = 0; i < ARRAY_SIZE(matches); i++) {
		submatches = matches[i].submatches;
		for (j = 0; submatches[j].name != NULL; j++) {

			ftype = static_cast<sensors_feature_type>(submatches[j].type >> 8);
			if (ftype < SENSORS_FEATURE_VID) {
				offset = submatches[j].type & 0x7F;
				if (offset >= max)
					max = offset + 1;
			} else {
				offset = submatches[j].type & 0xFF;
				if (offset >= max * 2)
					max = ((offset + 1) + 1) / 2; // max 
			}
		}
	}

	return max;
}

static int sensors_get_attr_mode(const std::string& device, const std::string& attr)
{
	std::string path = device + '/' + attr;
        struct stat st;
	int mode = 0;

        if (!stat(path.c_str(), &st)) {
	        if (st.st_mode & S_IRUSR)
			mode |= SENSORS_MODE_R;
		if (st.st_mode & S_IWUSR)
			mode |= SENSORS_MODE_W;
	}
	return mode;
}


static int sensors_read_dynamic_chip(sensors_chip_features *chip,
				     const char *dev_path)
{
	int i, fnum = 0, sfnum = 0, prev_slot;
	static int max_subfeatures, feature_size;
	DIR *dir;
	struct dirent *ent;
	struct {
		int count;
		sensors_subfeature *sf;
	} all_types[SENSORS_FEATURE_MAX];
	sensors_subfeature *dyn_subfeatures;
	sensors_feature *dyn_features;
	sensors_feature_type ftype;
	sensors_subfeature_type sftype;

	if (!(dir = opendir(dev_path)))
		return -errno;
        
	/* Dynamically figure out the max number of subfeatures */
	if (!max_subfeatures) {
		max_subfeatures = sensors_compute_max_sf();
		feature_size = max_subfeatures * 2;
	}

	/* We use a set of large sparse tables at first (one per main
	   feature type present) to store all found subfeatures, so that we
	   can store them sorted and then later create a dense sorted table. */
	memset(&all_types, 0, sizeof(all_types));

	int nr;
	while ((ent = readdir(dir)))
	{
		/* Skip directories and symlinks */
		if(ent->d_type != DT_REG)
			continue;			

		std::string attr_name = ent->d_name;
		sftype = sensors_subfeature_get_type(attr_name.c_str(), &nr);
		if (sftype == SENSORS_SUBFEATURE_UNKNOWN)
			continue;
		ftype = static_cast<sensors_feature_type>(sftype >> 8);
		
		/* Adjust the channel number */
		switch (ftype) {
		case SENSORS_FEATURE_FAN:
		case SENSORS_FEATURE_TEMP:
		case SENSORS_FEATURE_POWER:
		case SENSORS_FEATURE_ENERGY:
		case SENSORS_FEATURE_CURR:
		case SENSORS_FEATURE_HUMIDITY:
			nr--;
			break;
		default:
			break;
		}

		/* Skip invalid entries. The high limit is arbitrary, we just
		   don't want to allocate an insane amount of memory. */
		if (nr < 0 || nr >= 1024) {
#ifdef DEBUG
			sensors_fatal_error(__func__, "Invalid channel number!");
#endif
			continue;
		}

		/* (Re-)allocate memory if needed */
		if (all_types[ftype].count < nr + 1) {
			int old_count = all_types[ftype].count;
			int step = ftype < SENSORS_FEATURE_VID ? 8 :
				   ftype < SENSORS_FEATURE_BEEP_ENABLE ? 2 : 1;

			while (all_types[ftype].count < nr + 1)
				all_types[ftype].count += step;

			all_types[ftype].sf = static_cast<sensors_subfeature *>(realloc(all_types[ftype].sf,
						all_types[ftype].count * feature_size * sizeof(sensors_subfeature)));

			if (!all_types[ftype].sf)
				sensors_fatal_error(__func__, "Out of memory");
			memset(all_types[ftype].sf + old_count * feature_size,
			       0, (all_types[ftype].count - old_count) *
				  feature_size * sizeof(sensors_subfeature));
		}

		/* "calculate" a place to store the subfeature in our sparse,
		   sorted table */
		if (ftype < SENSORS_FEATURE_VID)
			i = nr * feature_size +
			    ((sftype & 0x80) >> 7) * max_subfeatures +
			    (sftype & 0x7F);
		else
			i = nr * feature_size + (sftype & 0xFF);

		if (all_types[ftype].sf[i].name) {
#ifdef DEBUG
			sensors_fatal_error(__func__, "Duplicate subfeature");
#endif
			continue;
		}

		/* fill in the subfeature members */
		all_types[ftype].sf[i].type = sftype;
		all_types[ftype].sf[i].name = strdup(attr_name.data());
		if (!all_types[ftype].sf[i].name)
			sensors_fatal_error(__func__, "Out of memory");

		/* Other and misc subfeatures are never scaled */
		if (sftype < SENSORS_SUBFEATURE_VID && !(sftype & 0x80))
			all_types[ftype].sf[i].flags |= SENSORS_COMPUTE_MAPPING;
		all_types[ftype].sf[i].flags |=
					sensors_get_attr_mode(dev_path, attr_name);
		sfnum++;
	}

	if (!sfnum) { /* No subfeature */
		chip->subfeature = NULL;
		goto exit_free;
	}

	/* How many main features? */
	for (int ftype = 0; ftype < SENSORS_FEATURE_MAX; ftype++) {
		prev_slot = -1;
		for (i = 0; i < all_types[ftype].count * feature_size; i++) {
			if (all_types[ftype].sf[i].name)
				continue;

			if (i / feature_size != prev_slot) {
				fnum++;
				prev_slot = i / feature_size;
			}
		}
	}

	dyn_subfeatures = static_cast<sensors_subfeature *>(calloc(sfnum, sizeof(sensors_subfeature)));
	dyn_features = static_cast<sensors_feature *>(calloc(fnum, sizeof(sensors_feature)));
	if (!dyn_subfeatures || !dyn_features)
		sensors_fatal_error(__func__, "Out of memory");

	/* Copy from the sparse array to the compact array */
	sfnum = 0;
	fnum = -1;
	for (int ftype = SENSORS_FEATURE_IN; ftype < SENSORS_FEATURE_MAX; ftype++) {
		prev_slot = -1;
		for (i = 0; i < all_types[ftype].count * feature_size; i++) {
			if (!all_types[ftype].sf[i].name)
				continue;

			/* New main feature? */
			if (i / feature_size != prev_slot) {
				fnum++;
				prev_slot = i / feature_size;
				dyn_features[fnum].name =
					get_feature_name(	static_cast<sensors_feature_type>(ftype),	
										all_types[ftype].sf[i].name);

				dyn_features[fnum].number = fnum;
				dyn_features[fnum].first_subfeature = sfnum;
				dyn_features[fnum].type = static_cast<sensors_feature_type>(ftype);
			}

			dyn_subfeatures[sfnum] = all_types[ftype].sf[i];
			dyn_subfeatures[sfnum].number = sfnum;
			/* Back to the feature */
			dyn_subfeatures[sfnum].mapping = fnum;

			sfnum++;
		}
	}
	closedir(dir);

	chip->subfeature = dyn_subfeatures;
	chip->subfeature_count = sfnum;
	chip->feature = dyn_features;
	chip->feature_count = ++fnum;

exit_free:
	for (int ftype = 0; ftype < SENSORS_FEATURE_MAX; ftype++)
		free(all_types[ftype].sf);
	return 0;
}

/* returns: number of devices added (0 or 1) if successful, <0 otherwise */
int sysfs_read_one_chip(const char *hwmon_path)
{
	int ret = 1;
	sensors_chip_features entry;

	/* ignore any device without name attribute */
	if(!(entry.chip.name = sysfs_read_attr(hwmon_path, "name")))
		return 0;

	entry.chip.path = strdup(hwmon_path);
	if (!entry.chip.path)
		sensors_fatal_error(__func__, "Out of memory");

	if (sensors_read_dynamic_chip(&entry, hwmon_path) < 0) {
		ret = -SENSORS_ERR_KERNEL;
		goto exit_free;
	}
	/* No subfeature, discard chip */
	if (!entry.subfeature){ 
		ret = 0;
		goto exit_free;
	}

	sensors_add_array_el( 
	(void*)&entry, (void**)&sensors_proc_chips, &sensors_proc_chips_count,
	&sensors_proc_chips_max, sizeof(struct sensors_chip_features));

	return ret;

exit_free:
	free(entry.chip.name);
	free(entry.chip.path);
	return ret;
}

int sysfs_read_chips(void)
{
	int ret;
	DIR *dir;
	struct dirent *ent;
	std::string path = "/sys/class/hwmon";
	std::string fullpath;
	if (!(dir = opendir(path.c_str())))
		return -errno;
	while ((ent = readdir(dir))) {
		 if (ent->d_name[0] == '.')
			continue;
	        fullpath = path + '/' + ent->d_name;
		ret = sysfs_read_one_chip(fullpath.c_str());
		if(ret < 0)
			break;
	}

	return ret;
}
