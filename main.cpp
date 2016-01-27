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
	std::string port = "6667";
	std::string nick = "so-sehe";
	std::string chan = "#CBNA";	

	Irc irc(server, port, [&irc, &nick, &chan](){
		irc.user(nick);
		irc.nick(nick);	
		irc.join(chan);
	});
	

	irc.run();

	return 0;
}

