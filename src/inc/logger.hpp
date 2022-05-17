#pragma once

#include <string>
#include <string_view>
#include <fstream>

class Logger {
  std::string _file_name{};
  std::ofstream _ofs{};

public:
  bool to_console{};

  Logger(std::string_view file_name, bool to_console = false);

  void log(std::string_view str);
  void clear();
};