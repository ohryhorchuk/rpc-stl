cmake_minimum_required (VERSION 2.8.12)

project (rpc-stl-external)

### ZeroMQ Library
# build directory
set(ZMQ_PREFIX ${CMAKE_BINARY_DIR}/zmq/ZMQ-prefix)

# install directory
set(ZMQ_INSTALL ${CMAKE_BINARY_DIR}/zmq/ZMQ-install)

# we build ZMQ statically, but want to link it into the caffe shared library
# this requires position-independent code
if (UNIX)
    set(ZMQ_EXTRA_COMPILER_FLAGS "-fPIC")
endif()


set(ZMQ_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${ZMQ_EXTRA_COMPILER_FLAGS})
set(ZMQ_C_FLAGS ${CMAKE_C_FLAGS} ${ZMQ_EXTRA_COMPILER_FLAGS})

include(ExternalProject)
ExternalProject_Add(ZMQ
PREFIX ${ZMQ_PREFIX}
GIT_REPOSITORY "https://github.com/zeromq/libZMQ.git"
UPDATE_COMMAND ""
INSTALL_DIR ${ZMQ_INSTALL}
CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${ZMQ_INSTALL}
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_STATIC_LIBS=ON
            -DBUILD_PACKAGING=OFF
            -DBUILD_TESTING=OFF
            -DBUILD_NC_TESTS=OFF
            -BUILD_CONFIG_TESTS=OFF
            -DINSTALL_HEADERS=ON
            -DCMAKE_C_FLAGS=${ZMQ_C_FLAGS}
            -DCMAKE_CXX_FLAGS=${ZMQ_CXX_FLAGS}
LOG_DOWNLOAD 1
LOG_INSTALL 1
)

set(ZMQ_FOUND TRUE)
set(ZMQ_INCLUDE_DIRS ${ZMQ_INSTALL}/include)

if(MSVC)
    FILE(GLOB_RECURSE ZMQ_LIBRARIES "${ZMQ_INSTALL}/lib/libzmq-${CMAKE_VS_PLATFORM_TOOLSET}*.lib")
    #set(ZMQ_LIBRARIES ${ZMQ_INSTALL}/lib/ZMQ.lib ${CMAKE_THREAD_LIBS_INIT})
else()
    FILE(GLOB_RECURSE ZMQ_LIBRARIES "${ZMQ_INSTALL}/lib/libzmq-*.a")
    #set(ZMQ_LIBRARIES ${ZMQ_INSTALL}/lib/libZMQ.a ${CMAKE_THREAD_LIBS_INIT})
endif()

set(ZMQ_LIBRARY_DIRS ${ZMQ_INSTALL}/lib)
set(ZMQ_EXTERNAL TRUE)

###ASIO
set(ASIO_URL https://github.com/ohryhorchuk/asio.git)

execute_process(COMMAND           ${GIT_EXECUTABLE} clone ${ASIO_URL}
                WORKING_DIRECTORY ${SOURCE_DIR}
                OUTPUT_VARIABLE   ${git_output})

add_library(external external.cpp)
