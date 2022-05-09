#include "bunny.hpp"
#include "bunny_names.hpp"

const int& Bunny::age() const { return _age; }
const Gender& Bunny::gender() const { return _gender; }
const BunnyColour& Bunny::colour() const { return _colour; }
const std::string& Bunny::name() const { return _name; }
const bool& Bunny::infected() const { return _mutant; }

Bunny::Bunny(sf::Vector2i bunny_pos) :
  _gender(util::rnd_enum_class<Gender>()),
  _colour(util::rnd_enum_class<BunnyColour>()),
  _age(util::rnd_range(0, 10)),
  _name(bunny_names[util::rnd_range(0, bunny_names_size - 1)]),
  _mutant(util::rnd_range(1, 100) == 1),
  pos(bunny_pos)
{
  
}

Bunny::Bunny(sf::Vector2i pos, int age, BunnyColour colour) : Bunny(pos) {
  _age = age;
  _colour = colour;
}

void Bunny::grow(int years) { _age += years; }
void Bunny::infect() { _mutant = true; }