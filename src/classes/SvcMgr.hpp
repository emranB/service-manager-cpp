// SvcMgr.hpp
#ifndef SVCMGR_HPP
#define SVCMGR_HPP

#ifndef _WIN32
#define _AMD64_
#define _WIN32
#endif

#include <windows.h>
#include "ConfigMgr.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <map>
#include <memory>
#include <string>

/**
 * Service Manager class responsible for managing background services.
 */
class SvcMgr {
public:
    SvcMgr(std::shared_ptr<ConfigMgr> config);
    void start();
    void run();
    void stop();
    void showProcesses();
    void killProcess(const std::string& processName);
private:
    std::shared_ptr<ConfigMgr> config;
    std::atomic<bool> running;
    std::map<std::string, PROCESS_INFORMATION> processes;
};

#endif // SVCMGR_HPP