#include <iostream>
#include <unordered_map>
#include <string>

#define ASIO_STANDALONE
#include <asio.hpp>

#include "src/storage-service.hpp"

class session : public std::enable_shared_from_this<session>
{
public:
    session(asio::ip::tcp::socket socket) 
    : m_socket(std::move(socket))
    {
    }

    void start()
    {
        std::cout << "start" << std::endl;
        auto self(shared_from_this());

        std::string s = {"asd"};
        asio::mutable_buffer buffer(s.data(), s.size());
        // asio::buffer buf(s.c_str());
        asio::async_read(m_socket, [this, self](std::error_code ec, std::size_t bytesTransfered) 
                                    {
                                        std::cout << "async_read" << std::endl;    
                                    });
        // m_socket.read_async(buffer, );
    }

    asio::ip::tcp::socket m_socket;
};

class server
{
public:
    server(asio::io_context& io_context)
    : acceptor(io_context, asio::ip::tcp::endpoint (asio::ip::tcp::v4(), 55555)),
    socket(io_context)
    {
        do_accept();
    }

    void do_accept()
    {
        acceptor.async_accept(socket, 
                            [this](std::error_code ec) 
                            {
                                std::cout << "async_accept" << std::endl;
                                if (!ec)
                                {
                                    std::make_shared<session>(std::move(socket))->start();
                                }
                                do_accept();
                            });
    }

    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;
};

int main(int argc, char const *argv[])
{
    asio::io_context io;
    server srv(io);

    io.run();

    return 0;
}
