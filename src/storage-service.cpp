#include "storage-service.hpp"
#include <zmq.hpp>
#include <iostream>

namespace rpc_stl
{
    storage_service::storage_service(zmq::context_t& context)
    : zmq_context(context),
    serving_socket(context, zmq::socket_type::rep)
    {
    }

    void storage_service::bind(const std::string& addr)
    {
        serving_socket.bind(addr);
    }

    void storage_service::serve()
    {
        while(1)
        {
            zmq::message_t message;
            if(serving_socket.recv(&message))
            {
                std::cout << message.str();
            }
        }
    }
}