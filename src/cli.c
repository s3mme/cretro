#include "cli.h"
#include "logging.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>

/******************************************************
 *** LOCAL DEFINES                                  ***
 ******************************************************/

#define MAX_HZ (1000000L)

/******************************************************
 *** LOCAL VARIABLES                                ***
 ******************************************************/

static char const *usage_str = "cretro [-d <0..4>] [-f <FREQUENCY_HZ>] <ROM>";

/******************************************************
 *** LOCAL METHODS                                  ***
 ******************************************************/

static int safe_strtol(char const *str_to_conv, int *store_into) {
	char *end;
	long const strtol_in = strtol(str_to_conv, &end, 10);

	errno = 0;

	if (end == str_to_conv) {
		LOG_FATAL("%s: not a decimal number", str_to_conv);
	} else if ('\0' != *end) {
		LOG_FATAL("%s: extra characters at end of input: %s", str_to_conv, end);
	} else if ((LONG_MIN == strtol_in || LONG_MAX == strtol_in) && ERANGE == errno) {
		LOG_FATAL("%s out of range of type long", str_to_conv);
	} else if (strtol_in > INT_MAX) {
		LOG_FATAL("%ld greater than INT_MAX", strtol_in);
	} else if (strtol_in < INT_MIN) {
		LOG_FATAL("%ld less than INT_MIN", strtol_in);
	} else {
		*store_into = (int) strtol_in;
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

static void handle_arg_frequency(arg_conf *conf, long frequency) {
	// calculate delay from hertz input
	LOG_DEBUG("Frequency [Hz] input specified: %ld.", frequency);

	if (frequency > MAX_HZ) {
		LOG_ERROR("Provided frequency %ld is higher than allowed frequency %ld. Used Fallback: %ld", frequency,
				  MAX_HZ, MAX_HZ);
		frequency = MAX_HZ;
	} else if (frequency <= 0) {
		LOG_ERROR("Provided frequency %ld is lower or equal to 0. Used Fallback: %ld", frequency, MAX_HZ);
		frequency = MAX_HZ;
	}

	long delay = MAX_HZ / frequency;
	if (delay > INT_MAX)
		LOG_FATAL("Delay is out of bounds: %d > %d", delay, INT_MAX);

	conf->us_delay = (int) (delay);
	LOG_INFO("Delay set to %d us.", conf->us_delay);
}

/******************************************************
 *** EXPOSED METHODS                                ***
 ******************************************************/

arg_conf *cli_config_default(void) {
	arg_conf *conf = malloc(sizeof(arg_conf));

	conf->debug	   = FATAL;
	conf->us_delay = 0;
	conf->rom_path = "";

	return conf;
}

void cli_config_destroy(arg_conf *conf) {
	free(conf);
}

int cli_config_handle(arg_conf *const conf, int argc, char **argv) {
	int c;

	if (argc < 2) {
		LOG_FATAL("No ROM path specified!");
		fprintf(stderr, "%s\n", usage_str);
		return EXIT_FAILURE;
	}

	// parse all options first
	int strtol_in;
	while ((c = getopt(argc, argv, "d:f:")) != -1) {
		switch (c) {
			case 'd':
				if (safe_strtol(optarg, &strtol_in))
					return EXIT_FAILURE;

				conf->debug = strtol_in;
				log_set_lvl(conf);
				break;
			case 'f':
				if (safe_strtol(optarg, &strtol_in))
					return EXIT_FAILURE;

				handle_arg_frequency(conf, strtol_in);
				break;
			default:
				fprintf(stderr, "%s\n", usage_str);
				return EXIT_FAILURE;
		}
	}

	// parse the remaining options

	if (argc - optind > 1) {
		LOG_FATAL("You provided too many arguments!");
		fprintf(stderr, "%s\n", usage_str);
		return EXIT_FAILURE;
	}

	if (optind >= argc) {
		LOG_FATAL("No ROM path specified!");
		fprintf(stderr, "%s\n", usage_str);
		return EXIT_FAILURE;
	}

	// path to rom is the only remaining argument
	conf->rom_path = argv[optind++];
	LOG_DEBUG("Path to ROM: %s", conf->rom_path);

	return EXIT_SUCCESS;
}
