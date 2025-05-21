# uavsdk

ROS2 package with useful templates, behaviour control classes, navigation utils and other useful things.

## Installation

Follow the guide copying and pasting codeblocks in the same order as they appear to install the package.

```bash
cd ~
mkdir -p ros2_ws/src && cd ros2_ws/src
git clone https://github.com/rirpc-uav-lab/uavsdk.git
```

### Dependencies

0. Ubuntu 22.04 LTS

Currently, while it might be working on other distributions, the package has only been tested on ubuntu 22.04 LTS.

1. ROS2

While ROS2 is not a direct dependency, uavsdk is created as an ament_cmake package for easier integration with ROS2.

Install ROS2 using an [official guide](https://docs.ros.org/en/humble/Installation.html).

2. nlohmann json

While there is an apt package for nlohmann json, it is outdated and doesn't build correctly with the package, so we are going to build it from source.

```bash
cd uavsdk
source scripts/install_dependencies.bash
```

### Build the package

```bash
cd ../.. 
colcon build --packages-select uavsdk
```

### Testing the package

> **Attention**:
> Currently test coverage is very sparse but we are going to improve it over time when the interfaces become more stable.

```bash
colcon test --packages-select uavsdk
colcon test-result
```

You can also use Docker to test the package in a clean environment:
```bash
docker build src/uavsdk/docker -t uavsdk-ci
```

### Examples 

TODO
