#include <SFML/System/Vector2.hpp>
#include <string_view>

#include "util.hpp"

enum class Gender {
  male,
  female,
  end
};

enum class BunnyColour {
  white,
  brown,
  black,
  spotted,
  end
};

const std::string_view bunny_colour_str[] {
  "white",
  "brown",
  "black",
  "spotted",
};

class Bunny {
  Gender _gender{};
  BunnyColour _colour{};
  int _age{};
  std::string _name{}; // const
  bool _mutant{};

public:
  sf::Vector2i pos{};

  int age() const;
  const Gender& gender() const;
  const BunnyColour& colour() const;
  const std::string& name() const;
  bool infected() const;

  explicit Bunny(sf::Vector2i bunny_pos);
  Bunny(sf::Vector2i pos, int age, BunnyColour colour);

  void grow(int years);
  void infect();
};