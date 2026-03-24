FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy full source (includes Poker_sim/HandRanks.dat).
COPY . .

# Build the C++ backend.
RUN cmake -S Poker_sim -B build && cmake --build build -j

# Render provides PORT dynamically; server_main.cpp already reads PORT.
EXPOSE 8080

CMD ["./build/poker_server"]
