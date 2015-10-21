#ifndef CS_LOG_H
#define CS_LOG_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h> /* for exit() */
#include <iostream>
//Author: Jake Merdish

// make sure to define LOGFILE before including this.
// also, this only works on GNU C
#ifndef LOGFILE
#define LOGFILE "output.log"
#endif

extern FILE* __LOGFILE;

// keeping it as a macro for ease of varargs
#define log(file, line, level, termout, format, ...) {\
		fprintf(termout, "[%s][%s:%d] ", level, file, line); \
		fprintf(termout, format, ##__VA_ARGS__); \
		fprintf(termout, "\n"); \
        if (!__LOGFILE) __LOGFILE = fopen(LOGFILE, "w"); \
		if (__LOGFILE) { \
				fprintf(__LOGFILE, "[%s][%s:%d] ", level, file, line); \
				fprintf(__LOGFILE, format, ##__VA_ARGS__);\
				fprintf(__LOGFILE, "\n"); \
				fflush(__LOGFILE); \
		} \
}

#define log_info(format, ...) log(__FILE__, __LINE__, "INFO", stdout,  format, ##__VA_ARGS__)
#define log_warn(format, ...) log(__FILE__, __LINE__, "WARN", stderr, format, ##__VA_ARGS__)
#define log_error(format, ...) log(__FILE__, __LINE__, "ERROR", stderr,  format, ##__VA_ARGS__)
#define log_debug(format, ...) log(__FILE__, __LINE__, "DEBUG", stdout, format, ##__VA_ARGS__)
#define DieWithError(msg, ...) {log(__FILE__, __LINE__, "FATAL", stderr, msg, ##__VA_ARGS__); _Exit(1);}

#endif // guard
