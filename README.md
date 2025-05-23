# uavsdk

CMake robotics-oriented C++ package with useful templates, behaviour control classes, navigation utils and other useful things.

## Installation

Follow the guide copying and pasting codeblocks in the same order as they appear to install the package.

```bash
cd ~
git clone https://github.com/rirpc-uav-lab/uavsdk.git
```

### Dependencies

0. Ubuntu 22.04 LTS

Currently, while it might be working on other distributions, the package has only been tested on ubuntu 22.04 LTS.


1. nlohmann json

While there is an apt package for nlohmann json, it is outdated and doesn't build correctly with the package, so we are going to build it from source.

```bash
cd uavsdk
source scripts/install_dependencies.bash
```

### Build the package

```bash
cmake -S . -B build 
# Or with tests enabled
cmake -S . -B build -DBUILD_TESTING=true

cmake --build build
sudo cmake --install build
```

### Testing the package

> **Attention**:
> Currently test coverage is very sparse but we are going to improve it over time when the interfaces become more stable.

```bash
cd build 
ctest -C Debug
```

You can also use Docker to test the package in a clean environment:
```bash
cd ..
docker build . -t uavsdk-ci
```

### Examples 

TODO
