#include <iostream>
#include <fstream>

#include "bunny_manager.hpp"
#include "tile_map.hpp"
#include "path_finding.hpp"

#define OUT(args, ofs, console_on) { \
  if (console_on) \
    std::cout << args; \
  \
  ofs << args; \
}

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

static std::string rnd_dirs() {
  std::string dirs{"LRUD"};
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

void BunnyManager::print_bunny_born(const Bunny& bunny, std::ofstream& ofs,
  bool out_console)
{
  if (bunny.infected())
    OUT("Infected ", ofs, out_console); 
  
  OUT("Bunny " << bunny.name() << " was born! ("
    << bunny_info(bunny) << ")\n", ofs, out_console);
}

void BunnyManager::print_bunny_died(const Bunny& bunny, std::ofstream& ofs,
  bool out_console)
{
  if (bunny.infected())
    OUT("Infected ", ofs, out_console);

  OUT("Bunny " << bunny.name() << " died! ("
    << bunny_info(bunny) << ")\n", ofs, out_console);
}

void BunnyManager::spawn_initial(int amount) {
  std::ofstream ofs(_out_file_name);

  for (int i{0}; i < amount; i++) {
    sf::Vector2i pos{};

    do {
      pos.x = util::rnd_range(0, _tile_map.width() - 1);
      pos.y = util::rnd_range(0, _tile_map.height() - 1);
    } while (_bunny_pos_map.contains(pos));

    auto it{_bunnies.insert(_bunnies.end(), Bunny(pos))};
    _bunny_pos_map.insert({it->pos, *it});

    set_bunny_tile(*it);
    print_bunny_born(*it, ofs, out_console);
  }
}

void BunnyManager::set_bunny_tile(const Bunny& bunny) {
  auto tile_types{bunny_colour_map.at(bunny.colour())};
  TileType tile_type{bunny.age() < 2 ? tile_types.first : tile_types.second};
  
  if (bunny.infected())
    tile_type = bunny_mutant_map.at(tile_type);
  
  _tile_map.set_tile(bunny.pos.x, bunny.pos.y, (int)tile_type);
}

void BunnyManager::mutate_adj(sf::Vector2i pos) {
  for (const auto ch : rnd_dirs()) {
    std::pair<int, int> adj_pos_pair{
      path_finding::traverse({pos.x, pos.y}, ch)
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

BunnyManager::BunnyManager(TileMap& tile_map, TileType floor_tile,
  std::string out_file_name) :
    _tile_map(tile_map),
    _floor_tile(floor_tile),
    _out_file_name(out_file_name),
    _cull_bunnies(bunny_limit),
    out_console(false)
{
  spawn_initial(5);
}

bool BunnyManager::next_turn() {
  if (_bunnies.empty())
    return true;

  int breedable_male_count{0};
  std::list<std::pair<sf::Vector2i, BunnyColour>> breedable_females{};

  std::ofstream ofs(_out_file_name, std::ios_base::app);

  for(auto it{_bunnies.begin()}; it != _bunnies.end();) {
    Bunny& bunny{*it};

    if ((bunny.infected() && bunny.age() >= util::rnd_range(7, 10)) ||
      !bunny.infected() && bunny.age() >= util::rnd_range(10, 12))
    {
      _tile_map.set_tile(bunny.pos.x, bunny.pos.y, (int)_floor_tile);
      print_bunny_died(bunny, ofs, out_console);
      _bunny_pos_map.erase(it->pos);
      it = _bunnies.erase(it);

      continue;
    }

    for (const auto ch : rnd_dirs()) {
      std::pair<int, int> adj_pos{
        path_finding::traverse({bunny.pos.x, bunny.pos.y}, ch)
      };
      
      sf::Vector2i new_pos(adj_pos.first, adj_pos.second);

      if (!_tile_map.in_bounds(new_pos.x, new_pos.y))
        continue;

      if (!_bunny_pos_map.contains(new_pos)) {
        _tile_map.set_tile(bunny.pos.x, bunny.pos.y, (int)_floor_tile);

        _bunny_pos_map.erase(bunny.pos);
        bunny.pos = new_pos;
        _bunny_pos_map.insert({bunny.pos, bunny});

        set_bunny_tile(*it);

        break;
      }
    }

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

  if (breedable_male_count) {
    for (const auto& female : breedable_females) {
      sf::Vector2i pos{female.first};

      for (const auto ch : rnd_dirs()) {
        std::pair<int, int> adj_pos{path_finding::traverse({pos.x, pos.y}, ch)};
        sf::Vector2i new_pos(adj_pos.first, adj_pos.second);

        if (!_tile_map.in_bounds(new_pos.x, new_pos.y))
          continue;

        if (!_bunny_pos_map.contains(new_pos)) {
          if (true) {
            auto it{
              _bunnies.insert(_bunnies.end(), Bunny(new_pos, 0, female.second))
            };

            _bunny_pos_map.insert({it->pos, *it});

            set_bunny_tile(*it);
            print_bunny_born(*it, ofs, out_console);

            if (it->infected())
              mutate_adj(it->pos);

            break;
          }
        }
      }
    }

    _bunnies.sort(
      [](const Bunny& b1, const Bunny& b2) {
        return b1.age() < b2.age();
      }
    );

    OUT("\nBunnies remaining: \n", ofs, out_console);

    for (const auto& bunny : _bunnies) {
      if (bunny.infected())
        OUT("Infected ", ofs, out_console); 
      
      OUT("Bunny " << bunny.name() << " (" << bunny_info(bunny) <<
        ") at (" << bunny.pos.x << ", " << bunny.pos.y << ")\n",
        ofs, out_console);
    }

    OUT("\n", ofs, out_console);
  }

  if (_bunnies.size() > bunny_limit) {
    OUT("Food shortage occured!\n", ofs, out_console);
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

  return false;
}

void BunnyManager::reset() {
  _bunnies.clear();
  _bunny_pos_map.clear();
  _tile_map.clear((int)_floor_tile);
  spawn_initial(5);
}