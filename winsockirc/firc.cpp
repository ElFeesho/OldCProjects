			if(str_buf.find("PRIVMSG")!=std::string::npos)
			{
				if(on_privmsg!=NULL)
				{
					std::string nick, channel, message;
					try
					{
						nick = str_buf.substr(1,str_buf.find("!")-1);
					}
					catch(...)
					{
						std::cout << "Caught an exception trying to find someone's nick" << std::endl;
						return 0;
					}
					try
					{
						if(str_buf.find("#")!=std::string::npos && str_buf.find("#")>str_buf.find(" :"))
							channel = str_buf.substr(str_buf.find("#"),str_buf.find(" :")-str_buf.find("#"));
						else
							channel = "";
					}
					catch(...)
					{	
						std::cout << "Caught an exception trying to find the channel name in a PM" << std::endl;
						return 0;
					}
					try
					{
						message = str_buf.substr(str_buf.find(" :")+2,(str_buf.length()-2)-(str_buf.find(" :")+2));			
					}
					catch(...)
					{
						std::cout << "Caught an exception trying to retrieve the message from a privmsg" << std::endl;
						return 0;
					}
					on_privmsg(this, nick.c_str(), (channel.length()>0)?(channel.c_str()):(NULL), message.c_str());
					if(message.find(command_delim)!=std::string::npos && message.length()!=std::string(command_delim).length())
					{
						on_command(this,nick.c_str(),(channel.length()>0)?(channel.c_str()):(NULL),message.substr(std::string(command_delim).length(),message.find(" ")-std::string(command_delim).length()).c_str(),(message.find(" ")!=std::string::npos)?(message.substr(message.find(" ")+1,(message.length())-(message.find(" ")+1)).c_str()):(NULL));
					}
				}
			}
			else
			{
				if(on_raw_data!=NULL)
					on_raw_data(this, str_buf.c_str(),len);	
			}
		}
