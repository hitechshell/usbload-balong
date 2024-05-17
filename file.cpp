// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Denis Burkov <hitechshell@mail.ru>
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file.h"

void File::_resize()
{
	if(is_data_alloc) {
		free(this->data);
	}
	this->data = (unsigned char*)malloc(file_size);
	is_data_alloc = true;
}
File::File(const char *name)
{
	this->name = name;
}
File::File()
{
}
void File::set_filename(const char *name)
{
	this->name = name;
}
unsigned long File::get_real_size()
{
	struct stat info;
	if (stat(name, &info) != 0) {
		printf("Cannot read filesize.\n");
		return -1;
	}
	file_size = info.st_size;
	return info.st_size;
}
unsigned long File::size()
{
	return file_size;
}
const unsigned char& File::operator[](unsigned long idx) const
{
	return this->data[idx];
}
unsigned char& File::operator[](unsigned long idx)
{
	return this->data[idx];
}
unsigned char* File::get_content() {
	return this->data;
}
int File::load()
{
	this->get_real_size();
	_resize();
	FILE *fp = fopen(name, "rb");
	if (fp == NULL) {
		printf("Cannot open file.\n");
		return -1;
	}
	size_t blocks_read = fread(data, this->get_real_size(), 1, fp);
	if (blocks_read != 1) {
		/* error handling */
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}
int File::save()
{
	FILE *file = fopen(name, "wb");
	if(file == NULL) {
		printf("Cannot open file.\n");
		return -1;
	}
	int ret = fwrite(this->data, 1, file_size, file);
	if(ret != file_size) {
		printf("Error when write to file.\n");
		return -1;
	}
	fclose(file);
	return 0;
}
void File::set(unsigned char* buffer, int len)
{
	file_size = len;
	_resize();
	for(int i = 0; i < len; i++) {
		this->data[i] = buffer[i];
	}
}
File::~File()
{
	if(is_data_alloc) {
		free(this->data);
	}
}
