/*
 * log.c
 *
 * Simple logging library.
 *
 * https://github.com/e03213ac/liblog
 *
 * This work belongs to the Public Domain. Everyone is free to use, modify,
 * republish, sell or give away this work without prior consent from anybody.
 *
 * This software is provided on an "AS IS" basis, without warranty of any kind.
 * Use at your own risk! Under no circumstances shall the author(s) or
 * contributor(s) be liable for damages resulting directly or indirectly from
 * the use or non-use of this documentation.
 */


#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "log.h"

struct log
{
	FILE            * log_f;
	pid_t             log_pid;
	pthread_mutex_t   log_mux;
};

int
log_open_f(
	FILE            * const f,
	struct log     ** const newlog
	)
{
	struct log      * log;
	int               err;

	if ((log = malloc(sizeof(struct log))) == NULL) {
		err = errno;
		goto fail_malloc;
	}

	memset(log, 0, sizeof(struct log));
	log->log_f = f;
	log->log_pid = getpid();
	if ((err = pthread_mutex_init(&log->log_mux, NULL)))
		goto fail_mux;

	(void) setvbuf(f, NULL, _IOLBF, 0);

	*newlog = log;
	return 0;

fail_mux:
	free(log);
fail_malloc:
	*newlog = NULL;
	return err;
}

void
log_close(struct log * const log)
{
	if (log->log_f != stderr)
		fclose(log->log_f);
	pthread_mutex_destroy(&log->log_mux);
	free(log);
}

int
log_printf_at(
	struct log    * log,
	char const    * file,
	int unsigned    line,
	char const    * func,
	enum
	log_level       level,
	char const    * fmt,
	                ...
	)
{
	struct timeval  tv;
	va_list         ap;

	static char const * const levelstr[] = {
		[LOG_CRIT]  = "[PAN]",
		[LOG_ERR]   = "[ERR]",
		[LOG_WARN]  = "[WRN]",
		[LOG_INFO]  = "[INF]",
		[LOG_TRACE] = "[TRC]",
		[LOG_DEBUG] = "[DBG]",
	};

	if ((unsigned) level >= (sizeof(levelstr) / sizeof(levelstr[0])))
		return EINVAL;

	va_start(ap, fmt);
	pthread_mutex_lock(&log->log_mux);
	FILE * const f = log->log_f;
	gettimeofday(&tv, NULL);
	fprintf(f, "%ld.%06ld: %05u: %s %s:%u: in %s(): ",
		(long) tv.tv_sec,
		(long) tv.tv_usec,
		(unsigned) log->log_pid,
		levelstr[level],
		file, line, func);
	vfprintf(f, fmt, ap);
	putc('\n', f);
	pthread_mutex_unlock(&log->log_mux);
	va_end(ap);

	return 0;
}

int
log_perror_at(
	struct log    * const log,
	char const    * const file,
	int unsigned    const line,
	char const    * const func,
	enum
	log_level       const level,
	char const    * const msg,
	int             const err
	)
{
	char buf[128];

	if (err == 0)
		log_abort_at(log, file, line, func,
			     "log_perror() called with a zero error code");

	memset(buf, 0, sizeof(buf));
#ifdef __APPLE__
	strerror_r(err, buf, sizeof(buf) - 1);
	return log_printf_at(log, file, line, func, level, "%s: %s", msg, buf);
#elif defined __linux__
	char const * const s = strerror_r(err, buf, sizeof(buf) - 1);
	return log_printf_at(log, file, line, func, level, "%s: %s", msg, s);
#endif
}

void
log_abort_at(
	struct log    * const log,
	char const    * const file,
	int unsigned    const line,
	char const    * const func,
	char const    * const msg
	)
{
	log_printf_at(log, file, line, func, LOG_CRIT, "%s", msg);
	pthread_mutex_lock(&log->log_mux);
	fclose(log->log_f);
	abort();
}
