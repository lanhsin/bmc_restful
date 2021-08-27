/*
    access.c - Part of libsensors, a Linux library for reading sensor data.
    Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl>
    Copyright (C) 2007-2010   Jean Delvare <jdelvare@suse.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

#include <capi/error.h>
#include <capi/access.h>
#include <cstring>

/* We watch the recursion depth for variables only, as an easy way to
   detect cycles. */
#define DEPTH_MAX	8

/* Compare two chips name descriptions, to see whether they could match.
   Return 0 if it does not match, return 1 if it does match. */
static int sensors_match_chip(const sensors_chip_name *chip1,
		       const sensors_chip_name *chip2)
{
	if (strcmp(chip1->name, chip2->name))
		return 0;

	return 1;
}

/* Look up a chip in the intern chip list, and return a pointer to it.
   Do not modify the struct the return value points to! Returns NULL if
   not found.*/
static const sensors_chip_features *
sensors_lookup_chip(const sensors_chip_name *name)
{
	int i;

	for (i = 0; i < sensors_proc_chips_count; i++)
		if (sensors_match_chip(&sensors_proc_chips[i].chip, name))
			return &sensors_proc_chips[i];

	return NULL;
}

/* Look up a subfeature of the given chip, and return a pointer to it.
   Do not modify the struct the return value points to! Returns NULL if
   not found.*/
static const sensors_subfeature *
sensors_lookup_subfeature_nr(const sensors_chip_features *chip,
			     int subfeat_nr)
{
	if (subfeat_nr < 0 ||
	    subfeat_nr >= chip->subfeature_count)
		return NULL;
	return chip->subfeature + subfeat_nr;
}

/* Look up a feature of the given chip, and return a pointer to it.
   Do not modify the struct the return value points to! Returns NULL if
   not found.*/
static const sensors_feature *
sensors_lookup_feature_nr(const sensors_chip_features *chip, int feat_nr)
{
	if (feat_nr < 0 ||
	    feat_nr >= chip->feature_count)
		return NULL;
	return chip->feature + feat_nr;
}

int sensors_get_value(const sensors_chip_name *name, int subfeat_nr,
		      double *result)
{
	const sensors_chip_features *chip_features;
	const sensors_subfeature *subfeature;
	double val;
	int res, depth = 0;
	
	if (depth >= DEPTH_MAX)
		return -SENSORS_ERR_RECURSION;
	if (!(chip_features = sensors_lookup_chip(name)))
		return -SENSORS_ERR_NO_ENTRY;
	if (!(subfeature = sensors_lookup_subfeature_nr(chip_features, subfeat_nr)))
		return -SENSORS_ERR_NO_ENTRY;
	if (!(subfeature->flags & SENSORS_MODE_R))
		return -SENSORS_ERR_ACCESS_R;

	res = sysfs_read_attr(name->path, subfeature->name, &val);
	if (res)
		return res;
	else
		*result = val;
	return 0;
}

/* Set the value of a subfeature of a certain chip. Note that chip should not
   contain wildcard values! This function will return 0 on success, and <0
   on failure. */
int sensors_set_value(const sensors_chip_name *name, int subfeat_nr,
		      const std::string& s_val)
{
	const sensors_chip_features *chip_features;
	const sensors_subfeature *subfeature;

	if (!(chip_features = sensors_lookup_chip(name)))
		return -SENSORS_ERR_NO_ENTRY;
	if (!(subfeature = sensors_lookup_subfeature_nr(chip_features, subfeat_nr)))
		return -SENSORS_ERR_NO_ENTRY;
	if (!(subfeature->flags & SENSORS_MODE_W))
		return -SENSORS_ERR_ACCESS_W;

	sysfs_write_attr(name->path, subfeature->name, s_val);
	return 0;
}

const sensors_feature *
sensors_get_features(const sensors_chip_name *name, int *nr)
{
	const sensors_chip_features *chip;

	if (!(chip = sensors_lookup_chip(name)))
		return NULL;	/* No such chip */

	if (*nr >= chip->feature_count)
		return NULL;
	return &chip->feature[(*nr)++];
}

const sensors_subfeature *
sensors_get_all_subfeatures(const sensors_chip_name *name,
			const sensors_feature *feature, int *nr)
{
	const sensors_chip_features *chip;
	const sensors_subfeature *subfeature;

	if (!(chip = sensors_lookup_chip(name)))
		return NULL;	/* No such chip */

	/* Seek directly to the first subfeature */
	if (*nr < feature->first_subfeature)
		*nr = feature->first_subfeature;

	if (*nr >= chip->subfeature_count)
		return NULL;	/* end of list */
	subfeature = &chip->subfeature[(*nr)++];
	if (subfeature->mapping == feature->number)
		return subfeature;
	return NULL;	/* end of subfeature list */
}

const sensors_subfeature *
sensors_get_subfeature(const sensors_chip_name *name,
		       const sensors_feature *feature,
		       sensors_subfeature_type type)
{
	const sensors_chip_features *chip;
	int i;

	if (!(chip = sensors_lookup_chip(name)))
		return NULL;	/* No such chip */

	for (i = feature->first_subfeature; i < chip->subfeature_count &&
	     chip->subfeature[i].mapping == feature->number; i++) {
		if (chip->subfeature[i].type == type)
			return &chip->subfeature[i];
	}
	return NULL;	/* No such subfeature */
}
