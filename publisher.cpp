#include "market_publisher.hpp"

#include <chrono>
#include <iostream>
#include <thread>

int main() {
    MarketPublisher::Config config;
    config.bind_address = "tcp://*:5555";

    MarketPublisher publisher(config);
    std::cout << "Market data publisher started on port 5555..." << std::endl;

    publisher.start();

    std::this_thread::sleep_for(std::chrono::seconds(10));
    publisher.stop();

    return 0;
}