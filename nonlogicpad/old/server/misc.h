/*
   Copyright Christopher "Feesh" Sawczuk 2006 (c)

   This file is part of Nonlogic Pad Server.

   Nonlogicpad is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Nonlogic Pad is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Nonlogic Pad; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/


#ifndef __MISC_H__
#define __MISC_H__
#include <stdio.h>
#include <fcntl.h>

#define LOG_ERROR 0
#define LOG_WARN 1
#define LOG_INFO 2


void *challoc(int bytes, char val);
void writeToLog(char *message, char priority);
void setNonBlock(int sock);
unsigned char *mkMsg(char head, int id, char *message,unsigned char *len);
#endif
