// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Denis Burkov <hitechshell@mail.ru>
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <termios.h>

const unsigned int magic_const[] = {
	0x0000, 0x1021, 0x2042, 0x3063,
	0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B,
	0xC18C, 0xD1AD, 0xE1CE, 0xF1EF
};

struct data_block {
	int lmode;  //boot mode: 1 - direct start, 2 - with restart A-core
	unsigned int size;
	unsigned int addr;
	unsigned char* data;
};

// raminit lmode = 1
// usbloader lmode = 2

void calculate_checksum(unsigned char* data, int len);
bool send_command(int fd, unsigned char* cmdbuf, int len);
bool check_mode(int fd);
void upload(int fd, data_block& blk);
int open_port(const char* devname);
