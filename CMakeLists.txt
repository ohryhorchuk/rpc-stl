cmake_minimum_required (VERSION 2.8.12)
project (rpc-stl-app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(ZMQ_HEADERS_DIR ${CMAKE_BINARY_DIR}/../external/zmq/ZMQ-install/include)
set(ZMQ_LIB_DIR ${CMAKE_BINARY_DIR}/../external/zmq/ZMQ-install/lib)

set(ASIO_HEADERS_DIR /Users/g_alik/Development/asio/asio/include)
set(CPPZMQ_HEADERS_DIR ${CMAKE_BINARY_DIR}/../external/cppzmq)

set(ZMQ_LIBRARIES ${ZMQ_LIB_DIR}/libzmq.a)

message(STATUS "!!!!!" ${ZMQ_HEADERS_DIR} 
                       ${ZMQ_LIB_DIR} 
                       ${CPPZMQ_HEADERS_DIR})

add_executable(rpc-stl-app main.cpp)

target_include_directories(rpc-stl-app PUBLIC ${ZMQ_HEADERS_DIR} 
                                              ${CPPZMQ_HEADERS_DIR}
                                              ${ASIO_HEADERS_DIR})

add_subdirectory(src)

link_directories(${ZMQ_LIB_DIR})
target_link_libraries(rpc-stl-app ${ZMQ_LIBRARIES} rpcstl)