// STL
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

// ASIO
#define ASIO_STANDALONE
#include <asio.hpp>

constexpr size_t port = 55555;

// struct Message
// {

// };

class Connection
{
public:
    enum 
    {
        header_length = 8
    };

public:
    Connection(asio::ip::tcp::socket& io)
    : m_socket(io)
    {

    }

    asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    size_t write(const std::string& data)
    {
        m_outbound_body = std::string(data);

        m_outbound_header = encode_size(m_outbound_body.size());

        std::vector<asio::const_buffer> buffers_packaged_for_send;
        buffers_packaged_for_send.push_back(asio::buffer(m_outbound_header));
        buffers_packaged_for_send.push_back(asio::buffer(m_outbound_body));

        return asio::write(m_socket, buffers_packaged_for_send);
    }

    std::string read()
    {
        // Read first series of bytes, should be ecnoded length of overal message
        const size_t read_size = asio::read(m_socket, asio::buffer(m_inbound_header));

        const size_t body_size = decode_size(std::string(m_inbound_header, header_length));

        m_inbound_body.resize(body_size);

        asio::read(m_socket, asio::buffer(m_inbound_body));
        return std::string(&m_inbound_body[0], body_size);
    }

protected:
    static std::string encode_size(const size_t& size)
    {
        std::ostringstream header_enc_stream;

        header_enc_stream << std::setw(header_length) << std::hex << size;

        if (!header_enc_stream)
        {
            throw std::logic_error("Failed to encode header size because of error on stream obj");
        }

        const std::string encoded_header_size = header_enc_stream.str();

        if (encoded_header_size.size() != header_length)
        {
            throw std::invalid_argument("Failed to encode header size as hex because encoded_header_size.size() != HEADER_LENGTH"); 
        }

        return encoded_header_size;
    }

    static size_t decode_size(std::string encoded_size)
    {
        std::istringstream is(encoded_size);
        std::size_t decoded_size = 0;
        if (!(is >> std::hex >> decoded_size))
        {
            throw std::invalid_argument("Error decoding body size recieved from socket");
        }

        return decoded_size;
    }

private:
    // Networking
    asio::ip::tcp::socket& m_socket;

    // Current data to send
    std::string m_outbound_header;
    std::string m_outbound_body;

    // Recieved data from last read
    char m_inbound_header[header_length];
    std::vector<char> m_inbound_body;
};

class Server
{
public:
    Server(asio::io_context& io_context)
    : m_acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
    m_socket(io_context)
    {
    }

    void do_serve()
    {
        try
        {
            while (1)
            {
                wait_and_accept_connection();
                const std::string curr_msg = retrieve_one();
                const std::string curr_resp = handle_one(curr_msg);
                respond_one(curr_resp);
                std::cout << std::endl;
            }
        } 
        catch (const std::exception& e)
        {
            std::cout << e.what();
        }
    }

    void wait_and_accept_connection()
    {
        std::cout << "~";
        m_acceptor.accept(m_socket);
        std::cout << "~";
        m_connection_ptr = std::make_shared<Connection>(m_socket);
    }

    std::string retrieve_one()
    {
        const std::string currentMessage = m_connection_ptr->read();
        return currentMessage;
    }

    std::string handle_one(const std::string& message)
    {
        std::cout << "*";
        std::string copy{message};
        std::next_permutation(copy.begin(), copy.end());

        std::cout << "*";
        return copy;
    }

    void respond_one(const std::string& response)
    {
        std::cout << "±";
        const size_t sent_size = m_connection_ptr->write(response);
        if (sent_size == response.size())
            std::cout << "±";
    }

private:
    asio::ip::tcp::acceptor m_acceptor;
    std::shared_ptr<Connection> m_connection_ptr;
    asio::ip::tcp::socket m_socket;
};

class Client
{
public:
    Client(asio::io_context& io_context)
    : m_socket(io_context),
    m_resolver(io_context)
    {
    }

    void resolve_and_connect(const std::string& host, const std::string& service)
    {
        asio::ip::tcp::resolver::query query(host, service);
        asio::ip::tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);

        std::error_code ec;
        asio::connect(m_socket, endpoint_iterator, ec);
        if (!ec)
        {
            std::cout << "*";
            m_connection_ptr = std::make_shared<Connection>(m_socket);
        }
    }

    void request(const std::string& message)
    {
        const std::size_t sent_size = m_connection_ptr->write(message);
        if (sent_size == message.size())
        {
            
        }
        
    }

    std::string fetch_response()
    {
        return m_connection_ptr->read();
    }

private:
    asio::ip::tcp::socket m_socket;
    asio::ip::tcp::resolver m_resolver;
    std::shared_ptr<Connection> m_connection_ptr;
};



int main(int argc, char const *argv[])
{
    if (argc != 2)
        return EXIT_FAILURE;

    std::string run_option = std::string(argv[1]);

    asio::io_context io;
    if (run_option == "server")
    {
        Server srv(io);
        io.run();
    }
    else if (run_option == "client")
    {

    }


    return 0;
}