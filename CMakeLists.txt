cmake_minimum_required(VERSION 3.20)

# Set CMake policies to ensure compatibility with modern CMake features
cmake_policy(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
cmake_policy(SET CMP0097 NEW)

# Define the project name
project(ServiceManagerCPP)

# Set target platform to Windows and architecture to AMD64
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# Set the required C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fetch nlohmann/json library using CMake FetchContent
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Define the executable target
add_executable(ServiceManagerCPP src/main.cpp src/classes/SvcMgr.cpp src/classes/ConfigMgr.cpp)

# Include directories for header files
include_directories(src/classes)

# Link external libraries
target_link_libraries(ServiceManagerCPP PRIVATE nlohmann_json::nlohmann_json)

# Enable compiler warnings for better debugging
if (MSVC)
    target_compile_options(ServiceManagerCPP PRIVATE /W4)
else()
    target_compile_options(ServiceManagerCPP PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Dummy process for testing
add_executable(DummyProcess src/dummy_process.cpp)
