#include <sstream>

#include "bunny_manager.hpp"
#include "tile_map.hpp"
#include "path_finding.hpp"

using namespace bunny_manager;
using path_finding::Dir;

static const std::unordered_map<TileType, TileType> bunny_mutant_map {
  {TileType::white_juvenile, TileType::white_juvenile_mutant},
  {TileType::white_adult, TileType::white_adult_mutant},
  {TileType::brown_juvenile, TileType::brown_juvenile_mutant},
  {TileType::brown_adult, TileType::brown_adult_mutant},
  {TileType::black_juvenile, TileType::black_juvenile_mutant},
  {TileType::black_adult, TileType::black_adult_mutant},
  {TileType::spotted_juvenile, TileType::spotted_juvenile_mutant},
  {TileType::spotted_adult, TileType::spotted_adult_mutant},
};

static const std::unordered_map<BunnyColour, std::pair<TileType, TileType>> bunny_colour_map {
  {BunnyColour::white, {TileType::white_juvenile, TileType::white_adult}},
  {BunnyColour::brown, {TileType::brown_juvenile, TileType::brown_adult}},
  {BunnyColour::black, {TileType::black_juvenile, TileType::black_adult}},
  {BunnyColour::spotted, {TileType::spotted_juvenile, TileType::spotted_adult}}
};

static std::vector<Dir> rnd_dirs() {
  std::vector<Dir> dirs{Dir::left, Dir::right, Dir::up, Dir::down};
  std::random_shuffle(dirs.begin(), dirs.end());

  return dirs;
}

std::string BunnyManager::bunny_info(const Bunny& bunny) {
  std::string info{};

  info.append(std::to_string(bunny.age()));
  info.append(" years old, ");
  info.append(bunny.gender() == Gender::male ? "male" : "female");
  info.append(", ");
  info.append(bunny_colour_str[(int)bunny.colour()]);

  return info;
}

bool BunnyManager::is_overaged(const Bunny& bunny) {
  return ((bunny.infected() && bunny.age() >= util::rnd_range(7, 10)) ||
    !bunny.infected() && bunny.age() >= util::rnd_range(10, 12));
}

void BunnyManager::print_bunny_born(const Bunny& bunny) {
  if (bunny.infected())
    _log_info("Infected "); 
  
  std::stringstream output{};

  output << "Bunny " << bunny.name() << " was born! ("
    << bunny_info(bunny) << ")\n";

  _log_info(output.str());
}

void BunnyManager::print_bunny_died(const Bunny& bunny)
{
  if (bunny.infected())
    _log_info("Infected "); 

  std::stringstream output{};

  output << "Bunny " << bunny.name() << " died! ("
    << bunny_info(bunny) << ")\n";

  _log_info(output.str());
}

void BunnyManager::spawn_initial(int amount) {
  for (int i{0}; i < amount; i++) {
    sf::Vector2i pos{};

    do {
      pos.x = util::rnd_range(0, _tile_map.width() - 1);
      pos.y = util::rnd_range(0, _tile_map.height() - 1);
    } while (_bunny_pos_map.contains(pos));

    auto it{_bunnies.insert(_bunnies.end(), Bunny(pos))};
    _bunny_pos_map.insert({it->pos, *it});

    set_bunny_tile(*it);
    print_bunny_born(*it);
  }

  _log_info("\n");
}

void BunnyManager::set_bunny_tile(const Bunny& bunny) {
  auto tile_types{bunny_colour_map.at(bunny.colour())};
  TileType tile_type{bunny.age() < 2 ? tile_types.first : tile_types.second};
  
  if (bunny.infected())
    tile_type = bunny_mutant_map.at(tile_type);
  
  _tile_map.set_tile(bunny.pos.x, bunny.pos.y, (int)tile_type);
}

void BunnyManager::move_bunny_adj(Bunny& bunny) {
  for (const auto dir : rnd_dirs()) { // move each bunny
    std::pair<int, int> adj_pos{
      path_finding::traverse({bunny.pos.x, bunny.pos.y}, dir)
    };
    
    sf::Vector2i new_pos(adj_pos.first, adj_pos.second);

    if (!_tile_map.in_bounds(new_pos.x, new_pos.y))
      continue;

    if (!_bunny_pos_map.contains(new_pos)) {
      _tile_map.set_tile(bunny.pos.x, bunny.pos.y, (int)_floor_tile);
      _bunny_pos_map.erase(bunny.pos);

      bunny.pos = new_pos;

      _bunny_pos_map.insert({bunny.pos, bunny});
      set_bunny_tile(bunny);
      
      break;
    }
  }
}

void BunnyManager::mutate_adj(sf::Vector2i pos) {
  for (const auto dir : rnd_dirs()) {
    std::pair<int, int> adj_pos_pair{
      path_finding::traverse({pos.x, pos.y}, dir)
    };

    sf::Vector2i adj_pos(adj_pos_pair.first, adj_pos_pair.second);

    if (_bunny_pos_map.contains(adj_pos)) {
      Bunny& bunny{_bunny_pos_map.at(adj_pos)};

      if (bunny.infected())
        continue;

      bunny.infect();
      set_bunny_tile(bunny);

      break;
    }
  }
}

void BunnyManager::birth_bunnies(breedable_females_t& breedable_females) {
  for (const auto& female : breedable_females) {
    sf::Vector2i pos{female.first};

    for (const auto dir : rnd_dirs()) {
      std::pair<int, int> adj_pos{path_finding::traverse({pos.x, pos.y}, dir)};
      sf::Vector2i new_pos(adj_pos.first, adj_pos.second);

      if (!_tile_map.in_bounds(new_pos.x, new_pos.y))
        continue;

      if (!_bunny_pos_map.contains(new_pos)) {
        auto it{
          _bunnies.insert(_bunnies.end(), Bunny(new_pos, 0, female.second))
        };

        _bunny_pos_map.insert({it->pos, *it});

        set_bunny_tile(*it);
        print_bunny_born(*it);

        if (it->infected())
          mutate_adj(it->pos);

        break;
      }
    }
  }
}

void BunnyManager::sort_by_age() {
  _bunnies.sort(
    [](const Bunny& b1, const Bunny& b2) {
      return b1.age() < b2.age();
    }
  );
}

void BunnyManager::food_shortage() {
  _log_info("Food shortage occured!\n");
  _cull_bunnies.resize(_bunnies.size());
  
  std::fill(
    _cull_bunnies.begin(),
    _cull_bunnies.begin() + (bunny_limit / 2), 
    false
  );

  std::fill(
    _cull_bunnies.begin() + (bunny_limit / 2),
    _cull_bunnies.end(), 
    true
  );

  static std::random_device dev{};
  auto rng = std::default_random_engine{dev()};

  std::shuffle(_cull_bunnies.begin(), _cull_bunnies.end(), rng);
  
  int i{0};

  // cull half at random
  for (auto it{_bunnies.begin()}; it != _bunnies.end(); i++) {
    if (_cull_bunnies.at(i)) {
      _tile_map.set_tile(it->pos.x, it->pos.y, (int)_floor_tile);
      _bunny_pos_map.erase(it->pos);

      it = _bunnies.erase(it);
    }

    else
      it++;
  }
}

void BunnyManager::set_log_info(bunny_manager::log_info_t log_info) {
  _log_info = log_info;
}

BunnyManager::BunnyManager(TileMap& tile_map, TileType floor_tile,
  log_info_t log_info) :
    _tile_map(tile_map),
    _floor_tile(floor_tile),
    _log_info(log_info),
    _cull_bunnies(bunny_limit)
{
  spawn_initial(5);
}

bool BunnyManager::next_turn() {
  if (_bunnies.empty())
    return true;

  int breedable_male_count{0};
  breedable_females_t breedable_females{};

  for(auto it{_bunnies.begin()}; it != _bunnies.end();) {
    Bunny& bunny{*it};

    // kill over-aged bunnies
    if (is_overaged(bunny)) {
      _tile_map.set_tile(bunny.pos.x, bunny.pos.y, (int)_floor_tile);
      print_bunny_died(bunny);
      _bunny_pos_map.erase(it->pos);
      it = _bunnies.erase(it);

      continue;
    }
    
    move_bunny_adj(bunny);

    if (bunny.infected())
      mutate_adj(bunny.pos);

    bunny.grow(1);

    if (!bunny.infected() && bunny.age() >= 2) {
      if (bunny.gender() == Gender::male)
        breedable_male_count += 1;
      
      else
        breedable_females.push_back({bunny.pos, bunny.colour()});
    }

    ++it;
  }

  if (breedable_male_count)
    birth_bunnies(breedable_females);

  sort_by_age();

  _log_info("\nBunnies remaining: \n");

  for (const auto& bunny : _bunnies) {
    if (bunny.infected())
      _log_info("Infected ");

    std::stringstream output{};
    
    output << "Bunny " << bunny.name() << " (" << bunny_info(bunny) <<
      ") at (" << bunny.pos.x << ", " << bunny.pos.y << ")\n";

    _log_info(output.str());
  }

  _log_info("\n");

  if (_bunnies.size() > bunny_limit)
    food_shortage();

  return false;
}

void BunnyManager::reset() {
  _bunnies.clear();
  _bunny_pos_map.clear();
  _tile_map.clear((int)_floor_tile);
  spawn_initial(5);
}