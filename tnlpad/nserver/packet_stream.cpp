#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "packet_stream.h"

packet_stream::packet_stream(long length) : data(new char[length]), offset(0), length(length), size(length)
{
	memset(data,'\0',length);
}

packet_stream::~packet_stream()
{
	delete data;
}
packet_stream &packet_stream::operator<<(char cdata)
{
	memcpy((void*)(data+offset),(void*)&cdata,1);
	offset++;
	length = offset;
	return *this;
}

packet_stream &packet_stream::operator<<(char *cdata)
{
	memcpy((void*)(data+offset),(void*)cdata,strlen(cdata));
	offset+=strlen(cdata);
	length = offset;
	return *this;
}

packet_stream &packet_stream::operator<<(const char *cdata)
{
	memcpy((void*)(data+offset),(void*)cdata,strlen(cdata));
	offset+=strlen(cdata);
	length = offset;
	return *this;
}

packet_stream &packet_stream::operator<<(unsigned int cdata)
{
	memcpy((void*)(data+offset),(void*)&cdata,4);
	offset+=4;
	length = offset;
	return *this;
}

long packet_stream::get_length()
{
	return length;
}

char *packet_stream::get_data()
{
	return data;
}

void packet_stream::clear_data()
{
	memset(data,'\0',size);
	offset = 0;
	length = 0;
}
