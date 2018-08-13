#include <zmq.hpp>

namespace rpc_stl
{
    class storage_service
    {
    public:
        storage_service(zmq::context_t& context);

        void bind(const std::string& addr);

        void serve();

    protected:
        zmq::context_t& zmq_context;
        zmq::socket_t serving_socket;



    private:
        storage_service(const storage_service& other) = delete;
    };
}