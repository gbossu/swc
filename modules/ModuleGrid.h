#pragma once

#include <QWidget>
#include <vector>
#include <memory>
#include <exception>
#include <string>
#include "ModuleUtils.h"
#include "ModuleSize.h"

namespace modules {

class ModuleGridError : public std::exception {
public:
  ModuleGridError(const std::string &details) :
      msg("ModuleGrid error: " + details) {}
  const char *what() const noexcept override {
    return msg.c_str();
  }
private:
  std::string msg;
};

class ModuleGrid {
public:
  ModuleGrid(const ModuleSize &size, QWidget *parent,
             const std::string &settingsPath);

private:
  std::vector<std::unique_ptr<DataForwarderBase>> forwarders;
  std::vector<std::unique_ptr<ModuleBase>> modules;
};

}