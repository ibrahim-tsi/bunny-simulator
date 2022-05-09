#pragma once

#include <vector>

class TileMap {
  int _width{};
  int _height{};
  std::vector<std::vector<int>> _data{};
  int _tile_size{};
  std::vector<std::pair<int, int>> _modified_tiles{};

public:
  const int& width() const;
  const int& height() const;
  const std::vector<std::vector<int>>& data() const;
  const int& tile_size() const;
  const std::vector<std::pair<int, int>>& modified_tiles() const;

  TileMap(int width, int height, int tile_size, int tile);

  void clear(int tile);
  void reset_modified_tiles();
  bool in_bounds(int c, int r);
  const int& get_tile(int c, int r);
  void set_tile(int c, int r, int tile);
};