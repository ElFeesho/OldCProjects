/*
	Copyright Christopher "Feesh" Sawczuk 2006 (c)
	
	This file is part of Nonlogicpad.

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
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __HELPER_H__
#define __HELPER_H__

#include <gtk/gtk.h>
#include <gnet.h>
#include <stdlib.h> // FOR int32_t DEF!
#ifdef WIN32
#include <stdint.h>
#endif
#include "main.h"
#include "callbacks.h"

void showWelcome();
void flushCanvas();
void flushCanvasArea(int32_t x, int32_t y, int32_t width, int32_t height);
void addLineD(int32_t id,int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, unsigned char width,unsigned char alpha);
void addRectD(int32_t id,int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, unsigned char width,unsigned char alpha);
void addTempLineD(int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, char width);
void addTempRectD(int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, char width);

void addClient(char *name, int32_t id);
void purgeClientList();
void changeName(char *nname, int32_t id);

void connectToServer(char *name, int32_t port, char *channel);
void sendData(unsigned char *data, int32_t len);

void errorDialog(char *msg);
void addToHistory(char *message);
void addMsgHistory(int32_t id, char *msg, int32_t len);
void addActionHistory(int32_t id, char *action, int32_t len);
void clearCanvas();
void dumpCanvas();

char *getClientName(int id);
void removeClient(int id);

GdkColor *createColour(int r, int g, int b);

void drawGrid(int sepSize);
void changeCursor();
gboolean removeLineXY(int32_t x, int32_t y,int32_t x2, int32_t y2);
char *subStr(char *string, char delim, int num, size_t size);
void changeChannel();

#endif
