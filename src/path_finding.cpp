#include <queue>
#include <set>

#include "path_finding.hpp"

namespace path_finding {
  std::pair<int, int> traverse(std::pair<int, int> start, Dir dir) {
    std::pair<int, int> end(start);

    switch (dir) {
      case Dir::left:
        end.first -= 1;

        break;

      case Dir::right:
        end.first += 1;

        break;

      case Dir::up:
        end.second -= 1;

        break;

      case Dir::down:
        end.second += 1;

        break;
    }

    return end;
  }

  // std::pair<int, int> traverse(std::pair<int, int> start,
  //   std::string_view path)
  // {
  //   std::pair<int, int> end(start);

  //   for (const auto& ch : path)
  //     end = traverse(end, ch);

  //   return end;
  // }

  // std::pair<bool, std::string> bfs(std::pair<int, int> start,
  //   std::pair<int, int> goal, std::function<bool(int, int)> valid_move)
  // {
  //   std::queue<std::string> paths{};

  //   paths.push("");

  //   std::string path{""};
  //   std::set<std::pair<int, int>> visited{};

  //   while (traverse(start, path) != goal) {
  //     if (paths.empty())
  //       return {false, path};

  //     path = paths.front();
  //     paths.pop();

  //     for (const auto ch : "LRUD") {
  //       std::string put{path + ch};
  //       std::pair<int, int> end{traverse(start, put)};

  //       if (!visited.contains(end) && valid_move(end.first, end.second)) {
  //         visited.insert(end);

  //         paths.push(put);
  //       }
  //     }
  //   }

  //   return {true, path};
  // }
}