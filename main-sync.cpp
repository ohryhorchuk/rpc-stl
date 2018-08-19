// STL
#include <iomanip>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

// ASIO
#define ASIO_STANDALONE
#include <asio.hpp>


constexpr size_t port = 55555;

class connection
{
public:
    enum 
    {
        HEADER_LENGTH = 8
    };

public:
    connection(asio::ip::tcp::socket& io)
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

        const size_t body_size = decode_size(std::string(m_inbound_header, HEADER_LENGTH));

        m_inbound_body.resize(body_size);

        asio::read(m_socket, asio::buffer(m_inbound_body));
        return std::string(&m_inbound_body[0], body_size);
    }

protected:
    static std::string encode_size(const size_t& size)
    {
        std::ostringstream header_enc_stream;

        header_enc_stream << std::setw(HEADER_LENGTH) << std::hex << size;

        if (!header_enc_stream)
        {
            throw std::logic_error("Failed to encode header size because of error on stream obj");
        }

        const std::string encoded_header_size = header_enc_stream.str();

        if (encoded_header_size.size() != HEADER_LENGTH)
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
    char m_inbound_header[HEADER_LENGTH];
    std::vector<char> m_inbound_body;
};

class server
{
public:
    server(asio::io_context& io_context)
    : m_acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
    m_socket(io_context)
    {
        try
        {
            accept();
        } 
        catch (const std::exception& e)
        {
            std::cout << e.what();
        }
    }

    void accept()
    {
        m_acceptor.accept(m_socket);
        // asio::ip::tcp::socket socket = ;
        m_connection_ptr = std::make_shared<connection>(m_socket);
    }

    void read()
    {

    }

private:
    asio::ip::tcp::acceptor m_acceptor;
    std::shared_ptr<connection> m_connection_ptr;
    asio::ip::tcp::socket m_socket;
};



int main(int argc, char const *argv[])
{
    std::string run_option = std::string(argv[1]);

    asio::io_context io;
    if (run_option == "server")
    {
        server srv(io);
        io.run();

    }
    else if (run_option == "client")
    {

    }


    return 0;
}