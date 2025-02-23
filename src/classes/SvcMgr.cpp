// SvcMgr.cpp
#include "SvcMgr.hpp"
#include <fmt/core.h>
#include <processthreadsapi.h>  // Required for CreateProcessA and PROCESS_INFORMATION
#include <iostream>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <tlhelp32.h>

/**
 * Retrieves the current timestamp as a formatted string.
 */
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    
    char buffer[20]; // Buffer for formatted timestamp
    struct tm timeinfo;
    localtime_s(&timeinfo, &now_c);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
    return std::string(buffer);
}

/**
 * Compiles an output message with a timestamp.
 */
std::string SvcMgr::compileOutMessage(const std::string& msg) {
    return fmt::format("[{}] {}", getTimestamp(), msg);
}

SvcMgr::SvcMgr(std::shared_ptr<ConfigMgr> config) : config(config), running(true) {
    std::cout << compileOutMessage("SvcMgr initialized.") << std::endl;
}

/**
 * Starts the configured services.
 */
void SvcMgr::start() {
    for (const auto& service : config->getServices()) {
        if (!service.skip) {
            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi = {};

            if (CreateProcessA(service.path.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                processes.emplace(service.name, pi);
                std::cout << compileOutMessage(fmt::format("Started {} (PID: {}).", service.name, pi.dwProcessId)) << std::endl;
            } else {
                std::cerr << compileOutMessage(fmt::format("Failed to start {}.", service.name)) << std::endl;
            }
        }
    }
}

/**
 * Runs the Service Manager command loop.
 */
void SvcMgr::run() {
    reportCommandsUsage();
    while (running) {
        std::string command;
        std::cout << compileOutMessage("> ");
        std::getline(std::cin, command);

        if (command == "show_processes") {
            showProcesses();
        } else if (command.rfind("kill_process ", 0) == 0) {
            killProcess(command.substr(13));
        } else if (command == "exit") {
            std::cout << compileOutMessage("Exiting service manager...") << std::endl;
            stop();
            running = false;
            break;
        } else {
            std::cout << compileOutMessage("Unknown command. Try again.") << std::endl;
            reportCommandsUsage();
        }
    }
}

/**
 * Displays the currently running processes.
 */
void SvcMgr::showProcesses() {
    std::cout << compileOutMessage("Running processes:") << std::endl;
    for (const auto& process : processes) {
        std::cout << fmt::format("  - {}", process.first) << std::endl;
    }
    if (processes.empty()) {
        std::cout << "  No running processes." << std::endl;
    }
}

/**
 * Kills a specific process by name.
 */
void SvcMgr::killProcess(const std::string& processName) {
    auto it = processes.find(processName);
    if (it != processes.end()) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, it->second.dwProcessId);
        if (hProcess == NULL) {
            std::cerr << compileOutMessage(fmt::format("Failed to get process handle for {}. Error: {}", processName, GetLastError())) << std::endl;
            return;
        }

        if (TerminateProcess(hProcess, 0)) {
            std::cout << compileOutMessage(fmt::format("Process {} (PID: {}) terminated successfully.", processName, it->second.dwProcessId)) << std::endl;
            CloseHandle(hProcess);
            CloseHandle(it->second.hProcess);
            CloseHandle(it->second.hThread);
            processes.erase(it);
        } else {
            std::cerr << compileOutMessage(fmt::format("Failed to terminate {}. Error: {}", processName, GetLastError())) << std::endl;
        }
    } else {
        std::cout << compileOutMessage(fmt::format("Process {} not found.", processName)) << std::endl;
    }
}

/**
 * Stops all running services and cleans up process resources.
 */
void SvcMgr::stop() {
    std::cout << compileOutMessage("Stopping all services...") << std::endl;
    for (auto& process : processes) {
        std::cout << compileOutMessage(fmt::format("Stopping {} (PID: {}).", process.first, process.second.dwProcessId)) << std::endl;

        if (TerminateProcess(process.second.hProcess, 0)) {
            std::cout << compileOutMessage(fmt::format("Process {} terminated successfully.", process.first)) << std::endl;
        } else {
            std::cerr << compileOutMessage(fmt::format("Failed to terminate {}.", process.first)) << std::endl;
        }

        CloseHandle(process.second.hProcess);
        CloseHandle(process.second.hThread);
    }
    processes.clear();
}

/**
 * Reports the supported commands.
 */
void SvcMgr::reportCommandsUsage() {
    std::cout << compileOutMessage("Service Manager running.") << std::endl;
    std::cout << "Commands:\n"
              "  show_processes - List running processes\n"
              "  kill_process <name> - Stop a running process\n"
              "  exit - Stop all services and exit\n" << std::endl;
}
