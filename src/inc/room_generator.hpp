#pragma once

#include <SFML/Graphics.hpp>

#include "util.hpp"

typedef sf::IntRect room_t;

enum class CorridorType {
  horizontal,
  vertical
};

bool y_intersects(room_t room1, room_t room2) {
  return room1.left < room2.left + room2.width &&
    room1.left + room1.width > room2.left;
}

bool x_intersects(room_t room1, room_t room2) {
  return room1.top < room2.top + room2.height &&
    room1.top + room1.height > room2.top;
}

class RoomGenerator {
  std::vector<room_t> _rooms{};
  std::vector<std::pair<room_t, CorridorType>> _corridors{};
  int _tile_map_width{};
  int _tile_map_height{};

public:
  RoomGenerator(int tile_map_width, int tile_map_height) : 
    _tile_map_width(tile_map_width), _tile_map_height(tile_map_height) {}

  void gen() {
    const int iterations{(_tile_map_width + _tile_map_height) * 2};

    auto create_room = [this](int min_size) {
      room_t new_room{};
      new_room.width = util::rnd_range(min_size, _tile_map_width / 3);
      new_room.height = util::rnd_range(min_size, _tile_map_height / 3);
      new_room.left = util::rnd_range(0, _tile_map_width - new_room.width);
      new_room.top = util::rnd_range(0, _tile_map_height - new_room.height);

      return new_room;
    };

    for (int i = 0; i < iterations; i++) {
      room_t new_room = create_room(5);
    }
  }
};