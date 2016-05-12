/*
 * log.h
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

#ifndef __LIBLOG_H__
#define __LIBLOG_H__

#include <stdio.h>
#include <string.h>

typedef struct log log_t;  ///< log handle

enum log_level
{
	LOG_CRIT,   // critical errors, system instability
	LOG_ERR,    // error messages
	LOG_WARN,   // warning messages
	LOG_INFO,   // informational messages
	LOG_TRACE,  // used to trace error events, function calls, etc.
	LOG_DEBUG,
};

extern int
log_open_f(
	FILE         * f,
	log_t       ** log
	);

extern void
log_close(
	log_t        * log
	);


__attribute__ ((format (printf, 6, 7)))
extern int
log_printf_at(
	log_t        * log,
	char const   * file,
	int unsigned   line,
	char const   * func,
	enum
	log_level      level,
	char const   * fmt,
	               ...
	);

extern int
log_perror_at(
	log_t        * log,
	char const   * file,
	int unsigned   line,
	char const   * func,
	enum
	log_level      level,
	char const   * msg,
	int            err
	);

__attribute__ ((noreturn))
extern void
log_abort_at(
	log_t        * log,
	char const   * file,
	int unsigned   line,
	char const   * func,
	char const   * msg
	);

#define log_printf(log, level, ...) \
    log_printf_at(log, __FILE__, __LINE__, __func__, level, __VA_ARGS__)

#define log_perror(log, level, msg, err) \
    log_perror_at(log, __FILE__, __LINE__, __func__, level, msg, err)

#define log_abort(log, msg) \
    log_abort_at(log, __FILE__, __LINE__, __func__, msg)

#define log_panic(log) \
    log_abort(log, "*** panic, unrecoverable error")

#define log_bug(log) \
    log_abort(log, "*** oops, a bug!")

#define log_bug_nopanic(log) \
    log_printf_at(log, __FILE__, __LINE__, __func__, LOG_CRIT, \
                  "*** oops, a bug! don't panic...")

#define log_assert(log, cond) \
    do { \
	if (__builtin_expect(!(cond), 0)) \
            log_abort(log, "*** Assertion error"); \
    } while (0)

#define log_entering(log) \
    log_printf_at(log, __FILE__, __LINE__, __func__, LOG_DEBUG, \
                  "entering %s()", __func__)

#define log_entering_args(log, fmt, ...) \
    log_printf_at(log, __FILE__, __LINE__, __func__, LOG_DEBUG, \
                  "entering %s(" fmt ")", __func__, ## __VA_ARGS__)

#define log_leaving(log) \
    log_printf_at(log, __FILE__, __LINE__, __func__, LOG_DEBUG, \
                  "leaving %s()", __func__)

#define log_leaving_ret(log, fmt, ...) \
    log_printf_at(log, __FILE__, __LINE__, __func__, LOG_DEBUG, \
                  "leaving %s() = " fmt, __func__, ## __VA_ARGS__)

#define log_leaving_err(log, err) \
do { \
    int const __err = (err); \
    int const __abserr = __err > 0 ? (__err) : -(__err); \
    if (__err) \
        log_leaving_ret(log, "%d (%s)", __err, strerror(__abserr)); \
    else \
        log_leaving_ret(log, "%d", __err); \
} while(0)

#define log_unexpected(log) \
    log_printf_at(log, __FILE__, __LINE__, __func__, LOG_ERR, \
                  "unexpected condition")

#endif
