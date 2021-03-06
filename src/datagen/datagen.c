/*
 * Copyright 2016-2017 PostgreSQL Global Development Group
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#include "datagen.h"

#define TABLE_PRODUCT 1
#define TABLE_SALES 2
#define TABLE_STORE 3
#define TABLE_TIME 4

void usage(char *filename)
{
	printf("usage: %s [options]\n", filename);
	printf("  options:\n");
	printf("    -c <int> - number of data file chunks to generate, default: 1\n");
	printf("    -C <int> - specify which chunk to generate\n");
	printf("    -d <path> - directory to generate data files, default: .\n");
	printf("    -p <int> - number of products to generate, default: 1\n");
	printf("    -s <int> - gibibytes of data to generate default: 1,\n");
	printf("    -t <table> - generate data for only 1 table: product,\n");
	printf("                 sales, store, time\n");
	printf("    -y <int> - number of years of data to generate, default: 1\n");
}

int main(int argc, char *argv[])
{
	int c;
	char outdir[DIR_LEN + 1] = ".";
	int data_format = DATA_FORMAT_PGSQL;
	struct df_t df;

	int table = 0;

	/* Initialize default values. */

	df.chunks = 1;
	df.days = 0;
	df.only_one_chunk = 0;
	df.products = 1;
	df.scale_factor = 1;
	df.seed = -1;
	df.years = 1;

	while (1) {
		int option_index = 1;
		static struct option long_options[] = {
			{0, 0, 0, 0,}
		};

		c = getopt_long(argc, argv, "c:C:d:f:hp:s:S:t:y:",
				long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			break;
		case 'c':
			df.chunks = atoi(optarg);
			break;
		case 'C':
			df.only_one_chunk = 1;
			df.chunk = atoi(optarg);
			break;
		case 'd':
			strncpy(outdir, optarg, DIR_LEN);
			break;
		case 'f':
			if (strcmp("pgsql", optarg) == 0) {
				data_format = DATA_FORMAT_PGSQL;
			} else {
				printf("unknown data format: %s\n", optarg);
				return 3;
			}
			break;
		case 'h':
			usage(argv[0]);
			return 0;
		case 'p':
			df.products = atoi(optarg);
			break;
		case 'S':
			df.seed = atoll(optarg);
			break;
		case 's':
			df.scale_factor = atoi(optarg);
			break;
		case 't':
			if (strcmp("product", optarg) == 0) {
				table = TABLE_PRODUCT;
			} else if (strcmp("sales", optarg) == 0) {
				table = TABLE_SALES;
			} else if (strcmp("store", optarg) == 0) {
				table = TABLE_STORE;
			} else if (strcmp("time", optarg) == 0) {
				table = TABLE_TIME;
			} else {
				printf("unknown table: %s\n", optarg);
				return 4;
			}
			break;
		case 'y':
			df.years = atoi(optarg);
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
			return 2;
		}
	}

	if (df.chunks < 1) {
		fprintf(stderr, "ERROR: -c must be at least 1\n");
		return 10;
	}

	if (df.only_one_chunk && (df.chunk < 1 || df.chunk > df.chunks)) {
		fprintf(stderr, "ERROR: -C must be at least 1 and at most %d\n",
				df.chunks);
		return 11;
	}

	init_format(data_format, &df);

	if (table == 0 || table == TABLE_PRODUCT)
		gen_product_data(outdir, &df);

	if (table == 0 || table == TABLE_SALES)
		gen_sales_data(outdir, &df);

	if (table == 0 || table == TABLE_STORE)
		gen_store_data(outdir, &df);

	if (table == 0 || table == TABLE_TIME)
		gen_time_data(outdir, &df);

	return 0;
}
