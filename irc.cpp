#include "irc.hpp"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

Irc::Irc(const std::string &server, unsigned short port, bool verbose, bool autoconnect) :
	_server(server), _port(port), _verbose(verbose), _ios(), _socket(_ios), _buffer(1024, 0)
{
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
				receive();
				break;
			}

		}
		if(_verbose) printf("Connection to %s successful\n", _server.c_str());
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

void Irc::nick(const std::string &nickname, bool skipmsg)
{
	_send("NICK " + nickname + "\r\n");
	receive();
}

void Irc::user(const std::string &username, bool skipmsg)
{
	user(username, username, username, username, skipmsg);
}

void Irc::user(const std::string &username, const std::string &hostname, const std::string &server, const std::string &realname, bool skipmsg)
{
	_send("USER " + username + " " + hostname + " " + server + " : " + realname + "\r\n");
	receive();
}

void Irc::join(const std::string &chan, bool skipmsg)
{
	if(_chan == "")
		_chan = chan;
	
	if(_chan != chan)
		return;

	_send("PART " + _chan + "\r\n");
	_send("JOIN " + _chan + "\r\n");
	receive();
}

void Irc::part(const std::string &chan, bool skipmsg)
{
	if(_chan == "")
		_chan = chan;

	if(_chan != chan)
		return;
	
	_send("PART " + _chan + "\r\n");
	receive();
}

void Irc::command(const std::string &cmd, const std::string &msg, bool skipmsg)
{
	_send(cmd + " " + msg + "\r\n");
	receive();
}

void Irc::privmsg(const std::string &username, const std::string &msg, bool skipmsg)
{
	_send("PRIVMSG " + username + " " + msg + "\r\n");
	receive();
}

std::string Irc::receive()
{
	_read();
	_data.clear();
	_data = std::string(_buffer.begin(), _buffer.end());
	if(_verbose) printf("%s\n", _data.c_str());
	_parse();
	return _data;
}

/////////////

void Irc::_send(const std::string &msg)
{
	_socket.send(boost::asio::buffer(msg));
}

void Irc::_read()
{
	std::vector<char> _buf(1024);
	std::size_t len = _socket.read_some(boost::asio::buffer(_buf));
	_buf[len - 1] = '\0';
	_buffer = _buf.data();
}

void Irc::_parse()
{
	// Tokenize the string
	boost::split(_tokens, _data, boost::is_any_of(" "));
	if(_tokens[0] == "PING")
	{
		std::string pong = "PONG " + _tokens[1];
		_send(pong);
		return;
	}		

	/*
	printf("%s \n", _tokens[0].c_str());
	printf("%s \n", _tokens[1].c_str());
	printf("\n");
	printf("\n");
	V*/
}
