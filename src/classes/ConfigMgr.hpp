// ConfigMgr.hpp
#ifndef CONFIGMGR_HPP
#define CONFIGMGR_HPP

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

struct ServiceConfig {
    std::string name;
    std::string path;
    bool skip;
};

class ConfigMgr {
public:
    ConfigMgr(const std::string& configPath);
    std::vector<ServiceConfig> getServices() const;
private:
    std::vector<ServiceConfig> services;
};

#endif // CONFIGMGR_HPP
