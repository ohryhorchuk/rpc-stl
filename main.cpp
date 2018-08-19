#include <iostream>
#include <iomanip> 
#include <unordered_map>
#include <string>
#include <thread>

#define ASIO_STANDALONE
#include <asio.hpp>

#include "src/storage-service.hpp"

constexpr char data[101] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"; 
constexpr size_t port = 55555;
struct message
{
    char header[8];
    std::vector<char> body;
};

class server_session : public std::enable_shared_from_this<server_session>
{
public:
    server_session(asio::ip::tcp::socket socket) 
    : m_socket(std::move(socket))
    {
    }

    void start()
    {
        std::cout << "start" << std::endl;
        auto self(shared_from_this());

        // Issue an operation to read header
        asio::async_read(m_socket, asio::buffer(m_message.header), [this, self](std::error_code ec, std::size_t bytesTransfered) 
                                    {
                                        std::cout << "async_read" << std::endl;    
                                        if (!ec)
                                        {
                                            std::istringstream is(std::string(&m_message.header[0], 8));
                                            std::size_t inbound_data_size = 0;
                                            if (!(is >> std::hex >> inbound_data_size))
                                            {
                                                // fail
                                            }
                                            std::cout << "data_size=" << inbound_data_size << std::endl;
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
                                std::cout << "R" << m_message.body.size();
                            });
    }

    asio::ip::tcp::socket m_socket;
    message m_message;
};

class server
{
public:
    server(asio::io_context& io_context)
    : acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
    m_socket(io_context)
    {
        do_accept();
    }

    void do_accept()
    {
        // acceptor.accept(m_socket)
        acceptor.async_accept(m_socket, 
                            [this](std::error_code ec) 
                            {
                                if (!ec)
                                {
                                    std::cout << "async_accept" << std::endl;
                                    std::make_shared<server_session>(std::move(m_socket))->start();
                                }

                                do_accept();
                            });
    }

    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket m_socket;
};

class client
{
public:
    client(asio::io_context& io_context, const std::string& host, const std::string& service)
    : m_socket(io_context)
    {
        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::tcp::resolver::query query(host, service);
        asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        std::cout << "client, after resolve, about to connect";
        do_connect_and_send(endpoint_iterator);
    }

    ~client()
    {
    }

    void do_connect_and_send(const asio::ip::tcp::resolver::iterator& endpoint_iterator)
    {
        asio::connect(m_socket, endpoint_iterator);
        
        done_connect();
    } 

    void done_connect()
    {
        std::cout << "done_connect" << std::endl;

        m_message.body.assign(data, data+100);

        // Format the header.
        std::ostringstream header_size_stream;
        header_size_stream << std::setw(8) << std::hex << m_message.body.size();
        if (!header_size_stream || header_size_stream.str().size() != 8)
        {
            // Error coding body size
        }

        const std::string encoded_header_length = header_size_stream.str();

        strcpy(&m_message.header[0], encoded_header_length.c_str());

        std::vector<asio::const_buffer> buffers_for_send;
        buffers_for_send.push_back(asio::buffer(m_message.header));
        buffers_for_send.push_back(asio::buffer(m_message.body));

        std::cout << "bytes written " << asio::write(m_socket, buffers_for_send);
    }

protected:
    asio::ip::tcp::socket m_socket;
    message m_message;
};

int main(int argc, char const *argv[])
{
    std::string run_option = std::string(argv[1]);

    asio::io_context io;

    // std::thread t(std::bind(&asio::io_context::run, io ));
    
    try
    {
        if (run_option == "server")
        {
            server srv(io);
            io.run();
        } 
        else if (run_option == "client")
        {
            client clnt(io, "localhost", "55555");                
            io.run();
        }
    } 
    catch (const std::exception& e)
    {
        std::cout << e.what();
    }

    return 0;
}
