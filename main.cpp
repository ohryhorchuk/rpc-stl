#include <iostream>
#include <unordered_map>
#include <string>

#define ASIO_STANDALONE
#include <asio.hpp>

#include "src/storage-service.hpp"

struct message
{
    message()
    {
    }

    char *get_header()
    {
        return header;
    }
    char header[8];
    std::vector<char> body;
};

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

        // asio::buffer buffer(m_message.header);

        // Issue an operation to read header
        asio::async_read(m_socket, asio::buffer(m_message.header), [this, self](std::error_code ec, std::size_t bytesTransfered) 
                                    {
                                        std::cout << "async_read" << std::endl;    
                                        if (!ec)
                                        {
                                            std::istringstream is(std::string(m_message.get_header(), 4));
                                            std::size_t inbound_data_size = 0;
                                            if (!(is >> std::hex >> inbound_data_size))
                                            {
                                                // fail
                                            }
                                            m_message.body.resize(inbound_data_size);
                                            // Everything is ok, proceed with body
                                            read_body();

                                        }
                                    });
    }

    void read_body()
    {
        std::cout << "read_body" << std::endl;
        auto self(shared_from_this());

        asio::async_read(m_socket, 
                            asio::buffer(m_message.body), 
                            [this, self](std::error_code ec, std::size_t bytesTransfered) 
                            {
                                // Done reading actuall message

                            });
    }

    asio::ip::tcp::socket m_socket;
    message m_message;
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
