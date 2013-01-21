#ifndef __PACKET_HPP__
#define __PACKET_HPP__

enum packet_t
{
	DATA_LINE = 0x01,
	DATA_TEXT = 0x02,
	DATA_QUIT = 0x04,
	DATA_CLEAR = 0x06,
	DATA_HNICK = 0x08, // Handshake Nickname
	DATA_NICK = 0x09,
	DATA_RECT = 0x0A,
	DATA_FILL = 0x0B,
	DATA_ACTION = 0x0C, // They speak louder than words...
	DATA_CHAN = 0x0D,
	DATA_TOPIC = 0x0E,
	DATA_NAME = 0x11,
	DATA_WEBS = 0x12,
	DATA_MAIL = 0x13,
	DATA_LOG = 0x14, // Log in and become an op on a channel
	DATA_REG = 0x15, // Register a channel.
	DATA_LOCK = 0x17, // Prevent new connections to channel
	DATA_LOCKTOPIC = 0x18,
	DATA_TAKEINK = 0x19,
	DATA_TAKEVOICE = 0x1A,
	DATA_GIVEINK = 0x1B,
	DATA_CIRC = 0x1C,
	DATA_GIVEVOICE = 0x1D
};

class Packet
{

};

#endif
