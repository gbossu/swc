#pragma once

#include <exception>

namespace exceptions {

class XdoError : public std::exception {
public:
  const char *what() const noexcept override {
    return "Error: Failure when creating the swc window container.";
  }
};

}
