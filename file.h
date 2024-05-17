// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Denis Burkov <hitechshell@mail.ru>
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

class File {
private:
	const char *name;
	unsigned char* data;
	bool is_data_alloc = false;
	unsigned long file_size = 0;
	void _resize();
public:
	File(const char *name);
	File();
	void set_filename(const char *name);
	unsigned long get_real_size();
	unsigned long size();
	const unsigned char& operator[](unsigned long idx) const;
	unsigned char& operator[](unsigned long idx);
	unsigned char* get_content();
	int load();
	int save();
	void set(unsigned char* buffer, int len);
	~File();
};
