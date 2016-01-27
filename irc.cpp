#include "irc.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

Irc::Irc(const std::string &server, const std::string &port, const std::function<void()> onConnect)
    : _server(server), _port(port), _onConnected(onConnect), 
      _socket(_ios)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    // Ping back handler
    _readHandlers.push_back([this](const boost::tokenizer<boost::char_separator<char> > &tokenizer) {
        std::vector<std::string> tokens(begin(tokenizer), end(tokenizer)); 

        if(tokens[0].compare("PING") == 0)
            _pong(tokens[1]);   
    });

}

void Irc::connect()
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    using boost::asio::ip::tcp;
    tcp::resolver resolver(_ios);
    tcp::resolver::query query(_server, _port);
    tcp::resolver::iterator it = resolver.resolve(query), end;

    std::cout << "Connecting to " << _server << " " << _port << std::endl;

    boost::asio::async_connect(_socket, it, boost::bind(&Irc::_connectHandler, this, boost::asio::placeholders::error()));
}

void Irc::close()
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    _socket.close();
    _ios.stop();
}

void Irc::user(const std::string &username)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	user(username, username, username, username);
}

void Irc::user(const std::string &username, const std::string &hostname, const std::string &server, const std::string &realname)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	command("USER ", username + " " + hostname + " " + server + " :" + realname);	
}

void Irc::nick(std::string &nickname)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	command("NICK", nickname);
}

void Irc::join(std::string chan)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	part(_chan);
    if (chan.front() != '#')
        chan.insert(chan.begin(), '#');
	command("JOIN", chan);

    _chan = chan;
}

void Irc::part(std::string chan)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    if (chan.front() != '#')
        chan.insert(chan.begin(), '#');
	command("PART", chan);
}

void Irc::privmsg(const std::string &to, const std::string &msg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	command("PRIVMSG", to, msg);
}

void Irc::command(const std::string &cmd, const std::string &msg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	std::string message(cmd + " " + msg + "\r\n");
	_send(message);
}

void Irc::command(const std::string &cmd, const std::string &to, const std::string &msg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	std::string message(cmd + " " + to + " " + msg + "\r\n");
	_send(message);
}

void Irc::run()
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
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
    std::cout << __PRETTY_FUNCTION__ << "\n";
    if(error)
    {
        std::cerr << "Error in read : " << error.message() << std::endl;
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
    std::cout << __PRETTY_FUNCTION__ << "\n";
    boost::asio::write(_socket, boost::asio::buffer(message + "\r\n"));
}

void Irc::_readHandler(const boost::tokenizer<boost::char_separator<char> > &tokenizer)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    for(auto it : _readHandlers)
        it(tokenizer);
}

void Irc::_connectHandler(const boost::system::error_code &error)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    if(!error)
    {
        _onConnected();
    } else
    {
        std::cout << "Error connecting to " << _server << " " << error.message() << std::endl;
    }
}

void Irc::_pong(const std::string &ping)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
	std::string resp("PONG :" + ping);
	_send(resp);
}

