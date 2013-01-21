#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#define CHANNEL_LOCK		0x1
#define CHANNEL_TOPIC_LOCK 0x02
#define CHANNEL_HIDDEN	0x4

#define USER_INK			0x1
#define USER_VOICE		0x2

#include <vector>
#include "client.h"

class Channel
{
	
public:
	
	/*
		CTor/DTor
	*/
		Channel(char *nname, int id);
		~Channel();
	
	/*
		Setters
	*/
	
	void set_channel_attribs(int nattrs);
	void set_default_attribs(int ndefs);
	void set_topic(char *ntopic);
	void set_pass(char *npass);
	
	/*
		Getters
	*/
	
	int get_channel_attribs() const;
	int get_default_attribs() const;
	
	char *get_name();
	char *get_pass();
	char *get_topic();
	
	bool is_op(Client *client);
	
	std::vector<Client*> *get_client_list();
	
	/*
		Misc
	*/
	
	void add_op(Client *client);
	void remove_op(Client *client);
	
private:
	
	int id;						// Stores the channel's unique identification number
	int channel_attribs;		// Stores whether the channel is locked (only users inside can use it), hidden (doesn't show up on a channel list), topic is locked 
	int default_attribs;		// Stores a bitmask of the default attributes clients get in the room.
	
	std::vector<Client*> *client_list;	// Store a list of clients currently in the room
	std::vector<Client*> *ops;				// The operators
	char name[33];				// Stores the name of the channel
	char topic[129];			// Stores the channels topic
	char pass[33];				// Stores the channel's password - Used for checking whether ops can be granted
	
	static int channel_count;
};

#endif
