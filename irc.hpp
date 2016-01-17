#ifndef H_IRC
#define H_IRC

#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>

class Irc
{
	public:
		Irc(const std::string &server, unsigned short port, bool verbose, bool autoconnect = true);
		~Irc();

		bool connect();
		bool disconnect();

		void nick(const std::string &nickname, bool skipmsg = true);
		void user(const std::string &username, bool skipmsg = true);
		void user(const std::string &username, const std::string &hostname, const std::string &server, const std::string &realname, bool skipmsg = true);
		void join(const std::string &chan, bool skipmsg = true);
		void part(const std::string &chan, bool skipmsg = true);
		void command(const std::string &cmd, const std::string &msg, bool skipmsg = true);
		void privmsg(const std::string &username, const std::string &msg, bool skipmsg = true);

		std::string receive();

	private:
		void _send(const std::string &msg);
		void _read();
		void _parse();
	
		std::string _server;
		unsigned short _port;
		bool _verbose;
		std::string	_chan;
		std::string	_nick;
		boost::asio::io_service _ios;
		boost::asio::ip::tcp::socket _socket;
		std::string	_buffer;
		std::string	_data;
		std::vector<std::string> _tokens;
};

#endif
