mkdir third-party && cd third-party
touch COLCON_IGNORE
git clone https://github.com/nlohmann/json.git
cd json
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
cd ../../..
