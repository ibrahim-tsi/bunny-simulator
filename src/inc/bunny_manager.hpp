#pragma once

#include <SFML/System/Vector2.hpp>
#include <list>
#include <unordered_map>

#include "util.hpp"
#include "bunny.hpp"
#include "tile_map.hpp"
#include "tile_type.hpp"

template<typename T>
struct std::hash<sf::Vector2<T>> {
  std::size_t operator()(const sf::Vector2<T>& k) const {
    std::size_t seed{0};

    util::hash_combine(seed, k.x);
    util::hash_combine(seed, k.y);

    return seed;
  }
};

class BunnyManager {
  static constexpr int bunny_limit{1000};

  std::list<Bunny> _bunnies{};
  std::unordered_map<sf::Vector2i, Bunny&> _bunny_pos_map;
  TileMap& _tile_map;
  TileType _floor_tile{};
  std::string _out_file_name{};
  std::vector<bool> _cull_bunnies{};
  
  static std::string bunny_info(const Bunny& bunny);
  static void print_bunny_born(const Bunny& bunny, std::ofstream& ofs,
    bool out_console);
  
  static void print_bunny_died(const Bunny& bunny, std::ofstream& ofs,
    bool out_console);

  void spawn_initial(int amount);
  void set_bunny_tile(const Bunny& bunny);
  void mutate_adj(sf::Vector2i pos);

public:
  bool out_console{};

  BunnyManager(TileMap& tile_map, TileType floor_tile,
    std::string out_file_name);

  bool next_turn();
  void reset();
};