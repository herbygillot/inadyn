/* Custom error logging system
 *
 * Copyright (C) 2010-2016  Joachim Nilsson <troglobit@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit the Free Software Foundation
 * website at http://www.gnu.org/licenses/gpl-2.0.html or write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#define SYSLOG_NAMES		/* Expose syslog.h:prioritynames[] */
#include <syslog.h>

#include "compat.h"

static int level   = LOG_NOTICE;
static int enabled = 0;

void log_init(char *ident, int log, int bg)
{
	int log_opts = LOG_PID | LOG_CONS | LOG_NDELAY;

#ifdef LOG_PERROR
	if (!bg && log < 1)
		log_opts |= LOG_PERROR;
#endif

	openlog(ident, log_opts, LOG_USER);
	setlogmask(LOG_UPTO(level));
	enabled = 1;
}

void log_exit(void)
{
	if (enabled)
		closelog();
}

int log_level(char *arg)
{
	int rc;

	for (int i = 0; prioritynames[i].c_name; i++) {
		if (string_match(prioritynames[i].c_name, arg)) {
			level = prioritynames[i].c_val;
			return 0;
		}
	}

	rc = atoi(arg);
	if (-1 == rc)
		return rc;

	level = rc;
	return 0;
}

void vlogit(int prio, const char *fmt, va_list args)
{
	if (level == INTERNAL_NOPRI)
		return;

	vsyslog(prio, fmt, args);
}

void logit(int prio, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	if (enabled)
		vlogit(prio, fmt, args);
	else if (prio <= level)
		vfprintf(stderr, fmt, args), fprintf(stderr, "\n");
	va_end(args);
}


/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */