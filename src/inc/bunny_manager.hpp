#pragma once

#include <SFML/System/Vector2.hpp>
#include <list>
#include <unordered_map>

#include "util.hpp"
#include "bunny.hpp"
#include "tile_map.hpp"
#include "tile_type.hpp"

namespace bunny_manager {
  typedef std::function<void(std::string_view)> log_info_t;
  typedef std::list<std::pair<sf::Vector2i, BunnyColour>> breedable_females_t;
}

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
  static const int bunny_limit{1000};

  bunny_manager::log_info_t _log_info{};
  std::list<Bunny> _bunnies{};
  std::unordered_map<sf::Vector2i, Bunny&> _bunny_pos_map{};
  TileMap& _tile_map;
  TileType _floor_tile{};
  std::vector<bool> _cull_bunnies{};
  
  static std::string bunny_info(const Bunny& bunny);
  static bool is_overaged(const Bunny& bunny);

  void print_bunny_born(const Bunny& bunny);
  void print_bunny_died(const Bunny& bunny);
  void spawn_initial(int amount);
  void set_bunny_tile(const Bunny& bunny);
  void move_bunny_adj(Bunny& bunny);
  void mutate_adj(sf::Vector2i pos);
  void birth_bunnies(bunny_manager::breedable_females_t& breedable_females);
  void sort_by_age();
  void food_shortage();

public:
  void set_log_info(bunny_manager::log_info_t log_info);

  BunnyManager(TileMap& tile_map, TileType floor_tile,
    bunny_manager::log_info_t log_info);

  bool next_turn();
  void reset();
};