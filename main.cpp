#include <iostream>
#include <unordered_map>
#include <string>
#include <zmq.hpp>
#include "src/storage-service.hpp"
#include <unordered_map>

int main(int argc, char const *argv[])
{
    std::string s{"asdasdasd"};
    std::wstring ws{L"jksdhgfksjdhfgskdf"};

    const char* c = s.c_str();
    const wchar_t* wc = ws.c_str();
    int size = s.size();
    int wsize = ws.size();


    zmq::context_t context(4);

    rpc_stl::storage_service storage(context);
    try
    {
        storage.bind("tcp://192.168.1.15:55556");
    } catch (zmq::error_t exception)
    {
        std::cerr << exception.what();
    }
    
    auto v = zmq::version();

    return 0;
}
