# build_and_run.sh
#!/bin/bash
set -e

BUILD_DIR=build

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Run CMake and build
cmake ..
cmake --build . --config Release

# Run the executable with config.json
cd Release
./ServiceManagerCPP ../../config.json