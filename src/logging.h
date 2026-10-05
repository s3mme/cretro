#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#include "cli.h"

typedef enum log_level {
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
} log_level;

void log_str(log_level dbg_lvl, char const *dbg_lvl_str, FILE *stream, char const *msg, ...);

void log_set_lvl(arg_conf const *conf);

#define LOG_DEBUG(msg, ...)	  log_str(DEBUG, "DEBUG", stdout, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...)	  log_str(INFO, "INFO", stdout, msg, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) log_str(WARNING, "WARNING", stdout, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...)	  log_str(ERROR, "ERROR", stderr, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...)	  log_str(FATAL, "FATAL", stderr, msg, ##__VA_ARGS__)

#endif /* LOGGING_H */
