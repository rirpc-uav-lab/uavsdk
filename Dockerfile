# syntax=docker/dockerfile:1-labs

# Setup ros2
FROM ubuntu:jammy
WORKDIR /home/workspace/uavsdk/

# Install GTest
RUN apt-get update && \
    apt-get -y install googletest cmake libgtest-dev git clang-12 build-essential


# (Docker specific) Get uavsdk
COPY . /home/workspace/uavsdk/
SHELL ["/bin/bash", "-c"]


# Install dependencies
# RUN bash ./scripts/install_dependencies.bash
RUN mkdir third-party && cd third-party && \
    touch COLCON_IGNORE && \
    git clone https://github.com/nlohmann/json.git --depth 1 && \
    cd json && \
    mkdir build && cd build && \
    cmake .. && \
    cmake --build . && \
    cmake --install . && \
    cd ../../..

# Build
RUN cmake -S . -B build -DBUILD_TESTING=true && \
    cmake --build build

# Test
RUN ctest -C Debug
