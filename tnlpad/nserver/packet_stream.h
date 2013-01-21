#ifndef _PACKET_STREAM_H_
#define _PACKET_STREAM_H_

class packet_stream
{
public:
	packet_stream(long length);
	~packet_stream();
	packet_stream &operator<<(char cdata);
	packet_stream &operator<<(char *cdata);
	packet_stream &operator<<(const char *cdata);
	packet_stream &operator<<(unsigned int cdata);
	char *get_data();
	long get_length();
	void clear_data();
private:
	char *data;	
	int offset;
	long length;
	long size;
};

#endif
