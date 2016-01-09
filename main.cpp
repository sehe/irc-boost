#include <iostream>
#include <csignal>
#include <vector>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "irc.hpp"

int main()
{
	std::string server = "irc.mibbit.net";
	unsigned short port = 6667;
	std::string nick = "Bl4ckb0t";
	std::string chan = "#Bl4cktest";	

	Irc irc(server, port, true, true);
	irc.user(nick);
	irc.nick(nick);	
	irc.join(chan);
	irc.privmsg(chan, "D-z est pas beau");	

	std::vector<std::string> data;
	
	while(1)
	{
		irc.receive();	
	}

	return 0;
}

