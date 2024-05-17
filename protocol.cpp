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
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

#include "protocol.h"

void calculate_checksum(unsigned char* data, int len)
{
	unsigned int checksum = 0;
	unsigned int c = 0;
	for(int i = 0; i < len - 2; i++) {
		c = (data[i] & 0xff);
		checksum=((checksum << 4) & 0xffff) ^ magic_const[(c >> 4) ^ (checksum >> 12)];
		checksum=((checksum << 4) & 0xffff) ^ magic_const[(c & 0xf) ^ (checksum >> 12)];
	}
	data[len - 2] = (checksum >> 8) & 0xff;
	data[len - 1] = checksum & 0xff;
}

bool send_command(int fd, unsigned char* cmdbuf, int len)
{
	unsigned char replybuf[1024];
	unsigned int replylen;

	calculate_checksum(cmdbuf, len);
	write(fd, cmdbuf, len);
	tcdrain(fd);
	replylen = read(fd, replybuf, 1024);
	if (replylen == 0) {
		return false;
	}
	if (replybuf[0] == 0xAA) {
		return true;
	}
	return false;
}

bool check_mode(int fd)
{
	unsigned char c = 0;
	int res;
	write(fd, "A", 1); // A
	res = read(fd, &c, 1);
	if(res == 0) {
		return false;
	}
	if(c == 0x55) { // U
		return true;
	}
	return false;
}

void upload(int fd, data_block& blk)
{
	bool ret;

	/* package header */
	unsigned char cmdhead[14] = {0xfe, 0, 0xff};
	*((unsigned int*)&cmdhead[4]) = htonl(blk.size); //TODO: fix this hack
	*((unsigned int*)&cmdhead[8]) = htonl(blk.addr); //TODO: fix this hack
	cmdhead[3] = blk.lmode;

	ret = send_command(fd, cmdhead, 14);
	if(!ret) {
		printf("Modem rejected header package\n");
		return;
	}

	/* package body */
	unsigned char cmddata[1040] = {0xda, 0, 0};
	unsigned int datasize = 1024;
	unsigned int packages_count = 1;
	unsigned int addr;
	for(addr = 0; addr < blk.size; addr += 1024) {
		if ((addr + 1024) >= blk.size) {
			datasize = blk.size - addr;
		}
		cmddata[1] = packages_count;
		cmddata[2] = (~packages_count) & 0xff;
		memcpy(cmddata + 3, blk.data + addr, datasize);
		packages_count++;

		ret = send_command(fd, cmddata, datasize + 5);
		if(!ret) {
			printf("Modem rejected data package\n");
			return;
		}
	}

	/* end of package */
	unsigned char cmdeod[5] = {0xed, 0, 0, 0, 0};
	cmdeod[1] = packages_count;
	cmdeod[2] = (~packages_count) & 0xff;
	ret = send_command(fd, cmdeod, 5);
	if(!ret) {
		printf("Modem rejected the end of data packet\n");
		return;
	}
	printf("data packet sent successfully\n");
}

int open_port(const char* devname) {
	int fd = open(devname, O_RDWR | O_NOCTTY |O_SYNC);
	if (fd == -1) {
		return -1;
	}

	struct termios sioparm;

	bzero(&sioparm, sizeof(sioparm));
	sioparm.c_cflag = B115200 | CS8 | CLOCAL | CREAD ;
	sioparm.c_iflag = 0;
	sioparm.c_oflag = 0;
	sioparm.c_lflag = 0;
	sioparm.c_cc[VTIME]=30; // timeout
	sioparm.c_cc[VMIN]=0;
	tcsetattr(fd, TCSANOW, &sioparm);
	return fd;
}
