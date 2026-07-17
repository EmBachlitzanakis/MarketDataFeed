#include "market_publisher.hpp"

#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

MarketPublisher::MarketPublisher(Config config)
    : config_(std::move(config)),
      context_(1),
      publisher_(context_, zmq::socket_type::pub),
      current_price_(config_.initial_price),
      generator_(std::random_device{}()),
      normal_distribution_(0.0, 1.0) {
    publisher_.bind(config_.bind_address);
}

bool MarketPublisher::isRunning() const noexcept {
    return running_.load(std::memory_order_relaxed);
}

void MarketPublisher::start() {
    if (running_.exchange(true)) {
        return;
    }
    run();
}

void MarketPublisher::stop() {
    running_.store(false, std::memory_order_relaxed);
}

void MarketPublisher::publishTick() {
    double epsilon = normal_distribution_(generator_);
    double exponent = (config_.drift - 0.5 * config_.volatility * config_.volatility) * config_.dt + config_.volatility * std::sqrt(config_.dt) * epsilon;
    current_price_ = current_price_ * std::exp(exponent);

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    std::ostringstream payload_stream;
    payload_stream << "TICK "
                   << millis << " "
                   << config_.symbol << " "
                   << std::fixed << std::setprecision(4) << current_price_ << " "
                   << std::fixed << std::setprecision(2) << config_.spread;

    std::string payload = payload_stream.str();

    zmq::message_t message(payload.size());
    std::memcpy(message.data(), payload.data(), payload.size());
    publisher_.send(message, zmq::send_flags::none);

    std::cout << "Published: " << payload << std::endl;
}

void MarketPublisher::run() {
    while (running_.load(std::memory_order_relaxed)) {
        publishTick();
        std::this_thread::sleep_for(config_.update_interval);
    }
}
