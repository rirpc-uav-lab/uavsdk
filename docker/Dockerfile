# osrf/ros:humble-desktop
# ubuntu 22
# nlohmann/json
# zmq?
# GTest, GMock

# Setup ros2
FROM osrf/ros:humble-desktop
WORKDIR /home/workspace/

RUN sudo apt update && sudo apt install -y lsb-release wget && \
    echo "deb [arch=amd64] http://packages.ros.org/ros2/ubuntu jammy main" | sudo tee /etc/apt/sources.list.d/ros2-latest.list && \
    wget http://packages.ros.org/ros.key -O - | sudo apt-key add - && \
    sudo apt update && \
    # sudo apt install ros-humble-desktop
    sudo apt update && sudo apt install -y ros-humble-ament-cmake && \
    sudo apt install -y ros-dev-tools

# Get uavsdk
ADD https://github.com/rirpc-uav-lab/uavsdk.git /home/workspace/src/uavsdk/
SHELL ["/bin/bash", "-c"]

# Install dependencies
RUN source ./src/uavsdk/scripts/install_dependecies.bash
# RUN sudo apt install python3-pip && \
#     pip3 install colcon-devtools ament-cmake-test
# RUN apt-get update && apt-get install -y python3-pip
# RUN python3 -m pip install ament-package

# Setup ROS2 and build workspace
RUN source /opt/ros/humble/setup.sh && \
    colcon build --packages-select uavsdk --symlink-install && \
    source install/setup.bash

# Test
RUN source /opt/ros/humble/setup.sh && \
    colcon test --packages-select uavsdk && \
    colcon test-result --verbose

