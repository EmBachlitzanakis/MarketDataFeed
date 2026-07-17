#pragma once

#include <atomic>
#include <chrono>
#include <random>
#include <string>

#include <zmq.hpp>

class MarketPublisher {
public:
    struct Config {
        std::string bind_address = "tcp://*:5555";
        std::string symbol = "AAPL";
        double initial_price = 150.0;
        double drift = 0.05;
        double volatility = 0.2;
        double dt = 0.01;
        double spread = 0.05;
        std::chrono::milliseconds update_interval{100};
    };

    explicit MarketPublisher(Config config = {});
    ~MarketPublisher() = default;

    bool isRunning() const noexcept;
    void start();
    void stop();
    void publishTick();
    void run();

private:
    Config config_;
    zmq::context_t context_;
    zmq::socket_t publisher_;
    std::atomic<bool> running_{false};
    double current_price_;
    std::mt19937 generator_;
    std::normal_distribution<double> normal_distribution_;
};
