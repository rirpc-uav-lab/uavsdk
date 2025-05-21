# osrf/ros:humble-desktop
# ubuntu 22
# nlohmann/json
# zmq?
# GTest, GMock

# Setup ros2
FROM osrf/ros:humble-desktop
WORKDIR /home/workspace/

# Get uavsdk
ADD https://github.com/rirpc-uav-lab/uavsdk.git /home/workspace/src/
SHELL ["/bin/bash", "-c"]

# Install dependencies
RUN source src/uavsdk/scripts/install_dependencies.bash

# Setup ROS2 and build workspace
RUN source /opt/ros/humble/setup.sh && \
    colcon build --packages-select uavsdk && \
    bash install/setup.bash

# Test
RUN ["colcon", "test", "--packages-select", "uavsdk"]
RUN ["colcon", "test-result"]

