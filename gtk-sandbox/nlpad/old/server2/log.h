#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>

enum log_levels
{
	LOG_INFO=0,
	LOG_WARN,
	LOG_ERROR,
	LOG_TEMP
};

extern void log_file_open();
extern void log_writef(int level, const char *format, ...);
extern void log_file_close();

#endif
