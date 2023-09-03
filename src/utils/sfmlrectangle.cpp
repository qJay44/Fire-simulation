#include "sfmlrectangle.h"

sf::RectangleShape createOutlineRectangle(
  sf::Vector2f size,
  sf::Vector2f pos,
  sf::Color col
) {
  sf::RectangleShape rect(size);
  rect.setPosition(pos);
  rect.setFillColor(sf::Color::Transparent);
  rect.setOutlineColor(col);
  rect.setOutlineThickness(1.f);

  return rect;
}

sf::RectangleShape createOutlineRectangle(
  sf::Vector2f size,
  sf::Vector2i pos,
  sf::Color col
) {
  sf::Vector2f p{
    static_cast<float>(pos.x),
    static_cast<float>(pos.y)
  };
  return createOutlineRectangle(size, p, col);
}

