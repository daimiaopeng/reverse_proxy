//
// Created by daimiaopeng on 2020/5/26.
//



#ifndef PORT_FORWARD_CLIENT_CPP
#define PORT_FORWARD_CLIENT_CPP


#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

class Client {
public:
    Client(boost::asio::io_context &ioContext, tcp::endpoint endpoint, tcp::socket& serverSocket) : _ioContext(ioContext), _endpoint(endpoint),
                                                                          _sock(ioContext),_serverSocket(serverSocket){
        do_connect();
    }

    void do_write(char *sendData, int len) {
        _sock.async_write_some(boost::asio::buffer(sendData,len),
                               [this, sendData](boost::system::error_code ec, std::size_t) {
                                   if (!ec) {

                                   } else {
                                       std::cout << "ec " << ec << "\n";
                                   }
                               });
    }
    void close(){

        _sock.close();
    }
    ~Client(){
        close();
    }
private:

    void do_read() {
        shared_ptr<char[]> buff(new char[1026*4], [](char *ptr) { delete[](ptr); });
        _sock.async_read_some(boost::asio::buffer(buff.get(), 1024*4),
                              [this, buff](boost::system::error_code ec, std::size_t len) {
                                  if (!ec) {
                                      cout<<"-----client read len：----"<<len<<endl;
                                      cout<<string(buff.get(),len)<<endl;
                                      cout<<"--------------------"<<endl;
                                      boost::asio::async_write(_serverSocket, boost::asio::buffer(buff.get(), len),
                                                               [this,buff](boost::system::error_code ec, std::size_t len) {
                                                                   if (!ec) {

                                                                   }
                                                               });
                                      do_read();
                                  } else {
                                      _sock.close();
                                  }
                              });
    }

    void do_connect() {
        _sock.async_connect(_endpoint, [this](error_code ec) {
            if (!ec) {
                do_read();
            }
        });
    }


    tcp::endpoint _endpoint;
    boost::asio::io_context &_ioContext;
    tcp::socket _sock;
    tcp::socket& _serverSocket;
};

#endif //PORT_FORWARD_CLIENT_CPP
