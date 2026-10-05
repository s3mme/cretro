#ifndef CLI_H
#define CLI_H

#include <stdint.h>

typedef struct arg_conf {
	int debug;
	int us_delay;
	char const *rom_path;
} arg_conf;

arg_conf *cli_config_default(void);

void cli_config_destroy(arg_conf *conf);

int cli_config_handle(arg_conf *const conf, int argc, char **argv);

#endif /* CLI_H */
