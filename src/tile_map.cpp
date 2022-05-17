#include <algorithm>

#include "tile_map.hpp"

int TileMap::width() const { return _width; }
int TileMap::height() const { return _height; }
const std::vector<std::vector<int>>& TileMap::data() const { return _data; }
int TileMap::tile_size() const { return _tile_size; };

const std::vector<std::pair<int, int>>& TileMap::modified_tiles() const {
  return _modified_tiles;
}

TileMap::TileMap(int width, int height, int tile_size, int tile) :
  _width(width), _height(height), _tile_size(tile_size)
{
  std::vector<int> cols(width, tile);

  for (int r{0}; r < _height; r++) {
    _data.push_back(cols);

    for (int c{0}; c < _width; c++)
      _modified_tiles.push_back({r, c});
  }
}

void TileMap::clear(int tile) {
  for (int r{0}; r < _height; r++) {
    std::fill(_data[r].begin(), _data[r].end(), tile);

    for (int c{0}; c < _width; c++)
      _modified_tiles.push_back({r, c});
  }
}

void TileMap::reset_modified_tiles() { _modified_tiles.clear(); }

bool TileMap::in_bounds(int c, int r) const {
  return c >= 0 && c < _width && r >= 0 && r < _height;
}

int TileMap::get_tile(int c, int r) const {
  try {
    return _data.at(r).at(c);
  }

  catch (const std::out_of_range& ex) {
    throw;
  }
}

void TileMap::set_tile(int c, int r, int tile) {
  if (_data.at(r).at(c) != tile) {
    _data[r][c] = tile;

    _modified_tiles.push_back({r, c});
  }
}