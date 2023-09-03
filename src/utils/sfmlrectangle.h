#include "SFML/Graphics.hpp"

sf::RectangleShape createOutlineRectangle(
  sf::Vector2f size,
  sf::Vector2f pos,
  sf::Color col = sf::Color(40, 40, 40)
);

sf::RectangleShape createOutlineRectangle(
  sf::Vector2f size,
  sf::Vector2i pos,
  sf::Color col
);

template<typename T>
sf::RectangleShape createOutlineRectangle(
  T size,
  sf::Vector2i pos,
  sf::Color col = sf::Color(40, 40, 40)
) {
  float s = static_cast<float>(size);
  sf::Vector2f p{
    static_cast<float>(pos.x),
    static_cast<float>(pos.y)
  };
  return createOutlineRectangle({s, s}, p, col);
}

template<typename T>
sf::RectangleShape createOutlineRectangle(
  T size,
  sf::Vector2f pos,
  sf::Color col = sf::Color(40, 40, 40)
) {
  float s = static_cast<float>(size);
  return createOutlineRectangle({s, s}, pos, col);
}

