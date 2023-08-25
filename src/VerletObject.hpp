#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "myutils.hpp"
#include "preferences.h"
#include <algorithm>
#include <cmath>

struct VerletObject : sf::CircleShape {
  sf::Vector2f positionPrevious;
  sf::Vector2f acceleration;

  VerletObject(sf::Vector2f pos) {
    positionPrevious = pos;
  }

  void setOriginToCenter() {
    sf::FloatRect floatRect = getLocalBounds();
    setOrigin(
      floatRect.left + floatRect.width / 2.f,
      floatRect.top + floatRect.height / 2.f
    );
  }

  void updatePosition(float dt) {
    float radius = getRadius();
    sf::Vector2f currPos = getPosition();
    sf::Vector2f velocity = currPos - positionPrevious;
    sf::Vector2f nextPos;

    positionPrevious = currPos;
    nextPos = currPos + velocity + acceleration * dt * dt;

    // Check horizontal bounds
    if      (nextPos.x - radius <= 0)     nextPos.x = radius;
    else if (nextPos.x + radius >= WIDTH) nextPos.x = WIDTH - radius;

    // Check vertical bounds
    if      (nextPos.y - radius <= 0)      nextPos.y = radius;
    else if (nextPos.y + radius >= HEIGHT) nextPos.y = HEIGHT - radius;

    setPosition(nextPos);
    acceleration = {};
  }

  void accelerate(sf::Vector2f acc) {
    acceleration += acc;
  }

  void checkCollision(VerletObject& rhs) {
    if (this == &rhs) return;

    sf::Vector2f pos1 = getPosition();
    sf::Vector2f pos2 = rhs.getPosition();
    float minDist = getRadius() * 2;

    sf::Vector2f collisionAxis = {pos1.x - pos2.x, pos1.y - pos2.y};
    float dist = sqrt(collisionAxis.x * collisionAxis.x + collisionAxis.y * collisionAxis.y);

    if (dist < minDist) {
      sf::Vector2f n = collisionAxis / dist;
      float delta = minDist - dist;
      setPosition(pos1 + (0.5f * delta * n));
      rhs.setPosition(pos2 - (0.5f * delta * n));
    }
  }
};

