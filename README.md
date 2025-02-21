# Service Manager C++

## Overview
This project is a **C++ Service Manager** that can start, monitor, and stop multiple services based on a configuration file. The project uses **CMake** for build automation and is designed to run on **Windows**.

## Features
- Start multiple processes defined in a `config.json` file.
- Monitor and display running processes.
- Stop individual processes by name.
- Gracefully stop all services on exit.
- Uses **GitHub Actions** to automate builds on Windows.

## Build Instructions
### **Using CMake (Windows)**
```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

OR

```sh
sh build_and_run.sh
```

The resulting executable will be available in `build/Release/ServiceManagerCPP.exe`.

## Running the Application
To start the **Service Manager**, provide the path to a valid `config.json`:
```sh
ServiceManagerCPP.exe config.json
```

## GitHub Actions CI/CD
This project includes a **GitHub Actions Workflow** that:
- Automatically builds the executable on Windows.
- Uploads the resulting `.exe` as an artifact.

## File Structure
```
📦 service-manager-cpp
 ┣ 📂 src
 ┃ ┣ 📂 classes
 ┃ ┃ ┣ 📜 SvcMgr.hpp  # Service Manager Header
 ┃ ┃ ┣ 📜 SvcMgr.cpp  # Service Manager Implementation
 ┃ ┃ ┣ 📜 ConfigMgr.hpp  # Configuration Manager Header
 ┃ ┃ ┗ 📜 ConfigMgr.cpp  # Configuration Manager Implementation
 ┣ 📜 CMakeLists.txt  # Build Configuration
 ┣ 📜 config.json  # Service Configuration
 ┣ 📜 .gitignore  # Ignore build & IDE files
 ┣ 📜 README.md  # Project Overview
 ┗ 📜 .github/workflows/build.yml  # GitHub Actions Workflow
```

