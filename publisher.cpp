#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <thread>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <zmq.hpp>

int main() {
    // 1. Initialize ZeroMQ context and PUB socket
    zmq::context_t context(1);
    zmq::socket_t publisher(context, zmq::socket_type::pub);
    publisher.bind("tcp://*:5555");
    std::cout << "Market data publisher started on port 5555..." << std::endl;

    // 2. GBM Simulation Parameters
    double S_t = 150.0;       // Initial stock price (e.g., $150.00)
    double mu = 0.05;         // Drift (5% annual return)
    double sigma = 0.2;       // Volatility (20% annual volatility)
    double dt = 0.01;         // Time step (roughly representing 100ms steps in trading years)
    double spread = 0.05;     // Static bid-ask spread
    std::string symbol = "AAPL";

    // Random number generator for the Wiener process (normal distribution)
    std::random_device rd;
    std::mt_19937 generator(rd());
    std::normal_distribution<double> normal_dist(0.0, 1.0);

    while (true) {
        // 3. Calculate next price using Geometric Brownian Motion
        double epsilon = normal_dist(generator);
        double exponent = (mu - 0.5 * sigma * sigma) * dt + sigma * std::sqrt(dt) * epsilon;
        S_t = S_t * std::exp(exponent);

        // Get current timestamp in milliseconds
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

        // 4. Format payload: TICK <Timestamp> <Symbol> <MidPrice> <Spread>
        std::stringstream ss;
        ss << "TICK " 
           << millis << " " 
           << symbol << " " 
           << std::fixed << std::setprecision(4) << S_t << " " 
           << std::fixed << std::setprecision(2) << spread;
        
        std::string payload = ss.str();

        // 5. Publish the message
        zmq::message_t message(payload.size());
        std::memcpy(message.data(), payload.data(), payload.size());
        publisher.send(message, zmq::send_flags::none);

        std::cout << "Published: " << payload << std::endl;

        // Sleep for 100ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}