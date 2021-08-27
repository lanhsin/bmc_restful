#ifndef _C_ACCESS_H
#define _C_ACCESS_H

#include <capi/sysfs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* This returns all main features of a specific chip. nr is an internally
   used variable. Set it to zero to start at the begin of the list. If no
   more features are found NULL is returned.
   Do not try to change the returned structure; you will corrupt internal
   data structures. */
const sensors_feature *
sensors_get_features(const sensors_chip_name *name, int *nr);

/* This returns all subfeatures of a given main feature. nr is an internally
   used variable. Set it to zero to start at the begin of the list. If no
   more features are found NULL is returned.
   Do not try to change the returned structure; you will corrupt internal
   data structures. */
const sensors_subfeature *
sensors_get_all_subfeatures(const sensors_chip_name *name,
			    const sensors_feature *feature, int *nr);

int sensors_get_value(const sensors_chip_name *name, int subfeat_nr,
		      double *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* def _C_ACCESS_H */