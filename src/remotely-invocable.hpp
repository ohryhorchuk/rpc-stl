#include <zmq.hpp>
#include <string>

namespace rpc_stl
{
    class remotely_invocable
    {
    public:
        inline remotely_invocable(zmq::context_t& context) : m_context(context), m_socket(context, zmq::socket_type::rep) {}
        inline virtual ~remotely_invocable() { m_socket.close();}

        inline void bind(const std::string& addr) { m_socket.bind(addr); }
        inline std::string recieve()
        {
            zmq::message_t message;
            m_socket.recv(&message);
            message.str
            return "";
        }

        inline std::wstring wrecieve()
        {
            return L"";
        }

        inline bool send(const std::string& message)
        {
            
        }

        inline bool wsend(const std::string& message)
        {

        }


    protected:
        zmq::context_t& m_context;
        zmq::socket_t m_socket;
    };
    
}