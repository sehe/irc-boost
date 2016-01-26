#include "irc.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/bind.hpp>

Irc::Irc(const std::string &server, unsigned short port, bool verbose, bool autoconnect) :
	_server(server), _port(port), _verbose(verbose), _ios(), _socket(_ios)
{
	_readHandlers.push_back([this](const std::vector<std::string> &t) {
		for(std::size_t i = 0; i < t.size(); i++)
			//std::cout << i << " " << t[i] << std::endl;
	});

	if(autoconnect)
		connect();
}

Irc::~Irc()
{
	disconnect();
}

bool Irc::connect()
{
	if(_verbose) printf("Attempting to connect to %s\n", _server.c_str());
	try {
		boost::asio::ip::tcp::resolver resolver(_ios);		
		boost::asio::ip::tcp::resolver::query query(_server, std::to_string(_port));
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;
		boost::system::error_code error = boost::asio::error::host_not_found;
		
		if(_verbose) printf("Searching for a valid endpoint \n");
		while(it != end)
		{
			boost::asio::ip::tcp::endpoint endpoint = *it++;
			if(_verbose) printf("\tTrying to connect to %s\n", endpoint.address().to_string().c_str());				

			_socket.close();
			_socket.connect(endpoint, error);
			
			if(error)
			{
				if(_verbose) printf("\t\tError\n");
				continue;
			}
			else
			{
				if(_verbose) printf("\t\tSuccess\n");
				break;
			}

		}
		if(_verbose) printf("Connection to %s successful\n", _server.c_str());
		std::this_thread::sleep_for(std::chrono::seconds(1));
		return true;
	}
	catch(std::exception const &e) {
		if(_verbose) printf("Connection failed, please check the address\n");
		return false;
	}
}

bool Irc::disconnect()
{
	part(_chan);
	command("QUIT", "Goodbye cruel world");

	boost::system::error_code error;
	_socket.close(error);
	if (error)
		return false;
	
	return true;
}

void Irc::nick(const std::string &nickname)
{
	command("NICK", nickname);
}

void Irc::user(const std::string &username)
{
	user(username, username, username, username);
}

void Irc::user(const std::string &username, const std::string &hostname, const std::string &server, const std::string &realname)
{
	command("USER", username + " " + hostname + " " + server + " : " + realname);
}

void Irc::join(const std::string &chan)
{
	(chan.front() == '#' ? _chan = chan : _chan = '#' + chan);

	command("PART", _chan);
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
	std::string message = cmd + " " + msg + "\r\n";
	_send(message);
}

void Irc::command(const std::string &cmd, const std::string &to, const std::string &msg)
{
	std::string message = cmd + " " + to + " " + msg + "\r\n";
	_send(message);
}

void Irc::run()
{
	_socket.async_read_some(boost::asio::buffer(_buffer), 
		boost::bind(
			&Irc::_read, 
			this, 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
	
	_ios.run();
}

/*
 *	Private
 */

void Irc::_send(const std::string &msg)
{
	boost::asio::async_write(_socket, boost::asio::buffer(msg),
		boost::bind(&Irc::_sendHandler, this,
		boost::asio::placeholders::error)
	);
}

void Irc::_read(const boost::system::error_code &error, std::size_t length)
{
	if(error)
	{
		std::cerr << error.message() << std::endl;
	}
	else
	{
		std::cout << std::string(_buffer.data(), length) << std::endl;

		boost::char_separator<char> sep("!.@:; ");
		boost::tokenizer<boost::char_separator<char> > tokenizer(std::string(_buffer.data(), length), sep);
		std::vector<std::string> tokens(begin(tokenizer), end(tokenizer));

		_readHandler(tokens);	

		boost::asio::async_read(_socket, boost::asio::buffer(_buffer),
			boost::asio::transfer_at_least(20),
			boost::bind(
				&Irc::_read, 
				this, 
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}
}

void Irc::_readHandler(const std::vector<std::string> &tokens)
{
	for(auto func: _readHandlers) {
		func(tokens);
	}
}


void Irc::_sendHandler(const boost::system::error_code &error)
{
	if(error)
		std::cerr << error.message() << std::endl;
}
