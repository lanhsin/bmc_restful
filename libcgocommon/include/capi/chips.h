/*
    chips.h - Part of sensors, a user-space program for hardware monitoring
    Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl>
    Copyright (C) 2007        Jean Delvare <jdelvare@suse.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

#ifndef _C_CHIPS_H
#define _C_CHIPS_H

#include <capi/sysfs.h>
#include <sstream>

sensors_chip_name *chip_get_name(int nr);

void chip_cout_json(const sensors_chip_name *name);
void chip_dump_json(std::stringstream& ss, const sensors_chip_name *name);

int chips_get_count();
void chips_cleanup(void);

void chips_cout_json(void);
void chips_dump_json(std::stringstream& ss);




#endif /* def _C_CHIPS_H */
