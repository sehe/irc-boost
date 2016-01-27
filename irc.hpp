#ifndef H_IRC
#define H_IRC

#include <vector>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/shared_ptr.hpp>

class Irc 
{
    public:
        Irc(const std::string &server, const std::string &port, const std::function<void()> onConnect);

        void connect();
        void close();

        void user(const std::string &username);
        void user(const std::string &username, const std::string &hostname, const std::string &server, const std::string &realname);
        void nick(std::string &nickname);
        void join(const std::string &chan);
        void part(const std::string &chan);
        void privmsg(const std::string &to, const std::string &msg);
        void command(const std::string &cmd, const std::string &msg);
        void command(const std::string &cmd, const std::string &to, const std::string &msg);

        void run();

    private:
        void _read(const boost::system::error_code &error);
        void _send(std::string &message);
        void _readHandler(const boost::tokenizer<boost::char_separator<char> > &tokenizer);
        void _connectHandler(const boost::system::error_code &error);

        void _pong(const std::string &ping);

        std::string _server;
        std::string _port;
        std::string _chan;
        std::vector<std::function<void (const boost::tokenizer<boost::char_separator<char> >&)>> _readHandlers;
        std::function<void()> _onConnected;
        boost::asio::streambuf _buffer;
        boost::asio::io_service _ios;
        boost::asio::ip::tcp::socket _socket;
};

#endif
