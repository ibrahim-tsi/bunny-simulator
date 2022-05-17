#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <list>
#include <fstream>
#include <iostream>

#include "config.h"
#include "util.hpp"
#include "tile_type.hpp"
#include "tile_map.hpp"
#include "logger.hpp"
#include "bunny_manager.hpp"

static const TileType floor_tile{TileType::dirt};
static const char *const win_title{"Bunny Simulator"};
static const char *const out_file_name{"output.txt"};

static const std::unordered_map<TileType, std::string> tile_type_map {
  {TileType::dirt, "dirt"},
  {TileType::white_adult, "white_adult"},
  {TileType::white_adult_mutant, "white_adult_mutant"},
  {TileType::white_juvenile, "white_juvenile"},
  {TileType::white_juvenile_mutant, "white_juvenile_mutant"},
  {TileType::brown_adult, "brown_adult"},
  {TileType::brown_adult_mutant, "brown_adult_mutant"},
  {TileType::brown_juvenile, "brown_juvenile"},
  {TileType::brown_juvenile_mutant, "brown_juvenile_mutant"},
  {TileType::black_adult, "black_adult"},
  {TileType::black_adult_mutant, "black_adult_mutant"},
  {TileType::black_juvenile, "black_juvenile"},
  {TileType::black_juvenile_mutant, "black_juvenile_mutant"},
  {TileType::spotted_adult, "spotted_adult"},
  {TileType::spotted_adult_mutant, "spotted_adult_mutant"},
  {TileType::spotted_juvenile, "spotted_juvenile"},
  {TileType::spotted_juvenile_mutant, "spotted_juvenile_mutant"}
};

typedef std::unordered_map<
  TileType,
  std::pair<sf::Texture, sf::Sprite>
> tile_sprite_map_t;

static std::string get_tile_dir(TileType tile_type) {
  std::string dir("resources/tiles/");

  dir.append(tile_type_map.at(tile_type));
  dir.append(".png");

  return dir;
}

static void resize_sprite(sf::Sprite& sprite, int width, int height) {
  auto size{sprite.getTexture()->getSize()};

  sprite.setScale((float)width / size.x, (float)height / size.y);
}

static void init_tile_sprite_map(tile_sprite_map_t& tile_sprite_map,
  int tile_size)
{
  for (int i{0}; i < (int)TileType::end; i++) {
    TileType tile_type{(TileType)i};

    auto ret{tile_sprite_map.insert(
      {tile_type, {sf::Texture(), sf::Sprite()}})
    };
    
    sf::Texture& tex{ret.first->second.first};

    if (!tex.loadFromFile(get_tile_dir(tile_type)))
      exit(1);

    sf::Sprite& sprite{ret.first->second.second};

    sprite.setTexture(tex);
    resize_sprite(sprite, tile_size, tile_size);
  }
}

static void draw_tile_map(sf::RenderTexture& tex, TileMap& tile_map,
  tile_sprite_map_t& tile_sprite_map)
{
  for (const auto& tile : tile_map.modified_tiles()) {
    int r{tile.first};
    int c{tile.second};

    TileType tile_type{(TileType)tile_map.get_tile(c, r)};

    if (tile_type != floor_tile) {
      sf::Sprite& sprite{tile_sprite_map.at(floor_tile).second };

      sprite.setPosition(c * tile_map.tile_size(), r * tile_map.tile_size());
      tex.draw(sprite);
    }

    sf::Sprite& sprite{tile_sprite_map.at(tile_type).second };

    sprite.setPosition(c * tile_map.tile_size(), r * tile_map.tile_size());
    tex.draw(sprite);
  }

  tex.display();
  tile_map.reset_modified_tiles();
}

static void init_win(sf::RenderWindow& win, TileMap& tile_map,
  bool full_screen = false, char const *title = win_title)
{
  auto get_win_size = [&tile_map]() {
    return sf::Vector2i(
      tile_map.width() * tile_map.tile_size(),
      tile_map.height() * tile_map.tile_size()
    );
  };

  sf::Vector2i win_size{get_win_size()};
  
  if (full_screen)
    win.create(
      sf::VideoMode::getFullscreenModes()[0],
      win_title,
      sf::Style::Fullscreen
    );

  else
    win.create(
      sf::VideoMode(win_size.x, win_size.y),
      win_title
    );
}

static void init_iterations_text(sf::Text& iterations_text, sf::Font& font) {
  iterations_text.setFont(font);
  iterations_text.setCharacterSize(24);
  iterations_text.setFillColor(sf::Color::Black);
  iterations_text.setPosition(10, 10);
}

static void game_loop(sf::RenderWindow& win, TileMap& tile_map,
  tile_sprite_map_t& tile_sprite_map)
{
  sf::RenderTexture screen_tex{};
  
  if (!screen_tex.create(win.getSize().x, win.getSize().y))
    exit(1);

  sf::Sprite screen_sprite(screen_tex.getTexture());

  auto update_screen = [&]() {
    win.clear(sf::Color::Black);
    draw_tile_map(screen_tex, tile_map, tile_sprite_map);
    win.draw(screen_sprite);
  };

  update_screen();

  int iterations{0};
  sf::Text iterations_text{};
  sf::Font font;
  
  if (!font.loadFromFile("resources/m6x11.ttf"))
    exit(1);

  init_iterations_text(iterations_text, font);
  
  auto update_ui = [&]() {
    iterations_text.setString("Iterations: " + std::to_string(iterations));
    win.draw(iterations_text);
  };

  Logger logger(out_file_name);
  BunnyManager bunny_manager(tile_map, floor_tile, logger); 

  bool log_to_console{false};
  
  while (win.isOpen()) {
    sf::Event event{};

    while (win.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        win.close();
      
      else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::T) {
          if (!bunny_manager.next_turn())
            iterations += 1;
        }

        else if (event.key.code == sf::Keyboard::R) {
          bunny_manager.reset();
          iterations = 0;

          logger.clear();
        }

        else if (event.key.code == sf::Keyboard::C)
          logger.to_console = !logger.to_console;
      }
    }

    update_screen();
    update_ui();
    win.display();
  }
}

int main(int argc, char *argv[]) {
  TileMap tile_map(
    80, // width
    80, // height
    16, // tile size
    (int)floor_tile
  );

  tile_sprite_map_t tile_sprite_map{};

  init_tile_sprite_map(tile_sprite_map, tile_map.tile_size());

  sf::RenderWindow win{};
  
  init_win(win, tile_map);
  game_loop(win, tile_map, tile_sprite_map);

  return 0;
}