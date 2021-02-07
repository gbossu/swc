#pragma once

#include <exception>
#include <string>

class DataFetchError : public std::exception {
public:
  DataFetchError(const std::string &message, const std::string &prefix = "Error: ")
      : message(prefix + message) {}
  const char *what() const noexcept override {
    return message.c_str();
  }
private:
  std::string message;
};