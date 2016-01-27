#include "irc.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

Irc::Irc(const std::string &server, const std::string &port, const std::function<void()> onConnect)
	: _server(server), _port(port), _onConnect(onConnect), _socket(_ios)
{
	// Ping back handler
	_readHandlers.push_back([this](const boost::tokenizer<boost::char_separator<char> > &tokenizer) {
		std::vector<std::string> tokens(begin(tokenizer), end(tokenizer)); 

		if(tokens[0].compare("PING") == 0)
			_pong(tokens[1]);	
	});

}

void Irc::connect()
{
	boost::asio::ip::tcp::resolver resolver(_ios);
	boost::asio::ip::tcp::resolver::query query(_server, _port);
	boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
	boost::asio::ip::tcp::resolver::iterator end;
	boost::system::error_code error = boost::asio::error::host_not_found;

	while(it != end)
	{
		if(!error)
			break;

		std::cout << "Connecting to " << _server << " " << _port << std::endl;

		boost::asio::async_connect(_socket, it,
        	boost::bind(&Irc::_connectHandler, this, error)
		);

		it++;
	
		if(error)
			std::cout << "Error : " << error.message() << std::endl;

	}

	if(error)
		std::cout << "Error connectinf to " << _server << " " << error.message() << std::endl;
	else
		std::cout << "Connection success" << std::endl;
	
}

void Irc::close()
{
	_socket.close();
	_ios.stop();
}

void Irc::user(const std::string &username)
{
	user(username, username, username, username);
}

void Irc::user(const std::string &username, const std::string &hostname, const std::string &server, const std::string &realname)
{
	command("USER ", username + " " + hostname + " " + server + " :" + realname);	
}

void Irc::nick(std::string &nickname)
{
	command("NICK", nickname);
}

void Irc::join(const std::string &chan)
{
	(chan.front() == '#' ? _chan = chan : _chan = '#' + chan);
	part(_chan);
	command("JOIN", _chan);
}

void Irc::part(const std::string &chan)
{
	command("PART", chan);
}

void Irc::privmsg(const std::string &to, const std::string &msg)
{
	command("PRIVMSG", to, msg);
}

void Irc::command(const std::string &cmd, const std::string &msg)
{
	std::string message(cmd + " " + msg + "\r\n");
	_send(message);
}

void Irc::command(const std::string &cmd, const std::string &to, const std::string &msg)
{
	std::string message(cmd + " " + to + " " + msg + "\r\n");
	_send(message);
}

void Irc::run()
{
	boost::asio::async_read_until(_socket, _buffer, "\r\n",
		boost::bind(&Irc::_read, this,
			boost::asio::placeholders::error
		)
	);

	_ios.run();
}

/*
 * Private
 */

void Irc::_read(const boost::system::error_code &error)
{
	if(error)
	{
		std::cerr << error.message() << std::endl;
		close();
	}
	else
	{
		std::string data(buffers_begin(_buffer.data()), buffers_begin(_buffer.data()) + _buffer.size());
		std::cout << data << std::endl;		

		boost::char_separator<char> sep("!@:; ");
    	boost::tokenizer<boost::char_separator<char> > tokenizer(data, sep);

		_readHandler(tokenizer);
		boost::asio::async_read_until(_socket, _buffer, "\r\n",
			boost::bind(&Irc::_read, this,
				boost::asio::placeholders::error
			)
		);
		
	}
}

inline void Irc::_send(std::string &message)
{
	boost::asio::write(_socket, boost::asio::buffer(message + "\r\n"));
}

void Irc::_readHandler(const boost::tokenizer<boost::char_separator<char> > &tokenizer)
{
	for(auto it : _readHandlers)
		it(tokenizer);
}

void Irc::_connectHandler(const boost::system::error_code &error)
{
	if(!error)
	{
		_onConnect();
	}
}

void Irc::_pong(const std::string &ping)
{
	std::string resp("PONG :" + ping);
	_send(resp);
}

