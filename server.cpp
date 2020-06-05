//
// Created by daimiaopeng on 2020/4/10.
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "client.cpp"

using boost::asio::ip::tcp;
using namespace std;
typedef boost::asio::io_context io_context;

class Session;

class Client;

vector<shared_ptr<Session>> cilentList;
string proxyHost;
string proxyPort;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, io_context &ioContext) : _socket(std::move(socket)), _ioContext(ioContext) {}

    void initClent() {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(proxyHost),
                               atoi(proxyPort.c_str()));
        client = new Client(_ioContext, endpoint, _socket);
    }

    void start() {
        initClent();
        do_read();
    }

    ~Session() {
        delete client;
        _socket.close();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        shared_ptr<char[]> buff(new char[1026 * 4], [](char *ptr) { delete[](ptr); });
        _socket.async_read_some(boost::asio::buffer(buff.get(), 1024 * 4),
                                [this, self, buff](boost::system::error_code ec, std::size_t len) {
                                    if (!ec) {
                                        string data(buff.get(), len);
                                        cout << "-----server read len：----" << len << endl;
                                        cout << data << endl;
                                        cout << "--------------------" << endl;
                                        client->do_write(buff ,len);
                                        do_read();
                                    } else {
                                        cout << "Client close" << endl;
                                        client->close();
                                        _socket.close();
                                    }
                                });
    }


    tcp::socket _socket;
    boost::asio::io_context &_ioContext;
    Client *client;
};


class Server {
public:
    Server(boost::asio::io_context &ioContext, short port) : _ioContext(ioContext), _port(port),
                                                             _acceptor(ioContext, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }


private:
    void do_accept() {
        _acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                cout << "Client connection" << endl;
                auto session(std::make_shared<Session>(std::move(socket), _ioContext));
                cilentList.push_back(session);
                session->start();
            }
            do_accept();
        });
    }

    boost::asio::io_context &_ioContext;
    tcp::acceptor _acceptor;
    short _port;
};

int main(int argc, char *argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: <serverPort> <proxyHost> <proxyPort>\n";
            return 1;
        }
        string serverPort(argv[1]);
        proxyHost = string(argv[2]);
        proxyPort = string(argv[3]);
        boost::asio::io_context ioContext;
        Server server(ioContext, atoi(serverPort.c_str()));
        cout << "The reverse_proxy is running" << endl;
        ioContext.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

