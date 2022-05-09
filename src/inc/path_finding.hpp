#pragma once

#include <utility>
#include <string_view>
#include <string>
#include <functional>

namespace path_finding {
  std::pair<int, int> traverse(std::pair<int, int> start, char dir);
  
  std::pair<int, int> traverse(std::pair<int, int> start,
    std::string_view path);

  std::pair<bool, std::string> bfs(std::pair<int, int> start,
    std::pair<int, int> goal, std::function<bool(int, int)> valid_move);
}