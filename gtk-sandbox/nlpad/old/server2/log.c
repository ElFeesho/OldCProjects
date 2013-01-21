#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <time.h>

enum log_levels
{
	LOG_INFO=0,
	LOG_WARN,
	LOG_ERROR,
	LOG_TEMP
};

void log_file_open();
void log_writef(int level, const char *format, ...);
void log_file_close();

static FILE *log_file = 0;

void log_file_open()
{
	char *filename = malloc(64);
	time_t current_time = time(NULL);
	struct tm *now = localtime(&current_time);
	sprintf(filename,"logs/nlpad-%.2d-%.2d-%.2d.log",now->tm_mday,now->tm_mon,now->tm_year);
	log_file = fopen(filename,"w");
	log_writef(0,"Nonlogic Pad Logging Started\n");
}

void log_writef(int level, const char *format, ...)
{
	if(!log_file)
	{
		fprintf(stderr,"Logfile not opened!!!\n");
		return;
	}
	/* Any work arounds for buffer? I don't think so */
	char *buffer = malloc(1024);
	time_t current_time = time(NULL);
	struct tm *now = localtime(&current_time);
	sprintf(buffer,"%.2d:%.2d:%.2d - ",now->tm_hour, now->tm_min, now->tm_sec);
	switch(level)
	{
		case LOG_INFO:
			strcat(buffer,"(II) ");
			break;
		case LOG_WARN:
			strcat(buffer,"(WW) ");
			break;
		case LOG_ERROR:
			strcat(buffer,"(EE) ");
			break;
		case LOG_TEMP:
			strcat(buffer,"(TT) ");
			break;
		default:
			strcat(buffer,"(UD) ");
			break;
	}
	va_list args;
	va_start(args, format);
	while(*format)
	{
		if((*format) == '%')
		{
			format++;
			if((*format) == '%')
			{
				sprintf(buffer,"%s%%",buffer);
			}
			if((*format) == 's')
			{
				char *string = va_arg(args, char *);
				sprintf(buffer,"%s%s",buffer,string);
			}
			else if((*format) == 'd')
			{
				int num = va_arg(args, int);
				sprintf(buffer,"%s%d", buffer, num);
			}
		}
		else
		{
			sprintf(buffer,"%s%c",buffer,(*format));
		}
		format++;
	}
	va_end(args);
	fprintf(log_file,"%s",buffer);
	fflush(log_file);
}

void log_file_close()
{
	log_writef(0,"Nonlogic Pad Logging Stopped\n");
	fclose(log_file);
}
