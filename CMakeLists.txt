cmake_minimum_required(VERSION 3.16)
project(MarketDataFeed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(market_publisher
    publisher.cpp
    market_publisher.cpp
)

target_include_directories(market_publisher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(ZeroMQ REQUIRED)
find_package(cppzmq CONFIG REQUIRED)

if(TARGET cppzmq::cppzmq)
    target_link_libraries(market_publisher PRIVATE cppzmq::cppzmq)
elseif(TARGET cppzmq)
    target_link_libraries(market_publisher PRIVATE cppzmq)
endif()

if(TARGET ZeroMQ::libzmq)
    target_link_libraries(market_publisher PRIVATE ZeroMQ::libzmq)
elseif(TARGET ZeroMQ::ZeroMQ)
    target_link_libraries(market_publisher PRIVATE ZeroMQ::ZeroMQ)
else()
    target_link_libraries(market_publisher PRIVATE zmq)
endif()

if(MSVC)
    target_compile_options(market_publisher PRIVATE /W4)
endif()

message(STATUS "Configured MarketDataFeed for ${CMAKE_SYSTEM_NAME}")
