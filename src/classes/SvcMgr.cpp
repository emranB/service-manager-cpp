// SvcMgr.cpp
#include "SvcMgr.hpp"
#include <processthreadsapi.h>  // Required for CreateProcessA and PROCESS_INFORMATION
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <tlhelp32.h>


std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now_c);
    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

SvcMgr::SvcMgr(std::shared_ptr<ConfigMgr> config) : config(config), running(true) {
    std::cout << "[" << getTimestamp() << "] SvcMgr initialized." << std::endl;
}

// Function to enable debug privileges
bool EnableDebugPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        std::cerr << "[" << getTimestamp() << "] Failed to open process token. Error: " << GetLastError() << std::endl;
        return false;
    }

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL)) {
        std::cerr << "[" << getTimestamp() << "] Failed to adjust token privileges. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);
    return true;
}

/**
 * Starts the configured services.
 * Launches processes defined in the configuration JSON file.
 */
void SvcMgr::start() {
    for (const auto& service : config->getServices()) {
        if (!service.skip) {
            STARTUPINFOA si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi;
            ZeroMemory(&pi, sizeof(pi));

            if (CreateProcessA(service.path.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                processes.emplace(service.name, pi);
                std::cout << "[" << getTimestamp() << "] Started " << service.name 
                          << " (PID: " << pi.dwProcessId << ")." << std::endl;
            } else {
                std::cerr << "[" << getTimestamp() << "] Failed to start " << service.name << std::endl;
            }
        }
    }
}

// Updated killProcess function
void SvcMgr::killProcess(const std::string& processName) {
    if (!EnableDebugPrivilege()) {
        std::cerr << "[" << getTimestamp() << "] Warning: Could not enable debug privileges. Process termination may fail." << std::endl;
    }

    auto it = processes.find(processName);
    if (it != processes.end()) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, it->second.dwProcessId);
        if (hProcess == NULL) {
            std::cerr << "[" << getTimestamp() << "] Failed to get process handle for " << processName 
                      << " (PID: " << it->second.dwProcessId << "). Error: " << GetLastError() << std::endl;
            return;
        }

        if (TerminateProcess(hProcess, 0)) {
            std::cout << "[" << getTimestamp() << "] Process " << processName << " (PID: " 
                      << it->second.dwProcessId << ") terminated successfully." << std::endl;
            CloseHandle(hProcess);
            CloseHandle(it->second.hProcess);
            CloseHandle(it->second.hThread);
            processes.erase(it);
        } else {
            std::cerr << "[" << getTimestamp() << "] Failed to terminate " << processName 
                      << ". Error: " << GetLastError() << std::endl;
        }
    } else {
        std::cout << "[" << getTimestamp() << "] Process " << processName << " not found." << std::endl;
    }
}

/**
 * Runs the Service Manager command loop.
 * Accepts user input to show, stop, or exit managed processes.
 */
void SvcMgr::run() {
    std::cout << "[" << getTimestamp() << "] Service Manager running.\n"
              << "Commands:\n"
              << "  show_processes - List running processes\n"
              << "  kill_process <name> - Stop a running process\n"
              << "  exit - Stop all services and exit\n" << std::endl;

    while (running) {
        std::string command;
        std::cout << "[" << getTimestamp() << "] > ";
        std::getline(std::cin, command);

        if (command == "show_processes") {
            showProcesses();
        } else if (command.find("kill_process ") == 0) {
            std::string processName = command.substr(13);
            killProcess(processName);
        } else if (command == "exit") {
            std::cout << "[" << getTimestamp() << "] Exiting service manager..." << std::endl;
            stop();
            running = false;
            break;
        } else {
            std::cout << "[" << getTimestamp() << "] Unknown command. Try again." << std::endl;
        }
    }
}

/**
 * Displays the currently running processes.
 */
void SvcMgr::showProcesses() {
    std::cout << "[" << getTimestamp() << "] Running processes:" << std::endl;
    for (const auto& process : processes) {
        std::cout << "  - " << process.first << std::endl;
    }
    if (processes.empty()) {
        std::cout << "  No running processes." << std::endl;
    }
}

/**
 * Stops all running services and cleans up process resources.
 */
void SvcMgr::stop() {
    std::cout << "[" << getTimestamp() << "] Stopping all services..." << std::endl;
    for (auto& process : processes) {
        std::cout << "[" << getTimestamp() << "] Stopping " << process.first << " (PID: " << process.second.dwProcessId << ")" << std::endl;
        
        // Kill the process
        if (TerminateProcess(process.second.hProcess, 0)) {
            std::cout << "[" << getTimestamp() << "] Process " << process.first << " terminated successfully." << std::endl;
        } else {
            std::cerr << "[" << getTimestamp() << "] Failed to terminate " << process.first << "." << std::endl;
        }

        // Close handles
        CloseHandle(process.second.hProcess);
        CloseHandle(process.second.hThread);
    }
    processes.clear();
}