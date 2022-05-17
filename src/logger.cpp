#include <iostream>

#include "logger.hpp"

Logger::Logger(std::string_view file_name, bool console_output) :
  _file_name(file_name), to_console(console_output)
{
  _ofs.open(_file_name);
}

void Logger::log(std::string_view str) {
  _ofs << str;

  if (to_console)
    std::cout << str;
}

void Logger::clear() {
  _ofs.close();
  _ofs.open(_file_name);
}