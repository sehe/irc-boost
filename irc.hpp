#ifndef H_IRC
#define H_IRC

#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/tokenizer.hpp>

class Irc
{
	public:
		Irc(const std::string &server, unsigned short port, bool verbose, bool autoconnect = true);
		~Irc();

		bool connect();
		bool disconnect();

		void nick(const std::string &nickname);
		void user(const std::string &username);
		void user(const std::string &username, const std::string &hostname, const std::string &server, const std::string &realname);
		void join(const std::string &chan);
		void part(const std::string &chan);
		void privmsg(const std::string &to, const std::string &msg);
		void command(const std::string &cmd, const std::string &msg);
		void command(const std::string &cmd, const std::string &to, const std::string &msg);

		void run();

	private:
		void _pong(const std::string &ping);
		void _send(const std::string &msg);
		void _read(const boost::system::error_code &error, std::size_t length);
		void _readHandler(const std::string &command);
		void _sendHandler(const boost::system::error_code &error);

		std::string _server;
		unsigned short _port;
		bool _verbose;
		std::string	_chan;
		std::string	_nick;
		std::array<char, 1024> _buffer;
		boost::asio::io_service _ios;
		boost::asio::ip::tcp::socket _socket;
		std::vector<std::function<void (const std::string&)>> _readHandlers;
};

#endif
