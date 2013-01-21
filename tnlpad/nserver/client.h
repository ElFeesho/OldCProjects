#ifndef __CLIENT_H__
#define __CLIENT_H__

class Client
{
public:
	/*
		CTor / DTor
	*/

	Client();
	~Client();

	/*
		Setters
	*/

	void set_id(int nid);
	void set_logged_in(); // Will only ever be allowed to log in...
	void set_status(int nstatus);
	void set_attribs(int nattribs);
	
	void set_nick(char *nnick);
	void set_name(char *nname);
	void set_website(char *nwebsite);
	void set_email(char *nemail);
	
	void set_channel(void *ncurrent_channel);
	void set_delete();
	
	/*
		Getters
	*/
	
	int get_id() const;
	int get_status() const;
	int get_attribs() const;
	bool get_delete();
	bool get_logged_in() const;
	
	char *get_nick();
	char *get_name();
	char *get_website();
	char *get_email();
	
	void *get_channel();
	
private:
	int id;						// Doubles as socket descriptor
	int status;					// Stores whether the client is available or not, i.e. online, away, busy
	int attribs;				// Stores whether the client has Voice, Ops, Ink and so on
	
	bool logged_in;			// Contains whether or not the client is logged in
	bool to_delete;
	char nick[17];				// 16 character nick name
	char name[33];				// 32 character real name
	char website[33];			// 32 character web address
	char email[33];			// 32 character email address
		
	void *current_channel;	// A pointer to a Channel object, which is the current channel the user is in
};

#endif
