# osrf/ros:humble-desktop
# ubuntu 22
# nlohmann/json
# zmq?
# GTest, GMock

# Setup ros2
FROM ubuntu:jammy
WORKDIR /home/workspace/uavsdk/


# # Install ROS2 tools
# RUN sudo apt update && sudo apt install -y lsb-release wget && \
#     echo "deb [arch=amd64] http://packages.ros.org/ros2/ubuntu jammy main" | sudo tee /etc/apt/sources.list.d/ros2-latest.list && \
#     wget http://packages.ros.org/ros.key -O - | sudo apt-key add - && \
#     sudo apt update && sudo apt install -y ros-humble-ament-cmake && \
#     sudo apt install -y ros-dev-tools

# Install GTest
RUN apt-get update && \
    apt-get -y install googletest cmake


# (Docker specific) Get uavsdk
# ADD https://github.com/rirpc-uav-lab/uavsdk.git /home/workspace/uavsdk/
# ADD ./* .
SHELL ["/bin/bash", "-c"]


# Install dependencies
RUN source ./scripts/install_dependencies.bash && \
    apt -y install python3-pip


# Build
RUN cmake -S . -B build && \
    cmake --build build -DBUILD_TESTING=true

# Test
RUN ctest -C Debug
