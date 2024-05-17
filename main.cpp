// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Denis Burkov <hitechshell@mail.ru>
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>

#include "file.h"
#include "protocol.h"

#include <vector>
#include <string>
#include <cstring>

void print_help() {
	printf(
"The utility is designed to load arbitrary code on a device with the BalongV7 chipset.\n\n"
"--help to display this help\n"
"--raminit <filename> path to the raminit file\n"
"--payload <filename> path to the payload file\n"
"--payload_addr <addr> address in memory to which the payload will be loaded (can be either a hex or decimal)\n\n"
"Thanks to forth32 for the loading algorithm\n"
"Copyright 2024 Denis Burkov <hitechshell@mail.ru>\n");
}

int main (int argc, char **argv)
{
	std::string raminit_filename;
	std::string payload_filename;
	std::string portname;
	int raminit_addr = 0;
	int payload_addr = -1;

	int c;

	if(argc == 1) {
		print_help();
		return 0;
	}

	while (1)
	{
		static struct option long_options[] =
		{
			{"help",	 no_argument,		 0, 'h'},
			{"raminit",	required_argument, 0, 'r'},
			{"payload",	required_argument, 0, 'p'},
			{"payload_addr",	required_argument, 0, 'a'},
			{"port",	required_argument, 0, 'P'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "hr:p:P:a:",
						 long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
		break;

		switch (c)
		{
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
			break;
			printf ("option %s", long_options[option_index].name);
			if (optarg)
			printf (" with arg %s", optarg);
			printf ("\n");
			break;

		case 'h':
			print_help();
			return 0;
			break;

		case 'r':
			raminit_filename = optarg;
			break;

		case 'a':
			printf("size %ld\n", strlen(optarg));
			if(strlen(optarg) > 2 && optarg[1] == 'x') {
				payload_addr = (int)strtol(optarg, NULL, 16);
			} else {
				payload_addr = (int)strtol(optarg, NULL, 10);
			}
			break;

		case 'p':
			payload_filename = optarg;
			break;

		case 'P':
			portname = optarg;
			break;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort ();
		}
	}

	if(portname.empty()) {
		printf("You need to specify the COM port (as example --port /dev/ttyUSB0)\n");
		return 0;
	}

	if(payload_filename.empty()) {
		printf("You must provide payload file.\n");
		return 0;
	}

	if(payload_addr < 0) {
		printf("You must provide payload load address.\n");
		return 0;
	}

	if(raminit_filename.empty()) {
		printf("You must provide raminit file.\n");
		return 0;
	}

	printf("COM port: %s\n", portname.c_str());

	printf("raminit: %s\n", raminit_filename.c_str());
	printf("pyload: %s\n", payload_filename.c_str());
	printf("pyload address: 0x%x\n", payload_addr);

	File raminit_file(raminit_filename.c_str());
	raminit_file.load();
	File payload_file(payload_filename.c_str());
	payload_file.load();

	printf("raminit size: %lu\n", raminit_file.size());
	printf("payload size: %lu\n", payload_file.size());

	data_block raminit_block;
	raminit_block.lmode = 1;
	raminit_block.size = raminit_file.size();
	raminit_block.addr = raminit_addr;
	raminit_block.data = raminit_file.get_content();

	data_block payload_block;
	payload_block.lmode = 2;
	payload_block.size = payload_file.size();
	payload_block.addr = payload_addr;
	payload_block.data = payload_file.get_content();

	int fd = open_port(portname.c_str());
	if(fd < 0) {
		printf("Error: COM port dont answer\n");
		return 0;
	}
	if(!check_mode(fd)) {
		printf("Error: The port is not in USB Boot mode\n");
		return 0;
	}
	upload(fd, raminit_block);
	upload(fd, payload_block);

	return 0;
}
