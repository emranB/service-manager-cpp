// main.cpp
#include "SvcMgr.hpp"
#include "ConfigMgr.hpp"
#include <iostream>
#include <csignal>
#include <memory>


std::shared_ptr<ConfigMgr> configManager;
std::shared_ptr<SvcMgr> serviceManager;

/**
 * Signal handler for graceful shutdown.
 * Ensures that all running processes are terminated cleanly before exiting.
 */
void signalHandler(int signal) {
    std::cout << "\n[Signal Received] Terminating Service Manager..." << std::endl;
    if (serviceManager) {
        serviceManager->stop();
    }
    exit(signal);
}

/**
 * Entry point of the application.
 * Parses command-line arguments, initializes configuration, and starts the service manager.
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config.json>" << std::endl;
        return 1;
    }

    std::string configPath = argv[1];
    configManager = std::make_shared<ConfigMgr>(configPath);
    serviceManager = std::make_shared<SvcMgr>(configManager);

    // Register signal handlers for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Start the service manager
    serviceManager->start();
    serviceManager->run();

    return 0;
}