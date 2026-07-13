Simulates the exchange's public data feed.

Implementation: Loops continuously, calculating Geometric Brownian Motion prices.

ZMQ Pattern: Binds a ZMQ_PUB socket to port 5555.

Payload: Publishes a packet containing [Topic: "TICK", Timestamp, Symbol, MidPrice, Spread].

# MarketDataFeed
