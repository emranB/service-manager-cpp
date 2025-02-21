// ConfigMgr.cpp
#include "ConfigMgr.hpp"
#include <fstream>
#include <iostream>

ConfigMgr::ConfigMgr(const std::string& configPath) {
    std::ifstream configFile(configPath);
    if (!configFile) {
        std::cerr << "Failed to open config file: " << configPath << std::endl;
        return;
    }
    nlohmann::json configJson;
    configFile >> configJson;

    for (const auto& service : configJson["services"]) {
        services.push_back({ service["name"], service["path"], service["skip"] });
    }
}

std::vector<ServiceConfig> ConfigMgr::getServices() const {
    return services;
}