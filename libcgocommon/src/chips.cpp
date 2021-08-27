/*
    chips.c - Part of sensors, a user-space program for hardware monitoring
    Copyright (C) 1998-2003  Frodo Looijaard <frodol@dds.nl> and
                             Mark D. Studebaker <mdsxyz123@yahoo.com>
    Copyright (C) 2007-2012  Jean Delvare <jdelvare@suse.de>

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

#include <capi/chips.h>
#include <capi/access.h>
#include <capi/error.h>
#include <iostream>

int chips_get_count()
{
	return sensors_proc_chips_count;
}

sensors_chip_name *chip_get_name(int nr)
{
	if(nr >= chips_get_count())
		return nullptr;
	return &(sensors_proc_chips[nr].chip);
}

static void chip_free_name(sensors_chip_name *name)
{
	free(name->name);
	free(name->path);
}

static void chip_free_features(sensors_chip_features *features)
{
	int i;

	for (i = 0; i < features->subfeature_count; i++)
		free(features->subfeature[i].name);
	free(features->subfeature);
	for (i = 0; i < features->feature_count; i++)
		free(features->feature[i].name);
	free(features->feature);
}


static void _chip_cout_json(const sensors_chip_name *name)
{
	int a, b, cnt, subCnt, err;
	const sensors_feature *feature;
	const sensors_subfeature *sub;
	double val;

	a = 0;
	cnt = 0;
	while ((feature = sensors_get_features(name, &a))) {
		if (!(feature->name)) {
			std::cerr << "ERROR: Can't get label of feature " << feature->name << "!\n";
			continue;
		}
		if (cnt > 0)
			std::cout << ",\n";
		std::cout<<"      \""<<feature->name<<"\":{\n";

		b = 0;
		subCnt = 0;
		while ((sub = sensors_get_all_subfeatures(name, feature, &b))) {
			if (sub->flags & SENSORS_MODE_R) {
				if ((err = sensors_get_value(name, sub->number, &val))) {
					std::cerr << "ERROR: Can't get value of subfeature ";
					std::cerr << sub->name << ": " << sensors_strerror(err) << '\n';
				} else {
					if (subCnt > 0)
						std::cout << ",\n";
					std::cout<<"         \""<<sub->name<<"\": " << val;
					subCnt++;
				}

			} else {
				std::cout << "(" << feature->name << ")";
				subCnt++;
			}
		}
		std::cout << "\n      }";
		cnt++;
	}
	if (cnt > 0)
		std::cout << "\n";
}

void chip_cout_json(const sensors_chip_name *name)
{
	std::cout << "   \"" << name->name << "\":{\n";
	_chip_cout_json(name);
	std::cout << "   }";
}

/* returns number of chips found */
void chips_cout_json(void)
{
	const sensors_chip_name *chip;
	bool do_json = true;
	int chip_nr;
	int cnt = 0;

	if (do_json)
		std::cout << "{\n";
	chip_nr = 0;

	for (chip_nr = 0; chip_nr < sensors_proc_chips_count; chip_nr++) 
	{
		chip = &(sensors_proc_chips[chip_nr].chip);
		if (do_json) {
			if (cnt > 0)
				std::cout << ",\n";
			chip_cout_json(chip);
		} else {
			//do_a_print(chip);
		}
		cnt++;
	}
	if (do_json)
		std::cout << "\n}\n";
}

static void chip_dump_one_json(std::stringstream& ss, const sensors_chip_name *name)
{
	int a, b, cnt, subCnt, err;
	const sensors_feature *feature;
	const sensors_subfeature *sub;
	double val;

	a = 0;
	cnt = 0;
	ss << "   \"" << name->path << "\":{\n";
	ss << "      \"name\":\""<< name->name <<"\"";
	cnt++;
	while ((feature = sensors_get_features(name, &a))) {
		if (!(feature->name)) {
			std::cerr << "ERROR: Can't get label of feature " << feature->name << "!\n";
			continue;
		}

		if (cnt > 0)
			ss << ",\n";
		ss <<"      \""<<feature->name<<"\":{\n";

		b = 0;
		subCnt = 0;
		while ((sub = sensors_get_all_subfeatures(name, feature, &b))) {
			if (sub->flags & SENSORS_MODE_R) {
				if ((err = sensors_get_value(name, sub->number, &val))) {
					std::cerr << "ERROR: Can't get value of subfeature ";
					std::cerr << sub->name << ": " << sensors_strerror(err) << '\n';
				} else {
					if (subCnt > 0)
						ss << ",\n";
					ss <<"         \""<<sub->name<<"\": " << val;
					subCnt++;
				}

			} else {
				ss << "(" << feature->name << ")";
				subCnt++;
			}
		}
		ss << "\n      }";
		cnt++;
	}
	if (cnt > 0)
		ss << "\n";

	ss << "   }";
}


void chip_dump_json(std::stringstream& ss, const sensors_chip_name *name)
{
	int a, b, cnt, subCnt, err;
	const sensors_feature *feature;
	const sensors_subfeature *sub;
	double val;

	a = 0;
	cnt = 0;
	ss << "\"" << name->path << "\":{\n";
	ss << "   \"name\":\""<< name->name <<"\"";
	cnt++;
	while ((feature = sensors_get_features(name, &a))) {
		if (!(feature->name)) {
			std::cerr << "ERROR: Can't get label of feature " << feature->name << "!\n";
			continue;
		}

		if (cnt > 0)
			ss << ",\n";
		ss <<"   \""<<feature->name<<"\":{\n";

		b = 0;
		subCnt = 0;
		while ((sub = sensors_get_all_subfeatures(name, feature, &b))) {
			if (sub->flags & SENSORS_MODE_R) {
				if ((err = sensors_get_value(name, sub->number, &val))) {
					std::cerr << "ERROR: Can't get value of subfeature ";
					std::cerr << sub->name << ": " << sensors_strerror(err) << '\n';
				} else {
					if (subCnt > 0)
						ss << ",\n";
					ss <<"      \""<<sub->name<<"\": " << val;
					subCnt++;
				}

			} else {
				ss << "(" << feature->name << ")";
				subCnt++;
			}
		}
		ss << "\n   }";
		cnt++;
	}
	if (cnt > 0)
		ss << "\n";

	ss << "}";
}

/* returns number of chips found */
void chips_dump_json(std::stringstream& ss)
{
	const sensors_chip_name *chip;
	int chip_nr = 0;
	int cnt = 0;

	ss << "{\n";
	for (chip_nr = 0; chip_nr < sensors_proc_chips_count; chip_nr++) 
	{
		chip = &(sensors_proc_chips[chip_nr].chip);
		if (cnt > 0)
			ss << ",\n";
		chip_dump_one_json(ss, chip);
		cnt++;
	}
	ss << "\n}\n";
}

void chips_cleanup(void)
{
	int i;
	for (i = 0; i < sensors_proc_chips_count; i++) {
		chip_free_name(&sensors_proc_chips[i].chip);
		chip_free_features(&sensors_proc_chips[i]);
	}
	free(sensors_proc_chips);
	sensors_proc_chips = NULL;
	sensors_proc_chips_count = sensors_proc_chips_max = 0;
}