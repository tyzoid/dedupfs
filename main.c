#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>

#include "fuse_layer.h"

void show_help();

static struct option long_options[] = {
	{"datadir", required_argument, NULL, 'd'},
	{"fsdir",   required_argument, NULL, 'f'},
	{"help",   required_argument, NULL, 'h'}
};

int main(int argc, char **argv) {
	const char *data_location = "dedup_datastore";
	const char *fs_location =   "dedup_fs";

	int c;
	while ((c = getopt_long(argc, argv, "hd:f:", long_options, NULL)) >= 0) {
		switch (c) {
			case 'd':
				data_location = optarg;
				break;

			case 'f':
				fs_location = optarg;
				break;
			case 'h':
			case '?':
				show_help();
				return 0;
		}
	}
	

	initialize_filesystem(fs_location);

	return 0;
}

void show_help() {
	printf("Dedupfs Help:\n");
	printf("  --datadir -d  Specify the directory where dedupfs should look for/store it's data.\n");
	printf("  --fsdir   -f  Specify the directory of the FUSE mount.\n");
	printf("  --help    -h  Shows this help menu.\n");
}
